`timescale 1ns / 1ps

module PreProcess_Top (
    input  wire        clk,
    input  wire        rst_n,

    // 模拟来自 DAS_Core 的输入
    input  wire signed [23:0] das_data,   // 假设波束合成输出是 24 位
    input  wire               das_valid,

    // 最终发往 DMA 的 AXI-Stream 输出
    output wire [7:0] m_axis_tdata,
    output wire       m_axis_tvalid,
    output wire       m_axis_tlast
);

    // ==========================================
    // 1. 内部连线 (Wires) 声明
    // ==========================================
    wire [23:0] rect_data;
    wire        rect_valid;
    
    wire [7:0]  fir_data; // 注意：你在 IP 核里截断成了 8 位
    wire        fir_valid;

    // ==========================================
    // 2. 例化：绝对值整流模块
    // ==========================================
    RF_Rectifier #(
        .DATA_WIDTH(24)
    ) u_rectifier (
        .clk           (clk),
        .rst_n         (rst_n),
        .data_in       (das_data),
        .data_in_valid (das_valid),
        .data_out      (rect_data),
        .data_out_valid(rect_valid)
    );

    // ==========================================
    // 3. 例化：FIR 抽取滤波器 IP 核
    // ==========================================
    // 注意：名字必须和你生成的 IP 核名字完全一致 (如 fir_compiler_0)
    fir_compiler_0 u_fir_filter (
        .aclk               (clk),
        .s_axis_data_tvalid (rect_valid),
        .s_axis_data_tdata  (rect_data), 
        .m_axis_data_tvalid (fir_valid),
        .m_axis_data_tdata  (fir_data)
    );

    // ==========================================
    // 4. 例化：AXI-Stream 组包模块
    // ==========================================
    Data_Packer u_packer (
        .clk           (clk),
        .rst_n         (rst_n),
        .fir_data      (fir_data),
        .fir_valid     (fir_valid),
        .m_axis_tdata  (m_axis_tdata),
        .m_axis_tvalid (m_axis_tvalid),
        .m_axis_tlast  (m_axis_tlast),
        .m_axis_tready (1'b1) // 仿真时假设 DMA 永远准备好接收
    );

endmodule