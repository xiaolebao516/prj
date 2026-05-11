`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2026/04/15 14:49:34
// Design Name: 
// Module Name: AdcClock
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


//module AdcClock (
//    input  wire dco_p, dco_n, // 来自外部引脚
//    output wire clk_bit,     // 位时钟 (高速，连到 Serdes 的 CLK)
//    output wire clk_word     // 字时钟 (低速/7分频，连到 Serdes 的 CLKDIV)
//);
//    wire dco_in;

//    // 1. 差分转单端
//    IBUFGDS i_ibufgds_dco (.I(dco_p), .IB(dco_n), .O(dco_in));

//    // 2. 产生位时钟 (直接驱动局部 IO 时钟网络)
//    BUFIO i_bufio (.I(dco_in), .O(clk_bit));

//    // 3. 产生字时钟 (14位DDR模式下，进行 7 分频)
//    BUFR #(
//        .BUFR_DIVIDE("7"), 
//        .SIM_DEVICE("7SERIES")
//    ) i_bufr (
//        .I(dco_in), .O(clk_word), .CE(1'b1), .CLR(1'b0)
//    );
//endmodule

`timescale 1ns / 1ps

module AdcClock (
    input  wire dco_p, dco_n, 
    output wire clk_bit,       
    output wire clk_word       
);
    wire dco_out;
    wire dco_mr_out;

    // 1. 差分转单端
    IBUFGDS i_ibufgds_dco (.I(dco_p), .IB(dco_n), .O(dco_out));

    // 2. 【核心改动】添加 BUFMR，允许时钟跨 Bank 驱动数据
    BUFMR i_bufmr (.I(dco_out), .O(dco_mr_out));

    // 3. 用 BUFMR 的输出去驱动 BUFIO 和 BUFR
    BUFIO i_bufio (.I(dco_mr_out), .O(clk_bit));

    BUFR #(
        .BUFR_DIVIDE("7"), 
        .SIM_DEVICE("7SERIES")
    ) i_bufr (
        .I(dco_mr_out), .O(clk_word), .CE(1'b1), .CLR(1'b0)
    );
endmodule