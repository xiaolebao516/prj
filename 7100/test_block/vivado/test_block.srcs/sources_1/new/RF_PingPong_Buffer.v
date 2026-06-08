`timescale 1ns / 1ps

module RF_PingPong_Buffer_32CH #(
    parameter MAX_DEPTH  = 4096,   // 单线最大物理深度
    parameter TOTAL_CH   = 32,     // 探头总阵元数
    parameter CH_PER_ADC = 8,      // 每次 ADC(LVDS) 传进来的通道数
    parameter DATA_WIDTH = 16,     // 单点数据位宽 (对齐到 16-bit)
    parameter ADDR_WIDTH = 12      // 深度地址位宽 (log2(4096) = 12)
)(
    input  wire clk,
    input  wire rst_n,

    // 接口 1：来自 MUX 控制器和 LVDS 的【写操作】
    input  wire [CH_PER_ADC*DATA_WIDTH-1:0] adc_data_in,     // 128-bit (8通道输入)
    input  wire                             adc_acq_en,      // 全局写使能
    input  wire [1:0]                       bram_write_bank, // 0~3：指示当前分时复用是哪一批
    input  wire                             frame_ready,     // 一帧采完，触发乒乓翻转

    // 接口 2：面向后端 DAS 模块的【读操作】(核心大改动)
    // 注意：DAS 现在要给出 32 个独立的深度地址，所以总线宽 32 * 12 = 384 bit
    input  wire [TOTAL_CH*ADDR_WIDTH-1:0]   das_read_addr_bus, 
    output wire [TOTAL_CH*DATA_WIDTH-1:0]   das_data_out       // 拼好的 512-bit (32路并行)
);

    // 1. 乒乓状态翻转逻辑与写地址生成器 (与之前保持一致)
    reg ping_pong_flag; 
    reg [ADDR_WIDTH-1:0] write_depth_cnt;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            ping_pong_flag <= 1'b0;
            write_depth_cnt <= 0;
        end else begin
            // 翻转逻辑
            if (frame_ready) begin
                ping_pong_flag <= ~ping_pong_flag; 
            end
            
            // 深度计数逻辑
            if (adc_acq_en) begin
                if (write_depth_cnt == MAX_DEPTH - 1)
                    write_depth_cnt <= 0;
                else
                    write_depth_cnt <= write_depth_cnt + 1'b1;
            end else begin
                write_depth_cnt <= 0;
            end
        end
    end

    // 2. 批量生成 32 块独立窄 BRAM 
    genvar i;
    generate
        for (i = 0; i < TOTAL_CH; i = i + 1) begin : BRAM_ARRAY
            
            // A. 精确切片：把输入的 8 通道拆分给特定的 1 个通道
            // 计算当前通道属于哪一批 (例如: 17号通道属于第 2 批，即 target_bank = 2)
            wire [1:0] target_bank = i / CH_PER_ADC;
            
            // 计算当前通道在这一批 8 个数据中的偏移量 (例如: 17号通道是第 2 批里的第 1 个)
            wire [2:0] local_idx   = i % CH_PER_ADC;

            // 只有全局使能拉高，并且当前的批次匹配时，这个 BRAM 才允许被写入
            wire we = adc_acq_en & (bram_write_bank == target_bank);
            
            // 从 128-bit 宽总线上，精准咬下一块 16-bit 喂给这个 BRAM
            wire [DATA_WIDTH-1:0] din = adc_data_in[local_idx * DATA_WIDTH +: DATA_WIDTH];
            
            // 拼接最终写地址：{乒乓最高位, 12位深度} -> 13位地址
            wire [ADDR_WIDTH:0] wr_addr = {ping_pong_flag, write_depth_cnt};

            // B. 独立读取：从 384-bit 的地址池里，拿出属于自己的那个深度要求
            wire [ADDR_WIDTH-1:0] rd_depth = das_read_addr_bus[i * ADDR_WIDTH +: ADDR_WIDTH];
            wire [ADDR_WIDTH:0]   rd_addr  = {~ping_pong_flag, rd_depth};

            // C. 真双口 BRAM 推断 (Vivado 会自动将其映射为物理 RAMB36E1)
            // 声明一块深度为 8192 (4096*2)，宽度为 16-bit 的内存
            reg [DATA_WIDTH-1:0] ram [0 : (2*MAX_DEPTH)-1];
            reg [DATA_WIDTH-1:0] dout_reg;

            always @(posedge clk) begin
                // 端口 A：只写不读
                if (we) begin
                    ram[wr_addr] <= din;
                end
                // 端口 B：只读不写
                dout_reg <= ram[rd_addr];
            end

            // ------------------------------------------
            // D. 汇总输出：把读取结果并入 512-bit 的大总线
            // ------------------------------------------
            assign das_data_out[i * DATA_WIDTH +: DATA_WIDTH] = dout_reg;
            
        end
    endgenerate

endmodule