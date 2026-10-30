`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2026/04/19 15:35:52
// Design Name: 
// Module Name: idelay_ctrl_wrapper
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


module idelay_ctrl_wrapper (
    input ref_clk,   // 接 200MHz
    input rst,       // 接复位
    output ready     // 悬空即可
);
    // 实例化 7 系列原语
    IDELAYCTRL #(
        .SIM_DEVICE("7SERIES")
    ) IDELAYCTRL_inst (
        .RDY(ready),
        .REFCLK(ref_clk),
        .RST(rst)
    );
endmodule
