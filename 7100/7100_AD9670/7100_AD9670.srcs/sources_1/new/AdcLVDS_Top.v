`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2026/04/15 14:53:16
// Design Name: 
// Module Name: AdcLVDS_Top
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module AdcLVDS_Top (
    // 1. 物理引脚接口 (对应 AD9670 的 FMC 引脚)
    input  wire        lvds_dco_p, lvds_dco_n, // 数据时钟
    input  wire        lvds_fco_p, lvds_fco_n, // 帧时钟
    input  wire [7:0]  lvds_dat_p, lvds_dat_n, // 8路数据
    input  wire        sys_rst,                // 全局复位

    // 2. AXI-Stream 接口 (用于连接 DMA)
    input  wire         m_axis_aclk,    // 连到 Zynq 的 FCLK0
    output wire         m_axis_tvalid,
    output wire [127:0] m_axis_tdata,   // 8通道 * 16位
    output wire         m_axis_tlast
);

    wire clk_f, clk_s;
    wire bitslip_cmd;
    wire align_done;
    wire [15:0] adc_data_lanes [7:0];

    // --- 1. 时钟处理模块 ---
    AdcClock u_clock (
        .dco_p(lvds_dco_p), .dco_n(lvds_dco_n),
        .clk_bit(clk_f), .clk_word(clk_s)
    );

    // --- 2. 帧对齐控制模块 (大脑) ---
    // 通过 FCO 信号判断是否对齐，不对齐则发出 bitslip
    AdcFrame #( .AdcBits(14) ) u_frame (
        .FrmFCLK_p(lvds_fco_p), .FrmFCLK_n(lvds_fco_n),
        .FrmClk(clk_f), .FrmClkb(~clk_f), .FrmClkDiv(clk_s),
        .FrmRst(sys_rst), .BitClkDone(1'b1),
        .FrmBitslip(bitslip_cmd), .FrmAlignDone(align_done)
    );

    // --- 3. 实例化 8 个数据通道 ---
    genvar i;
    generate
        for (i=0; i<8; i=i+1) begin: lane_gen
            AdcLane #( .AdcBits(14) ) u_lane (
                .DatLine_p(lvds_dat_p[i]), .DatLine_n(lvds_dat_n[i]),
                .DatClk(clk_f), .DatClkb(~clk_f), .DatClkDiv(clk_s),
                .DatRst(sys_rst), .DatBitslip(bitslip_cmd), // 共用对齐信号
                .FrmAlignDone(align_done),
                .DatData(adc_data_lanes[i])
            );
        end
    endgenerate

    // --- 4. 数据拼接成 128位 AXI-Stream ---
    // 将 8 个 16 位数据拼接，连到 DMA
    assign m_axis_tdata = {
        adc_data_lanes[7], adc_data_lanes[6], adc_data_lanes[7], adc_data_lanes[4],
        adc_data_lanes[3], adc_data_lanes[2], adc_data_lanes[1], adc_data_lanes[0]
    };
    
    assign m_axis_tvalid = align_done; // 只有对齐好了，数据才有效
    assign m_axis_tlast  = 1'b0;      // 简单模式下填0

endmodule
