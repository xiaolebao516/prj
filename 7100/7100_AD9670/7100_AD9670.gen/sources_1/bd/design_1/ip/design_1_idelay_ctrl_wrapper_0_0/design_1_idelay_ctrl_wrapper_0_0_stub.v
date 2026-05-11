// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2020.2 (win64) Build 3064766 Wed Nov 18 09:12:45 MST 2020
// Date        : Sun Apr 19 15:52:56 2026
// Host        : DESKTOP-MM8H4FT running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode synth_stub
//               c:/Study/7100_AD9670/7100_AD9670.gen/sources_1/bd/design_1/ip/design_1_idelay_ctrl_wrapper_0_0/design_1_idelay_ctrl_wrapper_0_0_stub.v
// Design      : design_1_idelay_ctrl_wrapper_0_0
// Purpose     : Stub declaration of top-level module interface
// Device      : xc7z100ffg900-2
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* X_CORE_INFO = "idelay_ctrl_wrapper,Vivado 2020.2" *)
module design_1_idelay_ctrl_wrapper_0_0(ref_clk, rst, ready)
/* synthesis syn_black_box black_box_pad_pin="ref_clk,rst,ready" */;
  input ref_clk;
  input rst;
  output ready;
endmodule
