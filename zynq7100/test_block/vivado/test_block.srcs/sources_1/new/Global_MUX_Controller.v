`timescale 1ns / 1ps

module Global_MUX_Controller #(
    // 占位符区域：等待具体型号确定后，直接在这里填数字
    parameter SYS_CLK_FREQ  = 50_000_000, // 系统时钟 50MHz
    parameter LIMIT_MAX_PRF  = 10000,      // 探头物理极限：绝对不允许超过 10kHz
    parameter MAX_DEPTH     = 4096,       // [待定] 单条线最大采样点数 (预设 4096 防溢出)
    parameter DEAD_TIME_CYC = 250         // [待定] MAX芯片切换死区时间 (预设 250拍 = 5微秒)
)(
    input  wire        clk,
    input  wire        rst_n,
    // 接收来自 Zynq PS 端 (上位机) 的动态计数值
    input  wire [31:0] dyn_prf_max_cnt,
    
    // --- 接口 1：控制 MAX 芯片 (通道切换) ---
    // [待定] 如果是 SPI，这里会变成 mosi, sclk, cs 等引脚
    output reg  [1:0]  max_ch_sel,     // 00:阵元1-8, 01:阵元9-16, 10:17-24, 11:25-32
    output reg         max_cfg_valid,  // 触发 MAX 芯片配置动作
    
    // --- 接口 2：控制脉冲发生器 (Pulser) ---
    output reg         pulser_fire,    // 发送高压开火脉冲
    
    // --- 接口 3：控制 ADC/LVDS 接收端 ---
    output reg         adc_acq_en,     // 拉高表示：ADC数据有效，开始写入 BRAM
    
    // --- 接口 4：BRAM 水库与 DAS 模块握手 ---
    output reg  [1:0]  bram_write_bank,// 告诉 BRAM 目前写的是第几个区块 (0~3)
    output reg         frame_ready     // 告诉 DAS 模块：32通道已拼齐，开始波束合成！
);
// 计算 10kHz 对应的最小允许计数值 (50MHz 下为 5000 拍)
    // 注意：频率越高，计数值越小。所以实际计数值绝对不能低于这个底线。
    localparam LIMIT_MIN_CNT = SYS_CLK_FREQ / LIMIT_MAX_PRF;

    reg [31:0] safe_prf_max_cnt;

    // 核心安全逻辑：硬件级一票否决
    always @(*) begin
        // 如果上位机发来的频率太高（即计数值小于底线 LIMIT_MIN_CNT）
        if (dyn_prf_max_cnt < LIMIT_MIN_CNT) begin
            // 强制截断，锁死在最高安全频率 (10kHz)
            safe_prf_max_cnt = LIMIT_MIN_CNT;
        end else begin
            // 在安全范围内，信任上位机指令
            safe_prf_max_cnt = dyn_prf_max_cnt;
        end
    end

    // 第一部分：PRF 计数器 (使用经过安检的 safe_prf_max_cnt)
    reg [31:0] prf_cnt;
    wire       prf_trigger;
    
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            prf_cnt <= 32'd0;
        end else if (prf_cnt >= safe_prf_max_cnt) begin
            prf_cnt <= 32'd0;
        end else begin
            prf_cnt <= prf_cnt + 1'b1;
        end
    end
    assign prf_trigger = (prf_cnt >= safe_prf_max_cnt);

    // 第二部分：核心状态机定义
    localparam S_IDLE        = 3'd0; // 休眠，等待 PRF 触发
    localparam S_CFG_MAX     = 3'd1; // 配置 MAX 芯片切换物理通道
    localparam S_WAIT_DEAD   = 3'd2; // 死区等待（极其重要，保护硬件）
    localparam S_TX_FIRE     = 3'd3; // 触发高压发射
    localparam S_ACQUIRE     = 3'd4; // 接收回波，写入 BRAM
    localparam S_CHECK_DONE  = 3'd5; // 检查 4 个区块是否全部拼完

    reg [2:0] current_state, next_state;
    
    // 通用延时/深度计数器
    reg [15:0] delay_cnt;
    
    // 复用轮次计数器 (0, 1, 2, 3)，对应 BRAM 的 4 个 Bank
    reg [1:0]  mux_cycle_cnt; 

    // [状态机 - 时序段]
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) current_state <= S_IDLE;
        else        current_state <= next_state;
    end

    // [状态机 - 跳转逻辑段]
    always @(*) begin
        next_state = current_state;
        case (current_state)
            S_IDLE: begin
                if (prf_trigger) 
                    next_state = S_CFG_MAX;
            end
            
            S_CFG_MAX: begin
                // [待定] 如果是普通 GPIO，1 拍就够了；如果是 SPI，这里要等 SPI_Done 信号
                next_state = S_WAIT_DEAD; 
            end
            
            S_WAIT_DEAD: begin
                if (delay_cnt == DEAD_TIME_CYC - 1) 
                    next_state = S_TX_FIRE;
            end
            
            S_TX_FIRE: begin
                // 高压触发脉冲通常只需要维持几个时钟周期 (后续可调整)
                if (delay_cnt == 16'd5) 
                    next_state = S_ACQUIRE;
            end
            
            S_ACQUIRE: begin
                if (delay_cnt == MAX_DEPTH - 1) 
                    next_state = S_CHECK_DONE;
            end
            
            S_CHECK_DONE: begin
                // 无论是否满 4 次，都回到 IDLE 等待下一次 PRF 脉冲
                next_state = S_IDLE; 
            end
            
            default: next_state = S_IDLE;
        endcase
    end

    // 第三部分：状态机动作与计数器维护
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            delay_cnt       <= 16'd0;
            mux_cycle_cnt   <= 2'd0;
            
            max_ch_sel      <= 2'b00;
            max_cfg_valid   <= 1'b0;
            pulser_fire     <= 1'b0;
            adc_acq_en      <= 1'b0;
            bram_write_bank <= 2'b00;
            frame_ready     <= 1'b0;
        end else begin
            // 默认值：脉冲信号自动归零，防止多发
            max_cfg_valid <= 1'b0;
            pulser_fire   <= 1'b0;
            frame_ready   <= 1'b0;
            
        case (current_state)
                S_IDLE: begin
                    delay_cnt <= 16'd0;
                end
                
                S_CFG_MAX: begin
                    max_ch_sel    <= mux_cycle_cnt; 
                    max_cfg_valid <= 1'b1;
                    delay_cnt     <= 16'd0;
                end
                
                S_WAIT_DEAD: begin
                    // 修复：达到目标值时，清零计数器，为下一个状态做准备
                    if (delay_cnt == DEAD_TIME_CYC - 1) begin
                        delay_cnt <= 16'd0;
                    end else begin
                        delay_cnt <= delay_cnt + 1'b1;
                    end
                end
                
                S_TX_FIRE: begin
                    pulser_fire <= 1'b1;
                    // 修复：同理，发射完成准备进入采集时清零
                    if (delay_cnt == 16'd5) begin
                        delay_cnt <= 16'd0;
                    end else begin
                        delay_cnt <= delay_cnt + 1'b1;
                    end
                end
                
                S_ACQUIRE: begin
                    adc_acq_en      <= 1'b1; 
                    bram_write_bank <= mux_cycle_cnt; 
                    // 修复：采集满深度后清零
                    if (delay_cnt == MAX_DEPTH - 1) begin
                        delay_cnt <= 16'd0;
                    end else begin
                        delay_cnt <= delay_cnt + 1'b1;
                    end
                end
                
                S_CHECK_DONE: begin
                    adc_acq_en <= 1'b0; 
                    delay_cnt  <= 16'd0;
                    
                    if (mux_cycle_cnt == 2'd3) begin
                        frame_ready   <= 1'b1;  
                        mux_cycle_cnt <= 2'd0;  
                    end else begin
                        mux_cycle_cnt <= mux_cycle_cnt + 1'b1; 
                    end
                end
            endcase
        end
    end

endmodule