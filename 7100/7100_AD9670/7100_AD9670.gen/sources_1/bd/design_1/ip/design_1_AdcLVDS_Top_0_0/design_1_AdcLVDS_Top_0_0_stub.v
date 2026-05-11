// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2020.2 (win64) Build 3064766 Wed Nov 18 09:12:45 MST 2020
// Date        : Sun Apr 19 15:52:58 2026
// Host        : DESKTOP-MM8H4FT running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode synth_stub
//               c:/Study/7100_AD9670/7100_AD9670.gen/sources_1/bd/design_1/ip/design_1_AdcLVDS_Top_0_0/design_1_AdcLVDS_Top_0_0_stub.v
// Design      : design_1_AdcLVDS_Top_0_0
// Purpose     : Stub declaration of top-level module interface
// Device      : xc7z100ffg900-2
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* X_CORE_INFO = "AdcLVDS_Top,Vivado 2020.2" *)
module design_1_AdcLVDS_Top_0_0(lvds_dco_p, lvds_dco_n, lvds_fco_p, lvds_fco_n, 
  lvds_dat_p, lvds_dat_n, sys_rst, m_axis_aclk, m_axis_tvalid, m_axis_tdata, m_axis_tlast)
/* synthesis syn_black_box black_box_pad_pin="lvds_dco_p,lvds_dco_n,lvds_fco_p,lvds_fco_n,lvds_dat_p[7:0],lvds_dat_n[7:0],sys_rst,m_axis_aclk,m_axis_tvalid,m_axis_tdata[127:0],m_axis_tlast" */;
  input lvds_dco_p;
  input lvds_dco_n;
  input lvds_fco_p;
  input lvds_fco_n;
  input [7:0]lvds_dat_p;
  input [7:0]lvds_dat_n;
  input sys_rst;
  input m_axis_aclk;
  output m_axis_tvalid;
  output [127:0]m_axis_tdata;
  output m_axis_tlast;
endmodule
