`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2026/04/12 15:32:27
// Design Name: 
// Module Name: ad9670_lvds_rx_lane
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


// 核心逻辑骨架：ad9670_lvds_rx_lane.v
module ad9670_lvds_rx_lane (
    input  wire       lvds_data_p,  // 外部引脚 P
    input  wire       lvds_data_n,  // 外部引脚 N
    input  wire       clk_in,       // 高速位时钟 (来自 BUFIO)
    input  wire       clk_div_in,   // 字时钟 (来自 BUFR)
    input  wire       reset,
    input  wire       bitslip,      // 对齐控制信号
    output wire [13:0] data_out     // 14位并行输出
);

    wire data_serial;
    wire [7:0] q_master, q_slave;

    // 1. 差分转单端
    IBUFDS #(
        .DIFF_TERM("TRUE"),
        .IOSTANDARD("LVDS")
    ) ibufds_inst (
        .I(lvds_data_p),
        .IB(lvds_data_n),
        .O(data_serial)
    );

    // 2. 主 ISERDES (处理高位比特)
    ISERDESE2 #(
        .DATA_RATE      ("DDR"),
        .DATA_WIDTH     (7), // 14位模式下，主从各负责7位
        .SERDES_MODE    ("MASTER"),
        .INTERFACE_TYPE ("NETWORKING")
    ) iserdes_m (
        .D       (data_serial),
        .CLK     (clk_in),
        .CLKDIV  (clk_div_in),
        .BITSLIP (bitslip),
        .RST     (reset),
        .Q1(data_out[0]), .Q2(data_out[1]), .Q3(data_out[2]), .Q4(data_out[3]),
        .Q5(data_out[4]), .Q6(data_out[5]), .Q7(data_out[6]),
        .SHIFTOUT(cascade_wire) // 级联信号
    );

    // 3. 从 ISERDES (处理低位比特)
    ISERDESE2 #(
        .DATA_RATE      ("DDR"),
        .DATA_WIDTH     (7),
        .SERDES_MODE    ("SLAVE"),
        .INTERFACE_TYPE ("NETWORKING")
    ) iserdes_s (
        .D       (1'b0), // 级联模式下 D 输入固定
        .CLK     (clk_in),
        .CLKDIV  (clk_div_in),
        .BITSLIP (bitslip),
        .RST     (reset),
        .SHIFTIN (cascade_wire), // 接收来自 Master 的级联
        .Q1(data_out[7]), .Q2(data_out[8]), .Q3(data_out[9]), .Q4(data_out[10]),
        .Q5(data_out[11]), .Q6(data_out[12]), .Q7(data_out[13])
    );

endmodule
