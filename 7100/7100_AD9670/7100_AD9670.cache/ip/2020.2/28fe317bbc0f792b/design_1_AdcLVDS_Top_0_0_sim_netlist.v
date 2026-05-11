// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2020.2 (win64) Build 3064766 Wed Nov 18 09:12:45 MST 2020
// Date        : Sun Apr 19 15:52:58 2026
// Host        : DESKTOP-MM8H4FT running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode funcsim -rename_top decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix -prefix
//               decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_ design_1_AdcLVDS_Top_0_0_sim_netlist.v
// Design      : design_1_AdcLVDS_Top_0_0
// Purpose     : This verilog netlist is a functional simulation representation of the design and should not be modified
//               or synthesized. This netlist cannot be used for SDF annotated simulation.
// Device      : xc7z100ffg900-2
// --------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcClock
   (clk_bit,
    clk_word,
    lvds_dco_p,
    lvds_dco_n);
  output clk_bit;
  output clk_word;
  input lvds_dco_p;
  input lvds_dco_n;

  wire clk_bit;
  wire clk_word;
  wire dco_mr_out;
  wire dco_out;
  wire lvds_dco_n;
  wire lvds_dco_p;

  (* BOX_TYPE = "PRIMITIVE" *) 
  BUFIO i_bufio
       (.I(dco_mr_out),
        .O(clk_bit));
  (* BOX_TYPE = "PRIMITIVE" *) 
  BUFMR i_bufmr
       (.I(dco_out),
        .O(dco_mr_out));
  (* BOX_TYPE = "PRIMITIVE" *) 
  BUFR #(
    .BUFR_DIVIDE("7"),
    .SIM_DEVICE("7SERIES")) 
    i_bufr
       (.CE(1'b1),
        .CLR(1'b0),
        .I(dco_mr_out),
        .O(clk_word));
  (* BOX_TYPE = "PRIMITIVE" *) 
  (* CAPACITANCE = "DONT_CARE" *) 
  (* IBUF_DELAY_VALUE = "0" *) 
  (* XILINX_LEGACY_PRIM = "IBUFGDS" *) 
  IBUFDS #(
    .IOSTANDARD("DEFAULT")) 
    i_ibufgds_dco
       (.I(lvds_dco_p),
        .IB(lvds_dco_n),
        .O(dco_out));
endmodule

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcFrame
   (bitslip_cmd,
    m_axis_tvalid,
    lvds_fco_p,
    lvds_fco_n,
    clk_bit,
    clkb,
    clk_word,
    sys_rst);
  output bitslip_cmd;
  output m_axis_tvalid;
  input lvds_fco_p;
  input lvds_fco_n;
  input clk_bit;
  input clkb;
  input clk_word;
  input sys_rst;

  wire bitslip_cmd;
  wire clk_bit;
  wire clk_word;
  wire clkb;
  wire inst_Serdes_1x14_DDR_Data_Line_n_0;
  wire inst_Serdes_1x14_DDR_Data_Line_n_1;
  wire lvds_fco_n;
  wire lvds_fco_p;
  wire m_axis_tvalid;
  wire [4:0]p_1_in;
  wire \rBitslipCnt_reg_n_0_[0] ;
  wire \rBitslipCnt_reg_n_0_[1] ;
  wire \rBitslipCnt_reg_n_0_[2] ;
  wire \rBitslipCnt_reg_n_0_[3] ;
  wire \rBitslipCnt_reg_n_0_[4] ;
  wire rFrmAlignDone;
  wire sys_rst;

  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR inst_Serdes_1x14_DDR_Data_Line
       (.bitslip_cmd(bitslip_cmd),
        .clk_bit(clk_bit),
        .clk_word(clk_word),
        .clkb(clkb),
        .lvds_fco_n(lvds_fco_n),
        .lvds_fco_p(lvds_fco_p),
        .m_axis_tvalid(m_axis_tvalid),
        .rFrmAlignDone(rFrmAlignDone),
        .rFrmAlignDone_reg(inst_Serdes_1x14_DDR_Data_Line_n_0),
        .sys_rst(sys_rst),
        .sys_rst_0(inst_Serdes_1x14_DDR_Data_Line_n_1));
  (* SOFT_HLUTNM = "soft_lutpair2" *) 
  LUT1 #(
    .INIT(2'h1)) 
    \rBitslipCnt[0]_i_1 
       (.I0(\rBitslipCnt_reg_n_0_[0] ),
        .O(p_1_in[0]));
  (* SOFT_HLUTNM = "soft_lutpair2" *) 
  LUT2 #(
    .INIT(4'h6)) 
    \rBitslipCnt[1]_i_1 
       (.I0(\rBitslipCnt_reg_n_0_[0] ),
        .I1(\rBitslipCnt_reg_n_0_[1] ),
        .O(p_1_in[1]));
  (* SOFT_HLUTNM = "soft_lutpair1" *) 
  LUT3 #(
    .INIT(8'h78)) 
    \rBitslipCnt[2]_i_1 
       (.I0(\rBitslipCnt_reg_n_0_[1] ),
        .I1(\rBitslipCnt_reg_n_0_[0] ),
        .I2(\rBitslipCnt_reg_n_0_[2] ),
        .O(p_1_in[2]));
  (* SOFT_HLUTNM = "soft_lutpair1" *) 
  LUT4 #(
    .INIT(16'h7F80)) 
    \rBitslipCnt[3]_i_1 
       (.I0(\rBitslipCnt_reg_n_0_[2] ),
        .I1(\rBitslipCnt_reg_n_0_[0] ),
        .I2(\rBitslipCnt_reg_n_0_[1] ),
        .I3(\rBitslipCnt_reg_n_0_[3] ),
        .O(p_1_in[3]));
  (* SOFT_HLUTNM = "soft_lutpair0" *) 
  LUT5 #(
    .INIT(32'h7FFF8000)) 
    \rBitslipCnt[4]_i_1 
       (.I0(\rBitslipCnt_reg_n_0_[3] ),
        .I1(\rBitslipCnt_reg_n_0_[1] ),
        .I2(\rBitslipCnt_reg_n_0_[0] ),
        .I3(\rBitslipCnt_reg_n_0_[2] ),
        .I4(\rBitslipCnt_reg_n_0_[4] ),
        .O(p_1_in[4]));
  FDRE \rBitslipCnt_reg[0] 
       (.C(clk_word),
        .CE(1'b1),
        .D(p_1_in[0]),
        .Q(\rBitslipCnt_reg_n_0_[0] ),
        .R(sys_rst));
  FDRE \rBitslipCnt_reg[1] 
       (.C(clk_word),
        .CE(1'b1),
        .D(p_1_in[1]),
        .Q(\rBitslipCnt_reg_n_0_[1] ),
        .R(sys_rst));
  FDRE \rBitslipCnt_reg[2] 
       (.C(clk_word),
        .CE(1'b1),
        .D(p_1_in[2]),
        .Q(\rBitslipCnt_reg_n_0_[2] ),
        .R(sys_rst));
  FDRE \rBitslipCnt_reg[3] 
       (.C(clk_word),
        .CE(1'b1),
        .D(p_1_in[3]),
        .Q(\rBitslipCnt_reg_n_0_[3] ),
        .R(sys_rst));
  FDRE \rBitslipCnt_reg[4] 
       (.C(clk_word),
        .CE(1'b1),
        .D(p_1_in[4]),
        .Q(\rBitslipCnt_reg_n_0_[4] ),
        .R(sys_rst));
  (* SOFT_HLUTNM = "soft_lutpair0" *) 
  LUT5 #(
    .INIT(32'h00000001)) 
    rFrmAlignDone_i_5
       (.I0(\rBitslipCnt_reg_n_0_[1] ),
        .I1(\rBitslipCnt_reg_n_0_[0] ),
        .I2(\rBitslipCnt_reg_n_0_[2] ),
        .I3(\rBitslipCnt_reg_n_0_[3] ),
        .I4(\rBitslipCnt_reg_n_0_[4] ),
        .O(rFrmAlignDone));
  FDRE rFrmAlignDone_reg
       (.C(clk_word),
        .CE(1'b1),
        .D(inst_Serdes_1x14_DDR_Data_Line_n_0),
        .Q(m_axis_tvalid),
        .R(sys_rst));
  FDRE rFrmBitslip_reg
       (.C(clk_word),
        .CE(1'b1),
        .D(inst_Serdes_1x14_DDR_Data_Line_n_1),
        .Q(bitslip_cmd),
        .R(1'b0));
endmodule

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLVDS_Top
   (lvds_dco_p,
    lvds_dco_n,
    lvds_fco_p,
    lvds_fco_n,
    lvds_dat_p,
    lvds_dat_n,
    sys_rst,
    m_axis_aclk,
    m_axis_tvalid,
    m_axis_tdata,
    m_axis_tlast);
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

  wire \<const0> ;
  wire bitslip_cmd;
  wire clk_f;
  wire clk_s;
  wire [7:0]lvds_dat_n;
  wire [7:0]lvds_dat_p;
  wire lvds_dco_n;
  wire lvds_dco_p;
  wire lvds_fco_n;
  wire lvds_fco_p;
  wire [109:0]\^m_axis_tdata ;
  wire m_axis_tvalid;
  wire sys_rst;

  assign m_axis_tdata[127] = \^m_axis_tdata [93];
  assign m_axis_tdata[126] = \^m_axis_tdata [93];
  assign m_axis_tdata[125:112] = \^m_axis_tdata [93:80];
  assign m_axis_tdata[111] = \^m_axis_tdata [109];
  assign m_axis_tdata[110] = \^m_axis_tdata [109];
  assign m_axis_tdata[109:96] = \^m_axis_tdata [109:96];
  assign m_axis_tdata[95] = \^m_axis_tdata [93];
  assign m_axis_tdata[94] = \^m_axis_tdata [93];
  assign m_axis_tdata[93:80] = \^m_axis_tdata [93:80];
  assign m_axis_tdata[79] = \^m_axis_tdata [77];
  assign m_axis_tdata[78] = \^m_axis_tdata [77];
  assign m_axis_tdata[77:64] = \^m_axis_tdata [77:64];
  assign m_axis_tdata[63] = \^m_axis_tdata [61];
  assign m_axis_tdata[62] = \^m_axis_tdata [61];
  assign m_axis_tdata[61:48] = \^m_axis_tdata [61:48];
  assign m_axis_tdata[47] = \^m_axis_tdata [45];
  assign m_axis_tdata[46] = \^m_axis_tdata [45];
  assign m_axis_tdata[45:32] = \^m_axis_tdata [45:32];
  assign m_axis_tdata[31] = \^m_axis_tdata [29];
  assign m_axis_tdata[30] = \^m_axis_tdata [29];
  assign m_axis_tdata[29:16] = \^m_axis_tdata [29:16];
  assign m_axis_tdata[15] = \^m_axis_tdata [13];
  assign m_axis_tdata[14] = \^m_axis_tdata [13];
  assign m_axis_tdata[13:0] = \^m_axis_tdata [13:0];
  assign m_axis_tlast = \<const0> ;
  GND GND
       (.G(\<const0> ));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane \lane_gen[0].u_lane 
       (.bitslip_cmd(bitslip_cmd),
        .clk_bit(clk_f),
        .clk_word(clk_s),
        .clkb(clk_f),
        .lvds_dat_n(lvds_dat_n[0]),
        .lvds_dat_p(lvds_dat_p[0]),
        .m_axis_tdata(\^m_axis_tdata [13:0]),
        .sys_rst(sys_rst));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_0 \lane_gen[1].u_lane 
       (.bitslip_cmd(bitslip_cmd),
        .clk_bit(clk_f),
        .clk_word(clk_s),
        .clkb(clk_f),
        .lvds_dat_n(lvds_dat_n[1]),
        .lvds_dat_p(lvds_dat_p[1]),
        .m_axis_tdata(\^m_axis_tdata [29:16]),
        .sys_rst(sys_rst));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_1 \lane_gen[2].u_lane 
       (.bitslip_cmd(bitslip_cmd),
        .clk_bit(clk_f),
        .clk_word(clk_s),
        .clkb(clk_f),
        .lvds_dat_n(lvds_dat_n[2]),
        .lvds_dat_p(lvds_dat_p[2]),
        .m_axis_tdata(\^m_axis_tdata [45:32]),
        .sys_rst(sys_rst));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_2 \lane_gen[3].u_lane 
       (.bitslip_cmd(bitslip_cmd),
        .clk_bit(clk_f),
        .clk_word(clk_s),
        .clkb(clk_f),
        .lvds_dat_n(lvds_dat_n[3]),
        .lvds_dat_p(lvds_dat_p[3]),
        .m_axis_tdata(\^m_axis_tdata [61:48]),
        .sys_rst(sys_rst));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_3 \lane_gen[4].u_lane 
       (.bitslip_cmd(bitslip_cmd),
        .clk_bit(clk_f),
        .clk_word(clk_s),
        .clkb(clk_f),
        .lvds_dat_n(lvds_dat_n[4]),
        .lvds_dat_p(lvds_dat_p[4]),
        .m_axis_tdata(\^m_axis_tdata [77:64]),
        .sys_rst(sys_rst));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_4 \lane_gen[5].u_lane 
       (.bitslip_cmd(bitslip_cmd),
        .clk_bit(clk_f),
        .clk_word(clk_s),
        .clkb(clk_f),
        .lvds_dat_n(lvds_dat_n[5]),
        .lvds_dat_p(lvds_dat_p[5]),
        .sys_rst(sys_rst));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_5 \lane_gen[6].u_lane 
       (.bitslip_cmd(bitslip_cmd),
        .clk_bit(clk_f),
        .clk_word(clk_s),
        .clkb(clk_f),
        .lvds_dat_n(lvds_dat_n[6]),
        .lvds_dat_p(lvds_dat_p[6]),
        .m_axis_tdata(\^m_axis_tdata [109:96]),
        .sys_rst(sys_rst));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_6 \lane_gen[7].u_lane 
       (.bitslip_cmd(bitslip_cmd),
        .clk_bit(clk_f),
        .clk_word(clk_s),
        .clkb(clk_f),
        .lvds_dat_n(lvds_dat_n[7]),
        .lvds_dat_p(lvds_dat_p[7]),
        .m_axis_tdata(\^m_axis_tdata [93:80]),
        .sys_rst(sys_rst));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcClock u_clock
       (.clk_bit(clk_f),
        .clk_word(clk_s),
        .lvds_dco_n(lvds_dco_n),
        .lvds_dco_p(lvds_dco_p));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcFrame u_frame
       (.bitslip_cmd(bitslip_cmd),
        .clk_bit(clk_f),
        .clk_word(clk_s),
        .clkb(clk_f),
        .lvds_fco_n(lvds_fco_n),
        .lvds_fco_p(lvds_fco_p),
        .m_axis_tvalid(m_axis_tvalid),
        .sys_rst(sys_rst));
endmodule

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane
   (m_axis_tdata,
    lvds_dat_p,
    lvds_dat_n,
    bitslip_cmd,
    clk_bit,
    clkb,
    clk_word,
    sys_rst);
  output [13:0]m_axis_tdata;
  input [0:0]lvds_dat_p;
  input [0:0]lvds_dat_n;
  input bitslip_cmd;
  input clk_bit;
  input clkb;
  input clk_word;
  input sys_rst;

  wire [13:0]ADCDataLine;
  wire bitslip_cmd;
  wire clk_bit;
  wire clk_word;
  wire clkb;
  wire [0:0]lvds_dat_n;
  wire [0:0]lvds_dat_p;
  wire [13:0]m_axis_tdata;
  wire sys_rst;

  FDRE \ADCData_reg[0] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[0]),
        .Q(m_axis_tdata[0]),
        .R(1'b0));
  FDRE \ADCData_reg[10] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[10]),
        .Q(m_axis_tdata[10]),
        .R(1'b0));
  FDRE \ADCData_reg[11] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[11]),
        .Q(m_axis_tdata[11]),
        .R(1'b0));
  FDRE \ADCData_reg[12] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[12]),
        .Q(m_axis_tdata[12]),
        .R(1'b0));
  FDRE \ADCData_reg[13] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[13]),
        .Q(m_axis_tdata[13]),
        .R(1'b0));
  FDRE \ADCData_reg[1] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[1]),
        .Q(m_axis_tdata[1]),
        .R(1'b0));
  FDRE \ADCData_reg[2] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[2]),
        .Q(m_axis_tdata[2]),
        .R(1'b0));
  FDRE \ADCData_reg[3] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[3]),
        .Q(m_axis_tdata[3]),
        .R(1'b0));
  FDRE \ADCData_reg[4] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[4]),
        .Q(m_axis_tdata[4]),
        .R(1'b0));
  FDRE \ADCData_reg[5] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[5]),
        .Q(m_axis_tdata[5]),
        .R(1'b0));
  FDRE \ADCData_reg[6] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[6]),
        .Q(m_axis_tdata[6]),
        .R(1'b0));
  FDRE \ADCData_reg[7] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[7]),
        .Q(m_axis_tdata[7]),
        .R(1'b0));
  FDRE \ADCData_reg[8] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[8]),
        .Q(m_axis_tdata[8]),
        .R(1'b0));
  FDRE \ADCData_reg[9] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[9]),
        .Q(m_axis_tdata[9]),
        .R(1'b0));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_14 inst_Serdes_1x14_DDR_Data_Line
       (.D(ADCDataLine),
        .bitslip_cmd(bitslip_cmd),
        .clk_bit(clk_bit),
        .clk_word(clk_word),
        .clkb(clkb),
        .lvds_dat_n(lvds_dat_n),
        .lvds_dat_p(lvds_dat_p),
        .sys_rst(sys_rst));
endmodule

(* ORIG_REF_NAME = "AdcLane" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_0
   (m_axis_tdata,
    lvds_dat_p,
    lvds_dat_n,
    bitslip_cmd,
    clk_bit,
    clkb,
    clk_word,
    sys_rst);
  output [13:0]m_axis_tdata;
  input [0:0]lvds_dat_p;
  input [0:0]lvds_dat_n;
  input bitslip_cmd;
  input clk_bit;
  input clkb;
  input clk_word;
  input sys_rst;

  wire [13:0]ADCDataLine;
  wire bitslip_cmd;
  wire clk_bit;
  wire clk_word;
  wire clkb;
  wire [0:0]lvds_dat_n;
  wire [0:0]lvds_dat_p;
  wire [13:0]m_axis_tdata;
  wire sys_rst;

  FDRE \ADCData_reg[0] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[0]),
        .Q(m_axis_tdata[0]),
        .R(1'b0));
  FDRE \ADCData_reg[10] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[10]),
        .Q(m_axis_tdata[10]),
        .R(1'b0));
  FDRE \ADCData_reg[11] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[11]),
        .Q(m_axis_tdata[11]),
        .R(1'b0));
  FDRE \ADCData_reg[12] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[12]),
        .Q(m_axis_tdata[12]),
        .R(1'b0));
  FDRE \ADCData_reg[13] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[13]),
        .Q(m_axis_tdata[13]),
        .R(1'b0));
  FDRE \ADCData_reg[1] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[1]),
        .Q(m_axis_tdata[1]),
        .R(1'b0));
  FDRE \ADCData_reg[2] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[2]),
        .Q(m_axis_tdata[2]),
        .R(1'b0));
  FDRE \ADCData_reg[3] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[3]),
        .Q(m_axis_tdata[3]),
        .R(1'b0));
  FDRE \ADCData_reg[4] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[4]),
        .Q(m_axis_tdata[4]),
        .R(1'b0));
  FDRE \ADCData_reg[5] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[5]),
        .Q(m_axis_tdata[5]),
        .R(1'b0));
  FDRE \ADCData_reg[6] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[6]),
        .Q(m_axis_tdata[6]),
        .R(1'b0));
  FDRE \ADCData_reg[7] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[7]),
        .Q(m_axis_tdata[7]),
        .R(1'b0));
  FDRE \ADCData_reg[8] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[8]),
        .Q(m_axis_tdata[8]),
        .R(1'b0));
  FDRE \ADCData_reg[9] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[9]),
        .Q(m_axis_tdata[9]),
        .R(1'b0));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_13 inst_Serdes_1x14_DDR_Data_Line
       (.D(ADCDataLine),
        .bitslip_cmd(bitslip_cmd),
        .clk_bit(clk_bit),
        .clk_word(clk_word),
        .clkb(clkb),
        .lvds_dat_n(lvds_dat_n),
        .lvds_dat_p(lvds_dat_p),
        .sys_rst(sys_rst));
endmodule

(* ORIG_REF_NAME = "AdcLane" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_1
   (m_axis_tdata,
    lvds_dat_p,
    lvds_dat_n,
    bitslip_cmd,
    clk_bit,
    clkb,
    clk_word,
    sys_rst);
  output [13:0]m_axis_tdata;
  input [0:0]lvds_dat_p;
  input [0:0]lvds_dat_n;
  input bitslip_cmd;
  input clk_bit;
  input clkb;
  input clk_word;
  input sys_rst;

  wire [13:0]ADCDataLine;
  wire bitslip_cmd;
  wire clk_bit;
  wire clk_word;
  wire clkb;
  wire [0:0]lvds_dat_n;
  wire [0:0]lvds_dat_p;
  wire [13:0]m_axis_tdata;
  wire sys_rst;

  FDRE \ADCData_reg[0] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[0]),
        .Q(m_axis_tdata[0]),
        .R(1'b0));
  FDRE \ADCData_reg[10] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[10]),
        .Q(m_axis_tdata[10]),
        .R(1'b0));
  FDRE \ADCData_reg[11] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[11]),
        .Q(m_axis_tdata[11]),
        .R(1'b0));
  FDRE \ADCData_reg[12] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[12]),
        .Q(m_axis_tdata[12]),
        .R(1'b0));
  FDRE \ADCData_reg[13] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[13]),
        .Q(m_axis_tdata[13]),
        .R(1'b0));
  FDRE \ADCData_reg[1] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[1]),
        .Q(m_axis_tdata[1]),
        .R(1'b0));
  FDRE \ADCData_reg[2] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[2]),
        .Q(m_axis_tdata[2]),
        .R(1'b0));
  FDRE \ADCData_reg[3] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[3]),
        .Q(m_axis_tdata[3]),
        .R(1'b0));
  FDRE \ADCData_reg[4] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[4]),
        .Q(m_axis_tdata[4]),
        .R(1'b0));
  FDRE \ADCData_reg[5] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[5]),
        .Q(m_axis_tdata[5]),
        .R(1'b0));
  FDRE \ADCData_reg[6] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[6]),
        .Q(m_axis_tdata[6]),
        .R(1'b0));
  FDRE \ADCData_reg[7] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[7]),
        .Q(m_axis_tdata[7]),
        .R(1'b0));
  FDRE \ADCData_reg[8] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[8]),
        .Q(m_axis_tdata[8]),
        .R(1'b0));
  FDRE \ADCData_reg[9] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[9]),
        .Q(m_axis_tdata[9]),
        .R(1'b0));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_12 inst_Serdes_1x14_DDR_Data_Line
       (.D(ADCDataLine),
        .bitslip_cmd(bitslip_cmd),
        .clk_bit(clk_bit),
        .clk_word(clk_word),
        .clkb(clkb),
        .lvds_dat_n(lvds_dat_n),
        .lvds_dat_p(lvds_dat_p),
        .sys_rst(sys_rst));
endmodule

(* ORIG_REF_NAME = "AdcLane" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_2
   (m_axis_tdata,
    lvds_dat_p,
    lvds_dat_n,
    bitslip_cmd,
    clk_bit,
    clkb,
    clk_word,
    sys_rst);
  output [13:0]m_axis_tdata;
  input [0:0]lvds_dat_p;
  input [0:0]lvds_dat_n;
  input bitslip_cmd;
  input clk_bit;
  input clkb;
  input clk_word;
  input sys_rst;

  wire [13:0]ADCDataLine;
  wire bitslip_cmd;
  wire clk_bit;
  wire clk_word;
  wire clkb;
  wire [0:0]lvds_dat_n;
  wire [0:0]lvds_dat_p;
  wire [13:0]m_axis_tdata;
  wire sys_rst;

  FDRE \ADCData_reg[0] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[0]),
        .Q(m_axis_tdata[0]),
        .R(1'b0));
  FDRE \ADCData_reg[10] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[10]),
        .Q(m_axis_tdata[10]),
        .R(1'b0));
  FDRE \ADCData_reg[11] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[11]),
        .Q(m_axis_tdata[11]),
        .R(1'b0));
  FDRE \ADCData_reg[12] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[12]),
        .Q(m_axis_tdata[12]),
        .R(1'b0));
  FDRE \ADCData_reg[13] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[13]),
        .Q(m_axis_tdata[13]),
        .R(1'b0));
  FDRE \ADCData_reg[1] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[1]),
        .Q(m_axis_tdata[1]),
        .R(1'b0));
  FDRE \ADCData_reg[2] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[2]),
        .Q(m_axis_tdata[2]),
        .R(1'b0));
  FDRE \ADCData_reg[3] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[3]),
        .Q(m_axis_tdata[3]),
        .R(1'b0));
  FDRE \ADCData_reg[4] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[4]),
        .Q(m_axis_tdata[4]),
        .R(1'b0));
  FDRE \ADCData_reg[5] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[5]),
        .Q(m_axis_tdata[5]),
        .R(1'b0));
  FDRE \ADCData_reg[6] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[6]),
        .Q(m_axis_tdata[6]),
        .R(1'b0));
  FDRE \ADCData_reg[7] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[7]),
        .Q(m_axis_tdata[7]),
        .R(1'b0));
  FDRE \ADCData_reg[8] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[8]),
        .Q(m_axis_tdata[8]),
        .R(1'b0));
  FDRE \ADCData_reg[9] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[9]),
        .Q(m_axis_tdata[9]),
        .R(1'b0));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_11 inst_Serdes_1x14_DDR_Data_Line
       (.D(ADCDataLine),
        .bitslip_cmd(bitslip_cmd),
        .clk_bit(clk_bit),
        .clk_word(clk_word),
        .clkb(clkb),
        .lvds_dat_n(lvds_dat_n),
        .lvds_dat_p(lvds_dat_p),
        .sys_rst(sys_rst));
endmodule

(* ORIG_REF_NAME = "AdcLane" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_3
   (m_axis_tdata,
    lvds_dat_p,
    lvds_dat_n,
    bitslip_cmd,
    clk_bit,
    clkb,
    clk_word,
    sys_rst);
  output [13:0]m_axis_tdata;
  input [0:0]lvds_dat_p;
  input [0:0]lvds_dat_n;
  input bitslip_cmd;
  input clk_bit;
  input clkb;
  input clk_word;
  input sys_rst;

  wire [13:0]ADCDataLine;
  wire bitslip_cmd;
  wire clk_bit;
  wire clk_word;
  wire clkb;
  wire [0:0]lvds_dat_n;
  wire [0:0]lvds_dat_p;
  wire [13:0]m_axis_tdata;
  wire sys_rst;

  FDRE \ADCData_reg[0] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[0]),
        .Q(m_axis_tdata[0]),
        .R(1'b0));
  FDRE \ADCData_reg[10] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[10]),
        .Q(m_axis_tdata[10]),
        .R(1'b0));
  FDRE \ADCData_reg[11] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[11]),
        .Q(m_axis_tdata[11]),
        .R(1'b0));
  FDRE \ADCData_reg[12] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[12]),
        .Q(m_axis_tdata[12]),
        .R(1'b0));
  FDRE \ADCData_reg[13] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[13]),
        .Q(m_axis_tdata[13]),
        .R(1'b0));
  FDRE \ADCData_reg[1] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[1]),
        .Q(m_axis_tdata[1]),
        .R(1'b0));
  FDRE \ADCData_reg[2] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[2]),
        .Q(m_axis_tdata[2]),
        .R(1'b0));
  FDRE \ADCData_reg[3] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[3]),
        .Q(m_axis_tdata[3]),
        .R(1'b0));
  FDRE \ADCData_reg[4] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[4]),
        .Q(m_axis_tdata[4]),
        .R(1'b0));
  FDRE \ADCData_reg[5] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[5]),
        .Q(m_axis_tdata[5]),
        .R(1'b0));
  FDRE \ADCData_reg[6] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[6]),
        .Q(m_axis_tdata[6]),
        .R(1'b0));
  FDRE \ADCData_reg[7] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[7]),
        .Q(m_axis_tdata[7]),
        .R(1'b0));
  FDRE \ADCData_reg[8] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[8]),
        .Q(m_axis_tdata[8]),
        .R(1'b0));
  FDRE \ADCData_reg[9] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[9]),
        .Q(m_axis_tdata[9]),
        .R(1'b0));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_10 inst_Serdes_1x14_DDR_Data_Line
       (.D(ADCDataLine),
        .bitslip_cmd(bitslip_cmd),
        .clk_bit(clk_bit),
        .clk_word(clk_word),
        .clkb(clkb),
        .lvds_dat_n(lvds_dat_n),
        .lvds_dat_p(lvds_dat_p),
        .sys_rst(sys_rst));
endmodule

(* ORIG_REF_NAME = "AdcLane" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_4
   (lvds_dat_p,
    lvds_dat_n,
    bitslip_cmd,
    clk_bit,
    clkb,
    clk_word,
    sys_rst);
  input [0:0]lvds_dat_p;
  input [0:0]lvds_dat_n;
  input bitslip_cmd;
  input clk_bit;
  input clkb;
  input clk_word;
  input sys_rst;

  wire bitslip_cmd;
  wire clk_bit;
  wire clk_word;
  wire clkb;
  wire [0:0]lvds_dat_n;
  wire [0:0]lvds_dat_p;
  wire sys_rst;

  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_9 inst_Serdes_1x14_DDR_Data_Line
       (.bitslip_cmd(bitslip_cmd),
        .clk_bit(clk_bit),
        .clk_word(clk_word),
        .clkb(clkb),
        .lvds_dat_n(lvds_dat_n),
        .lvds_dat_p(lvds_dat_p),
        .sys_rst(sys_rst));
endmodule

(* ORIG_REF_NAME = "AdcLane" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_5
   (m_axis_tdata,
    lvds_dat_p,
    lvds_dat_n,
    bitslip_cmd,
    clk_bit,
    clkb,
    clk_word,
    sys_rst);
  output [13:0]m_axis_tdata;
  input [0:0]lvds_dat_p;
  input [0:0]lvds_dat_n;
  input bitslip_cmd;
  input clk_bit;
  input clkb;
  input clk_word;
  input sys_rst;

  wire [13:0]ADCDataLine;
  wire bitslip_cmd;
  wire clk_bit;
  wire clk_word;
  wire clkb;
  wire [0:0]lvds_dat_n;
  wire [0:0]lvds_dat_p;
  wire [13:0]m_axis_tdata;
  wire sys_rst;

  FDRE \ADCData_reg[0] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[0]),
        .Q(m_axis_tdata[0]),
        .R(1'b0));
  FDRE \ADCData_reg[10] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[10]),
        .Q(m_axis_tdata[10]),
        .R(1'b0));
  FDRE \ADCData_reg[11] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[11]),
        .Q(m_axis_tdata[11]),
        .R(1'b0));
  FDRE \ADCData_reg[12] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[12]),
        .Q(m_axis_tdata[12]),
        .R(1'b0));
  FDRE \ADCData_reg[13] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[13]),
        .Q(m_axis_tdata[13]),
        .R(1'b0));
  FDRE \ADCData_reg[1] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[1]),
        .Q(m_axis_tdata[1]),
        .R(1'b0));
  FDRE \ADCData_reg[2] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[2]),
        .Q(m_axis_tdata[2]),
        .R(1'b0));
  FDRE \ADCData_reg[3] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[3]),
        .Q(m_axis_tdata[3]),
        .R(1'b0));
  FDRE \ADCData_reg[4] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[4]),
        .Q(m_axis_tdata[4]),
        .R(1'b0));
  FDRE \ADCData_reg[5] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[5]),
        .Q(m_axis_tdata[5]),
        .R(1'b0));
  FDRE \ADCData_reg[6] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[6]),
        .Q(m_axis_tdata[6]),
        .R(1'b0));
  FDRE \ADCData_reg[7] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[7]),
        .Q(m_axis_tdata[7]),
        .R(1'b0));
  FDRE \ADCData_reg[8] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[8]),
        .Q(m_axis_tdata[8]),
        .R(1'b0));
  FDRE \ADCData_reg[9] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[9]),
        .Q(m_axis_tdata[9]),
        .R(1'b0));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_8 inst_Serdes_1x14_DDR_Data_Line
       (.D(ADCDataLine),
        .bitslip_cmd(bitslip_cmd),
        .clk_bit(clk_bit),
        .clk_word(clk_word),
        .clkb(clkb),
        .lvds_dat_n(lvds_dat_n),
        .lvds_dat_p(lvds_dat_p),
        .sys_rst(sys_rst));
endmodule

(* ORIG_REF_NAME = "AdcLane" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_6
   (m_axis_tdata,
    lvds_dat_p,
    lvds_dat_n,
    bitslip_cmd,
    clk_bit,
    clkb,
    clk_word,
    sys_rst);
  output [13:0]m_axis_tdata;
  input [0:0]lvds_dat_p;
  input [0:0]lvds_dat_n;
  input bitslip_cmd;
  input clk_bit;
  input clkb;
  input clk_word;
  input sys_rst;

  wire [13:0]ADCDataLine;
  wire bitslip_cmd;
  wire clk_bit;
  wire clk_word;
  wire clkb;
  wire [0:0]lvds_dat_n;
  wire [0:0]lvds_dat_p;
  wire [13:0]m_axis_tdata;
  wire sys_rst;

  FDRE \ADCData_reg[0] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[0]),
        .Q(m_axis_tdata[0]),
        .R(1'b0));
  FDRE \ADCData_reg[10] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[10]),
        .Q(m_axis_tdata[10]),
        .R(1'b0));
  FDRE \ADCData_reg[11] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[11]),
        .Q(m_axis_tdata[11]),
        .R(1'b0));
  FDRE \ADCData_reg[12] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[12]),
        .Q(m_axis_tdata[12]),
        .R(1'b0));
  FDRE \ADCData_reg[13] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[13]),
        .Q(m_axis_tdata[13]),
        .R(1'b0));
  FDRE \ADCData_reg[1] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[1]),
        .Q(m_axis_tdata[1]),
        .R(1'b0));
  FDRE \ADCData_reg[2] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[2]),
        .Q(m_axis_tdata[2]),
        .R(1'b0));
  FDRE \ADCData_reg[3] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[3]),
        .Q(m_axis_tdata[3]),
        .R(1'b0));
  FDRE \ADCData_reg[4] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[4]),
        .Q(m_axis_tdata[4]),
        .R(1'b0));
  FDRE \ADCData_reg[5] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[5]),
        .Q(m_axis_tdata[5]),
        .R(1'b0));
  FDRE \ADCData_reg[6] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[6]),
        .Q(m_axis_tdata[6]),
        .R(1'b0));
  FDRE \ADCData_reg[7] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[7]),
        .Q(m_axis_tdata[7]),
        .R(1'b0));
  FDRE \ADCData_reg[8] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[8]),
        .Q(m_axis_tdata[8]),
        .R(1'b0));
  FDRE \ADCData_reg[9] 
       (.C(clk_word),
        .CE(1'b1),
        .D(ADCDataLine[9]),
        .Q(m_axis_tdata[9]),
        .R(1'b0));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_7 inst_Serdes_1x14_DDR_Data_Line
       (.D(ADCDataLine),
        .bitslip_cmd(bitslip_cmd),
        .clk_bit(clk_bit),
        .clk_word(clk_word),
        .clkb(clkb),
        .lvds_dat_n(lvds_dat_n),
        .lvds_dat_p(lvds_dat_p),
        .sys_rst(sys_rst));
endmodule

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR
   (rFrmAlignDone_reg,
    sys_rst_0,
    lvds_fco_p,
    lvds_fco_n,
    bitslip_cmd,
    clk_bit,
    clkb,
    clk_word,
    sys_rst,
    rFrmAlignDone,
    m_axis_tvalid);
  output rFrmAlignDone_reg;
  output sys_rst_0;
  input lvds_fco_p;
  input lvds_fco_n;
  input bitslip_cmd;
  input clk_bit;
  input clkb;
  input clk_word;
  input sys_rst;
  input rFrmAlignDone;
  input m_axis_tvalid;

  wire ISERDESE2_inst_master_n_1;
  wire ISERDESE2_inst_master_n_2;
  wire ISERDESE2_inst_master_n_3;
  wire ISERDESE2_inst_master_n_4;
  wire ISERDESE2_inst_master_n_5;
  wire ISERDESE2_inst_master_n_6;
  wire ISERDESE2_inst_master_n_7;
  wire ISERDESE2_inst_master_n_8;
  wire ISERDESE2_inst_slave_n_3;
  wire ISERDESE2_inst_slave_n_4;
  wire ISERDESE2_inst_slave_n_5;
  wire ISERDESE2_inst_slave_n_6;
  wire ISERDESE2_inst_slave_n_7;
  wire ISERDESE2_inst_slave_n_8;
  wire SHIFTOUT1;
  wire SHIFTOUT2;
  wire bitslip_cmd;
  wire clk_bit;
  wire clk_word;
  wire clkb;
  wire data_in_single;
  wire lvds_fco_n;
  wire lvds_fco_p;
  wire m_axis_tvalid;
  wire rFrmAlignDone;
  wire rFrmAlignDone_i_2_n_0;
  wire rFrmAlignDone_i_3_n_0;
  wire rFrmAlignDone_i_4_n_0;
  wire rFrmAlignDone_reg;
  wire sys_rst;
  wire sys_rst_0;
  wire NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_O_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_OFB_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_D_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_O_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED;

  (* BOX_TYPE = "PRIMITIVE" *) 
  (* OPT_MODIFIED = "MLO" *) 
  ISERDESE2 #(
    .DATA_RATE("DDR"),
    .DATA_WIDTH(14),
    .DYN_CLKDIV_INV_EN("FALSE"),
    .DYN_CLK_INV_EN("FALSE"),
    .INIT_Q1(1'b0),
    .INIT_Q2(1'b0),
    .INIT_Q3(1'b0),
    .INIT_Q4(1'b0),
    .INTERFACE_TYPE("NETWORKING"),
    .IOBDELAY("NONE"),
    .IS_CLKB_INVERTED(1'b1),
    .IS_CLKDIVP_INVERTED(1'b0),
    .IS_CLKDIV_INVERTED(1'b0),
    .IS_CLK_INVERTED(1'b0),
    .IS_D_INVERTED(1'b0),
    .IS_OCLKB_INVERTED(1'b0),
    .IS_OCLK_INVERTED(1'b0),
    .NUM_CE(2),
    .OFB_USED("FALSE"),
    .SERDES_MODE("MASTER"),
    .SRVAL_Q1(1'b0),
    .SRVAL_Q2(1'b0),
    .SRVAL_Q3(1'b0),
    .SRVAL_Q4(1'b0)) 
    ISERDESE2_inst_master
       (.BITSLIP(bitslip_cmd),
        .CE1(1'b1),
        .CE2(1'b1),
        .CLK(clk_bit),
        .CLKB(clkb),
        .CLKDIV(clk_word),
        .CLKDIVP(1'b0),
        .D(data_in_single),
        .DDLY(NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED),
        .DYNCLKDIVSEL(1'b0),
        .DYNCLKSEL(1'b0),
        .O(NLW_ISERDESE2_inst_master_O_UNCONNECTED),
        .OCLK(1'b0),
        .OCLKB(NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED),
        .OFB(NLW_ISERDESE2_inst_master_OFB_UNCONNECTED),
        .Q1(ISERDESE2_inst_master_n_1),
        .Q2(ISERDESE2_inst_master_n_2),
        .Q3(ISERDESE2_inst_master_n_3),
        .Q4(ISERDESE2_inst_master_n_4),
        .Q5(ISERDESE2_inst_master_n_5),
        .Q6(ISERDESE2_inst_master_n_6),
        .Q7(ISERDESE2_inst_master_n_7),
        .Q8(ISERDESE2_inst_master_n_8),
        .RST(sys_rst),
        .SHIFTIN1(NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED),
        .SHIFTIN2(NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED),
        .SHIFTOUT1(SHIFTOUT1),
        .SHIFTOUT2(SHIFTOUT2));
  (* BOX_TYPE = "PRIMITIVE" *) 
  (* OPT_MODIFIED = "MLO" *) 
  ISERDESE2 #(
    .DATA_RATE("DDR"),
    .DATA_WIDTH(14),
    .DYN_CLKDIV_INV_EN("FALSE"),
    .DYN_CLK_INV_EN("FALSE"),
    .INIT_Q1(1'b0),
    .INIT_Q2(1'b0),
    .INIT_Q3(1'b0),
    .INIT_Q4(1'b0),
    .INTERFACE_TYPE("NETWORKING"),
    .IOBDELAY("NONE"),
    .IS_CLKB_INVERTED(1'b1),
    .IS_CLKDIVP_INVERTED(1'b0),
    .IS_CLKDIV_INVERTED(1'b0),
    .IS_CLK_INVERTED(1'b0),
    .IS_D_INVERTED(1'b0),
    .IS_OCLKB_INVERTED(1'b0),
    .IS_OCLK_INVERTED(1'b0),
    .NUM_CE(2),
    .OFB_USED("FALSE"),
    .SERDES_MODE("SLAVE"),
    .SRVAL_Q1(1'b0),
    .SRVAL_Q2(1'b0),
    .SRVAL_Q3(1'b0),
    .SRVAL_Q4(1'b0)) 
    ISERDESE2_inst_slave
       (.BITSLIP(bitslip_cmd),
        .CE1(1'b1),
        .CE2(1'b1),
        .CLK(clk_bit),
        .CLKB(clkb),
        .CLKDIV(clk_word),
        .CLKDIVP(1'b0),
        .D(NLW_ISERDESE2_inst_slave_D_UNCONNECTED),
        .DDLY(NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED),
        .DYNCLKDIVSEL(1'b0),
        .DYNCLKSEL(1'b0),
        .O(NLW_ISERDESE2_inst_slave_O_UNCONNECTED),
        .OCLK(1'b0),
        .OCLKB(1'b0),
        .OFB(1'b0),
        .Q1(NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED),
        .Q2(NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED),
        .Q3(ISERDESE2_inst_slave_n_3),
        .Q4(ISERDESE2_inst_slave_n_4),
        .Q5(ISERDESE2_inst_slave_n_5),
        .Q6(ISERDESE2_inst_slave_n_6),
        .Q7(ISERDESE2_inst_slave_n_7),
        .Q8(ISERDESE2_inst_slave_n_8),
        .RST(sys_rst),
        .SHIFTIN1(SHIFTOUT1),
        .SHIFTIN2(SHIFTOUT2),
        .SHIFTOUT1(NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED),
        .SHIFTOUT2(NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED));
  (* BOX_TYPE = "PRIMITIVE" *) 
  (* CAPACITANCE = "DONT_CARE" *) 
  (* IBUF_DELAY_VALUE = "0" *) 
  (* IFD_DELAY_VALUE = "AUTO" *) 
  IBUFDS ibufds_inst
       (.I(lvds_fco_p),
        .IB(lvds_fco_n),
        .O(data_in_single));
  LUT5 #(
    .INIT(32'h01FF0100)) 
    rFrmAlignDone_i_1
       (.I0(rFrmAlignDone_i_2_n_0),
        .I1(rFrmAlignDone_i_3_n_0),
        .I2(rFrmAlignDone_i_4_n_0),
        .I3(rFrmAlignDone),
        .I4(m_axis_tvalid),
        .O(rFrmAlignDone_reg));
  LUT6 #(
    .INIT(64'hFFFFFFFFFFFF7FFF)) 
    rFrmAlignDone_i_2
       (.I0(ISERDESE2_inst_slave_n_7),
        .I1(ISERDESE2_inst_slave_n_8),
        .I2(ISERDESE2_inst_slave_n_5),
        .I3(ISERDESE2_inst_slave_n_6),
        .I4(ISERDESE2_inst_master_n_2),
        .I5(ISERDESE2_inst_master_n_1),
        .O(rFrmAlignDone_i_2_n_0));
  LUT4 #(
    .INIT(16'hFFFE)) 
    rFrmAlignDone_i_3
       (.I0(ISERDESE2_inst_master_n_4),
        .I1(ISERDESE2_inst_master_n_3),
        .I2(ISERDESE2_inst_master_n_6),
        .I3(ISERDESE2_inst_master_n_5),
        .O(rFrmAlignDone_i_3_n_0));
  LUT4 #(
    .INIT(16'hDFFF)) 
    rFrmAlignDone_i_4
       (.I0(ISERDESE2_inst_master_n_8),
        .I1(ISERDESE2_inst_master_n_7),
        .I2(ISERDESE2_inst_slave_n_4),
        .I3(ISERDESE2_inst_slave_n_3),
        .O(rFrmAlignDone_i_4_n_0));
  LUT5 #(
    .INIT(32'h00FE0000)) 
    rFrmBitslip_i_1
       (.I0(rFrmAlignDone_i_2_n_0),
        .I1(rFrmAlignDone_i_3_n_0),
        .I2(rFrmAlignDone_i_4_n_0),
        .I3(sys_rst),
        .I4(rFrmAlignDone),
        .O(sys_rst_0));
endmodule

(* ORIG_REF_NAME = "Serdes_1x14_DDR" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_10
   (D,
    lvds_dat_p,
    lvds_dat_n,
    bitslip_cmd,
    clk_bit,
    clkb,
    clk_word,
    sys_rst);
  output [13:0]D;
  input [0:0]lvds_dat_p;
  input [0:0]lvds_dat_n;
  input bitslip_cmd;
  input clk_bit;
  input clkb;
  input clk_word;
  input sys_rst;

  wire [13:0]D;
  wire SHIFTOUT1;
  wire SHIFTOUT2;
  wire bitslip_cmd;
  wire clk_bit;
  wire clk_word;
  wire clkb;
  wire data_in_single;
  wire [0:0]lvds_dat_n;
  wire [0:0]lvds_dat_p;
  wire sys_rst;
  wire NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_O_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_OFB_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_D_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_O_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED;

  (* BOX_TYPE = "PRIMITIVE" *) 
  (* OPT_MODIFIED = "MLO" *) 
  ISERDESE2 #(
    .DATA_RATE("DDR"),
    .DATA_WIDTH(14),
    .DYN_CLKDIV_INV_EN("FALSE"),
    .DYN_CLK_INV_EN("FALSE"),
    .INIT_Q1(1'b0),
    .INIT_Q2(1'b0),
    .INIT_Q3(1'b0),
    .INIT_Q4(1'b0),
    .INTERFACE_TYPE("NETWORKING"),
    .IOBDELAY("NONE"),
    .IS_CLKB_INVERTED(1'b1),
    .IS_CLKDIVP_INVERTED(1'b0),
    .IS_CLKDIV_INVERTED(1'b0),
    .IS_CLK_INVERTED(1'b0),
    .IS_D_INVERTED(1'b0),
    .IS_OCLKB_INVERTED(1'b0),
    .IS_OCLK_INVERTED(1'b0),
    .NUM_CE(2),
    .OFB_USED("FALSE"),
    .SERDES_MODE("MASTER"),
    .SRVAL_Q1(1'b0),
    .SRVAL_Q2(1'b0),
    .SRVAL_Q3(1'b0),
    .SRVAL_Q4(1'b0)) 
    ISERDESE2_inst_master
       (.BITSLIP(bitslip_cmd),
        .CE1(1'b1),
        .CE2(1'b1),
        .CLK(clk_bit),
        .CLKB(clkb),
        .CLKDIV(clk_word),
        .CLKDIVP(1'b0),
        .D(data_in_single),
        .DDLY(NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED),
        .DYNCLKDIVSEL(1'b0),
        .DYNCLKSEL(1'b0),
        .O(NLW_ISERDESE2_inst_master_O_UNCONNECTED),
        .OCLK(1'b0),
        .OCLKB(NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED),
        .OFB(NLW_ISERDESE2_inst_master_OFB_UNCONNECTED),
        .Q1(D[0]),
        .Q2(D[1]),
        .Q3(D[2]),
        .Q4(D[3]),
        .Q5(D[4]),
        .Q6(D[5]),
        .Q7(D[6]),
        .Q8(D[7]),
        .RST(sys_rst),
        .SHIFTIN1(NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED),
        .SHIFTIN2(NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED),
        .SHIFTOUT1(SHIFTOUT1),
        .SHIFTOUT2(SHIFTOUT2));
  (* BOX_TYPE = "PRIMITIVE" *) 
  (* OPT_MODIFIED = "MLO" *) 
  ISERDESE2 #(
    .DATA_RATE("DDR"),
    .DATA_WIDTH(14),
    .DYN_CLKDIV_INV_EN("FALSE"),
    .DYN_CLK_INV_EN("FALSE"),
    .INIT_Q1(1'b0),
    .INIT_Q2(1'b0),
    .INIT_Q3(1'b0),
    .INIT_Q4(1'b0),
    .INTERFACE_TYPE("NETWORKING"),
    .IOBDELAY("NONE"),
    .IS_CLKB_INVERTED(1'b1),
    .IS_CLKDIVP_INVERTED(1'b0),
    .IS_CLKDIV_INVERTED(1'b0),
    .IS_CLK_INVERTED(1'b0),
    .IS_D_INVERTED(1'b0),
    .IS_OCLKB_INVERTED(1'b0),
    .IS_OCLK_INVERTED(1'b0),
    .NUM_CE(2),
    .OFB_USED("FALSE"),
    .SERDES_MODE("SLAVE"),
    .SRVAL_Q1(1'b0),
    .SRVAL_Q2(1'b0),
    .SRVAL_Q3(1'b0),
    .SRVAL_Q4(1'b0)) 
    ISERDESE2_inst_slave
       (.BITSLIP(bitslip_cmd),
        .CE1(1'b1),
        .CE2(1'b1),
        .CLK(clk_bit),
        .CLKB(clkb),
        .CLKDIV(clk_word),
        .CLKDIVP(1'b0),
        .D(NLW_ISERDESE2_inst_slave_D_UNCONNECTED),
        .DDLY(NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED),
        .DYNCLKDIVSEL(1'b0),
        .DYNCLKSEL(1'b0),
        .O(NLW_ISERDESE2_inst_slave_O_UNCONNECTED),
        .OCLK(1'b0),
        .OCLKB(1'b0),
        .OFB(1'b0),
        .Q1(NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED),
        .Q2(NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED),
        .Q3(D[8]),
        .Q4(D[9]),
        .Q5(D[10]),
        .Q6(D[11]),
        .Q7(D[12]),
        .Q8(D[13]),
        .RST(sys_rst),
        .SHIFTIN1(SHIFTOUT1),
        .SHIFTIN2(SHIFTOUT2),
        .SHIFTOUT1(NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED),
        .SHIFTOUT2(NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED));
  (* BOX_TYPE = "PRIMITIVE" *) 
  (* CAPACITANCE = "DONT_CARE" *) 
  (* IBUF_DELAY_VALUE = "0" *) 
  (* IFD_DELAY_VALUE = "AUTO" *) 
  IBUFDS ibufds_inst
       (.I(lvds_dat_p),
        .IB(lvds_dat_n),
        .O(data_in_single));
endmodule

(* ORIG_REF_NAME = "Serdes_1x14_DDR" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_11
   (D,
    lvds_dat_p,
    lvds_dat_n,
    bitslip_cmd,
    clk_bit,
    clkb,
    clk_word,
    sys_rst);
  output [13:0]D;
  input [0:0]lvds_dat_p;
  input [0:0]lvds_dat_n;
  input bitslip_cmd;
  input clk_bit;
  input clkb;
  input clk_word;
  input sys_rst;

  wire [13:0]D;
  wire SHIFTOUT1;
  wire SHIFTOUT2;
  wire bitslip_cmd;
  wire clk_bit;
  wire clk_word;
  wire clkb;
  wire data_in_single;
  wire [0:0]lvds_dat_n;
  wire [0:0]lvds_dat_p;
  wire sys_rst;
  wire NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_O_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_OFB_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_D_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_O_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED;

  (* BOX_TYPE = "PRIMITIVE" *) 
  (* OPT_MODIFIED = "MLO" *) 
  ISERDESE2 #(
    .DATA_RATE("DDR"),
    .DATA_WIDTH(14),
    .DYN_CLKDIV_INV_EN("FALSE"),
    .DYN_CLK_INV_EN("FALSE"),
    .INIT_Q1(1'b0),
    .INIT_Q2(1'b0),
    .INIT_Q3(1'b0),
    .INIT_Q4(1'b0),
    .INTERFACE_TYPE("NETWORKING"),
    .IOBDELAY("NONE"),
    .IS_CLKB_INVERTED(1'b1),
    .IS_CLKDIVP_INVERTED(1'b0),
    .IS_CLKDIV_INVERTED(1'b0),
    .IS_CLK_INVERTED(1'b0),
    .IS_D_INVERTED(1'b0),
    .IS_OCLKB_INVERTED(1'b0),
    .IS_OCLK_INVERTED(1'b0),
    .NUM_CE(2),
    .OFB_USED("FALSE"),
    .SERDES_MODE("MASTER"),
    .SRVAL_Q1(1'b0),
    .SRVAL_Q2(1'b0),
    .SRVAL_Q3(1'b0),
    .SRVAL_Q4(1'b0)) 
    ISERDESE2_inst_master
       (.BITSLIP(bitslip_cmd),
        .CE1(1'b1),
        .CE2(1'b1),
        .CLK(clk_bit),
        .CLKB(clkb),
        .CLKDIV(clk_word),
        .CLKDIVP(1'b0),
        .D(data_in_single),
        .DDLY(NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED),
        .DYNCLKDIVSEL(1'b0),
        .DYNCLKSEL(1'b0),
        .O(NLW_ISERDESE2_inst_master_O_UNCONNECTED),
        .OCLK(1'b0),
        .OCLKB(NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED),
        .OFB(NLW_ISERDESE2_inst_master_OFB_UNCONNECTED),
        .Q1(D[0]),
        .Q2(D[1]),
        .Q3(D[2]),
        .Q4(D[3]),
        .Q5(D[4]),
        .Q6(D[5]),
        .Q7(D[6]),
        .Q8(D[7]),
        .RST(sys_rst),
        .SHIFTIN1(NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED),
        .SHIFTIN2(NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED),
        .SHIFTOUT1(SHIFTOUT1),
        .SHIFTOUT2(SHIFTOUT2));
  (* BOX_TYPE = "PRIMITIVE" *) 
  (* OPT_MODIFIED = "MLO" *) 
  ISERDESE2 #(
    .DATA_RATE("DDR"),
    .DATA_WIDTH(14),
    .DYN_CLKDIV_INV_EN("FALSE"),
    .DYN_CLK_INV_EN("FALSE"),
    .INIT_Q1(1'b0),
    .INIT_Q2(1'b0),
    .INIT_Q3(1'b0),
    .INIT_Q4(1'b0),
    .INTERFACE_TYPE("NETWORKING"),
    .IOBDELAY("NONE"),
    .IS_CLKB_INVERTED(1'b1),
    .IS_CLKDIVP_INVERTED(1'b0),
    .IS_CLKDIV_INVERTED(1'b0),
    .IS_CLK_INVERTED(1'b0),
    .IS_D_INVERTED(1'b0),
    .IS_OCLKB_INVERTED(1'b0),
    .IS_OCLK_INVERTED(1'b0),
    .NUM_CE(2),
    .OFB_USED("FALSE"),
    .SERDES_MODE("SLAVE"),
    .SRVAL_Q1(1'b0),
    .SRVAL_Q2(1'b0),
    .SRVAL_Q3(1'b0),
    .SRVAL_Q4(1'b0)) 
    ISERDESE2_inst_slave
       (.BITSLIP(bitslip_cmd),
        .CE1(1'b1),
        .CE2(1'b1),
        .CLK(clk_bit),
        .CLKB(clkb),
        .CLKDIV(clk_word),
        .CLKDIVP(1'b0),
        .D(NLW_ISERDESE2_inst_slave_D_UNCONNECTED),
        .DDLY(NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED),
        .DYNCLKDIVSEL(1'b0),
        .DYNCLKSEL(1'b0),
        .O(NLW_ISERDESE2_inst_slave_O_UNCONNECTED),
        .OCLK(1'b0),
        .OCLKB(1'b0),
        .OFB(1'b0),
        .Q1(NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED),
        .Q2(NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED),
        .Q3(D[8]),
        .Q4(D[9]),
        .Q5(D[10]),
        .Q6(D[11]),
        .Q7(D[12]),
        .Q8(D[13]),
        .RST(sys_rst),
        .SHIFTIN1(SHIFTOUT1),
        .SHIFTIN2(SHIFTOUT2),
        .SHIFTOUT1(NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED),
        .SHIFTOUT2(NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED));
  (* BOX_TYPE = "PRIMITIVE" *) 
  (* CAPACITANCE = "DONT_CARE" *) 
  (* IBUF_DELAY_VALUE = "0" *) 
  (* IFD_DELAY_VALUE = "AUTO" *) 
  IBUFDS ibufds_inst
       (.I(lvds_dat_p),
        .IB(lvds_dat_n),
        .O(data_in_single));
endmodule

(* ORIG_REF_NAME = "Serdes_1x14_DDR" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_12
   (D,
    lvds_dat_p,
    lvds_dat_n,
    bitslip_cmd,
    clk_bit,
    clkb,
    clk_word,
    sys_rst);
  output [13:0]D;
  input [0:0]lvds_dat_p;
  input [0:0]lvds_dat_n;
  input bitslip_cmd;
  input clk_bit;
  input clkb;
  input clk_word;
  input sys_rst;

  wire [13:0]D;
  wire SHIFTOUT1;
  wire SHIFTOUT2;
  wire bitslip_cmd;
  wire clk_bit;
  wire clk_word;
  wire clkb;
  wire data_in_single;
  wire [0:0]lvds_dat_n;
  wire [0:0]lvds_dat_p;
  wire sys_rst;
  wire NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_O_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_OFB_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_D_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_O_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED;

  (* BOX_TYPE = "PRIMITIVE" *) 
  (* OPT_MODIFIED = "MLO" *) 
  ISERDESE2 #(
    .DATA_RATE("DDR"),
    .DATA_WIDTH(14),
    .DYN_CLKDIV_INV_EN("FALSE"),
    .DYN_CLK_INV_EN("FALSE"),
    .INIT_Q1(1'b0),
    .INIT_Q2(1'b0),
    .INIT_Q3(1'b0),
    .INIT_Q4(1'b0),
    .INTERFACE_TYPE("NETWORKING"),
    .IOBDELAY("NONE"),
    .IS_CLKB_INVERTED(1'b1),
    .IS_CLKDIVP_INVERTED(1'b0),
    .IS_CLKDIV_INVERTED(1'b0),
    .IS_CLK_INVERTED(1'b0),
    .IS_D_INVERTED(1'b0),
    .IS_OCLKB_INVERTED(1'b0),
    .IS_OCLK_INVERTED(1'b0),
    .NUM_CE(2),
    .OFB_USED("FALSE"),
    .SERDES_MODE("MASTER"),
    .SRVAL_Q1(1'b0),
    .SRVAL_Q2(1'b0),
    .SRVAL_Q3(1'b0),
    .SRVAL_Q4(1'b0)) 
    ISERDESE2_inst_master
       (.BITSLIP(bitslip_cmd),
        .CE1(1'b1),
        .CE2(1'b1),
        .CLK(clk_bit),
        .CLKB(clkb),
        .CLKDIV(clk_word),
        .CLKDIVP(1'b0),
        .D(data_in_single),
        .DDLY(NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED),
        .DYNCLKDIVSEL(1'b0),
        .DYNCLKSEL(1'b0),
        .O(NLW_ISERDESE2_inst_master_O_UNCONNECTED),
        .OCLK(1'b0),
        .OCLKB(NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED),
        .OFB(NLW_ISERDESE2_inst_master_OFB_UNCONNECTED),
        .Q1(D[0]),
        .Q2(D[1]),
        .Q3(D[2]),
        .Q4(D[3]),
        .Q5(D[4]),
        .Q6(D[5]),
        .Q7(D[6]),
        .Q8(D[7]),
        .RST(sys_rst),
        .SHIFTIN1(NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED),
        .SHIFTIN2(NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED),
        .SHIFTOUT1(SHIFTOUT1),
        .SHIFTOUT2(SHIFTOUT2));
  (* BOX_TYPE = "PRIMITIVE" *) 
  (* OPT_MODIFIED = "MLO" *) 
  ISERDESE2 #(
    .DATA_RATE("DDR"),
    .DATA_WIDTH(14),
    .DYN_CLKDIV_INV_EN("FALSE"),
    .DYN_CLK_INV_EN("FALSE"),
    .INIT_Q1(1'b0),
    .INIT_Q2(1'b0),
    .INIT_Q3(1'b0),
    .INIT_Q4(1'b0),
    .INTERFACE_TYPE("NETWORKING"),
    .IOBDELAY("NONE"),
    .IS_CLKB_INVERTED(1'b1),
    .IS_CLKDIVP_INVERTED(1'b0),
    .IS_CLKDIV_INVERTED(1'b0),
    .IS_CLK_INVERTED(1'b0),
    .IS_D_INVERTED(1'b0),
    .IS_OCLKB_INVERTED(1'b0),
    .IS_OCLK_INVERTED(1'b0),
    .NUM_CE(2),
    .OFB_USED("FALSE"),
    .SERDES_MODE("SLAVE"),
    .SRVAL_Q1(1'b0),
    .SRVAL_Q2(1'b0),
    .SRVAL_Q3(1'b0),
    .SRVAL_Q4(1'b0)) 
    ISERDESE2_inst_slave
       (.BITSLIP(bitslip_cmd),
        .CE1(1'b1),
        .CE2(1'b1),
        .CLK(clk_bit),
        .CLKB(clkb),
        .CLKDIV(clk_word),
        .CLKDIVP(1'b0),
        .D(NLW_ISERDESE2_inst_slave_D_UNCONNECTED),
        .DDLY(NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED),
        .DYNCLKDIVSEL(1'b0),
        .DYNCLKSEL(1'b0),
        .O(NLW_ISERDESE2_inst_slave_O_UNCONNECTED),
        .OCLK(1'b0),
        .OCLKB(1'b0),
        .OFB(1'b0),
        .Q1(NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED),
        .Q2(NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED),
        .Q3(D[8]),
        .Q4(D[9]),
        .Q5(D[10]),
        .Q6(D[11]),
        .Q7(D[12]),
        .Q8(D[13]),
        .RST(sys_rst),
        .SHIFTIN1(SHIFTOUT1),
        .SHIFTIN2(SHIFTOUT2),
        .SHIFTOUT1(NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED),
        .SHIFTOUT2(NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED));
  (* BOX_TYPE = "PRIMITIVE" *) 
  (* CAPACITANCE = "DONT_CARE" *) 
  (* IBUF_DELAY_VALUE = "0" *) 
  (* IFD_DELAY_VALUE = "AUTO" *) 
  IBUFDS ibufds_inst
       (.I(lvds_dat_p),
        .IB(lvds_dat_n),
        .O(data_in_single));
endmodule

(* ORIG_REF_NAME = "Serdes_1x14_DDR" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_13
   (D,
    lvds_dat_p,
    lvds_dat_n,
    bitslip_cmd,
    clk_bit,
    clkb,
    clk_word,
    sys_rst);
  output [13:0]D;
  input [0:0]lvds_dat_p;
  input [0:0]lvds_dat_n;
  input bitslip_cmd;
  input clk_bit;
  input clkb;
  input clk_word;
  input sys_rst;

  wire [13:0]D;
  wire SHIFTOUT1;
  wire SHIFTOUT2;
  wire bitslip_cmd;
  wire clk_bit;
  wire clk_word;
  wire clkb;
  wire data_in_single;
  wire [0:0]lvds_dat_n;
  wire [0:0]lvds_dat_p;
  wire sys_rst;
  wire NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_O_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_OFB_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_D_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_O_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED;

  (* BOX_TYPE = "PRIMITIVE" *) 
  (* OPT_MODIFIED = "MLO" *) 
  ISERDESE2 #(
    .DATA_RATE("DDR"),
    .DATA_WIDTH(14),
    .DYN_CLKDIV_INV_EN("FALSE"),
    .DYN_CLK_INV_EN("FALSE"),
    .INIT_Q1(1'b0),
    .INIT_Q2(1'b0),
    .INIT_Q3(1'b0),
    .INIT_Q4(1'b0),
    .INTERFACE_TYPE("NETWORKING"),
    .IOBDELAY("NONE"),
    .IS_CLKB_INVERTED(1'b1),
    .IS_CLKDIVP_INVERTED(1'b0),
    .IS_CLKDIV_INVERTED(1'b0),
    .IS_CLK_INVERTED(1'b0),
    .IS_D_INVERTED(1'b0),
    .IS_OCLKB_INVERTED(1'b0),
    .IS_OCLK_INVERTED(1'b0),
    .NUM_CE(2),
    .OFB_USED("FALSE"),
    .SERDES_MODE("MASTER"),
    .SRVAL_Q1(1'b0),
    .SRVAL_Q2(1'b0),
    .SRVAL_Q3(1'b0),
    .SRVAL_Q4(1'b0)) 
    ISERDESE2_inst_master
       (.BITSLIP(bitslip_cmd),
        .CE1(1'b1),
        .CE2(1'b1),
        .CLK(clk_bit),
        .CLKB(clkb),
        .CLKDIV(clk_word),
        .CLKDIVP(1'b0),
        .D(data_in_single),
        .DDLY(NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED),
        .DYNCLKDIVSEL(1'b0),
        .DYNCLKSEL(1'b0),
        .O(NLW_ISERDESE2_inst_master_O_UNCONNECTED),
        .OCLK(1'b0),
        .OCLKB(NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED),
        .OFB(NLW_ISERDESE2_inst_master_OFB_UNCONNECTED),
        .Q1(D[0]),
        .Q2(D[1]),
        .Q3(D[2]),
        .Q4(D[3]),
        .Q5(D[4]),
        .Q6(D[5]),
        .Q7(D[6]),
        .Q8(D[7]),
        .RST(sys_rst),
        .SHIFTIN1(NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED),
        .SHIFTIN2(NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED),
        .SHIFTOUT1(SHIFTOUT1),
        .SHIFTOUT2(SHIFTOUT2));
  (* BOX_TYPE = "PRIMITIVE" *) 
  (* OPT_MODIFIED = "MLO" *) 
  ISERDESE2 #(
    .DATA_RATE("DDR"),
    .DATA_WIDTH(14),
    .DYN_CLKDIV_INV_EN("FALSE"),
    .DYN_CLK_INV_EN("FALSE"),
    .INIT_Q1(1'b0),
    .INIT_Q2(1'b0),
    .INIT_Q3(1'b0),
    .INIT_Q4(1'b0),
    .INTERFACE_TYPE("NETWORKING"),
    .IOBDELAY("NONE"),
    .IS_CLKB_INVERTED(1'b1),
    .IS_CLKDIVP_INVERTED(1'b0),
    .IS_CLKDIV_INVERTED(1'b0),
    .IS_CLK_INVERTED(1'b0),
    .IS_D_INVERTED(1'b0),
    .IS_OCLKB_INVERTED(1'b0),
    .IS_OCLK_INVERTED(1'b0),
    .NUM_CE(2),
    .OFB_USED("FALSE"),
    .SERDES_MODE("SLAVE"),
    .SRVAL_Q1(1'b0),
    .SRVAL_Q2(1'b0),
    .SRVAL_Q3(1'b0),
    .SRVAL_Q4(1'b0)) 
    ISERDESE2_inst_slave
       (.BITSLIP(bitslip_cmd),
        .CE1(1'b1),
        .CE2(1'b1),
        .CLK(clk_bit),
        .CLKB(clkb),
        .CLKDIV(clk_word),
        .CLKDIVP(1'b0),
        .D(NLW_ISERDESE2_inst_slave_D_UNCONNECTED),
        .DDLY(NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED),
        .DYNCLKDIVSEL(1'b0),
        .DYNCLKSEL(1'b0),
        .O(NLW_ISERDESE2_inst_slave_O_UNCONNECTED),
        .OCLK(1'b0),
        .OCLKB(1'b0),
        .OFB(1'b0),
        .Q1(NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED),
        .Q2(NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED),
        .Q3(D[8]),
        .Q4(D[9]),
        .Q5(D[10]),
        .Q6(D[11]),
        .Q7(D[12]),
        .Q8(D[13]),
        .RST(sys_rst),
        .SHIFTIN1(SHIFTOUT1),
        .SHIFTIN2(SHIFTOUT2),
        .SHIFTOUT1(NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED),
        .SHIFTOUT2(NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED));
  (* BOX_TYPE = "PRIMITIVE" *) 
  (* CAPACITANCE = "DONT_CARE" *) 
  (* IBUF_DELAY_VALUE = "0" *) 
  (* IFD_DELAY_VALUE = "AUTO" *) 
  IBUFDS ibufds_inst
       (.I(lvds_dat_p),
        .IB(lvds_dat_n),
        .O(data_in_single));
endmodule

(* ORIG_REF_NAME = "Serdes_1x14_DDR" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_14
   (D,
    lvds_dat_p,
    lvds_dat_n,
    bitslip_cmd,
    clk_bit,
    clkb,
    clk_word,
    sys_rst);
  output [13:0]D;
  input [0:0]lvds_dat_p;
  input [0:0]lvds_dat_n;
  input bitslip_cmd;
  input clk_bit;
  input clkb;
  input clk_word;
  input sys_rst;

  wire [13:0]D;
  wire SHIFTOUT1;
  wire SHIFTOUT2;
  wire bitslip_cmd;
  wire clk_bit;
  wire clk_word;
  wire clkb;
  wire data_in_single;
  wire [0:0]lvds_dat_n;
  wire [0:0]lvds_dat_p;
  wire sys_rst;
  wire NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_O_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_OFB_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_D_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_O_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED;

  (* BOX_TYPE = "PRIMITIVE" *) 
  (* OPT_MODIFIED = "MLO" *) 
  ISERDESE2 #(
    .DATA_RATE("DDR"),
    .DATA_WIDTH(14),
    .DYN_CLKDIV_INV_EN("FALSE"),
    .DYN_CLK_INV_EN("FALSE"),
    .INIT_Q1(1'b0),
    .INIT_Q2(1'b0),
    .INIT_Q3(1'b0),
    .INIT_Q4(1'b0),
    .INTERFACE_TYPE("NETWORKING"),
    .IOBDELAY("NONE"),
    .IS_CLKB_INVERTED(1'b1),
    .IS_CLKDIVP_INVERTED(1'b0),
    .IS_CLKDIV_INVERTED(1'b0),
    .IS_CLK_INVERTED(1'b0),
    .IS_D_INVERTED(1'b0),
    .IS_OCLKB_INVERTED(1'b0),
    .IS_OCLK_INVERTED(1'b0),
    .NUM_CE(2),
    .OFB_USED("FALSE"),
    .SERDES_MODE("MASTER"),
    .SRVAL_Q1(1'b0),
    .SRVAL_Q2(1'b0),
    .SRVAL_Q3(1'b0),
    .SRVAL_Q4(1'b0)) 
    ISERDESE2_inst_master
       (.BITSLIP(bitslip_cmd),
        .CE1(1'b1),
        .CE2(1'b1),
        .CLK(clk_bit),
        .CLKB(clkb),
        .CLKDIV(clk_word),
        .CLKDIVP(1'b0),
        .D(data_in_single),
        .DDLY(NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED),
        .DYNCLKDIVSEL(1'b0),
        .DYNCLKSEL(1'b0),
        .O(NLW_ISERDESE2_inst_master_O_UNCONNECTED),
        .OCLK(1'b0),
        .OCLKB(NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED),
        .OFB(NLW_ISERDESE2_inst_master_OFB_UNCONNECTED),
        .Q1(D[0]),
        .Q2(D[1]),
        .Q3(D[2]),
        .Q4(D[3]),
        .Q5(D[4]),
        .Q6(D[5]),
        .Q7(D[6]),
        .Q8(D[7]),
        .RST(sys_rst),
        .SHIFTIN1(NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED),
        .SHIFTIN2(NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED),
        .SHIFTOUT1(SHIFTOUT1),
        .SHIFTOUT2(SHIFTOUT2));
  (* BOX_TYPE = "PRIMITIVE" *) 
  (* OPT_MODIFIED = "MLO" *) 
  ISERDESE2 #(
    .DATA_RATE("DDR"),
    .DATA_WIDTH(14),
    .DYN_CLKDIV_INV_EN("FALSE"),
    .DYN_CLK_INV_EN("FALSE"),
    .INIT_Q1(1'b0),
    .INIT_Q2(1'b0),
    .INIT_Q3(1'b0),
    .INIT_Q4(1'b0),
    .INTERFACE_TYPE("NETWORKING"),
    .IOBDELAY("NONE"),
    .IS_CLKB_INVERTED(1'b1),
    .IS_CLKDIVP_INVERTED(1'b0),
    .IS_CLKDIV_INVERTED(1'b0),
    .IS_CLK_INVERTED(1'b0),
    .IS_D_INVERTED(1'b0),
    .IS_OCLKB_INVERTED(1'b0),
    .IS_OCLK_INVERTED(1'b0),
    .NUM_CE(2),
    .OFB_USED("FALSE"),
    .SERDES_MODE("SLAVE"),
    .SRVAL_Q1(1'b0),
    .SRVAL_Q2(1'b0),
    .SRVAL_Q3(1'b0),
    .SRVAL_Q4(1'b0)) 
    ISERDESE2_inst_slave
       (.BITSLIP(bitslip_cmd),
        .CE1(1'b1),
        .CE2(1'b1),
        .CLK(clk_bit),
        .CLKB(clkb),
        .CLKDIV(clk_word),
        .CLKDIVP(1'b0),
        .D(NLW_ISERDESE2_inst_slave_D_UNCONNECTED),
        .DDLY(NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED),
        .DYNCLKDIVSEL(1'b0),
        .DYNCLKSEL(1'b0),
        .O(NLW_ISERDESE2_inst_slave_O_UNCONNECTED),
        .OCLK(1'b0),
        .OCLKB(1'b0),
        .OFB(1'b0),
        .Q1(NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED),
        .Q2(NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED),
        .Q3(D[8]),
        .Q4(D[9]),
        .Q5(D[10]),
        .Q6(D[11]),
        .Q7(D[12]),
        .Q8(D[13]),
        .RST(sys_rst),
        .SHIFTIN1(SHIFTOUT1),
        .SHIFTIN2(SHIFTOUT2),
        .SHIFTOUT1(NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED),
        .SHIFTOUT2(NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED));
  (* BOX_TYPE = "PRIMITIVE" *) 
  (* CAPACITANCE = "DONT_CARE" *) 
  (* IBUF_DELAY_VALUE = "0" *) 
  (* IFD_DELAY_VALUE = "AUTO" *) 
  IBUFDS ibufds_inst
       (.I(lvds_dat_p),
        .IB(lvds_dat_n),
        .O(data_in_single));
endmodule

(* ORIG_REF_NAME = "Serdes_1x14_DDR" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_7
   (D,
    lvds_dat_p,
    lvds_dat_n,
    bitslip_cmd,
    clk_bit,
    clkb,
    clk_word,
    sys_rst);
  output [13:0]D;
  input [0:0]lvds_dat_p;
  input [0:0]lvds_dat_n;
  input bitslip_cmd;
  input clk_bit;
  input clkb;
  input clk_word;
  input sys_rst;

  wire [13:0]D;
  wire SHIFTOUT1;
  wire SHIFTOUT2;
  wire bitslip_cmd;
  wire clk_bit;
  wire clk_word;
  wire clkb;
  wire data_in_single;
  wire [0:0]lvds_dat_n;
  wire [0:0]lvds_dat_p;
  wire sys_rst;
  wire NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_O_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_OFB_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_D_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_O_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED;

  (* BOX_TYPE = "PRIMITIVE" *) 
  (* OPT_MODIFIED = "MLO" *) 
  ISERDESE2 #(
    .DATA_RATE("DDR"),
    .DATA_WIDTH(14),
    .DYN_CLKDIV_INV_EN("FALSE"),
    .DYN_CLK_INV_EN("FALSE"),
    .INIT_Q1(1'b0),
    .INIT_Q2(1'b0),
    .INIT_Q3(1'b0),
    .INIT_Q4(1'b0),
    .INTERFACE_TYPE("NETWORKING"),
    .IOBDELAY("NONE"),
    .IS_CLKB_INVERTED(1'b1),
    .IS_CLKDIVP_INVERTED(1'b0),
    .IS_CLKDIV_INVERTED(1'b0),
    .IS_CLK_INVERTED(1'b0),
    .IS_D_INVERTED(1'b0),
    .IS_OCLKB_INVERTED(1'b0),
    .IS_OCLK_INVERTED(1'b0),
    .NUM_CE(2),
    .OFB_USED("FALSE"),
    .SERDES_MODE("MASTER"),
    .SRVAL_Q1(1'b0),
    .SRVAL_Q2(1'b0),
    .SRVAL_Q3(1'b0),
    .SRVAL_Q4(1'b0)) 
    ISERDESE2_inst_master
       (.BITSLIP(bitslip_cmd),
        .CE1(1'b1),
        .CE2(1'b1),
        .CLK(clk_bit),
        .CLKB(clkb),
        .CLKDIV(clk_word),
        .CLKDIVP(1'b0),
        .D(data_in_single),
        .DDLY(NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED),
        .DYNCLKDIVSEL(1'b0),
        .DYNCLKSEL(1'b0),
        .O(NLW_ISERDESE2_inst_master_O_UNCONNECTED),
        .OCLK(1'b0),
        .OCLKB(NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED),
        .OFB(NLW_ISERDESE2_inst_master_OFB_UNCONNECTED),
        .Q1(D[0]),
        .Q2(D[1]),
        .Q3(D[2]),
        .Q4(D[3]),
        .Q5(D[4]),
        .Q6(D[5]),
        .Q7(D[6]),
        .Q8(D[7]),
        .RST(sys_rst),
        .SHIFTIN1(NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED),
        .SHIFTIN2(NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED),
        .SHIFTOUT1(SHIFTOUT1),
        .SHIFTOUT2(SHIFTOUT2));
  (* BOX_TYPE = "PRIMITIVE" *) 
  (* OPT_MODIFIED = "MLO" *) 
  ISERDESE2 #(
    .DATA_RATE("DDR"),
    .DATA_WIDTH(14),
    .DYN_CLKDIV_INV_EN("FALSE"),
    .DYN_CLK_INV_EN("FALSE"),
    .INIT_Q1(1'b0),
    .INIT_Q2(1'b0),
    .INIT_Q3(1'b0),
    .INIT_Q4(1'b0),
    .INTERFACE_TYPE("NETWORKING"),
    .IOBDELAY("NONE"),
    .IS_CLKB_INVERTED(1'b1),
    .IS_CLKDIVP_INVERTED(1'b0),
    .IS_CLKDIV_INVERTED(1'b0),
    .IS_CLK_INVERTED(1'b0),
    .IS_D_INVERTED(1'b0),
    .IS_OCLKB_INVERTED(1'b0),
    .IS_OCLK_INVERTED(1'b0),
    .NUM_CE(2),
    .OFB_USED("FALSE"),
    .SERDES_MODE("SLAVE"),
    .SRVAL_Q1(1'b0),
    .SRVAL_Q2(1'b0),
    .SRVAL_Q3(1'b0),
    .SRVAL_Q4(1'b0)) 
    ISERDESE2_inst_slave
       (.BITSLIP(bitslip_cmd),
        .CE1(1'b1),
        .CE2(1'b1),
        .CLK(clk_bit),
        .CLKB(clkb),
        .CLKDIV(clk_word),
        .CLKDIVP(1'b0),
        .D(NLW_ISERDESE2_inst_slave_D_UNCONNECTED),
        .DDLY(NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED),
        .DYNCLKDIVSEL(1'b0),
        .DYNCLKSEL(1'b0),
        .O(NLW_ISERDESE2_inst_slave_O_UNCONNECTED),
        .OCLK(1'b0),
        .OCLKB(1'b0),
        .OFB(1'b0),
        .Q1(NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED),
        .Q2(NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED),
        .Q3(D[8]),
        .Q4(D[9]),
        .Q5(D[10]),
        .Q6(D[11]),
        .Q7(D[12]),
        .Q8(D[13]),
        .RST(sys_rst),
        .SHIFTIN1(SHIFTOUT1),
        .SHIFTIN2(SHIFTOUT2),
        .SHIFTOUT1(NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED),
        .SHIFTOUT2(NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED));
  (* BOX_TYPE = "PRIMITIVE" *) 
  (* CAPACITANCE = "DONT_CARE" *) 
  (* IBUF_DELAY_VALUE = "0" *) 
  (* IFD_DELAY_VALUE = "AUTO" *) 
  IBUFDS ibufds_inst
       (.I(lvds_dat_p),
        .IB(lvds_dat_n),
        .O(data_in_single));
endmodule

(* ORIG_REF_NAME = "Serdes_1x14_DDR" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_8
   (D,
    lvds_dat_p,
    lvds_dat_n,
    bitslip_cmd,
    clk_bit,
    clkb,
    clk_word,
    sys_rst);
  output [13:0]D;
  input [0:0]lvds_dat_p;
  input [0:0]lvds_dat_n;
  input bitslip_cmd;
  input clk_bit;
  input clkb;
  input clk_word;
  input sys_rst;

  wire [13:0]D;
  wire SHIFTOUT1;
  wire SHIFTOUT2;
  wire bitslip_cmd;
  wire clk_bit;
  wire clk_word;
  wire clkb;
  wire data_in_single;
  wire [0:0]lvds_dat_n;
  wire [0:0]lvds_dat_p;
  wire sys_rst;
  wire NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_O_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_OFB_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_D_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_O_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED;

  (* BOX_TYPE = "PRIMITIVE" *) 
  (* OPT_MODIFIED = "MLO" *) 
  ISERDESE2 #(
    .DATA_RATE("DDR"),
    .DATA_WIDTH(14),
    .DYN_CLKDIV_INV_EN("FALSE"),
    .DYN_CLK_INV_EN("FALSE"),
    .INIT_Q1(1'b0),
    .INIT_Q2(1'b0),
    .INIT_Q3(1'b0),
    .INIT_Q4(1'b0),
    .INTERFACE_TYPE("NETWORKING"),
    .IOBDELAY("NONE"),
    .IS_CLKB_INVERTED(1'b1),
    .IS_CLKDIVP_INVERTED(1'b0),
    .IS_CLKDIV_INVERTED(1'b0),
    .IS_CLK_INVERTED(1'b0),
    .IS_D_INVERTED(1'b0),
    .IS_OCLKB_INVERTED(1'b0),
    .IS_OCLK_INVERTED(1'b0),
    .NUM_CE(2),
    .OFB_USED("FALSE"),
    .SERDES_MODE("MASTER"),
    .SRVAL_Q1(1'b0),
    .SRVAL_Q2(1'b0),
    .SRVAL_Q3(1'b0),
    .SRVAL_Q4(1'b0)) 
    ISERDESE2_inst_master
       (.BITSLIP(bitslip_cmd),
        .CE1(1'b1),
        .CE2(1'b1),
        .CLK(clk_bit),
        .CLKB(clkb),
        .CLKDIV(clk_word),
        .CLKDIVP(1'b0),
        .D(data_in_single),
        .DDLY(NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED),
        .DYNCLKDIVSEL(1'b0),
        .DYNCLKSEL(1'b0),
        .O(NLW_ISERDESE2_inst_master_O_UNCONNECTED),
        .OCLK(1'b0),
        .OCLKB(NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED),
        .OFB(NLW_ISERDESE2_inst_master_OFB_UNCONNECTED),
        .Q1(D[0]),
        .Q2(D[1]),
        .Q3(D[2]),
        .Q4(D[3]),
        .Q5(D[4]),
        .Q6(D[5]),
        .Q7(D[6]),
        .Q8(D[7]),
        .RST(sys_rst),
        .SHIFTIN1(NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED),
        .SHIFTIN2(NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED),
        .SHIFTOUT1(SHIFTOUT1),
        .SHIFTOUT2(SHIFTOUT2));
  (* BOX_TYPE = "PRIMITIVE" *) 
  (* OPT_MODIFIED = "MLO" *) 
  ISERDESE2 #(
    .DATA_RATE("DDR"),
    .DATA_WIDTH(14),
    .DYN_CLKDIV_INV_EN("FALSE"),
    .DYN_CLK_INV_EN("FALSE"),
    .INIT_Q1(1'b0),
    .INIT_Q2(1'b0),
    .INIT_Q3(1'b0),
    .INIT_Q4(1'b0),
    .INTERFACE_TYPE("NETWORKING"),
    .IOBDELAY("NONE"),
    .IS_CLKB_INVERTED(1'b1),
    .IS_CLKDIVP_INVERTED(1'b0),
    .IS_CLKDIV_INVERTED(1'b0),
    .IS_CLK_INVERTED(1'b0),
    .IS_D_INVERTED(1'b0),
    .IS_OCLKB_INVERTED(1'b0),
    .IS_OCLK_INVERTED(1'b0),
    .NUM_CE(2),
    .OFB_USED("FALSE"),
    .SERDES_MODE("SLAVE"),
    .SRVAL_Q1(1'b0),
    .SRVAL_Q2(1'b0),
    .SRVAL_Q3(1'b0),
    .SRVAL_Q4(1'b0)) 
    ISERDESE2_inst_slave
       (.BITSLIP(bitslip_cmd),
        .CE1(1'b1),
        .CE2(1'b1),
        .CLK(clk_bit),
        .CLKB(clkb),
        .CLKDIV(clk_word),
        .CLKDIVP(1'b0),
        .D(NLW_ISERDESE2_inst_slave_D_UNCONNECTED),
        .DDLY(NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED),
        .DYNCLKDIVSEL(1'b0),
        .DYNCLKSEL(1'b0),
        .O(NLW_ISERDESE2_inst_slave_O_UNCONNECTED),
        .OCLK(1'b0),
        .OCLKB(1'b0),
        .OFB(1'b0),
        .Q1(NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED),
        .Q2(NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED),
        .Q3(D[8]),
        .Q4(D[9]),
        .Q5(D[10]),
        .Q6(D[11]),
        .Q7(D[12]),
        .Q8(D[13]),
        .RST(sys_rst),
        .SHIFTIN1(SHIFTOUT1),
        .SHIFTIN2(SHIFTOUT2),
        .SHIFTOUT1(NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED),
        .SHIFTOUT2(NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED));
  (* BOX_TYPE = "PRIMITIVE" *) 
  (* CAPACITANCE = "DONT_CARE" *) 
  (* IBUF_DELAY_VALUE = "0" *) 
  (* IFD_DELAY_VALUE = "AUTO" *) 
  IBUFDS ibufds_inst
       (.I(lvds_dat_p),
        .IB(lvds_dat_n),
        .O(data_in_single));
endmodule

(* ORIG_REF_NAME = "Serdes_1x14_DDR" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_9
   (lvds_dat_p,
    lvds_dat_n,
    bitslip_cmd,
    clk_bit,
    clkb,
    clk_word,
    sys_rst);
  input [0:0]lvds_dat_p;
  input [0:0]lvds_dat_n;
  input bitslip_cmd;
  input clk_bit;
  input clkb;
  input clk_word;
  input sys_rst;

  wire [13:0]ADCDataLine;
  wire SHIFTOUT1;
  wire SHIFTOUT2;
  wire bitslip_cmd;
  wire clk_bit;
  wire clk_word;
  wire clkb;
  wire data_in_single;
  wire [0:0]lvds_dat_n;
  wire [0:0]lvds_dat_p;
  wire sys_rst;
  wire NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_O_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_OFB_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_D_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_O_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED;
  wire NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED;

  (* BOX_TYPE = "PRIMITIVE" *) 
  (* OPT_MODIFIED = "MLO" *) 
  ISERDESE2 #(
    .DATA_RATE("DDR"),
    .DATA_WIDTH(14),
    .DYN_CLKDIV_INV_EN("FALSE"),
    .DYN_CLK_INV_EN("FALSE"),
    .INIT_Q1(1'b0),
    .INIT_Q2(1'b0),
    .INIT_Q3(1'b0),
    .INIT_Q4(1'b0),
    .INTERFACE_TYPE("NETWORKING"),
    .IOBDELAY("NONE"),
    .IS_CLKB_INVERTED(1'b1),
    .IS_CLKDIVP_INVERTED(1'b0),
    .IS_CLKDIV_INVERTED(1'b0),
    .IS_CLK_INVERTED(1'b0),
    .IS_D_INVERTED(1'b0),
    .IS_OCLKB_INVERTED(1'b0),
    .IS_OCLK_INVERTED(1'b0),
    .NUM_CE(2),
    .OFB_USED("FALSE"),
    .SERDES_MODE("MASTER"),
    .SRVAL_Q1(1'b0),
    .SRVAL_Q2(1'b0),
    .SRVAL_Q3(1'b0),
    .SRVAL_Q4(1'b0)) 
    ISERDESE2_inst_master
       (.BITSLIP(bitslip_cmd),
        .CE1(1'b1),
        .CE2(1'b1),
        .CLK(clk_bit),
        .CLKB(clkb),
        .CLKDIV(clk_word),
        .CLKDIVP(1'b0),
        .D(data_in_single),
        .DDLY(NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED),
        .DYNCLKDIVSEL(1'b0),
        .DYNCLKSEL(1'b0),
        .O(NLW_ISERDESE2_inst_master_O_UNCONNECTED),
        .OCLK(1'b0),
        .OCLKB(NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED),
        .OFB(NLW_ISERDESE2_inst_master_OFB_UNCONNECTED),
        .Q1(ADCDataLine[0]),
        .Q2(ADCDataLine[1]),
        .Q3(ADCDataLine[2]),
        .Q4(ADCDataLine[3]),
        .Q5(ADCDataLine[4]),
        .Q6(ADCDataLine[5]),
        .Q7(ADCDataLine[6]),
        .Q8(ADCDataLine[7]),
        .RST(sys_rst),
        .SHIFTIN1(NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED),
        .SHIFTIN2(NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED),
        .SHIFTOUT1(SHIFTOUT1),
        .SHIFTOUT2(SHIFTOUT2));
  (* BOX_TYPE = "PRIMITIVE" *) 
  (* OPT_MODIFIED = "MLO" *) 
  ISERDESE2 #(
    .DATA_RATE("DDR"),
    .DATA_WIDTH(14),
    .DYN_CLKDIV_INV_EN("FALSE"),
    .DYN_CLK_INV_EN("FALSE"),
    .INIT_Q1(1'b0),
    .INIT_Q2(1'b0),
    .INIT_Q3(1'b0),
    .INIT_Q4(1'b0),
    .INTERFACE_TYPE("NETWORKING"),
    .IOBDELAY("NONE"),
    .IS_CLKB_INVERTED(1'b1),
    .IS_CLKDIVP_INVERTED(1'b0),
    .IS_CLKDIV_INVERTED(1'b0),
    .IS_CLK_INVERTED(1'b0),
    .IS_D_INVERTED(1'b0),
    .IS_OCLKB_INVERTED(1'b0),
    .IS_OCLK_INVERTED(1'b0),
    .NUM_CE(2),
    .OFB_USED("FALSE"),
    .SERDES_MODE("SLAVE"),
    .SRVAL_Q1(1'b0),
    .SRVAL_Q2(1'b0),
    .SRVAL_Q3(1'b0),
    .SRVAL_Q4(1'b0)) 
    ISERDESE2_inst_slave
       (.BITSLIP(bitslip_cmd),
        .CE1(1'b1),
        .CE2(1'b1),
        .CLK(clk_bit),
        .CLKB(clkb),
        .CLKDIV(clk_word),
        .CLKDIVP(1'b0),
        .D(NLW_ISERDESE2_inst_slave_D_UNCONNECTED),
        .DDLY(NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED),
        .DYNCLKDIVSEL(1'b0),
        .DYNCLKSEL(1'b0),
        .O(NLW_ISERDESE2_inst_slave_O_UNCONNECTED),
        .OCLK(1'b0),
        .OCLKB(1'b0),
        .OFB(1'b0),
        .Q1(NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED),
        .Q2(NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED),
        .Q3(ADCDataLine[8]),
        .Q4(ADCDataLine[9]),
        .Q5(ADCDataLine[10]),
        .Q6(ADCDataLine[11]),
        .Q7(ADCDataLine[12]),
        .Q8(ADCDataLine[13]),
        .RST(sys_rst),
        .SHIFTIN1(SHIFTOUT1),
        .SHIFTIN2(SHIFTOUT2),
        .SHIFTOUT1(NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED),
        .SHIFTOUT2(NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED));
  (* BOX_TYPE = "PRIMITIVE" *) 
  (* CAPACITANCE = "DONT_CARE" *) 
  (* IBUF_DELAY_VALUE = "0" *) 
  (* IFD_DELAY_VALUE = "AUTO" *) 
  IBUFDS ibufds_inst
       (.I(lvds_dat_p),
        .IB(lvds_dat_n),
        .O(data_in_single));
endmodule

(* CHECK_LICENSE_TYPE = "design_1_AdcLVDS_Top_0_0,AdcLVDS_Top,{}" *) (* DowngradeIPIdentifiedWarnings = "yes" *) (* IP_DEFINITION_SOURCE = "package_project" *) 
(* X_CORE_INFO = "AdcLVDS_Top,Vivado 2020.2" *) 
(* NotValidForBitStream *)
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix
   (lvds_dco_p,
    lvds_dco_n,
    lvds_fco_p,
    lvds_fco_n,
    lvds_dat_p,
    lvds_dat_n,
    sys_rst,
    m_axis_aclk,
    m_axis_tvalid,
    m_axis_tdata,
    m_axis_tlast);
  (* X_INTERFACE_INFO = "xilinx.com:interface:diff_clock:1.0 lvds_dco CLK_P" *) input lvds_dco_p;
  (* X_INTERFACE_INFO = "xilinx.com:interface:diff_clock:1.0 lvds_dco CLK_N" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME lvds_dco, CAN_DEBUG false, FREQ_HZ 100000000" *) input lvds_dco_n;
  (* X_INTERFACE_INFO = "xilinx.com:interface:diff_clock:1.0 lvds_fco CLK_P" *) input lvds_fco_p;
  (* X_INTERFACE_INFO = "xilinx.com:interface:diff_clock:1.0 lvds_fco CLK_N" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME lvds_fco, CAN_DEBUG false, FREQ_HZ 100000000" *) input lvds_fco_n;
  input [7:0]lvds_dat_p;
  input [7:0]lvds_dat_n;
  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 sys_rst RST" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME sys_rst, POLARITY ACTIVE_HIGH, INSERT_VIP 0" *) input sys_rst;
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 m_axis_aclk CLK" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME m_axis_aclk, ASSOCIATED_BUSIF m_axis, FREQ_HZ 100000000, FREQ_TOLERANCE_HZ 0, PHASE 0.000, CLK_DOMAIN design_1_processing_system7_0_0_FCLK_CLK0, INSERT_VIP 0" *) input m_axis_aclk;
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 m_axis TVALID" *) output m_axis_tvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 m_axis TDATA" *) output [127:0]m_axis_tdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:axis:1.0 m_axis TLAST" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME m_axis, TDATA_NUM_BYTES 16, TDEST_WIDTH 0, TID_WIDTH 0, TUSER_WIDTH 0, HAS_TREADY 0, HAS_TSTRB 0, HAS_TKEEP 0, HAS_TLAST 1, FREQ_HZ 100000000, PHASE 0.000, CLK_DOMAIN design_1_processing_system7_0_0_FCLK_CLK0, LAYERED_METADATA undef, INSERT_VIP 0" *) output m_axis_tlast;

  wire \<const0> ;
  (* DIFF_TERM *) (* IBUF_LOW_PWR *) (* IOSTANDARD = "LVDS" *) wire [7:0]lvds_dat_n;
  (* DIFF_TERM *) (* IBUF_LOW_PWR *) (* IOSTANDARD = "LVDS" *) wire [7:0]lvds_dat_p;
  (* DIFF_TERM = 0 *) (* IBUF_LOW_PWR *) wire lvds_dco_n;
  (* DIFF_TERM = 0 *) (* IBUF_LOW_PWR *) wire lvds_dco_p;
  (* DIFF_TERM *) (* IBUF_LOW_PWR *) (* IOSTANDARD = "LVDS" *) wire lvds_fco_n;
  (* DIFF_TERM *) (* IBUF_LOW_PWR *) (* IOSTANDARD = "LVDS" *) wire lvds_fco_p;
  wire [127:0]m_axis_tdata;
  wire m_axis_tvalid;
  wire sys_rst;
  wire NLW_inst_m_axis_tlast_UNCONNECTED;

  assign m_axis_tlast = \<const0> ;
  GND GND
       (.G(\<const0> ));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLVDS_Top inst
       (.lvds_dat_n(lvds_dat_n),
        .lvds_dat_p(lvds_dat_p),
        .lvds_dco_n(lvds_dco_n),
        .lvds_dco_p(lvds_dco_p),
        .lvds_fco_n(lvds_fco_n),
        .lvds_fco_p(lvds_fco_p),
        .m_axis_aclk(1'b0),
        .m_axis_tdata(m_axis_tdata),
        .m_axis_tlast(NLW_inst_m_axis_tlast_UNCONNECTED),
        .m_axis_tvalid(m_axis_tvalid),
        .sys_rst(sys_rst));
endmodule
`ifndef GLBL
`define GLBL
`timescale  1 ps / 1 ps

module glbl ();

    parameter ROC_WIDTH = 100000;
    parameter TOC_WIDTH = 0;
    parameter GRES_WIDTH = 10000;
    parameter GRES_START = 10000;

//--------   STARTUP Globals --------------
    wire GSR;
    wire GTS;
    wire GWE;
    wire PRLD;
    wire GRESTORE;
    tri1 p_up_tmp;
    tri (weak1, strong0) PLL_LOCKG = p_up_tmp;

    wire PROGB_GLBL;
    wire CCLKO_GLBL;
    wire FCSBO_GLBL;
    wire [3:0] DO_GLBL;
    wire [3:0] DI_GLBL;
   
    reg GSR_int;
    reg GTS_int;
    reg PRLD_int;
    reg GRESTORE_int;

//--------   JTAG Globals --------------
    wire JTAG_TDO_GLBL;
    wire JTAG_TCK_GLBL;
    wire JTAG_TDI_GLBL;
    wire JTAG_TMS_GLBL;
    wire JTAG_TRST_GLBL;

    reg JTAG_CAPTURE_GLBL;
    reg JTAG_RESET_GLBL;
    reg JTAG_SHIFT_GLBL;
    reg JTAG_UPDATE_GLBL;
    reg JTAG_RUNTEST_GLBL;

    reg JTAG_SEL1_GLBL = 0;
    reg JTAG_SEL2_GLBL = 0 ;
    reg JTAG_SEL3_GLBL = 0;
    reg JTAG_SEL4_GLBL = 0;

    reg JTAG_USER_TDO1_GLBL = 1'bz;
    reg JTAG_USER_TDO2_GLBL = 1'bz;
    reg JTAG_USER_TDO3_GLBL = 1'bz;
    reg JTAG_USER_TDO4_GLBL = 1'bz;

    assign (strong1, weak0) GSR = GSR_int;
    assign (strong1, weak0) GTS = GTS_int;
    assign (weak1, weak0) PRLD = PRLD_int;
    assign (strong1, weak0) GRESTORE = GRESTORE_int;

    initial begin
	GSR_int = 1'b1;
	PRLD_int = 1'b1;
	#(ROC_WIDTH)
	GSR_int = 1'b0;
	PRLD_int = 1'b0;
    end

    initial begin
	GTS_int = 1'b1;
	#(TOC_WIDTH)
	GTS_int = 1'b0;
    end

    initial begin 
	GRESTORE_int = 1'b0;
	#(GRES_START);
	GRESTORE_int = 1'b1;
	#(GRES_WIDTH);
	GRESTORE_int = 1'b0;
    end

endmodule
`endif
