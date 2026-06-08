`timescale 1ns / 1ps
// ============================================================================
// 模块名称:    DAS_Core_32CH
// 模块描述:    32通道全流水线延迟求和 (DAS) 波束合成核心模块
// 架构升级:    1. 引入 5 级打拍二叉树加法网络，解决高扇入时序瓶颈
//              2. 输出强制对齐至 32-bit (4字节) 标准总线
//              3. 严格的 Valid 信号随流水线同步打拍延迟
// ============================================================================

module DAS_Core_32CH #(
    // --- 系统级参数 ---
    parameter CH_NUM       = 32,     // 接收通道的总数量 (固定为 32)
    parameter ADC_WIDTH    = 12,     // ADC 输入数据的位宽
    parameter DELAY_WIDTH  = 10,     // 延迟指针的位宽
    parameter BRAM_DEPTH   = 1024,   // 内部环形缓存的物理深度
    parameter WEIGHT_WIDTH = 8       // 变迹权重位宽
)(
    input  wire clk,
    input  wire rst_n,
    input  wire adc_valid,           // 输入数据有效标志
    
    // 展平的 32 通道宽总线输入
    input  wire [CH_NUM * ADC_WIDTH - 1 : 0]    adc_data_in, 
    input  wire [CH_NUM * DELAY_WIDTH - 1 : 0]  delay_in,    
    input  wire [CH_NUM * WEIGHT_WIDTH - 1 : 0] weight_in,   
    
    // --- 升级：输出接口强制对齐 32-bit ---
    output reg  [31:0] beamformed_out, 
    output reg         out_valid       
);

    // ========================================================================
    // 1. 全局写指针逻辑 (所有通道共享一个时间基准)
    // ========================================================================
    reg [DELAY_WIDTH - 1 : 0] wr_ptr;
    
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            wr_ptr <= {DELAY_WIDTH{1'b0}};
        end else if (adc_valid) begin
            wr_ptr <= wr_ptr + 1'b1;
        end
    end

    // ========================================================================
    // 2. 32通道独立处理流水线 (延迟提取 + 变迹乘法)
    // ========================================================================
    // 乘法结果位宽 = 12 + 8 = 20-bit
    localparam MULT_WIDTH = ADC_WIDTH + WEIGHT_WIDTH;
    wire [MULT_WIDTH - 1 : 0] mult_result_array [0 : CH_NUM - 1];
    
    genvar i;
    generate
        for (i = 0; i < CH_NUM; i = i + 1) begin : CHANNEL_PROCESS_BLOCK
            
            wire [ADC_WIDTH - 1 : 0]    ch_adc_data = adc_data_in [i * ADC_WIDTH +: ADC_WIDTH];
            wire [DELAY_WIDTH - 1 : 0]  ch_delay    = delay_in    [i * DELAY_WIDTH +: DELAY_WIDTH];
            wire [WEIGHT_WIDTH - 1 : 0] ch_weight   = weight_in   [i * WEIGHT_WIDTH +: WEIGHT_WIDTH];
            
            // 内部延时环形缓存 (推断为 BRAM)
            reg [ADC_WIDTH - 1 : 0] ring_buffer [0 : BRAM_DEPTH - 1];
            wire [DELAY_WIDTH - 1 : 0] rd_ptr = wr_ptr - ch_delay;
            
            reg [ADC_WIDTH - 1 : 0] delayed_data;
            reg [MULT_WIDTH - 1 : 0] mult_data;
            
            always @(posedge clk) begin
                if (adc_valid) begin
                    ring_buffer[wr_ptr] <= ch_adc_data;
                    delayed_data        <= ring_buffer[rd_ptr];
                    // 乘法器消耗 1 个时钟周期
                    mult_data           <= delayed_data * ch_weight;
                end
            end
            
            assign mult_result_array[i] = mult_data;
        end
    endgenerate

    // ========================================================================
    // 3. 5级流水线二叉加法树 (Pipelined Binary Adder Tree)
    // 解决 32 个数字同时相加导致的极长组合逻辑路径问题
    // ========================================================================
    
    // 每一级加法，位宽自然增加 1 位防止溢出
    reg [MULT_WIDTH : 0]     sum_stg1 [0 : 15]; // 32 变 16 (21-bit)
    reg [MULT_WIDTH + 1 : 0] sum_stg2 [0 : 7];  // 16 变 8  (22-bit)
    reg [MULT_WIDTH + 2 : 0] sum_stg3 [0 : 3];  // 8  变 4  (23-bit)
    reg [MULT_WIDTH + 3 : 0] sum_stg4 [0 : 1];  // 4  变 2  (24-bit)
    reg [MULT_WIDTH + 4 : 0] sum_stg5;          // 2  变 1  (25-bit)

    integer j;
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            for (j = 0; j < 16; j = j + 1) sum_stg1[j] <= 0;
            for (j = 0; j < 8;  j = j + 1) sum_stg2[j] <= 0;
            for (j = 0; j < 4;  j = j + 1) sum_stg3[j] <= 0;
            for (j = 0; j < 2;  j = j + 1) sum_stg4[j] <= 0;
            sum_stg5 <= 0;
        end else begin
            // Stage 1: 16 个加法器并行
            for (j = 0; j < 16; j = j + 1)
                sum_stg1[j] <= mult_result_array[2*j] + mult_result_array[2*j+1];
                
            // Stage 2: 8 个加法器并行
            for (j = 0; j < 8; j = j + 1)
                sum_stg2[j] <= sum_stg1[2*j] + sum_stg1[2*j+1];
                
            // Stage 3: 4 个加法器并行
            for (j = 0; j < 4; j = j + 1)
                sum_stg3[j] <= sum_stg2[2*j] + sum_stg2[2*j+1];
                
            // Stage 4: 2 个加法器并行
            for (j = 0; j < 2; j = j + 1)
                sum_stg4[j] <= sum_stg3[2*j] + sum_stg3[2*j+1];
                
            // Stage 5: 最终求和
            sum_stg5 <= sum_stg4[0] + sum_stg4[1];
        end
    end

    // ========================================================================
    // 4. 数据对齐与 Valid 信号延迟同步
    // ========================================================================
    // 从输入到乘法器需要 1 拍，加法树需要 5 拍，总共需要延迟 6 拍的 Valid 信号
    reg [5:0] valid_pipe;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            valid_pipe     <= 6'b0;
            beamformed_out <= 32'd0;
            out_valid      <= 1'b0;
        end else begin
            // Valid 信号移位寄存器
            valid_pipe <= {valid_pipe[4:0], adc_valid};
            out_valid  <= valid_pipe[5];
            
            // 将 25-bit 的最终结果填充至 32-bit 标准总线
            // 射频回波是有符号数，使用符号位扩展 (Sign Extension)
            if (valid_pipe[5]) begin
                beamformed_out <= { {7{sum_stg5[24]}}, sum_stg5 }; 
            end else begin
                beamformed_out <= 32'd0;
            end
        end
    end

endmodule