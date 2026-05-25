`timescale 1ns / 1ps
// ============================================================================
// 模块名称:    DAS_Core
// 模块描述:    参数化延迟求和 (DAS) 波束合成核心模块
//              实现了用于通道时间对齐、变迹加权 (Apodization) 和信号求和的
//              实时硬件流水线 (Real-time Pipeline)。
// ============================================================================

module DAS_Core #(
    // --- 系统级参数 (实例化时可自由修改) ---
    parameter CH_NUM       = 8,      // 接收通道的总数量
    parameter ADC_WIDTH    = 12,     // ADC 输入数据的位宽 (默认12-bit)
    parameter DELAY_WIDTH  = 10,     // 延迟指针的位宽 (决定了最大延迟深度，必须是 log2(BRAM_DEPTH))
    parameter BRAM_DEPTH   = 1024,   // 环形缓存的物理深度 (必须是 2 的 DELAY_WIDTH 次方)
    parameter WEIGHT_WIDTH = 8       // 变迹权重 (抑制权重) 的位宽
)(
    input  wire clk,                 // 系统时钟 (驱动流水线运转的心脏)
    input  wire rst_n,               // 异步复位信号 (低电平有效)
    input  wire adc_valid,           // 数据有效标志位 (高电平时表示当前射频流数据有效)
    
    // --- 展平的数据接口 (Flattened Data Interfaces) ---
    // 注意: 为了在标准 Verilog 中支持参数化的端口，我们将多维数组"拍扁"成了单根极宽的总线。
    // 内部逻辑会再将这根宽总线切片分配给各个通道。
    input  wire [CH_NUM * ADC_WIDTH - 1 : 0]    adc_data_in, // 包含所有通道ADC数据的总线
    input  wire [CH_NUM * DELAY_WIDTH - 1 : 0]  delay_in,    // 包含所有通道延迟参数的总线
    input  wire [CH_NUM * WEIGHT_WIDTH - 1 : 0] weight_in,   // 包含所有通道权重参数的总线   
    
    // --- 输出接口 ---
    // 输出位宽在经过乘法和加法累加后会变宽，防止数据溢出。
    // 计算公式: 最终位宽 = ADC位宽 + 权重位宽 + log2(通道数) 这里使用$clog2语法，这是一个编译器函数而不是运行期电路，不会消耗物理资源
    output reg  [(ADC_WIDTH + WEIGHT_WIDTH + $clog2(CH_NUM)) - 1 : 0] beamformed_out, // 波束合成最终输出数据
    output reg                                           out_valid       // 输出数据有效标志位
);

    // ========================================================================
    // 1. 全局写指针逻辑 (所有通道共享一个时间基准)
    // ========================================================================
    reg [DELAY_WIDTH - 1 : 0] wr_ptr;
    
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            wr_ptr <= {DELAY_WIDTH{1'b0}}; // 复位时，写指针清零
        end else if (adc_valid) begin
            wr_ptr <= wr_ptr + 1'b1;       // 数据有效时，每个时钟周期写指针加1 (溢出后自动绕回)
        end
    end

    // ========================================================================
    // 2. 通道处理流水线 (利用 generate 语句根据 CH_NUM 动态批量生成)
    // ========================================================================
    // 定义一个内部数组，用于暂时存放各个通道乘法器算出来的结果，等待进入求和树
    wire [(ADC_WIDTH + WEIGHT_WIDTH) - 1 : 0] mult_result_array [0 : CH_NUM - 1];
    
    genvar i; // generate 专属的循环变量
    generate
        for (i = 0; i < CH_NUM; i = i + 1) begin : CHANNEL_PROCESS_BLOCK
            
            // --- 从展平的宽总线中，精准切出属于当前通道(第i个)的数据 ---
            // 语法说明: [起始位置 +: 位宽] 是 Verilog-2001 的切片语法
            wire [ADC_WIDTH - 1 : 0]    ch_adc_data = adc_data_in [i * ADC_WIDTH +: ADC_WIDTH];
            wire [DELAY_WIDTH - 1 : 0]  ch_delay    = delay_in    [i * DELAY_WIDTH +: DELAY_WIDTH];
            wire [WEIGHT_WIDTH - 1 : 0] ch_weight   = weight_in   [i * WEIGHT_WIDTH +: WEIGHT_WIDTH];
            
            // --- 环形缓存 (综合器会自动将其推断为 FPGA 内部的 BRAM 硬核) ---
            reg [ADC_WIDTH - 1 : 0] ring_buffer [0 : BRAM_DEPTH - 1];
            
            // --- 读指针计算 (利用无符号减法的自然溢出，实现固定距离的回溯) ---
            wire [DELAY_WIDTH - 1 : 0] rd_ptr = wr_ptr - ch_delay;
            
            // --- 流水线寄存器 (用于锁定数据，满足时序要求) ---
            reg [ADC_WIDTH - 1 : 0] delayed_data;                      // 存放对齐后的历史数据
            reg [(ADC_WIDTH + WEIGHT_WIDTH) - 1 : 0] mult_data;        // 存放乘法结果
            
            // 同步的存储器读写与乘法操作
            always @(posedge clk) begin
                if (adc_valid) begin
                    // 步骤A: 将当前时刻的新数据写入环形缓存
                    ring_buffer[wr_ptr] <= ch_adc_data;
                    
                    // 步骤B: 抽出对齐后的历史数据
                    delayed_data <= ring_buffer[rd_ptr];
                    
                    // 步骤C: 变迹乘法 (综合器会自动推断并调用 DSP48E 乘法器硬核)
                    mult_data <= delayed_data * ch_weight;
                end
            end
            
            // 将当前通道的乘法结果，通过物理连线接入到全局的求和数组中
            assign mult_result_array[i] = mult_data;
            
        end
    endgenerate

    // ========================================================================
    // 3. 通道求和逻辑 (加法树 Summation Logic)
    // ========================================================================
    // 架构师注: 
    // 对于较少的通道数 (如 8 通道)，使用 behavioral 的 for 循环累加，综合器能够较好地处理。
    // 但如果要扩展到 64 或 128 通道，这种串行加法会导致极长的组合逻辑延迟，无法满足时序。
    // 届时必须将其替换为严格打拍的二叉树加法网络 (Pipelined Binary Adder Tree)。
    
    integer j;
    reg [(ADC_WIDTH + WEIGHT_WIDTH +  $clog2(CH_NUM)) - 1 : 0] sum_acc; // 累加寄存器
    
    always @(posedge clk) begin
        if (adc_valid) begin
            sum_acc = 0; // 每次计算前，累加器清零
            
            // 将 8 个通道的乘法结果累加
            for (j = 0; j < CH_NUM; j = j + 1) begin
                sum_acc = sum_acc + mult_result_array[j];
            end
            
            // 将累加结果打入最终的输出寄存器
            beamformed_out <= sum_acc;
            out_valid      <= 1'b1;
        end else begin
            out_valid      <= 1'b0;
        end
    end

endmodule