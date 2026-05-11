// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2020.2 (win64) Build 3064766 Wed Nov 18 09:12:45 MST 2020
// Date        : Sun Apr 19 15:53:31 2026
// Host        : DESKTOP-MM8H4FT running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode funcsim -rename_top decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix -prefix
//               decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_ design_1_auto_pc_1_sim_netlist.v
// Design      : design_1_auto_pc_1
// Purpose     : This verilog netlist is a functional simulation representation of the design and should not be modified
//               or synthesized. This netlist cannot be used for SDF annotated simulation.
// Device      : xc7z100ffg900-2
// --------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_data_fifo_v2_1_21_axic_fifo
   (dout,
    empty,
    SR,
    aresetn_0,
    m_axi_awvalid,
    length_counter_1_reg_1_sp_1,
    empty_fwft_i_reg,
    m_axi_wvalid,
    S_AXI_AREADY_I_reg,
    \areset_d_reg[1] ,
    aclk,
    m_axi_awlen,
    rd_en,
    aresetn,
    m_axi_awvalid_0,
    command_ongoing,
    m_axi_awready,
    length_counter_1_reg,
    first_mi_word,
    s_axi_wvalid,
    m_axi_wready,
    E,
    s_axi_awvalid,
    Q);
  output [3:0]dout;
  output empty;
  output [0:0]SR;
  output aresetn_0;
  output m_axi_awvalid;
  output length_counter_1_reg_1_sp_1;
  output empty_fwft_i_reg;
  output m_axi_wvalid;
  output S_AXI_AREADY_I_reg;
  output \areset_d_reg[1] ;
  input aclk;
  input [3:0]m_axi_awlen;
  input rd_en;
  input aresetn;
  input m_axi_awvalid_0;
  input command_ongoing;
  input m_axi_awready;
  input [1:0]length_counter_1_reg;
  input first_mi_word;
  input s_axi_wvalid;
  input m_axi_wready;
  input [0:0]E;
  input s_axi_awvalid;
  input [1:0]Q;

  wire [0:0]E;
  wire [1:0]Q;
  wire [0:0]SR;
  wire S_AXI_AREADY_I_reg;
  wire aclk;
  wire \areset_d_reg[1] ;
  wire aresetn;
  wire aresetn_0;
  wire command_ongoing;
  wire [3:0]dout;
  wire empty;
  wire empty_fwft_i_reg;
  wire first_mi_word;
  wire [1:0]length_counter_1_reg;
  wire length_counter_1_reg_1_sn_1;
  wire [3:0]m_axi_awlen;
  wire m_axi_awready;
  wire m_axi_awvalid;
  wire m_axi_awvalid_0;
  wire m_axi_wready;
  wire m_axi_wvalid;
  wire rd_en;
  wire s_axi_awvalid;
  wire s_axi_wvalid;

  assign length_counter_1_reg_1_sp_1 = length_counter_1_reg_1_sn_1;
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_data_fifo_v2_1_21_fifo_gen inst
       (.E(E),
        .Q(Q),
        .SR(SR),
        .S_AXI_AREADY_I_reg(S_AXI_AREADY_I_reg),
        .aclk(aclk),
        .\areset_d_reg[1] (\areset_d_reg[1] ),
        .aresetn(aresetn),
        .aresetn_0(aresetn_0),
        .command_ongoing(command_ongoing),
        .dout(dout),
        .empty(empty),
        .empty_fwft_i_reg(empty_fwft_i_reg),
        .first_mi_word(first_mi_word),
        .length_counter_1_reg(length_counter_1_reg),
        .length_counter_1_reg_1_sp_1(length_counter_1_reg_1_sn_1),
        .m_axi_awlen(m_axi_awlen),
        .m_axi_awready(m_axi_awready),
        .m_axi_awvalid(m_axi_awvalid),
        .m_axi_awvalid_0(m_axi_awvalid_0),
        .m_axi_wready(m_axi_wready),
        .m_axi_wvalid(m_axi_wvalid),
        .rd_en(rd_en),
        .s_axi_awvalid(s_axi_awvalid),
        .s_axi_wvalid(s_axi_wvalid));
endmodule

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_data_fifo_v2_1_21_fifo_gen
   (dout,
    empty,
    SR,
    aresetn_0,
    m_axi_awvalid,
    length_counter_1_reg_1_sp_1,
    empty_fwft_i_reg,
    m_axi_wvalid,
    S_AXI_AREADY_I_reg,
    \areset_d_reg[1] ,
    aclk,
    m_axi_awlen,
    rd_en,
    aresetn,
    m_axi_awvalid_0,
    command_ongoing,
    m_axi_awready,
    length_counter_1_reg,
    first_mi_word,
    s_axi_wvalid,
    m_axi_wready,
    E,
    s_axi_awvalid,
    Q);
  output [3:0]dout;
  output empty;
  output [0:0]SR;
  output aresetn_0;
  output m_axi_awvalid;
  output length_counter_1_reg_1_sp_1;
  output empty_fwft_i_reg;
  output m_axi_wvalid;
  output S_AXI_AREADY_I_reg;
  output \areset_d_reg[1] ;
  input aclk;
  input [3:0]m_axi_awlen;
  input rd_en;
  input aresetn;
  input m_axi_awvalid_0;
  input command_ongoing;
  input m_axi_awready;
  input [1:0]length_counter_1_reg;
  input first_mi_word;
  input s_axi_wvalid;
  input m_axi_wready;
  input [0:0]E;
  input s_axi_awvalid;
  input [1:0]Q;

  wire [0:0]E;
  wire [1:0]Q;
  wire [0:0]SR;
  wire S_AXI_AREADY_I_i_3_n_0;
  wire S_AXI_AREADY_I_reg;
  wire aclk;
  wire \areset_d_reg[1] ;
  wire aresetn;
  wire aresetn_0;
  wire cmd_push;
  wire command_ongoing;
  wire command_ongoing_i_2_n_0;
  wire [3:0]dout;
  wire empty;
  wire empty_fwft_i_reg;
  wire first_mi_word;
  wire full;
  wire [1:0]length_counter_1_reg;
  wire length_counter_1_reg_1_sn_1;
  wire [3:0]m_axi_awlen;
  wire m_axi_awready;
  wire m_axi_awvalid;
  wire m_axi_awvalid_0;
  wire m_axi_wready;
  wire m_axi_wvalid;
  wire rd_en;
  wire s_axi_awvalid;
  wire s_axi_wvalid;
  wire NLW_fifo_gen_inst_almost_empty_UNCONNECTED;
  wire NLW_fifo_gen_inst_almost_full_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_ar_dbiterr_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_ar_overflow_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_ar_prog_empty_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_ar_prog_full_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_ar_sbiterr_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_ar_underflow_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_aw_dbiterr_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_aw_overflow_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_aw_prog_empty_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_aw_prog_full_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_aw_sbiterr_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_aw_underflow_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_b_dbiterr_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_b_overflow_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_b_prog_empty_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_b_prog_full_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_b_sbiterr_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_b_underflow_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_r_dbiterr_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_r_overflow_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_r_prog_empty_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_r_prog_full_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_r_sbiterr_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_r_underflow_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_w_dbiterr_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_w_overflow_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_w_prog_empty_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_w_prog_full_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_w_sbiterr_UNCONNECTED;
  wire NLW_fifo_gen_inst_axi_w_underflow_UNCONNECTED;
  wire NLW_fifo_gen_inst_axis_dbiterr_UNCONNECTED;
  wire NLW_fifo_gen_inst_axis_overflow_UNCONNECTED;
  wire NLW_fifo_gen_inst_axis_prog_empty_UNCONNECTED;
  wire NLW_fifo_gen_inst_axis_prog_full_UNCONNECTED;
  wire NLW_fifo_gen_inst_axis_sbiterr_UNCONNECTED;
  wire NLW_fifo_gen_inst_axis_underflow_UNCONNECTED;
  wire NLW_fifo_gen_inst_dbiterr_UNCONNECTED;
  wire NLW_fifo_gen_inst_m_axi_arvalid_UNCONNECTED;
  wire NLW_fifo_gen_inst_m_axi_awvalid_UNCONNECTED;
  wire NLW_fifo_gen_inst_m_axi_bready_UNCONNECTED;
  wire NLW_fifo_gen_inst_m_axi_rready_UNCONNECTED;
  wire NLW_fifo_gen_inst_m_axi_wlast_UNCONNECTED;
  wire NLW_fifo_gen_inst_m_axi_wvalid_UNCONNECTED;
  wire NLW_fifo_gen_inst_m_axis_tlast_UNCONNECTED;
  wire NLW_fifo_gen_inst_m_axis_tvalid_UNCONNECTED;
  wire NLW_fifo_gen_inst_overflow_UNCONNECTED;
  wire NLW_fifo_gen_inst_prog_empty_UNCONNECTED;
  wire NLW_fifo_gen_inst_prog_full_UNCONNECTED;
  wire NLW_fifo_gen_inst_rd_rst_busy_UNCONNECTED;
  wire NLW_fifo_gen_inst_s_axi_arready_UNCONNECTED;
  wire NLW_fifo_gen_inst_s_axi_awready_UNCONNECTED;
  wire NLW_fifo_gen_inst_s_axi_bvalid_UNCONNECTED;
  wire NLW_fifo_gen_inst_s_axi_rlast_UNCONNECTED;
  wire NLW_fifo_gen_inst_s_axi_rvalid_UNCONNECTED;
  wire NLW_fifo_gen_inst_s_axi_wready_UNCONNECTED;
  wire NLW_fifo_gen_inst_s_axis_tready_UNCONNECTED;
  wire NLW_fifo_gen_inst_sbiterr_UNCONNECTED;
  wire NLW_fifo_gen_inst_underflow_UNCONNECTED;
  wire NLW_fifo_gen_inst_valid_UNCONNECTED;
  wire NLW_fifo_gen_inst_wr_ack_UNCONNECTED;
  wire NLW_fifo_gen_inst_wr_rst_busy_UNCONNECTED;
  wire [4:0]NLW_fifo_gen_inst_axi_ar_data_count_UNCONNECTED;
  wire [4:0]NLW_fifo_gen_inst_axi_ar_rd_data_count_UNCONNECTED;
  wire [4:0]NLW_fifo_gen_inst_axi_ar_wr_data_count_UNCONNECTED;
  wire [4:0]NLW_fifo_gen_inst_axi_aw_data_count_UNCONNECTED;
  wire [4:0]NLW_fifo_gen_inst_axi_aw_rd_data_count_UNCONNECTED;
  wire [4:0]NLW_fifo_gen_inst_axi_aw_wr_data_count_UNCONNECTED;
  wire [4:0]NLW_fifo_gen_inst_axi_b_data_count_UNCONNECTED;
  wire [4:0]NLW_fifo_gen_inst_axi_b_rd_data_count_UNCONNECTED;
  wire [4:0]NLW_fifo_gen_inst_axi_b_wr_data_count_UNCONNECTED;
  wire [10:0]NLW_fifo_gen_inst_axi_r_data_count_UNCONNECTED;
  wire [10:0]NLW_fifo_gen_inst_axi_r_rd_data_count_UNCONNECTED;
  wire [10:0]NLW_fifo_gen_inst_axi_r_wr_data_count_UNCONNECTED;
  wire [10:0]NLW_fifo_gen_inst_axi_w_data_count_UNCONNECTED;
  wire [10:0]NLW_fifo_gen_inst_axi_w_rd_data_count_UNCONNECTED;
  wire [10:0]NLW_fifo_gen_inst_axi_w_wr_data_count_UNCONNECTED;
  wire [10:0]NLW_fifo_gen_inst_axis_data_count_UNCONNECTED;
  wire [10:0]NLW_fifo_gen_inst_axis_rd_data_count_UNCONNECTED;
  wire [10:0]NLW_fifo_gen_inst_axis_wr_data_count_UNCONNECTED;
  wire [5:0]NLW_fifo_gen_inst_data_count_UNCONNECTED;
  wire [4:4]NLW_fifo_gen_inst_dout_UNCONNECTED;
  wire [31:0]NLW_fifo_gen_inst_m_axi_araddr_UNCONNECTED;
  wire [1:0]NLW_fifo_gen_inst_m_axi_arburst_UNCONNECTED;
  wire [3:0]NLW_fifo_gen_inst_m_axi_arcache_UNCONNECTED;
  wire [3:0]NLW_fifo_gen_inst_m_axi_arid_UNCONNECTED;
  wire [7:0]NLW_fifo_gen_inst_m_axi_arlen_UNCONNECTED;
  wire [1:0]NLW_fifo_gen_inst_m_axi_arlock_UNCONNECTED;
  wire [2:0]NLW_fifo_gen_inst_m_axi_arprot_UNCONNECTED;
  wire [3:0]NLW_fifo_gen_inst_m_axi_arqos_UNCONNECTED;
  wire [3:0]NLW_fifo_gen_inst_m_axi_arregion_UNCONNECTED;
  wire [2:0]NLW_fifo_gen_inst_m_axi_arsize_UNCONNECTED;
  wire [0:0]NLW_fifo_gen_inst_m_axi_aruser_UNCONNECTED;
  wire [31:0]NLW_fifo_gen_inst_m_axi_awaddr_UNCONNECTED;
  wire [1:0]NLW_fifo_gen_inst_m_axi_awburst_UNCONNECTED;
  wire [3:0]NLW_fifo_gen_inst_m_axi_awcache_UNCONNECTED;
  wire [3:0]NLW_fifo_gen_inst_m_axi_awid_UNCONNECTED;
  wire [7:0]NLW_fifo_gen_inst_m_axi_awlen_UNCONNECTED;
  wire [1:0]NLW_fifo_gen_inst_m_axi_awlock_UNCONNECTED;
  wire [2:0]NLW_fifo_gen_inst_m_axi_awprot_UNCONNECTED;
  wire [3:0]NLW_fifo_gen_inst_m_axi_awqos_UNCONNECTED;
  wire [3:0]NLW_fifo_gen_inst_m_axi_awregion_UNCONNECTED;
  wire [2:0]NLW_fifo_gen_inst_m_axi_awsize_UNCONNECTED;
  wire [0:0]NLW_fifo_gen_inst_m_axi_awuser_UNCONNECTED;
  wire [63:0]NLW_fifo_gen_inst_m_axi_wdata_UNCONNECTED;
  wire [3:0]NLW_fifo_gen_inst_m_axi_wid_UNCONNECTED;
  wire [7:0]NLW_fifo_gen_inst_m_axi_wstrb_UNCONNECTED;
  wire [0:0]NLW_fifo_gen_inst_m_axi_wuser_UNCONNECTED;
  wire [63:0]NLW_fifo_gen_inst_m_axis_tdata_UNCONNECTED;
  wire [3:0]NLW_fifo_gen_inst_m_axis_tdest_UNCONNECTED;
  wire [7:0]NLW_fifo_gen_inst_m_axis_tid_UNCONNECTED;
  wire [3:0]NLW_fifo_gen_inst_m_axis_tkeep_UNCONNECTED;
  wire [3:0]NLW_fifo_gen_inst_m_axis_tstrb_UNCONNECTED;
  wire [3:0]NLW_fifo_gen_inst_m_axis_tuser_UNCONNECTED;
  wire [5:0]NLW_fifo_gen_inst_rd_data_count_UNCONNECTED;
  wire [3:0]NLW_fifo_gen_inst_s_axi_bid_UNCONNECTED;
  wire [1:0]NLW_fifo_gen_inst_s_axi_bresp_UNCONNECTED;
  wire [0:0]NLW_fifo_gen_inst_s_axi_buser_UNCONNECTED;
  wire [63:0]NLW_fifo_gen_inst_s_axi_rdata_UNCONNECTED;
  wire [3:0]NLW_fifo_gen_inst_s_axi_rid_UNCONNECTED;
  wire [1:0]NLW_fifo_gen_inst_s_axi_rresp_UNCONNECTED;
  wire [0:0]NLW_fifo_gen_inst_s_axi_ruser_UNCONNECTED;
  wire [5:0]NLW_fifo_gen_inst_wr_data_count_UNCONNECTED;

  assign length_counter_1_reg_1_sp_1 = length_counter_1_reg_1_sn_1;
  LUT1 #(
    .INIT(2'h1)) 
    S_AXI_AREADY_I_i_1
       (.I0(aresetn),
        .O(SR));
  LUT6 #(
    .INIT(64'h22722272FFFF2272)) 
    S_AXI_AREADY_I_i_2
       (.I0(E),
        .I1(s_axi_awvalid),
        .I2(m_axi_awready),
        .I3(S_AXI_AREADY_I_i_3_n_0),
        .I4(Q[1]),
        .I5(Q[0]),
        .O(S_AXI_AREADY_I_reg));
  (* SOFT_HLUTNM = "soft_lutpair7" *) 
  LUT3 #(
    .INIT(8'h4F)) 
    S_AXI_AREADY_I_i_3
       (.I0(m_axi_awvalid_0),
        .I1(full),
        .I2(command_ongoing),
        .O(S_AXI_AREADY_I_i_3_n_0));
  (* SOFT_HLUTNM = "soft_lutpair5" *) 
  LUT5 #(
    .INIT(32'h00888A88)) 
    cmd_push_block_i_1
       (.I0(aresetn),
        .I1(m_axi_awvalid_0),
        .I2(full),
        .I3(command_ongoing),
        .I4(m_axi_awready),
        .O(aresetn_0));
  LUT6 #(
    .INIT(64'hF222FFFFD000D000)) 
    command_ongoing_i_1
       (.I0(Q[1]),
        .I1(Q[0]),
        .I2(E),
        .I3(s_axi_awvalid),
        .I4(command_ongoing_i_2_n_0),
        .I5(command_ongoing),
        .O(\areset_d_reg[1] ));
  (* SOFT_HLUTNM = "soft_lutpair5" *) 
  LUT4 #(
    .INIT(16'h8808)) 
    command_ongoing_i_2
       (.I0(m_axi_awready),
        .I1(command_ongoing),
        .I2(full),
        .I3(m_axi_awvalid_0),
        .O(command_ongoing_i_2_n_0));
  (* C_ADD_NGC_CONSTRAINT = "0" *) 
  (* C_APPLICATION_TYPE_AXIS = "0" *) 
  (* C_APPLICATION_TYPE_RACH = "0" *) 
  (* C_APPLICATION_TYPE_RDCH = "0" *) 
  (* C_APPLICATION_TYPE_WACH = "0" *) 
  (* C_APPLICATION_TYPE_WDCH = "0" *) 
  (* C_APPLICATION_TYPE_WRCH = "0" *) 
  (* C_AXIS_TDATA_WIDTH = "64" *) 
  (* C_AXIS_TDEST_WIDTH = "4" *) 
  (* C_AXIS_TID_WIDTH = "8" *) 
  (* C_AXIS_TKEEP_WIDTH = "4" *) 
  (* C_AXIS_TSTRB_WIDTH = "4" *) 
  (* C_AXIS_TUSER_WIDTH = "4" *) 
  (* C_AXIS_TYPE = "0" *) 
  (* C_AXI_ADDR_WIDTH = "32" *) 
  (* C_AXI_ARUSER_WIDTH = "1" *) 
  (* C_AXI_AWUSER_WIDTH = "1" *) 
  (* C_AXI_BUSER_WIDTH = "1" *) 
  (* C_AXI_DATA_WIDTH = "64" *) 
  (* C_AXI_ID_WIDTH = "4" *) 
  (* C_AXI_LEN_WIDTH = "8" *) 
  (* C_AXI_LOCK_WIDTH = "2" *) 
  (* C_AXI_RUSER_WIDTH = "1" *) 
  (* C_AXI_TYPE = "0" *) 
  (* C_AXI_WUSER_WIDTH = "1" *) 
  (* C_COMMON_CLOCK = "1" *) 
  (* C_COUNT_TYPE = "0" *) 
  (* C_DATA_COUNT_WIDTH = "6" *) 
  (* C_DEFAULT_VALUE = "BlankString" *) 
  (* C_DIN_WIDTH = "5" *) 
  (* C_DIN_WIDTH_AXIS = "1" *) 
  (* C_DIN_WIDTH_RACH = "32" *) 
  (* C_DIN_WIDTH_RDCH = "64" *) 
  (* C_DIN_WIDTH_WACH = "32" *) 
  (* C_DIN_WIDTH_WDCH = "64" *) 
  (* C_DIN_WIDTH_WRCH = "2" *) 
  (* C_DOUT_RST_VAL = "0" *) 
  (* C_DOUT_WIDTH = "5" *) 
  (* C_ENABLE_RLOCS = "0" *) 
  (* C_ENABLE_RST_SYNC = "1" *) 
  (* C_EN_SAFETY_CKT = "0" *) 
  (* C_ERROR_INJECTION_TYPE = "0" *) 
  (* C_ERROR_INJECTION_TYPE_AXIS = "0" *) 
  (* C_ERROR_INJECTION_TYPE_RACH = "0" *) 
  (* C_ERROR_INJECTION_TYPE_RDCH = "0" *) 
  (* C_ERROR_INJECTION_TYPE_WACH = "0" *) 
  (* C_ERROR_INJECTION_TYPE_WDCH = "0" *) 
  (* C_ERROR_INJECTION_TYPE_WRCH = "0" *) 
  (* C_FAMILY = "zynq" *) 
  (* C_FULL_FLAGS_RST_VAL = "0" *) 
  (* C_HAS_ALMOST_EMPTY = "0" *) 
  (* C_HAS_ALMOST_FULL = "0" *) 
  (* C_HAS_AXIS_TDATA = "0" *) 
  (* C_HAS_AXIS_TDEST = "0" *) 
  (* C_HAS_AXIS_TID = "0" *) 
  (* C_HAS_AXIS_TKEEP = "0" *) 
  (* C_HAS_AXIS_TLAST = "0" *) 
  (* C_HAS_AXIS_TREADY = "1" *) 
  (* C_HAS_AXIS_TSTRB = "0" *) 
  (* C_HAS_AXIS_TUSER = "0" *) 
  (* C_HAS_AXI_ARUSER = "0" *) 
  (* C_HAS_AXI_AWUSER = "0" *) 
  (* C_HAS_AXI_BUSER = "0" *) 
  (* C_HAS_AXI_ID = "0" *) 
  (* C_HAS_AXI_RD_CHANNEL = "0" *) 
  (* C_HAS_AXI_RUSER = "0" *) 
  (* C_HAS_AXI_WR_CHANNEL = "0" *) 
  (* C_HAS_AXI_WUSER = "0" *) 
  (* C_HAS_BACKUP = "0" *) 
  (* C_HAS_DATA_COUNT = "0" *) 
  (* C_HAS_DATA_COUNTS_AXIS = "0" *) 
  (* C_HAS_DATA_COUNTS_RACH = "0" *) 
  (* C_HAS_DATA_COUNTS_RDCH = "0" *) 
  (* C_HAS_DATA_COUNTS_WACH = "0" *) 
  (* C_HAS_DATA_COUNTS_WDCH = "0" *) 
  (* C_HAS_DATA_COUNTS_WRCH = "0" *) 
  (* C_HAS_INT_CLK = "0" *) 
  (* C_HAS_MASTER_CE = "0" *) 
  (* C_HAS_MEMINIT_FILE = "0" *) 
  (* C_HAS_OVERFLOW = "0" *) 
  (* C_HAS_PROG_FLAGS_AXIS = "0" *) 
  (* C_HAS_PROG_FLAGS_RACH = "0" *) 
  (* C_HAS_PROG_FLAGS_RDCH = "0" *) 
  (* C_HAS_PROG_FLAGS_WACH = "0" *) 
  (* C_HAS_PROG_FLAGS_WDCH = "0" *) 
  (* C_HAS_PROG_FLAGS_WRCH = "0" *) 
  (* C_HAS_RD_DATA_COUNT = "0" *) 
  (* C_HAS_RD_RST = "0" *) 
  (* C_HAS_RST = "1" *) 
  (* C_HAS_SLAVE_CE = "0" *) 
  (* C_HAS_SRST = "0" *) 
  (* C_HAS_UNDERFLOW = "0" *) 
  (* C_HAS_VALID = "0" *) 
  (* C_HAS_WR_ACK = "0" *) 
  (* C_HAS_WR_DATA_COUNT = "0" *) 
  (* C_HAS_WR_RST = "0" *) 
  (* C_IMPLEMENTATION_TYPE = "0" *) 
  (* C_IMPLEMENTATION_TYPE_AXIS = "1" *) 
  (* C_IMPLEMENTATION_TYPE_RACH = "1" *) 
  (* C_IMPLEMENTATION_TYPE_RDCH = "1" *) 
  (* C_IMPLEMENTATION_TYPE_WACH = "1" *) 
  (* C_IMPLEMENTATION_TYPE_WDCH = "1" *) 
  (* C_IMPLEMENTATION_TYPE_WRCH = "1" *) 
  (* C_INIT_WR_PNTR_VAL = "0" *) 
  (* C_INTERFACE_TYPE = "0" *) 
  (* C_MEMORY_TYPE = "2" *) 
  (* C_MIF_FILE_NAME = "BlankString" *) 
  (* C_MSGON_VAL = "1" *) 
  (* C_OPTIMIZATION_MODE = "0" *) 
  (* C_OVERFLOW_LOW = "0" *) 
  (* C_POWER_SAVING_MODE = "0" *) 
  (* C_PRELOAD_LATENCY = "0" *) 
  (* C_PRELOAD_REGS = "1" *) 
  (* C_PRIM_FIFO_TYPE = "512x36" *) 
  (* C_PRIM_FIFO_TYPE_AXIS = "512x36" *) 
  (* C_PRIM_FIFO_TYPE_RACH = "512x36" *) 
  (* C_PRIM_FIFO_TYPE_RDCH = "512x36" *) 
  (* C_PRIM_FIFO_TYPE_WACH = "512x36" *) 
  (* C_PRIM_FIFO_TYPE_WDCH = "512x36" *) 
  (* C_PRIM_FIFO_TYPE_WRCH = "512x36" *) 
  (* C_PROG_EMPTY_THRESH_ASSERT_VAL = "4" *) 
  (* C_PROG_EMPTY_THRESH_ASSERT_VAL_AXIS = "1022" *) 
  (* C_PROG_EMPTY_THRESH_ASSERT_VAL_RACH = "1022" *) 
  (* C_PROG_EMPTY_THRESH_ASSERT_VAL_RDCH = "1022" *) 
  (* C_PROG_EMPTY_THRESH_ASSERT_VAL_WACH = "1022" *) 
  (* C_PROG_EMPTY_THRESH_ASSERT_VAL_WDCH = "1022" *) 
  (* C_PROG_EMPTY_THRESH_ASSERT_VAL_WRCH = "1022" *) 
  (* C_PROG_EMPTY_THRESH_NEGATE_VAL = "5" *) 
  (* C_PROG_EMPTY_TYPE = "0" *) 
  (* C_PROG_EMPTY_TYPE_AXIS = "0" *) 
  (* C_PROG_EMPTY_TYPE_RACH = "0" *) 
  (* C_PROG_EMPTY_TYPE_RDCH = "0" *) 
  (* C_PROG_EMPTY_TYPE_WACH = "0" *) 
  (* C_PROG_EMPTY_TYPE_WDCH = "0" *) 
  (* C_PROG_EMPTY_TYPE_WRCH = "0" *) 
  (* C_PROG_FULL_THRESH_ASSERT_VAL = "31" *) 
  (* C_PROG_FULL_THRESH_ASSERT_VAL_AXIS = "1023" *) 
  (* C_PROG_FULL_THRESH_ASSERT_VAL_RACH = "1023" *) 
  (* C_PROG_FULL_THRESH_ASSERT_VAL_RDCH = "1023" *) 
  (* C_PROG_FULL_THRESH_ASSERT_VAL_WACH = "1023" *) 
  (* C_PROG_FULL_THRESH_ASSERT_VAL_WDCH = "1023" *) 
  (* C_PROG_FULL_THRESH_ASSERT_VAL_WRCH = "1023" *) 
  (* C_PROG_FULL_THRESH_NEGATE_VAL = "30" *) 
  (* C_PROG_FULL_TYPE = "0" *) 
  (* C_PROG_FULL_TYPE_AXIS = "0" *) 
  (* C_PROG_FULL_TYPE_RACH = "0" *) 
  (* C_PROG_FULL_TYPE_RDCH = "0" *) 
  (* C_PROG_FULL_TYPE_WACH = "0" *) 
  (* C_PROG_FULL_TYPE_WDCH = "0" *) 
  (* C_PROG_FULL_TYPE_WRCH = "0" *) 
  (* C_RACH_TYPE = "0" *) 
  (* C_RDCH_TYPE = "0" *) 
  (* C_RD_DATA_COUNT_WIDTH = "6" *) 
  (* C_RD_DEPTH = "32" *) 
  (* C_RD_FREQ = "1" *) 
  (* C_RD_PNTR_WIDTH = "5" *) 
  (* C_REG_SLICE_MODE_AXIS = "0" *) 
  (* C_REG_SLICE_MODE_RACH = "0" *) 
  (* C_REG_SLICE_MODE_RDCH = "0" *) 
  (* C_REG_SLICE_MODE_WACH = "0" *) 
  (* C_REG_SLICE_MODE_WDCH = "0" *) 
  (* C_REG_SLICE_MODE_WRCH = "0" *) 
  (* C_SELECT_XPM = "0" *) 
  (* C_SYNCHRONIZER_STAGE = "3" *) 
  (* C_UNDERFLOW_LOW = "0" *) 
  (* C_USE_COMMON_OVERFLOW = "0" *) 
  (* C_USE_COMMON_UNDERFLOW = "0" *) 
  (* C_USE_DEFAULT_SETTINGS = "0" *) 
  (* C_USE_DOUT_RST = "0" *) 
  (* C_USE_ECC = "0" *) 
  (* C_USE_ECC_AXIS = "0" *) 
  (* C_USE_ECC_RACH = "0" *) 
  (* C_USE_ECC_RDCH = "0" *) 
  (* C_USE_ECC_WACH = "0" *) 
  (* C_USE_ECC_WDCH = "0" *) 
  (* C_USE_ECC_WRCH = "0" *) 
  (* C_USE_EMBEDDED_REG = "0" *) 
  (* C_USE_FIFO16_FLAGS = "0" *) 
  (* C_USE_FWFT_DATA_COUNT = "1" *) 
  (* C_USE_PIPELINE_REG = "0" *) 
  (* C_VALID_LOW = "0" *) 
  (* C_WACH_TYPE = "0" *) 
  (* C_WDCH_TYPE = "0" *) 
  (* C_WRCH_TYPE = "0" *) 
  (* C_WR_ACK_LOW = "0" *) 
  (* C_WR_DATA_COUNT_WIDTH = "6" *) 
  (* C_WR_DEPTH = "32" *) 
  (* C_WR_DEPTH_AXIS = "1024" *) 
  (* C_WR_DEPTH_RACH = "16" *) 
  (* C_WR_DEPTH_RDCH = "1024" *) 
  (* C_WR_DEPTH_WACH = "16" *) 
  (* C_WR_DEPTH_WDCH = "1024" *) 
  (* C_WR_DEPTH_WRCH = "16" *) 
  (* C_WR_FREQ = "1" *) 
  (* C_WR_PNTR_WIDTH = "5" *) 
  (* C_WR_PNTR_WIDTH_AXIS = "10" *) 
  (* C_WR_PNTR_WIDTH_RACH = "4" *) 
  (* C_WR_PNTR_WIDTH_RDCH = "10" *) 
  (* C_WR_PNTR_WIDTH_WACH = "4" *) 
  (* C_WR_PNTR_WIDTH_WDCH = "10" *) 
  (* C_WR_PNTR_WIDTH_WRCH = "4" *) 
  (* C_WR_RESPONSE_LATENCY = "1" *) 
  (* KEEP_HIERARCHY = "soft" *) 
  (* is_du_within_envelope = "true" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_fifo_generator_v13_2_5 fifo_gen_inst
       (.almost_empty(NLW_fifo_gen_inst_almost_empty_UNCONNECTED),
        .almost_full(NLW_fifo_gen_inst_almost_full_UNCONNECTED),
        .axi_ar_data_count(NLW_fifo_gen_inst_axi_ar_data_count_UNCONNECTED[4:0]),
        .axi_ar_dbiterr(NLW_fifo_gen_inst_axi_ar_dbiterr_UNCONNECTED),
        .axi_ar_injectdbiterr(1'b0),
        .axi_ar_injectsbiterr(1'b0),
        .axi_ar_overflow(NLW_fifo_gen_inst_axi_ar_overflow_UNCONNECTED),
        .axi_ar_prog_empty(NLW_fifo_gen_inst_axi_ar_prog_empty_UNCONNECTED),
        .axi_ar_prog_empty_thresh({1'b0,1'b0,1'b0,1'b0}),
        .axi_ar_prog_full(NLW_fifo_gen_inst_axi_ar_prog_full_UNCONNECTED),
        .axi_ar_prog_full_thresh({1'b0,1'b0,1'b0,1'b0}),
        .axi_ar_rd_data_count(NLW_fifo_gen_inst_axi_ar_rd_data_count_UNCONNECTED[4:0]),
        .axi_ar_sbiterr(NLW_fifo_gen_inst_axi_ar_sbiterr_UNCONNECTED),
        .axi_ar_underflow(NLW_fifo_gen_inst_axi_ar_underflow_UNCONNECTED),
        .axi_ar_wr_data_count(NLW_fifo_gen_inst_axi_ar_wr_data_count_UNCONNECTED[4:0]),
        .axi_aw_data_count(NLW_fifo_gen_inst_axi_aw_data_count_UNCONNECTED[4:0]),
        .axi_aw_dbiterr(NLW_fifo_gen_inst_axi_aw_dbiterr_UNCONNECTED),
        .axi_aw_injectdbiterr(1'b0),
        .axi_aw_injectsbiterr(1'b0),
        .axi_aw_overflow(NLW_fifo_gen_inst_axi_aw_overflow_UNCONNECTED),
        .axi_aw_prog_empty(NLW_fifo_gen_inst_axi_aw_prog_empty_UNCONNECTED),
        .axi_aw_prog_empty_thresh({1'b0,1'b0,1'b0,1'b0}),
        .axi_aw_prog_full(NLW_fifo_gen_inst_axi_aw_prog_full_UNCONNECTED),
        .axi_aw_prog_full_thresh({1'b0,1'b0,1'b0,1'b0}),
        .axi_aw_rd_data_count(NLW_fifo_gen_inst_axi_aw_rd_data_count_UNCONNECTED[4:0]),
        .axi_aw_sbiterr(NLW_fifo_gen_inst_axi_aw_sbiterr_UNCONNECTED),
        .axi_aw_underflow(NLW_fifo_gen_inst_axi_aw_underflow_UNCONNECTED),
        .axi_aw_wr_data_count(NLW_fifo_gen_inst_axi_aw_wr_data_count_UNCONNECTED[4:0]),
        .axi_b_data_count(NLW_fifo_gen_inst_axi_b_data_count_UNCONNECTED[4:0]),
        .axi_b_dbiterr(NLW_fifo_gen_inst_axi_b_dbiterr_UNCONNECTED),
        .axi_b_injectdbiterr(1'b0),
        .axi_b_injectsbiterr(1'b0),
        .axi_b_overflow(NLW_fifo_gen_inst_axi_b_overflow_UNCONNECTED),
        .axi_b_prog_empty(NLW_fifo_gen_inst_axi_b_prog_empty_UNCONNECTED),
        .axi_b_prog_empty_thresh({1'b0,1'b0,1'b0,1'b0}),
        .axi_b_prog_full(NLW_fifo_gen_inst_axi_b_prog_full_UNCONNECTED),
        .axi_b_prog_full_thresh({1'b0,1'b0,1'b0,1'b0}),
        .axi_b_rd_data_count(NLW_fifo_gen_inst_axi_b_rd_data_count_UNCONNECTED[4:0]),
        .axi_b_sbiterr(NLW_fifo_gen_inst_axi_b_sbiterr_UNCONNECTED),
        .axi_b_underflow(NLW_fifo_gen_inst_axi_b_underflow_UNCONNECTED),
        .axi_b_wr_data_count(NLW_fifo_gen_inst_axi_b_wr_data_count_UNCONNECTED[4:0]),
        .axi_r_data_count(NLW_fifo_gen_inst_axi_r_data_count_UNCONNECTED[10:0]),
        .axi_r_dbiterr(NLW_fifo_gen_inst_axi_r_dbiterr_UNCONNECTED),
        .axi_r_injectdbiterr(1'b0),
        .axi_r_injectsbiterr(1'b0),
        .axi_r_overflow(NLW_fifo_gen_inst_axi_r_overflow_UNCONNECTED),
        .axi_r_prog_empty(NLW_fifo_gen_inst_axi_r_prog_empty_UNCONNECTED),
        .axi_r_prog_empty_thresh({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .axi_r_prog_full(NLW_fifo_gen_inst_axi_r_prog_full_UNCONNECTED),
        .axi_r_prog_full_thresh({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .axi_r_rd_data_count(NLW_fifo_gen_inst_axi_r_rd_data_count_UNCONNECTED[10:0]),
        .axi_r_sbiterr(NLW_fifo_gen_inst_axi_r_sbiterr_UNCONNECTED),
        .axi_r_underflow(NLW_fifo_gen_inst_axi_r_underflow_UNCONNECTED),
        .axi_r_wr_data_count(NLW_fifo_gen_inst_axi_r_wr_data_count_UNCONNECTED[10:0]),
        .axi_w_data_count(NLW_fifo_gen_inst_axi_w_data_count_UNCONNECTED[10:0]),
        .axi_w_dbiterr(NLW_fifo_gen_inst_axi_w_dbiterr_UNCONNECTED),
        .axi_w_injectdbiterr(1'b0),
        .axi_w_injectsbiterr(1'b0),
        .axi_w_overflow(NLW_fifo_gen_inst_axi_w_overflow_UNCONNECTED),
        .axi_w_prog_empty(NLW_fifo_gen_inst_axi_w_prog_empty_UNCONNECTED),
        .axi_w_prog_empty_thresh({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .axi_w_prog_full(NLW_fifo_gen_inst_axi_w_prog_full_UNCONNECTED),
        .axi_w_prog_full_thresh({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .axi_w_rd_data_count(NLW_fifo_gen_inst_axi_w_rd_data_count_UNCONNECTED[10:0]),
        .axi_w_sbiterr(NLW_fifo_gen_inst_axi_w_sbiterr_UNCONNECTED),
        .axi_w_underflow(NLW_fifo_gen_inst_axi_w_underflow_UNCONNECTED),
        .axi_w_wr_data_count(NLW_fifo_gen_inst_axi_w_wr_data_count_UNCONNECTED[10:0]),
        .axis_data_count(NLW_fifo_gen_inst_axis_data_count_UNCONNECTED[10:0]),
        .axis_dbiterr(NLW_fifo_gen_inst_axis_dbiterr_UNCONNECTED),
        .axis_injectdbiterr(1'b0),
        .axis_injectsbiterr(1'b0),
        .axis_overflow(NLW_fifo_gen_inst_axis_overflow_UNCONNECTED),
        .axis_prog_empty(NLW_fifo_gen_inst_axis_prog_empty_UNCONNECTED),
        .axis_prog_empty_thresh({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .axis_prog_full(NLW_fifo_gen_inst_axis_prog_full_UNCONNECTED),
        .axis_prog_full_thresh({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .axis_rd_data_count(NLW_fifo_gen_inst_axis_rd_data_count_UNCONNECTED[10:0]),
        .axis_sbiterr(NLW_fifo_gen_inst_axis_sbiterr_UNCONNECTED),
        .axis_underflow(NLW_fifo_gen_inst_axis_underflow_UNCONNECTED),
        .axis_wr_data_count(NLW_fifo_gen_inst_axis_wr_data_count_UNCONNECTED[10:0]),
        .backup(1'b0),
        .backup_marker(1'b0),
        .clk(aclk),
        .data_count(NLW_fifo_gen_inst_data_count_UNCONNECTED[5:0]),
        .dbiterr(NLW_fifo_gen_inst_dbiterr_UNCONNECTED),
        .din({1'b0,m_axi_awlen}),
        .dout({NLW_fifo_gen_inst_dout_UNCONNECTED[4],dout}),
        .empty(empty),
        .full(full),
        .injectdbiterr(1'b0),
        .injectsbiterr(1'b0),
        .int_clk(1'b0),
        .m_aclk(1'b0),
        .m_aclk_en(1'b0),
        .m_axi_araddr(NLW_fifo_gen_inst_m_axi_araddr_UNCONNECTED[31:0]),
        .m_axi_arburst(NLW_fifo_gen_inst_m_axi_arburst_UNCONNECTED[1:0]),
        .m_axi_arcache(NLW_fifo_gen_inst_m_axi_arcache_UNCONNECTED[3:0]),
        .m_axi_arid(NLW_fifo_gen_inst_m_axi_arid_UNCONNECTED[3:0]),
        .m_axi_arlen(NLW_fifo_gen_inst_m_axi_arlen_UNCONNECTED[7:0]),
        .m_axi_arlock(NLW_fifo_gen_inst_m_axi_arlock_UNCONNECTED[1:0]),
        .m_axi_arprot(NLW_fifo_gen_inst_m_axi_arprot_UNCONNECTED[2:0]),
        .m_axi_arqos(NLW_fifo_gen_inst_m_axi_arqos_UNCONNECTED[3:0]),
        .m_axi_arready(1'b0),
        .m_axi_arregion(NLW_fifo_gen_inst_m_axi_arregion_UNCONNECTED[3:0]),
        .m_axi_arsize(NLW_fifo_gen_inst_m_axi_arsize_UNCONNECTED[2:0]),
        .m_axi_aruser(NLW_fifo_gen_inst_m_axi_aruser_UNCONNECTED[0]),
        .m_axi_arvalid(NLW_fifo_gen_inst_m_axi_arvalid_UNCONNECTED),
        .m_axi_awaddr(NLW_fifo_gen_inst_m_axi_awaddr_UNCONNECTED[31:0]),
        .m_axi_awburst(NLW_fifo_gen_inst_m_axi_awburst_UNCONNECTED[1:0]),
        .m_axi_awcache(NLW_fifo_gen_inst_m_axi_awcache_UNCONNECTED[3:0]),
        .m_axi_awid(NLW_fifo_gen_inst_m_axi_awid_UNCONNECTED[3:0]),
        .m_axi_awlen(NLW_fifo_gen_inst_m_axi_awlen_UNCONNECTED[7:0]),
        .m_axi_awlock(NLW_fifo_gen_inst_m_axi_awlock_UNCONNECTED[1:0]),
        .m_axi_awprot(NLW_fifo_gen_inst_m_axi_awprot_UNCONNECTED[2:0]),
        .m_axi_awqos(NLW_fifo_gen_inst_m_axi_awqos_UNCONNECTED[3:0]),
        .m_axi_awready(1'b0),
        .m_axi_awregion(NLW_fifo_gen_inst_m_axi_awregion_UNCONNECTED[3:0]),
        .m_axi_awsize(NLW_fifo_gen_inst_m_axi_awsize_UNCONNECTED[2:0]),
        .m_axi_awuser(NLW_fifo_gen_inst_m_axi_awuser_UNCONNECTED[0]),
        .m_axi_awvalid(NLW_fifo_gen_inst_m_axi_awvalid_UNCONNECTED),
        .m_axi_bid({1'b0,1'b0,1'b0,1'b0}),
        .m_axi_bready(NLW_fifo_gen_inst_m_axi_bready_UNCONNECTED),
        .m_axi_bresp({1'b0,1'b0}),
        .m_axi_buser(1'b0),
        .m_axi_bvalid(1'b0),
        .m_axi_rdata({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .m_axi_rid({1'b0,1'b0,1'b0,1'b0}),
        .m_axi_rlast(1'b0),
        .m_axi_rready(NLW_fifo_gen_inst_m_axi_rready_UNCONNECTED),
        .m_axi_rresp({1'b0,1'b0}),
        .m_axi_ruser(1'b0),
        .m_axi_rvalid(1'b0),
        .m_axi_wdata(NLW_fifo_gen_inst_m_axi_wdata_UNCONNECTED[63:0]),
        .m_axi_wid(NLW_fifo_gen_inst_m_axi_wid_UNCONNECTED[3:0]),
        .m_axi_wlast(NLW_fifo_gen_inst_m_axi_wlast_UNCONNECTED),
        .m_axi_wready(1'b0),
        .m_axi_wstrb(NLW_fifo_gen_inst_m_axi_wstrb_UNCONNECTED[7:0]),
        .m_axi_wuser(NLW_fifo_gen_inst_m_axi_wuser_UNCONNECTED[0]),
        .m_axi_wvalid(NLW_fifo_gen_inst_m_axi_wvalid_UNCONNECTED),
        .m_axis_tdata(NLW_fifo_gen_inst_m_axis_tdata_UNCONNECTED[63:0]),
        .m_axis_tdest(NLW_fifo_gen_inst_m_axis_tdest_UNCONNECTED[3:0]),
        .m_axis_tid(NLW_fifo_gen_inst_m_axis_tid_UNCONNECTED[7:0]),
        .m_axis_tkeep(NLW_fifo_gen_inst_m_axis_tkeep_UNCONNECTED[3:0]),
        .m_axis_tlast(NLW_fifo_gen_inst_m_axis_tlast_UNCONNECTED),
        .m_axis_tready(1'b0),
        .m_axis_tstrb(NLW_fifo_gen_inst_m_axis_tstrb_UNCONNECTED[3:0]),
        .m_axis_tuser(NLW_fifo_gen_inst_m_axis_tuser_UNCONNECTED[3:0]),
        .m_axis_tvalid(NLW_fifo_gen_inst_m_axis_tvalid_UNCONNECTED),
        .overflow(NLW_fifo_gen_inst_overflow_UNCONNECTED),
        .prog_empty(NLW_fifo_gen_inst_prog_empty_UNCONNECTED),
        .prog_empty_thresh({1'b0,1'b0,1'b0,1'b0,1'b0}),
        .prog_empty_thresh_assert({1'b0,1'b0,1'b0,1'b0,1'b0}),
        .prog_empty_thresh_negate({1'b0,1'b0,1'b0,1'b0,1'b0}),
        .prog_full(NLW_fifo_gen_inst_prog_full_UNCONNECTED),
        .prog_full_thresh({1'b0,1'b0,1'b0,1'b0,1'b0}),
        .prog_full_thresh_assert({1'b0,1'b0,1'b0,1'b0,1'b0}),
        .prog_full_thresh_negate({1'b0,1'b0,1'b0,1'b0,1'b0}),
        .rd_clk(1'b0),
        .rd_data_count(NLW_fifo_gen_inst_rd_data_count_UNCONNECTED[5:0]),
        .rd_en(rd_en),
        .rd_rst(1'b0),
        .rd_rst_busy(NLW_fifo_gen_inst_rd_rst_busy_UNCONNECTED),
        .rst(SR),
        .s_aclk(1'b0),
        .s_aclk_en(1'b0),
        .s_aresetn(1'b0),
        .s_axi_araddr({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .s_axi_arburst({1'b0,1'b0}),
        .s_axi_arcache({1'b0,1'b0,1'b0,1'b0}),
        .s_axi_arid({1'b0,1'b0,1'b0,1'b0}),
        .s_axi_arlen({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .s_axi_arlock({1'b0,1'b0}),
        .s_axi_arprot({1'b0,1'b0,1'b0}),
        .s_axi_arqos({1'b0,1'b0,1'b0,1'b0}),
        .s_axi_arready(NLW_fifo_gen_inst_s_axi_arready_UNCONNECTED),
        .s_axi_arregion({1'b0,1'b0,1'b0,1'b0}),
        .s_axi_arsize({1'b0,1'b0,1'b0}),
        .s_axi_aruser(1'b0),
        .s_axi_arvalid(1'b0),
        .s_axi_awaddr({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .s_axi_awburst({1'b0,1'b0}),
        .s_axi_awcache({1'b0,1'b0,1'b0,1'b0}),
        .s_axi_awid({1'b0,1'b0,1'b0,1'b0}),
        .s_axi_awlen({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .s_axi_awlock({1'b0,1'b0}),
        .s_axi_awprot({1'b0,1'b0,1'b0}),
        .s_axi_awqos({1'b0,1'b0,1'b0,1'b0}),
        .s_axi_awready(NLW_fifo_gen_inst_s_axi_awready_UNCONNECTED),
        .s_axi_awregion({1'b0,1'b0,1'b0,1'b0}),
        .s_axi_awsize({1'b0,1'b0,1'b0}),
        .s_axi_awuser(1'b0),
        .s_axi_awvalid(1'b0),
        .s_axi_bid(NLW_fifo_gen_inst_s_axi_bid_UNCONNECTED[3:0]),
        .s_axi_bready(1'b0),
        .s_axi_bresp(NLW_fifo_gen_inst_s_axi_bresp_UNCONNECTED[1:0]),
        .s_axi_buser(NLW_fifo_gen_inst_s_axi_buser_UNCONNECTED[0]),
        .s_axi_bvalid(NLW_fifo_gen_inst_s_axi_bvalid_UNCONNECTED),
        .s_axi_rdata(NLW_fifo_gen_inst_s_axi_rdata_UNCONNECTED[63:0]),
        .s_axi_rid(NLW_fifo_gen_inst_s_axi_rid_UNCONNECTED[3:0]),
        .s_axi_rlast(NLW_fifo_gen_inst_s_axi_rlast_UNCONNECTED),
        .s_axi_rready(1'b0),
        .s_axi_rresp(NLW_fifo_gen_inst_s_axi_rresp_UNCONNECTED[1:0]),
        .s_axi_ruser(NLW_fifo_gen_inst_s_axi_ruser_UNCONNECTED[0]),
        .s_axi_rvalid(NLW_fifo_gen_inst_s_axi_rvalid_UNCONNECTED),
        .s_axi_wdata({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .s_axi_wid({1'b0,1'b0,1'b0,1'b0}),
        .s_axi_wlast(1'b0),
        .s_axi_wready(NLW_fifo_gen_inst_s_axi_wready_UNCONNECTED),
        .s_axi_wstrb({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .s_axi_wuser(1'b0),
        .s_axi_wvalid(1'b0),
        .s_axis_tdata({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .s_axis_tdest({1'b0,1'b0,1'b0,1'b0}),
        .s_axis_tid({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .s_axis_tkeep({1'b0,1'b0,1'b0,1'b0}),
        .s_axis_tlast(1'b0),
        .s_axis_tready(NLW_fifo_gen_inst_s_axis_tready_UNCONNECTED),
        .s_axis_tstrb({1'b0,1'b0,1'b0,1'b0}),
        .s_axis_tuser({1'b0,1'b0,1'b0,1'b0}),
        .s_axis_tvalid(1'b0),
        .sbiterr(NLW_fifo_gen_inst_sbiterr_UNCONNECTED),
        .sleep(1'b0),
        .srst(1'b0),
        .underflow(NLW_fifo_gen_inst_underflow_UNCONNECTED),
        .valid(NLW_fifo_gen_inst_valid_UNCONNECTED),
        .wr_ack(NLW_fifo_gen_inst_wr_ack_UNCONNECTED),
        .wr_clk(1'b0),
        .wr_data_count(NLW_fifo_gen_inst_wr_data_count_UNCONNECTED[5:0]),
        .wr_en(cmd_push),
        .wr_rst(1'b0),
        .wr_rst_busy(NLW_fifo_gen_inst_wr_rst_busy_UNCONNECTED));
  (* SOFT_HLUTNM = "soft_lutpair7" *) 
  LUT3 #(
    .INIT(8'h02)) 
    fifo_gen_inst_i_1
       (.I0(command_ongoing),
        .I1(full),
        .I2(m_axi_awvalid_0),
        .O(cmd_push));
  LUT6 #(
    .INIT(64'hE4E4CC664E4ECC66)) 
    \length_counter_1[1]_i_1 
       (.I0(empty_fwft_i_reg),
        .I1(length_counter_1_reg[1]),
        .I2(dout[1]),
        .I3(length_counter_1_reg[0]),
        .I4(first_mi_word),
        .I5(dout[0]),
        .O(length_counter_1_reg_1_sn_1));
  LUT3 #(
    .INIT(8'hA2)) 
    m_axi_awvalid_INST_0
       (.I0(command_ongoing),
        .I1(full),
        .I2(m_axi_awvalid_0),
        .O(m_axi_awvalid));
  (* SOFT_HLUTNM = "soft_lutpair6" *) 
  LUT2 #(
    .INIT(4'h2)) 
    m_axi_wvalid_INST_0
       (.I0(s_axi_wvalid),
        .I1(empty),
        .O(m_axi_wvalid));
  (* SOFT_HLUTNM = "soft_lutpair6" *) 
  LUT3 #(
    .INIT(8'h40)) 
    s_axi_wready_INST_0
       (.I0(empty),
        .I1(s_axi_wvalid),
        .I2(m_axi_wready),
        .O(empty_fwft_i_reg));
endmodule

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_protocol_converter_v2_1_22_a_axi3_conv
   (dout,
    empty,
    SR,
    m_axi_awlen,
    m_axi_awlock,
    E,
    m_axi_awvalid,
    length_counter_1_reg_1_sp_1,
    empty_fwft_i_reg,
    m_axi_wvalid,
    m_axi_awaddr,
    m_axi_awsize,
    m_axi_awburst,
    m_axi_awcache,
    m_axi_awprot,
    m_axi_awqos,
    aclk,
    rd_en,
    s_axi_awlock,
    aresetn,
    m_axi_awready,
    length_counter_1_reg,
    first_mi_word,
    s_axi_wvalid,
    m_axi_wready,
    s_axi_awvalid,
    s_axi_awaddr,
    s_axi_awlen,
    s_axi_awsize,
    s_axi_awburst,
    s_axi_awcache,
    s_axi_awprot,
    s_axi_awqos);
  output [3:0]dout;
  output empty;
  output [0:0]SR;
  output [3:0]m_axi_awlen;
  output [0:0]m_axi_awlock;
  output [0:0]E;
  output m_axi_awvalid;
  output length_counter_1_reg_1_sp_1;
  output empty_fwft_i_reg;
  output m_axi_wvalid;
  output [31:0]m_axi_awaddr;
  output [2:0]m_axi_awsize;
  output [1:0]m_axi_awburst;
  output [3:0]m_axi_awcache;
  output [2:0]m_axi_awprot;
  output [3:0]m_axi_awqos;
  input aclk;
  input rd_en;
  input [0:0]s_axi_awlock;
  input aresetn;
  input m_axi_awready;
  input [1:0]length_counter_1_reg;
  input first_mi_word;
  input s_axi_wvalid;
  input m_axi_wready;
  input s_axi_awvalid;
  input [31:0]s_axi_awaddr;
  input [3:0]s_axi_awlen;
  input [2:0]s_axi_awsize;
  input [1:0]s_axi_awburst;
  input [3:0]s_axi_awcache;
  input [2:0]s_axi_awprot;
  input [3:0]s_axi_awqos;

  wire [0:0]E;
  wire [0:0]SR;
  wire \USE_BURSTS.cmd_queue_n_11 ;
  wire \USE_BURSTS.cmd_queue_n_12 ;
  wire \USE_BURSTS.cmd_queue_n_6 ;
  wire aclk;
  wire [1:0]areset_d;
  wire aresetn;
  wire cmd_push_block_reg_n_0;
  wire command_ongoing;
  wire [3:0]dout;
  wire empty;
  wire empty_fwft_i_reg;
  wire first_mi_word;
  wire [1:0]length_counter_1_reg;
  wire length_counter_1_reg_1_sn_1;
  wire [31:0]m_axi_awaddr;
  wire [1:0]m_axi_awburst;
  wire [3:0]m_axi_awcache;
  wire [3:0]m_axi_awlen;
  wire [0:0]m_axi_awlock;
  wire [2:0]m_axi_awprot;
  wire [3:0]m_axi_awqos;
  wire m_axi_awready;
  wire [2:0]m_axi_awsize;
  wire m_axi_awvalid;
  wire m_axi_wready;
  wire m_axi_wvalid;
  wire rd_en;
  wire [31:0]s_axi_awaddr;
  wire [1:0]s_axi_awburst;
  wire [3:0]s_axi_awcache;
  wire [3:0]s_axi_awlen;
  wire [0:0]s_axi_awlock;
  wire [2:0]s_axi_awprot;
  wire [3:0]s_axi_awqos;
  wire [2:0]s_axi_awsize;
  wire s_axi_awvalid;
  wire s_axi_wvalid;

  assign length_counter_1_reg_1_sp_1 = length_counter_1_reg_1_sn_1;
  FDRE \S_AXI_AADDR_Q_reg[0] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[0]),
        .Q(m_axi_awaddr[0]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[10] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[10]),
        .Q(m_axi_awaddr[10]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[11] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[11]),
        .Q(m_axi_awaddr[11]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[12] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[12]),
        .Q(m_axi_awaddr[12]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[13] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[13]),
        .Q(m_axi_awaddr[13]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[14] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[14]),
        .Q(m_axi_awaddr[14]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[15] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[15]),
        .Q(m_axi_awaddr[15]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[16] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[16]),
        .Q(m_axi_awaddr[16]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[17] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[17]),
        .Q(m_axi_awaddr[17]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[18] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[18]),
        .Q(m_axi_awaddr[18]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[19] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[19]),
        .Q(m_axi_awaddr[19]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[1] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[1]),
        .Q(m_axi_awaddr[1]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[20] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[20]),
        .Q(m_axi_awaddr[20]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[21] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[21]),
        .Q(m_axi_awaddr[21]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[22] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[22]),
        .Q(m_axi_awaddr[22]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[23] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[23]),
        .Q(m_axi_awaddr[23]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[24] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[24]),
        .Q(m_axi_awaddr[24]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[25] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[25]),
        .Q(m_axi_awaddr[25]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[26] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[26]),
        .Q(m_axi_awaddr[26]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[27] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[27]),
        .Q(m_axi_awaddr[27]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[28] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[28]),
        .Q(m_axi_awaddr[28]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[29] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[29]),
        .Q(m_axi_awaddr[29]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[2] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[2]),
        .Q(m_axi_awaddr[2]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[30] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[30]),
        .Q(m_axi_awaddr[30]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[31] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[31]),
        .Q(m_axi_awaddr[31]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[3] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[3]),
        .Q(m_axi_awaddr[3]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[4] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[4]),
        .Q(m_axi_awaddr[4]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[5] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[5]),
        .Q(m_axi_awaddr[5]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[6] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[6]),
        .Q(m_axi_awaddr[6]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[7] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[7]),
        .Q(m_axi_awaddr[7]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[8] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[8]),
        .Q(m_axi_awaddr[8]),
        .R(SR));
  FDRE \S_AXI_AADDR_Q_reg[9] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awaddr[9]),
        .Q(m_axi_awaddr[9]),
        .R(SR));
  FDRE \S_AXI_ABURST_Q_reg[0] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awburst[0]),
        .Q(m_axi_awburst[0]),
        .R(SR));
  FDRE \S_AXI_ABURST_Q_reg[1] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awburst[1]),
        .Q(m_axi_awburst[1]),
        .R(SR));
  FDRE \S_AXI_ACACHE_Q_reg[0] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awcache[0]),
        .Q(m_axi_awcache[0]),
        .R(SR));
  FDRE \S_AXI_ACACHE_Q_reg[1] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awcache[1]),
        .Q(m_axi_awcache[1]),
        .R(SR));
  FDRE \S_AXI_ACACHE_Q_reg[2] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awcache[2]),
        .Q(m_axi_awcache[2]),
        .R(SR));
  FDRE \S_AXI_ACACHE_Q_reg[3] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awcache[3]),
        .Q(m_axi_awcache[3]),
        .R(SR));
  FDRE \S_AXI_ALEN_Q_reg[0] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awlen[0]),
        .Q(m_axi_awlen[0]),
        .R(SR));
  FDRE \S_AXI_ALEN_Q_reg[1] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awlen[1]),
        .Q(m_axi_awlen[1]),
        .R(SR));
  FDRE \S_AXI_ALEN_Q_reg[2] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awlen[2]),
        .Q(m_axi_awlen[2]),
        .R(SR));
  FDRE \S_AXI_ALEN_Q_reg[3] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awlen[3]),
        .Q(m_axi_awlen[3]),
        .R(SR));
  FDRE \S_AXI_ALOCK_Q_reg[0] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awlock),
        .Q(m_axi_awlock),
        .R(SR));
  FDRE \S_AXI_APROT_Q_reg[0] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awprot[0]),
        .Q(m_axi_awprot[0]),
        .R(SR));
  FDRE \S_AXI_APROT_Q_reg[1] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awprot[1]),
        .Q(m_axi_awprot[1]),
        .R(SR));
  FDRE \S_AXI_APROT_Q_reg[2] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awprot[2]),
        .Q(m_axi_awprot[2]),
        .R(SR));
  FDRE \S_AXI_AQOS_Q_reg[0] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awqos[0]),
        .Q(m_axi_awqos[0]),
        .R(SR));
  FDRE \S_AXI_AQOS_Q_reg[1] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awqos[1]),
        .Q(m_axi_awqos[1]),
        .R(SR));
  FDRE \S_AXI_AQOS_Q_reg[2] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awqos[2]),
        .Q(m_axi_awqos[2]),
        .R(SR));
  FDRE \S_AXI_AQOS_Q_reg[3] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awqos[3]),
        .Q(m_axi_awqos[3]),
        .R(SR));
  FDRE #(
    .INIT(1'b0)) 
    S_AXI_AREADY_I_reg
       (.C(aclk),
        .CE(1'b1),
        .D(\USE_BURSTS.cmd_queue_n_11 ),
        .Q(E),
        .R(SR));
  FDRE \S_AXI_ASIZE_Q_reg[0] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awsize[0]),
        .Q(m_axi_awsize[0]),
        .R(SR));
  FDRE \S_AXI_ASIZE_Q_reg[1] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awsize[1]),
        .Q(m_axi_awsize[1]),
        .R(SR));
  FDRE \S_AXI_ASIZE_Q_reg[2] 
       (.C(aclk),
        .CE(E),
        .D(s_axi_awsize[2]),
        .Q(m_axi_awsize[2]),
        .R(SR));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_data_fifo_v2_1_21_axic_fifo \USE_BURSTS.cmd_queue 
       (.E(E),
        .Q(areset_d),
        .SR(SR),
        .S_AXI_AREADY_I_reg(\USE_BURSTS.cmd_queue_n_11 ),
        .aclk(aclk),
        .\areset_d_reg[1] (\USE_BURSTS.cmd_queue_n_12 ),
        .aresetn(aresetn),
        .aresetn_0(\USE_BURSTS.cmd_queue_n_6 ),
        .command_ongoing(command_ongoing),
        .dout(dout),
        .empty(empty),
        .empty_fwft_i_reg(empty_fwft_i_reg),
        .first_mi_word(first_mi_word),
        .length_counter_1_reg(length_counter_1_reg),
        .length_counter_1_reg_1_sp_1(length_counter_1_reg_1_sn_1),
        .m_axi_awlen(m_axi_awlen),
        .m_axi_awready(m_axi_awready),
        .m_axi_awvalid(m_axi_awvalid),
        .m_axi_awvalid_0(cmd_push_block_reg_n_0),
        .m_axi_wready(m_axi_wready),
        .m_axi_wvalid(m_axi_wvalid),
        .rd_en(rd_en),
        .s_axi_awvalid(s_axi_awvalid),
        .s_axi_wvalid(s_axi_wvalid));
  FDRE #(
    .INIT(1'b0)) 
    \areset_d_reg[0] 
       (.C(aclk),
        .CE(1'b1),
        .D(SR),
        .Q(areset_d[0]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    \areset_d_reg[1] 
       (.C(aclk),
        .CE(1'b1),
        .D(areset_d[0]),
        .Q(areset_d[1]),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    cmd_push_block_reg
       (.C(aclk),
        .CE(1'b1),
        .D(\USE_BURSTS.cmd_queue_n_6 ),
        .Q(cmd_push_block_reg_n_0),
        .R(1'b0));
  FDRE #(
    .INIT(1'b0)) 
    command_ongoing_reg
       (.C(aclk),
        .CE(1'b1),
        .D(\USE_BURSTS.cmd_queue_n_12 ),
        .Q(command_ongoing),
        .R(SR));
endmodule

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_protocol_converter_v2_1_22_axi3_conv
   (m_axi_awlen,
    m_axi_awaddr,
    E,
    m_axi_awsize,
    m_axi_awburst,
    m_axi_awlock,
    m_axi_awcache,
    m_axi_awprot,
    m_axi_awqos,
    m_axi_awvalid,
    empty_fwft_i_reg,
    m_axi_wvalid,
    m_axi_wlast,
    aresetn,
    m_axi_awready,
    aclk,
    s_axi_awaddr,
    s_axi_awlen,
    s_axi_awsize,
    s_axi_awburst,
    s_axi_awlock,
    s_axi_awcache,
    s_axi_awprot,
    s_axi_awqos,
    m_axi_wready,
    s_axi_wvalid,
    s_axi_awvalid);
  output [3:0]m_axi_awlen;
  output [31:0]m_axi_awaddr;
  output [0:0]E;
  output [2:0]m_axi_awsize;
  output [1:0]m_axi_awburst;
  output [0:0]m_axi_awlock;
  output [3:0]m_axi_awcache;
  output [2:0]m_axi_awprot;
  output [3:0]m_axi_awqos;
  output m_axi_awvalid;
  output empty_fwft_i_reg;
  output m_axi_wvalid;
  output m_axi_wlast;
  input aresetn;
  input m_axi_awready;
  input aclk;
  input [31:0]s_axi_awaddr;
  input [3:0]s_axi_awlen;
  input [2:0]s_axi_awsize;
  input [1:0]s_axi_awburst;
  input [0:0]s_axi_awlock;
  input [3:0]s_axi_awcache;
  input [2:0]s_axi_awprot;
  input [3:0]s_axi_awqos;
  input m_axi_wready;
  input s_axi_wvalid;
  input s_axi_awvalid;

  wire [0:0]E;
  wire \USE_BURSTS.cmd_queue/inst/empty ;
  wire [3:0]\USE_WRITE.wr_cmd_length ;
  wire \USE_WRITE.wr_cmd_ready ;
  wire \USE_WRITE.write_addr_inst_n_13 ;
  wire \USE_WRITE.write_addr_inst_n_5 ;
  wire aclk;
  wire aresetn;
  wire empty_fwft_i_reg;
  wire first_mi_word;
  wire [1:0]length_counter_1_reg;
  wire [31:0]m_axi_awaddr;
  wire [1:0]m_axi_awburst;
  wire [3:0]m_axi_awcache;
  wire [3:0]m_axi_awlen;
  wire [0:0]m_axi_awlock;
  wire [2:0]m_axi_awprot;
  wire [3:0]m_axi_awqos;
  wire m_axi_awready;
  wire [2:0]m_axi_awsize;
  wire m_axi_awvalid;
  wire m_axi_wlast;
  wire m_axi_wready;
  wire m_axi_wvalid;
  wire [31:0]s_axi_awaddr;
  wire [1:0]s_axi_awburst;
  wire [3:0]s_axi_awcache;
  wire [3:0]s_axi_awlen;
  wire [0:0]s_axi_awlock;
  wire [2:0]s_axi_awprot;
  wire [3:0]s_axi_awqos;
  wire [2:0]s_axi_awsize;
  wire s_axi_awvalid;
  wire s_axi_wvalid;

  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_protocol_converter_v2_1_22_a_axi3_conv \USE_WRITE.write_addr_inst 
       (.E(E),
        .SR(\USE_WRITE.write_addr_inst_n_5 ),
        .aclk(aclk),
        .aresetn(aresetn),
        .dout(\USE_WRITE.wr_cmd_length ),
        .empty(\USE_BURSTS.cmd_queue/inst/empty ),
        .empty_fwft_i_reg(empty_fwft_i_reg),
        .first_mi_word(first_mi_word),
        .length_counter_1_reg(length_counter_1_reg),
        .length_counter_1_reg_1_sp_1(\USE_WRITE.write_addr_inst_n_13 ),
        .m_axi_awaddr(m_axi_awaddr),
        .m_axi_awburst(m_axi_awburst),
        .m_axi_awcache(m_axi_awcache),
        .m_axi_awlen(m_axi_awlen),
        .m_axi_awlock(m_axi_awlock),
        .m_axi_awprot(m_axi_awprot),
        .m_axi_awqos(m_axi_awqos),
        .m_axi_awready(m_axi_awready),
        .m_axi_awsize(m_axi_awsize),
        .m_axi_awvalid(m_axi_awvalid),
        .m_axi_wready(m_axi_wready),
        .m_axi_wvalid(m_axi_wvalid),
        .rd_en(\USE_WRITE.wr_cmd_ready ),
        .s_axi_awaddr(s_axi_awaddr),
        .s_axi_awburst(s_axi_awburst),
        .s_axi_awcache(s_axi_awcache),
        .s_axi_awlen(s_axi_awlen),
        .s_axi_awlock(s_axi_awlock),
        .s_axi_awprot(s_axi_awprot),
        .s_axi_awqos(s_axi_awqos),
        .s_axi_awsize(s_axi_awsize),
        .s_axi_awvalid(s_axi_awvalid),
        .s_axi_wvalid(s_axi_wvalid));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_protocol_converter_v2_1_22_w_axi3_conv \USE_WRITE.write_data_inst 
       (.SR(\USE_WRITE.write_addr_inst_n_5 ),
        .aclk(aclk),
        .dout(\USE_WRITE.wr_cmd_length ),
        .empty(\USE_BURSTS.cmd_queue/inst/empty ),
        .first_mi_word(first_mi_word),
        .\length_counter_1_reg[1]_0 (length_counter_1_reg),
        .\length_counter_1_reg[1]_1 (\USE_WRITE.write_addr_inst_n_13 ),
        .\length_counter_1_reg[2]_0 (empty_fwft_i_reg),
        .m_axi_wlast(m_axi_wlast),
        .m_axi_wready(m_axi_wready),
        .rd_en(\USE_WRITE.wr_cmd_ready ),
        .s_axi_wvalid(s_axi_wvalid));
endmodule

(* C_AXI_ADDR_WIDTH = "32" *) (* C_AXI_ARUSER_WIDTH = "1" *) (* C_AXI_AWUSER_WIDTH = "1" *) 
(* C_AXI_BUSER_WIDTH = "1" *) (* C_AXI_DATA_WIDTH = "64" *) (* C_AXI_ID_WIDTH = "1" *) 
(* C_AXI_RUSER_WIDTH = "1" *) (* C_AXI_SUPPORTS_READ = "0" *) (* C_AXI_SUPPORTS_USER_SIGNALS = "0" *) 
(* C_AXI_SUPPORTS_WRITE = "1" *) (* C_AXI_WUSER_WIDTH = "1" *) (* C_FAMILY = "zynq" *) 
(* C_IGNORE_ID = "1" *) (* C_M_AXI_PROTOCOL = "1" *) (* C_S_AXI_PROTOCOL = "0" *) 
(* C_TRANSLATION_MODE = "0" *) (* DowngradeIPIdentifiedWarnings = "yes" *) (* P_AXI3 = "1" *) 
(* P_AXI4 = "0" *) (* P_AXILITE = "2" *) (* P_AXILITE_SIZE = "3'b011" *) 
(* P_CONVERSION = "2" *) (* P_DECERR = "2'b11" *) (* P_INCR = "2'b01" *) 
(* P_PROTECTION = "1" *) (* P_SLVERR = "2'b10" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_protocol_converter_v2_1_22_axi_protocol_converter
   (aclk,
    aresetn,
    s_axi_awid,
    s_axi_awaddr,
    s_axi_awlen,
    s_axi_awsize,
    s_axi_awburst,
    s_axi_awlock,
    s_axi_awcache,
    s_axi_awprot,
    s_axi_awregion,
    s_axi_awqos,
    s_axi_awuser,
    s_axi_awvalid,
    s_axi_awready,
    s_axi_wid,
    s_axi_wdata,
    s_axi_wstrb,
    s_axi_wlast,
    s_axi_wuser,
    s_axi_wvalid,
    s_axi_wready,
    s_axi_bid,
    s_axi_bresp,
    s_axi_buser,
    s_axi_bvalid,
    s_axi_bready,
    s_axi_arid,
    s_axi_araddr,
    s_axi_arlen,
    s_axi_arsize,
    s_axi_arburst,
    s_axi_arlock,
    s_axi_arcache,
    s_axi_arprot,
    s_axi_arregion,
    s_axi_arqos,
    s_axi_aruser,
    s_axi_arvalid,
    s_axi_arready,
    s_axi_rid,
    s_axi_rdata,
    s_axi_rresp,
    s_axi_rlast,
    s_axi_ruser,
    s_axi_rvalid,
    s_axi_rready,
    m_axi_awid,
    m_axi_awaddr,
    m_axi_awlen,
    m_axi_awsize,
    m_axi_awburst,
    m_axi_awlock,
    m_axi_awcache,
    m_axi_awprot,
    m_axi_awregion,
    m_axi_awqos,
    m_axi_awuser,
    m_axi_awvalid,
    m_axi_awready,
    m_axi_wid,
    m_axi_wdata,
    m_axi_wstrb,
    m_axi_wlast,
    m_axi_wuser,
    m_axi_wvalid,
    m_axi_wready,
    m_axi_bid,
    m_axi_bresp,
    m_axi_buser,
    m_axi_bvalid,
    m_axi_bready,
    m_axi_arid,
    m_axi_araddr,
    m_axi_arlen,
    m_axi_arsize,
    m_axi_arburst,
    m_axi_arlock,
    m_axi_arcache,
    m_axi_arprot,
    m_axi_arregion,
    m_axi_arqos,
    m_axi_aruser,
    m_axi_arvalid,
    m_axi_arready,
    m_axi_rid,
    m_axi_rdata,
    m_axi_rresp,
    m_axi_rlast,
    m_axi_ruser,
    m_axi_rvalid,
    m_axi_rready);
  input aclk;
  input aresetn;
  input [0:0]s_axi_awid;
  input [31:0]s_axi_awaddr;
  input [7:0]s_axi_awlen;
  input [2:0]s_axi_awsize;
  input [1:0]s_axi_awburst;
  input [0:0]s_axi_awlock;
  input [3:0]s_axi_awcache;
  input [2:0]s_axi_awprot;
  input [3:0]s_axi_awregion;
  input [3:0]s_axi_awqos;
  input [0:0]s_axi_awuser;
  input s_axi_awvalid;
  output s_axi_awready;
  input [0:0]s_axi_wid;
  input [63:0]s_axi_wdata;
  input [7:0]s_axi_wstrb;
  input s_axi_wlast;
  input [0:0]s_axi_wuser;
  input s_axi_wvalid;
  output s_axi_wready;
  output [0:0]s_axi_bid;
  output [1:0]s_axi_bresp;
  output [0:0]s_axi_buser;
  output s_axi_bvalid;
  input s_axi_bready;
  input [0:0]s_axi_arid;
  input [31:0]s_axi_araddr;
  input [7:0]s_axi_arlen;
  input [2:0]s_axi_arsize;
  input [1:0]s_axi_arburst;
  input [0:0]s_axi_arlock;
  input [3:0]s_axi_arcache;
  input [2:0]s_axi_arprot;
  input [3:0]s_axi_arregion;
  input [3:0]s_axi_arqos;
  input [0:0]s_axi_aruser;
  input s_axi_arvalid;
  output s_axi_arready;
  output [0:0]s_axi_rid;
  output [63:0]s_axi_rdata;
  output [1:0]s_axi_rresp;
  output s_axi_rlast;
  output [0:0]s_axi_ruser;
  output s_axi_rvalid;
  input s_axi_rready;
  output [0:0]m_axi_awid;
  output [31:0]m_axi_awaddr;
  output [3:0]m_axi_awlen;
  output [2:0]m_axi_awsize;
  output [1:0]m_axi_awburst;
  output [1:0]m_axi_awlock;
  output [3:0]m_axi_awcache;
  output [2:0]m_axi_awprot;
  output [3:0]m_axi_awregion;
  output [3:0]m_axi_awqos;
  output [0:0]m_axi_awuser;
  output m_axi_awvalid;
  input m_axi_awready;
  output [0:0]m_axi_wid;
  output [63:0]m_axi_wdata;
  output [7:0]m_axi_wstrb;
  output m_axi_wlast;
  output [0:0]m_axi_wuser;
  output m_axi_wvalid;
  input m_axi_wready;
  input [0:0]m_axi_bid;
  input [1:0]m_axi_bresp;
  input [0:0]m_axi_buser;
  input m_axi_bvalid;
  output m_axi_bready;
  output [0:0]m_axi_arid;
  output [31:0]m_axi_araddr;
  output [3:0]m_axi_arlen;
  output [2:0]m_axi_arsize;
  output [1:0]m_axi_arburst;
  output [1:0]m_axi_arlock;
  output [3:0]m_axi_arcache;
  output [2:0]m_axi_arprot;
  output [3:0]m_axi_arregion;
  output [3:0]m_axi_arqos;
  output [0:0]m_axi_aruser;
  output m_axi_arvalid;
  input m_axi_arready;
  input [0:0]m_axi_rid;
  input [63:0]m_axi_rdata;
  input [1:0]m_axi_rresp;
  input m_axi_rlast;
  input [0:0]m_axi_ruser;
  input m_axi_rvalid;
  output m_axi_rready;

  wire \<const0> ;
  wire aclk;
  wire aresetn;
  wire [31:0]m_axi_awaddr;
  wire [1:0]m_axi_awburst;
  wire [3:0]m_axi_awcache;
  wire [3:0]m_axi_awlen;
  wire [0:0]\^m_axi_awlock ;
  wire [2:0]m_axi_awprot;
  wire [3:0]m_axi_awqos;
  wire m_axi_awready;
  wire [2:0]m_axi_awsize;
  wire m_axi_awvalid;
  wire [1:0]m_axi_bresp;
  wire m_axi_bvalid;
  wire m_axi_wlast;
  wire m_axi_wready;
  wire m_axi_wvalid;
  wire [31:0]s_axi_awaddr;
  wire [1:0]s_axi_awburst;
  wire [3:0]s_axi_awcache;
  wire [7:0]s_axi_awlen;
  wire [0:0]s_axi_awlock;
  wire [2:0]s_axi_awprot;
  wire [3:0]s_axi_awqos;
  wire s_axi_awready;
  wire [2:0]s_axi_awsize;
  wire s_axi_awvalid;
  wire s_axi_bready;
  wire [63:0]s_axi_wdata;
  wire s_axi_wready;
  wire [7:0]s_axi_wstrb;
  wire s_axi_wvalid;

  assign m_axi_araddr[31] = \<const0> ;
  assign m_axi_araddr[30] = \<const0> ;
  assign m_axi_araddr[29] = \<const0> ;
  assign m_axi_araddr[28] = \<const0> ;
  assign m_axi_araddr[27] = \<const0> ;
  assign m_axi_araddr[26] = \<const0> ;
  assign m_axi_araddr[25] = \<const0> ;
  assign m_axi_araddr[24] = \<const0> ;
  assign m_axi_araddr[23] = \<const0> ;
  assign m_axi_araddr[22] = \<const0> ;
  assign m_axi_araddr[21] = \<const0> ;
  assign m_axi_araddr[20] = \<const0> ;
  assign m_axi_araddr[19] = \<const0> ;
  assign m_axi_araddr[18] = \<const0> ;
  assign m_axi_araddr[17] = \<const0> ;
  assign m_axi_araddr[16] = \<const0> ;
  assign m_axi_araddr[15] = \<const0> ;
  assign m_axi_araddr[14] = \<const0> ;
  assign m_axi_araddr[13] = \<const0> ;
  assign m_axi_araddr[12] = \<const0> ;
  assign m_axi_araddr[11] = \<const0> ;
  assign m_axi_araddr[10] = \<const0> ;
  assign m_axi_araddr[9] = \<const0> ;
  assign m_axi_araddr[8] = \<const0> ;
  assign m_axi_araddr[7] = \<const0> ;
  assign m_axi_araddr[6] = \<const0> ;
  assign m_axi_araddr[5] = \<const0> ;
  assign m_axi_araddr[4] = \<const0> ;
  assign m_axi_araddr[3] = \<const0> ;
  assign m_axi_araddr[2] = \<const0> ;
  assign m_axi_araddr[1] = \<const0> ;
  assign m_axi_araddr[0] = \<const0> ;
  assign m_axi_arburst[1] = \<const0> ;
  assign m_axi_arburst[0] = \<const0> ;
  assign m_axi_arcache[3] = \<const0> ;
  assign m_axi_arcache[2] = \<const0> ;
  assign m_axi_arcache[1] = \<const0> ;
  assign m_axi_arcache[0] = \<const0> ;
  assign m_axi_arid[0] = \<const0> ;
  assign m_axi_arlen[3] = \<const0> ;
  assign m_axi_arlen[2] = \<const0> ;
  assign m_axi_arlen[1] = \<const0> ;
  assign m_axi_arlen[0] = \<const0> ;
  assign m_axi_arlock[1] = \<const0> ;
  assign m_axi_arlock[0] = \<const0> ;
  assign m_axi_arprot[2] = \<const0> ;
  assign m_axi_arprot[1] = \<const0> ;
  assign m_axi_arprot[0] = \<const0> ;
  assign m_axi_arqos[3] = \<const0> ;
  assign m_axi_arqos[2] = \<const0> ;
  assign m_axi_arqos[1] = \<const0> ;
  assign m_axi_arqos[0] = \<const0> ;
  assign m_axi_arregion[3] = \<const0> ;
  assign m_axi_arregion[2] = \<const0> ;
  assign m_axi_arregion[1] = \<const0> ;
  assign m_axi_arregion[0] = \<const0> ;
  assign m_axi_arsize[2] = \<const0> ;
  assign m_axi_arsize[1] = \<const0> ;
  assign m_axi_arsize[0] = \<const0> ;
  assign m_axi_aruser[0] = \<const0> ;
  assign m_axi_arvalid = \<const0> ;
  assign m_axi_awid[0] = \<const0> ;
  assign m_axi_awlock[1] = \<const0> ;
  assign m_axi_awlock[0] = \^m_axi_awlock [0];
  assign m_axi_awregion[3] = \<const0> ;
  assign m_axi_awregion[2] = \<const0> ;
  assign m_axi_awregion[1] = \<const0> ;
  assign m_axi_awregion[0] = \<const0> ;
  assign m_axi_awuser[0] = \<const0> ;
  assign m_axi_bready = s_axi_bready;
  assign m_axi_rready = \<const0> ;
  assign m_axi_wdata[63:0] = s_axi_wdata;
  assign m_axi_wid[0] = \<const0> ;
  assign m_axi_wstrb[7:0] = s_axi_wstrb;
  assign m_axi_wuser[0] = \<const0> ;
  assign s_axi_arready = \<const0> ;
  assign s_axi_bid[0] = \<const0> ;
  assign s_axi_bresp[1:0] = m_axi_bresp;
  assign s_axi_buser[0] = \<const0> ;
  assign s_axi_bvalid = m_axi_bvalid;
  assign s_axi_rdata[63] = \<const0> ;
  assign s_axi_rdata[62] = \<const0> ;
  assign s_axi_rdata[61] = \<const0> ;
  assign s_axi_rdata[60] = \<const0> ;
  assign s_axi_rdata[59] = \<const0> ;
  assign s_axi_rdata[58] = \<const0> ;
  assign s_axi_rdata[57] = \<const0> ;
  assign s_axi_rdata[56] = \<const0> ;
  assign s_axi_rdata[55] = \<const0> ;
  assign s_axi_rdata[54] = \<const0> ;
  assign s_axi_rdata[53] = \<const0> ;
  assign s_axi_rdata[52] = \<const0> ;
  assign s_axi_rdata[51] = \<const0> ;
  assign s_axi_rdata[50] = \<const0> ;
  assign s_axi_rdata[49] = \<const0> ;
  assign s_axi_rdata[48] = \<const0> ;
  assign s_axi_rdata[47] = \<const0> ;
  assign s_axi_rdata[46] = \<const0> ;
  assign s_axi_rdata[45] = \<const0> ;
  assign s_axi_rdata[44] = \<const0> ;
  assign s_axi_rdata[43] = \<const0> ;
  assign s_axi_rdata[42] = \<const0> ;
  assign s_axi_rdata[41] = \<const0> ;
  assign s_axi_rdata[40] = \<const0> ;
  assign s_axi_rdata[39] = \<const0> ;
  assign s_axi_rdata[38] = \<const0> ;
  assign s_axi_rdata[37] = \<const0> ;
  assign s_axi_rdata[36] = \<const0> ;
  assign s_axi_rdata[35] = \<const0> ;
  assign s_axi_rdata[34] = \<const0> ;
  assign s_axi_rdata[33] = \<const0> ;
  assign s_axi_rdata[32] = \<const0> ;
  assign s_axi_rdata[31] = \<const0> ;
  assign s_axi_rdata[30] = \<const0> ;
  assign s_axi_rdata[29] = \<const0> ;
  assign s_axi_rdata[28] = \<const0> ;
  assign s_axi_rdata[27] = \<const0> ;
  assign s_axi_rdata[26] = \<const0> ;
  assign s_axi_rdata[25] = \<const0> ;
  assign s_axi_rdata[24] = \<const0> ;
  assign s_axi_rdata[23] = \<const0> ;
  assign s_axi_rdata[22] = \<const0> ;
  assign s_axi_rdata[21] = \<const0> ;
  assign s_axi_rdata[20] = \<const0> ;
  assign s_axi_rdata[19] = \<const0> ;
  assign s_axi_rdata[18] = \<const0> ;
  assign s_axi_rdata[17] = \<const0> ;
  assign s_axi_rdata[16] = \<const0> ;
  assign s_axi_rdata[15] = \<const0> ;
  assign s_axi_rdata[14] = \<const0> ;
  assign s_axi_rdata[13] = \<const0> ;
  assign s_axi_rdata[12] = \<const0> ;
  assign s_axi_rdata[11] = \<const0> ;
  assign s_axi_rdata[10] = \<const0> ;
  assign s_axi_rdata[9] = \<const0> ;
  assign s_axi_rdata[8] = \<const0> ;
  assign s_axi_rdata[7] = \<const0> ;
  assign s_axi_rdata[6] = \<const0> ;
  assign s_axi_rdata[5] = \<const0> ;
  assign s_axi_rdata[4] = \<const0> ;
  assign s_axi_rdata[3] = \<const0> ;
  assign s_axi_rdata[2] = \<const0> ;
  assign s_axi_rdata[1] = \<const0> ;
  assign s_axi_rdata[0] = \<const0> ;
  assign s_axi_rid[0] = \<const0> ;
  assign s_axi_rlast = \<const0> ;
  assign s_axi_rresp[1] = \<const0> ;
  assign s_axi_rresp[0] = \<const0> ;
  assign s_axi_ruser[0] = \<const0> ;
  assign s_axi_rvalid = \<const0> ;
  GND GND
       (.G(\<const0> ));
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_protocol_converter_v2_1_22_axi3_conv \gen_axi4_axi3.axi3_conv_inst 
       (.E(s_axi_awready),
        .aclk(aclk),
        .aresetn(aresetn),
        .empty_fwft_i_reg(s_axi_wready),
        .m_axi_awaddr(m_axi_awaddr),
        .m_axi_awburst(m_axi_awburst),
        .m_axi_awcache(m_axi_awcache),
        .m_axi_awlen(m_axi_awlen),
        .m_axi_awlock(\^m_axi_awlock ),
        .m_axi_awprot(m_axi_awprot),
        .m_axi_awqos(m_axi_awqos),
        .m_axi_awready(m_axi_awready),
        .m_axi_awsize(m_axi_awsize),
        .m_axi_awvalid(m_axi_awvalid),
        .m_axi_wlast(m_axi_wlast),
        .m_axi_wready(m_axi_wready),
        .m_axi_wvalid(m_axi_wvalid),
        .s_axi_awaddr(s_axi_awaddr),
        .s_axi_awburst(s_axi_awburst),
        .s_axi_awcache(s_axi_awcache),
        .s_axi_awlen(s_axi_awlen[3:0]),
        .s_axi_awlock(s_axi_awlock),
        .s_axi_awprot(s_axi_awprot),
        .s_axi_awqos(s_axi_awqos),
        .s_axi_awsize(s_axi_awsize),
        .s_axi_awvalid(s_axi_awvalid),
        .s_axi_wvalid(s_axi_wvalid));
endmodule

module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_protocol_converter_v2_1_22_w_axi3_conv
   (\length_counter_1_reg[1]_0 ,
    first_mi_word,
    rd_en,
    m_axi_wlast,
    SR,
    aclk,
    \length_counter_1_reg[1]_1 ,
    \length_counter_1_reg[2]_0 ,
    m_axi_wready,
    s_axi_wvalid,
    empty,
    dout);
  output [1:0]\length_counter_1_reg[1]_0 ;
  output first_mi_word;
  output rd_en;
  output m_axi_wlast;
  input [0:0]SR;
  input aclk;
  input \length_counter_1_reg[1]_1 ;
  input \length_counter_1_reg[2]_0 ;
  input m_axi_wready;
  input s_axi_wvalid;
  input empty;
  input [3:0]dout;

  wire [0:0]SR;
  wire aclk;
  wire [3:0]dout;
  wire empty;
  wire first_mi_word;
  wire first_mi_word_i_1_n_0;
  wire \length_counter_1[0]_i_1_n_0 ;
  wire \length_counter_1[2]_i_1_n_0 ;
  wire \length_counter_1[3]_i_1_n_0 ;
  wire \length_counter_1[4]_i_1_n_0 ;
  wire \length_counter_1[4]_i_2_n_0 ;
  wire \length_counter_1[5]_i_1_n_0 ;
  wire \length_counter_1[6]_i_1_n_0 ;
  wire \length_counter_1[7]_i_1_n_0 ;
  wire [7:2]length_counter_1_reg;
  wire [1:0]\length_counter_1_reg[1]_0 ;
  wire \length_counter_1_reg[1]_1 ;
  wire \length_counter_1_reg[2]_0 ;
  wire m_axi_wlast;
  wire m_axi_wlast_INST_0_i_1_n_0;
  wire m_axi_wlast_INST_0_i_2_n_0;
  wire m_axi_wlast_INST_0_i_3_n_0;
  wire m_axi_wready;
  wire rd_en;
  wire s_axi_wvalid;

  LUT6 #(
    .INIT(64'h0000CC000000CC04)) 
    fifo_gen_inst_i_2
       (.I0(length_counter_1_reg[7]),
        .I1(\length_counter_1_reg[2]_0 ),
        .I2(length_counter_1_reg[5]),
        .I3(first_mi_word),
        .I4(m_axi_wlast_INST_0_i_1_n_0),
        .I5(length_counter_1_reg[6]),
        .O(rd_en));
  LUT6 #(
    .INIT(64'h0F0FFFFF00010000)) 
    first_mi_word_i_1
       (.I0(length_counter_1_reg[7]),
        .I1(length_counter_1_reg[5]),
        .I2(m_axi_wlast_INST_0_i_1_n_0),
        .I3(length_counter_1_reg[6]),
        .I4(\length_counter_1_reg[2]_0 ),
        .I5(first_mi_word),
        .O(first_mi_word_i_1_n_0));
  FDSE #(
    .INIT(1'b0)) 
    first_mi_word_reg
       (.C(aclk),
        .CE(1'b1),
        .D(first_mi_word_i_1_n_0),
        .Q(first_mi_word),
        .S(SR));
  LUT6 #(
    .INIT(64'hF2FFFFFF07000000)) 
    \length_counter_1[0]_i_1 
       (.I0(first_mi_word),
        .I1(dout[0]),
        .I2(empty),
        .I3(s_axi_wvalid),
        .I4(m_axi_wready),
        .I5(\length_counter_1_reg[1]_0 [0]),
        .O(\length_counter_1[0]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair9" *) 
  LUT5 #(
    .INIT(32'hD8D272D2)) 
    \length_counter_1[2]_i_1 
       (.I0(\length_counter_1_reg[2]_0 ),
        .I1(m_axi_wlast_INST_0_i_3_n_0),
        .I2(length_counter_1_reg[2]),
        .I3(first_mi_word),
        .I4(dout[2]),
        .O(\length_counter_1[2]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair8" *) 
  LUT5 #(
    .INIT(32'hB8B474B4)) 
    \length_counter_1[3]_i_1 
       (.I0(\length_counter_1[4]_i_2_n_0 ),
        .I1(\length_counter_1_reg[2]_0 ),
        .I2(length_counter_1_reg[3]),
        .I3(first_mi_word),
        .I4(dout[3]),
        .O(\length_counter_1[3]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'h0A0A3A35AAAAAAAA)) 
    \length_counter_1[4]_i_1 
       (.I0(length_counter_1_reg[4]),
        .I1(dout[3]),
        .I2(first_mi_word),
        .I3(length_counter_1_reg[3]),
        .I4(\length_counter_1[4]_i_2_n_0 ),
        .I5(\length_counter_1_reg[2]_0 ),
        .O(\length_counter_1[4]_i_1_n_0 ));
  (* SOFT_HLUTNM = "soft_lutpair9" *) 
  LUT4 #(
    .INIT(16'hFEAE)) 
    \length_counter_1[4]_i_2 
       (.I0(m_axi_wlast_INST_0_i_3_n_0),
        .I1(length_counter_1_reg[2]),
        .I2(first_mi_word),
        .I3(dout[2]),
        .O(\length_counter_1[4]_i_2_n_0 ));
  LUT6 #(
    .INIT(64'hF7FF0000FFF70808)) 
    \length_counter_1[5]_i_1 
       (.I0(m_axi_wready),
        .I1(s_axi_wvalid),
        .I2(empty),
        .I3(first_mi_word),
        .I4(length_counter_1_reg[5]),
        .I5(m_axi_wlast_INST_0_i_1_n_0),
        .O(\length_counter_1[5]_i_1_n_0 ));
  LUT5 #(
    .INIT(32'h3EFF0D00)) 
    \length_counter_1[6]_i_1 
       (.I0(length_counter_1_reg[5]),
        .I1(first_mi_word),
        .I2(m_axi_wlast_INST_0_i_1_n_0),
        .I3(\length_counter_1_reg[2]_0 ),
        .I4(length_counter_1_reg[6]),
        .O(\length_counter_1[6]_i_1_n_0 ));
  LUT6 #(
    .INIT(64'h3F3EFFFF30310000)) 
    \length_counter_1[7]_i_1 
       (.I0(length_counter_1_reg[6]),
        .I1(m_axi_wlast_INST_0_i_1_n_0),
        .I2(first_mi_word),
        .I3(length_counter_1_reg[5]),
        .I4(\length_counter_1_reg[2]_0 ),
        .I5(length_counter_1_reg[7]),
        .O(\length_counter_1[7]_i_1_n_0 ));
  FDRE \length_counter_1_reg[0] 
       (.C(aclk),
        .CE(1'b1),
        .D(\length_counter_1[0]_i_1_n_0 ),
        .Q(\length_counter_1_reg[1]_0 [0]),
        .R(SR));
  FDRE \length_counter_1_reg[1] 
       (.C(aclk),
        .CE(1'b1),
        .D(\length_counter_1_reg[1]_1 ),
        .Q(\length_counter_1_reg[1]_0 [1]),
        .R(SR));
  FDRE \length_counter_1_reg[2] 
       (.C(aclk),
        .CE(1'b1),
        .D(\length_counter_1[2]_i_1_n_0 ),
        .Q(length_counter_1_reg[2]),
        .R(SR));
  FDRE \length_counter_1_reg[3] 
       (.C(aclk),
        .CE(1'b1),
        .D(\length_counter_1[3]_i_1_n_0 ),
        .Q(length_counter_1_reg[3]),
        .R(SR));
  FDRE \length_counter_1_reg[4] 
       (.C(aclk),
        .CE(1'b1),
        .D(\length_counter_1[4]_i_1_n_0 ),
        .Q(length_counter_1_reg[4]),
        .R(SR));
  FDRE \length_counter_1_reg[5] 
       (.C(aclk),
        .CE(1'b1),
        .D(\length_counter_1[5]_i_1_n_0 ),
        .Q(length_counter_1_reg[5]),
        .R(SR));
  FDRE \length_counter_1_reg[6] 
       (.C(aclk),
        .CE(1'b1),
        .D(\length_counter_1[6]_i_1_n_0 ),
        .Q(length_counter_1_reg[6]),
        .R(SR));
  FDRE \length_counter_1_reg[7] 
       (.C(aclk),
        .CE(1'b1),
        .D(\length_counter_1[7]_i_1_n_0 ),
        .Q(length_counter_1_reg[7]),
        .R(SR));
  LUT5 #(
    .INIT(32'h00F000F1)) 
    m_axi_wlast_INST_0
       (.I0(length_counter_1_reg[7]),
        .I1(length_counter_1_reg[5]),
        .I2(first_mi_word),
        .I3(m_axi_wlast_INST_0_i_1_n_0),
        .I4(length_counter_1_reg[6]),
        .O(m_axi_wlast));
  LUT6 #(
    .INIT(64'hFFFFFFFEFCFCFFFE)) 
    m_axi_wlast_INST_0_i_1
       (.I0(length_counter_1_reg[4]),
        .I1(m_axi_wlast_INST_0_i_2_n_0),
        .I2(m_axi_wlast_INST_0_i_3_n_0),
        .I3(length_counter_1_reg[2]),
        .I4(first_mi_word),
        .I5(dout[2]),
        .O(m_axi_wlast_INST_0_i_1_n_0));
  (* SOFT_HLUTNM = "soft_lutpair8" *) 
  LUT3 #(
    .INIT(8'hB8)) 
    m_axi_wlast_INST_0_i_2
       (.I0(dout[3]),
        .I1(first_mi_word),
        .I2(length_counter_1_reg[3]),
        .O(m_axi_wlast_INST_0_i_2_n_0));
  LUT5 #(
    .INIT(32'hFFFACCFA)) 
    m_axi_wlast_INST_0_i_3
       (.I0(\length_counter_1_reg[1]_0 [1]),
        .I1(dout[1]),
        .I2(\length_counter_1_reg[1]_0 [0]),
        .I3(first_mi_word),
        .I4(dout[0]),
        .O(m_axi_wlast_INST_0_i_3_n_0));
endmodule

(* CHECK_LICENSE_TYPE = "design_1_auto_pc_1,axi_protocol_converter_v2_1_22_axi_protocol_converter,{}" *) (* DowngradeIPIdentifiedWarnings = "yes" *) (* X_CORE_INFO = "axi_protocol_converter_v2_1_22_axi_protocol_converter,Vivado 2020.2" *) 
(* NotValidForBitStream *)
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix
   (aclk,
    aresetn,
    s_axi_awaddr,
    s_axi_awlen,
    s_axi_awsize,
    s_axi_awburst,
    s_axi_awlock,
    s_axi_awcache,
    s_axi_awprot,
    s_axi_awregion,
    s_axi_awqos,
    s_axi_awvalid,
    s_axi_awready,
    s_axi_wdata,
    s_axi_wstrb,
    s_axi_wlast,
    s_axi_wvalid,
    s_axi_wready,
    s_axi_bresp,
    s_axi_bvalid,
    s_axi_bready,
    m_axi_awaddr,
    m_axi_awlen,
    m_axi_awsize,
    m_axi_awburst,
    m_axi_awlock,
    m_axi_awcache,
    m_axi_awprot,
    m_axi_awqos,
    m_axi_awvalid,
    m_axi_awready,
    m_axi_wdata,
    m_axi_wstrb,
    m_axi_wlast,
    m_axi_wvalid,
    m_axi_wready,
    m_axi_bresp,
    m_axi_bvalid,
    m_axi_bready);
  (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 CLK CLK" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME CLK, FREQ_HZ 100000000, FREQ_TOLERANCE_HZ 0, PHASE 0.000, CLK_DOMAIN design_1_processing_system7_0_0_FCLK_CLK0, ASSOCIATED_BUSIF S_AXI:M_AXI, ASSOCIATED_RESET ARESETN, INSERT_VIP 0" *) input aclk;
  (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 RST RST" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME RST, POLARITY ACTIVE_LOW, INSERT_VIP 0, TYPE INTERCONNECT" *) input aresetn;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWADDR" *) input [31:0]s_axi_awaddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWLEN" *) input [7:0]s_axi_awlen;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWSIZE" *) input [2:0]s_axi_awsize;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWBURST" *) input [1:0]s_axi_awburst;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWLOCK" *) input [0:0]s_axi_awlock;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWCACHE" *) input [3:0]s_axi_awcache;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWPROT" *) input [2:0]s_axi_awprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWREGION" *) input [3:0]s_axi_awregion;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWQOS" *) input [3:0]s_axi_awqos;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWVALID" *) input s_axi_awvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWREADY" *) output s_axi_awready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI WDATA" *) input [63:0]s_axi_wdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI WSTRB" *) input [7:0]s_axi_wstrb;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI WLAST" *) input s_axi_wlast;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI WVALID" *) input s_axi_wvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI WREADY" *) output s_axi_wready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI BRESP" *) output [1:0]s_axi_bresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI BVALID" *) output s_axi_bvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI BREADY" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME S_AXI, DATA_WIDTH 64, PROTOCOL AXI4, FREQ_HZ 100000000, ID_WIDTH 0, ADDR_WIDTH 32, AWUSER_WIDTH 0, ARUSER_WIDTH 0, WUSER_WIDTH 0, RUSER_WIDTH 0, BUSER_WIDTH 0, READ_WRITE_MODE WRITE_ONLY, HAS_BURST 1, HAS_LOCK 1, HAS_PROT 1, HAS_CACHE 1, HAS_QOS 1, HAS_REGION 1, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 0, SUPPORTS_NARROW_BURST 0, NUM_READ_OUTSTANDING 8, NUM_WRITE_OUTSTANDING 8, MAX_BURST_LENGTH 256, PHASE 0.000, CLK_DOMAIN design_1_processing_system7_0_0_FCLK_CLK0, NUM_READ_THREADS 1, NUM_WRITE_THREADS 1, RUSER_BITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0" *) input s_axi_bready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI AWADDR" *) output [31:0]m_axi_awaddr;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI AWLEN" *) output [3:0]m_axi_awlen;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI AWSIZE" *) output [2:0]m_axi_awsize;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI AWBURST" *) output [1:0]m_axi_awburst;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI AWLOCK" *) output [1:0]m_axi_awlock;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI AWCACHE" *) output [3:0]m_axi_awcache;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI AWPROT" *) output [2:0]m_axi_awprot;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI AWQOS" *) output [3:0]m_axi_awqos;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI AWVALID" *) output m_axi_awvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI AWREADY" *) input m_axi_awready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI WDATA" *) output [63:0]m_axi_wdata;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI WSTRB" *) output [7:0]m_axi_wstrb;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI WLAST" *) output m_axi_wlast;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI WVALID" *) output m_axi_wvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI WREADY" *) input m_axi_wready;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI BRESP" *) input [1:0]m_axi_bresp;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI BVALID" *) input m_axi_bvalid;
  (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 M_AXI BREADY" *) (* X_INTERFACE_PARAMETER = "XIL_INTERFACENAME M_AXI, DATA_WIDTH 64, PROTOCOL AXI3, FREQ_HZ 100000000, ID_WIDTH 0, ADDR_WIDTH 32, AWUSER_WIDTH 0, ARUSER_WIDTH 0, WUSER_WIDTH 0, RUSER_WIDTH 0, BUSER_WIDTH 0, READ_WRITE_MODE WRITE_ONLY, HAS_BURST 0, HAS_LOCK 0, HAS_PROT 1, HAS_CACHE 1, HAS_QOS 0, HAS_REGION 0, HAS_WSTRB 1, HAS_BRESP 1, HAS_RRESP 0, SUPPORTS_NARROW_BURST 0, NUM_READ_OUTSTANDING 8, NUM_WRITE_OUTSTANDING 8, MAX_BURST_LENGTH 16, PHASE 0.000, CLK_DOMAIN design_1_processing_system7_0_0_FCLK_CLK0, NUM_READ_THREADS 1, NUM_WRITE_THREADS 1, RUSER_BITS_PER_BYTE 0, WUSER_BITS_PER_BYTE 0, INSERT_VIP 0" *) output m_axi_bready;

  wire \<const0> ;
  wire aclk;
  wire aresetn;
  wire [31:0]m_axi_awaddr;
  wire [1:0]m_axi_awburst;
  wire [3:0]m_axi_awcache;
  wire [3:0]m_axi_awlen;
  wire [0:0]\^m_axi_awlock ;
  wire [2:0]m_axi_awprot;
  wire [3:0]m_axi_awqos;
  wire m_axi_awready;
  wire [2:0]m_axi_awsize;
  wire m_axi_awvalid;
  wire m_axi_bready;
  wire [1:0]m_axi_bresp;
  wire m_axi_bvalid;
  wire [63:0]m_axi_wdata;
  wire m_axi_wlast;
  wire m_axi_wready;
  wire [7:0]m_axi_wstrb;
  wire m_axi_wvalid;
  wire [31:0]s_axi_awaddr;
  wire [1:0]s_axi_awburst;
  wire [3:0]s_axi_awcache;
  wire [7:0]s_axi_awlen;
  wire [0:0]s_axi_awlock;
  wire [2:0]s_axi_awprot;
  wire [3:0]s_axi_awqos;
  wire s_axi_awready;
  wire [2:0]s_axi_awsize;
  wire s_axi_awvalid;
  wire s_axi_bready;
  wire [1:0]s_axi_bresp;
  wire s_axi_bvalid;
  wire [63:0]s_axi_wdata;
  wire s_axi_wready;
  wire [7:0]s_axi_wstrb;
  wire s_axi_wvalid;
  wire NLW_inst_m_axi_arvalid_UNCONNECTED;
  wire NLW_inst_m_axi_rready_UNCONNECTED;
  wire NLW_inst_s_axi_arready_UNCONNECTED;
  wire NLW_inst_s_axi_rlast_UNCONNECTED;
  wire NLW_inst_s_axi_rvalid_UNCONNECTED;
  wire [31:0]NLW_inst_m_axi_araddr_UNCONNECTED;
  wire [1:0]NLW_inst_m_axi_arburst_UNCONNECTED;
  wire [3:0]NLW_inst_m_axi_arcache_UNCONNECTED;
  wire [0:0]NLW_inst_m_axi_arid_UNCONNECTED;
  wire [3:0]NLW_inst_m_axi_arlen_UNCONNECTED;
  wire [1:0]NLW_inst_m_axi_arlock_UNCONNECTED;
  wire [2:0]NLW_inst_m_axi_arprot_UNCONNECTED;
  wire [3:0]NLW_inst_m_axi_arqos_UNCONNECTED;
  wire [3:0]NLW_inst_m_axi_arregion_UNCONNECTED;
  wire [2:0]NLW_inst_m_axi_arsize_UNCONNECTED;
  wire [0:0]NLW_inst_m_axi_aruser_UNCONNECTED;
  wire [0:0]NLW_inst_m_axi_awid_UNCONNECTED;
  wire [1:1]NLW_inst_m_axi_awlock_UNCONNECTED;
  wire [3:0]NLW_inst_m_axi_awregion_UNCONNECTED;
  wire [0:0]NLW_inst_m_axi_awuser_UNCONNECTED;
  wire [0:0]NLW_inst_m_axi_wid_UNCONNECTED;
  wire [0:0]NLW_inst_m_axi_wuser_UNCONNECTED;
  wire [0:0]NLW_inst_s_axi_bid_UNCONNECTED;
  wire [0:0]NLW_inst_s_axi_buser_UNCONNECTED;
  wire [63:0]NLW_inst_s_axi_rdata_UNCONNECTED;
  wire [0:0]NLW_inst_s_axi_rid_UNCONNECTED;
  wire [1:0]NLW_inst_s_axi_rresp_UNCONNECTED;
  wire [0:0]NLW_inst_s_axi_ruser_UNCONNECTED;

  assign m_axi_awlock[1] = \<const0> ;
  assign m_axi_awlock[0] = \^m_axi_awlock [0];
  GND GND
       (.G(\<const0> ));
  (* C_AXI_ADDR_WIDTH = "32" *) 
  (* C_AXI_ARUSER_WIDTH = "1" *) 
  (* C_AXI_AWUSER_WIDTH = "1" *) 
  (* C_AXI_BUSER_WIDTH = "1" *) 
  (* C_AXI_DATA_WIDTH = "64" *) 
  (* C_AXI_ID_WIDTH = "1" *) 
  (* C_AXI_RUSER_WIDTH = "1" *) 
  (* C_AXI_SUPPORTS_READ = "0" *) 
  (* C_AXI_SUPPORTS_USER_SIGNALS = "0" *) 
  (* C_AXI_SUPPORTS_WRITE = "1" *) 
  (* C_AXI_WUSER_WIDTH = "1" *) 
  (* C_FAMILY = "zynq" *) 
  (* C_IGNORE_ID = "1" *) 
  (* C_M_AXI_PROTOCOL = "1" *) 
  (* C_S_AXI_PROTOCOL = "0" *) 
  (* C_TRANSLATION_MODE = "0" *) 
  (* P_AXI3 = "1" *) 
  (* P_AXI4 = "0" *) 
  (* P_AXILITE = "2" *) 
  (* P_AXILITE_SIZE = "3'b011" *) 
  (* P_CONVERSION = "2" *) 
  (* P_DECERR = "2'b11" *) 
  (* P_INCR = "2'b01" *) 
  (* P_PROTECTION = "1" *) 
  (* P_SLVERR = "2'b10" *) 
  (* downgradeipidentifiedwarnings = "yes" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_axi_protocol_converter_v2_1_22_axi_protocol_converter inst
       (.aclk(aclk),
        .aresetn(aresetn),
        .m_axi_araddr(NLW_inst_m_axi_araddr_UNCONNECTED[31:0]),
        .m_axi_arburst(NLW_inst_m_axi_arburst_UNCONNECTED[1:0]),
        .m_axi_arcache(NLW_inst_m_axi_arcache_UNCONNECTED[3:0]),
        .m_axi_arid(NLW_inst_m_axi_arid_UNCONNECTED[0]),
        .m_axi_arlen(NLW_inst_m_axi_arlen_UNCONNECTED[3:0]),
        .m_axi_arlock(NLW_inst_m_axi_arlock_UNCONNECTED[1:0]),
        .m_axi_arprot(NLW_inst_m_axi_arprot_UNCONNECTED[2:0]),
        .m_axi_arqos(NLW_inst_m_axi_arqos_UNCONNECTED[3:0]),
        .m_axi_arready(1'b0),
        .m_axi_arregion(NLW_inst_m_axi_arregion_UNCONNECTED[3:0]),
        .m_axi_arsize(NLW_inst_m_axi_arsize_UNCONNECTED[2:0]),
        .m_axi_aruser(NLW_inst_m_axi_aruser_UNCONNECTED[0]),
        .m_axi_arvalid(NLW_inst_m_axi_arvalid_UNCONNECTED),
        .m_axi_awaddr(m_axi_awaddr),
        .m_axi_awburst(m_axi_awburst),
        .m_axi_awcache(m_axi_awcache),
        .m_axi_awid(NLW_inst_m_axi_awid_UNCONNECTED[0]),
        .m_axi_awlen(m_axi_awlen),
        .m_axi_awlock({NLW_inst_m_axi_awlock_UNCONNECTED[1],\^m_axi_awlock }),
        .m_axi_awprot(m_axi_awprot),
        .m_axi_awqos(m_axi_awqos),
        .m_axi_awready(m_axi_awready),
        .m_axi_awregion(NLW_inst_m_axi_awregion_UNCONNECTED[3:0]),
        .m_axi_awsize(m_axi_awsize),
        .m_axi_awuser(NLW_inst_m_axi_awuser_UNCONNECTED[0]),
        .m_axi_awvalid(m_axi_awvalid),
        .m_axi_bid(1'b0),
        .m_axi_bready(m_axi_bready),
        .m_axi_bresp(m_axi_bresp),
        .m_axi_buser(1'b0),
        .m_axi_bvalid(m_axi_bvalid),
        .m_axi_rdata({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .m_axi_rid(1'b0),
        .m_axi_rlast(1'b1),
        .m_axi_rready(NLW_inst_m_axi_rready_UNCONNECTED),
        .m_axi_rresp({1'b0,1'b0}),
        .m_axi_ruser(1'b0),
        .m_axi_rvalid(1'b0),
        .m_axi_wdata(m_axi_wdata),
        .m_axi_wid(NLW_inst_m_axi_wid_UNCONNECTED[0]),
        .m_axi_wlast(m_axi_wlast),
        .m_axi_wready(m_axi_wready),
        .m_axi_wstrb(m_axi_wstrb),
        .m_axi_wuser(NLW_inst_m_axi_wuser_UNCONNECTED[0]),
        .m_axi_wvalid(m_axi_wvalid),
        .s_axi_araddr({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .s_axi_arburst({1'b0,1'b1}),
        .s_axi_arcache({1'b0,1'b0,1'b0,1'b0}),
        .s_axi_arid(1'b0),
        .s_axi_arlen({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .s_axi_arlock(1'b0),
        .s_axi_arprot({1'b0,1'b0,1'b0}),
        .s_axi_arqos({1'b0,1'b0,1'b0,1'b0}),
        .s_axi_arready(NLW_inst_s_axi_arready_UNCONNECTED),
        .s_axi_arregion({1'b0,1'b0,1'b0,1'b0}),
        .s_axi_arsize({1'b0,1'b0,1'b0}),
        .s_axi_aruser(1'b0),
        .s_axi_arvalid(1'b0),
        .s_axi_awaddr(s_axi_awaddr),
        .s_axi_awburst(s_axi_awburst),
        .s_axi_awcache(s_axi_awcache),
        .s_axi_awid(1'b0),
        .s_axi_awlen({1'b0,1'b0,1'b0,1'b0,s_axi_awlen[3:0]}),
        .s_axi_awlock(s_axi_awlock),
        .s_axi_awprot(s_axi_awprot),
        .s_axi_awqos(s_axi_awqos),
        .s_axi_awready(s_axi_awready),
        .s_axi_awregion({1'b0,1'b0,1'b0,1'b0}),
        .s_axi_awsize(s_axi_awsize),
        .s_axi_awuser(1'b0),
        .s_axi_awvalid(s_axi_awvalid),
        .s_axi_bid(NLW_inst_s_axi_bid_UNCONNECTED[0]),
        .s_axi_bready(s_axi_bready),
        .s_axi_bresp(s_axi_bresp),
        .s_axi_buser(NLW_inst_s_axi_buser_UNCONNECTED[0]),
        .s_axi_bvalid(s_axi_bvalid),
        .s_axi_rdata(NLW_inst_s_axi_rdata_UNCONNECTED[63:0]),
        .s_axi_rid(NLW_inst_s_axi_rid_UNCONNECTED[0]),
        .s_axi_rlast(NLW_inst_s_axi_rlast_UNCONNECTED),
        .s_axi_rready(1'b0),
        .s_axi_rresp(NLW_inst_s_axi_rresp_UNCONNECTED[1:0]),
        .s_axi_ruser(NLW_inst_s_axi_ruser_UNCONNECTED[0]),
        .s_axi_rvalid(NLW_inst_s_axi_rvalid_UNCONNECTED),
        .s_axi_wdata(s_axi_wdata),
        .s_axi_wid(1'b0),
        .s_axi_wlast(1'b0),
        .s_axi_wready(s_axi_wready),
        .s_axi_wstrb(s_axi_wstrb),
        .s_axi_wuser(1'b0),
        .s_axi_wvalid(s_axi_wvalid));
endmodule

(* DEF_VAL = "1'b0" *) (* DEST_SYNC_FF = "2" *) (* INIT_SYNC_FF = "0" *) 
(* INV_DEF_VAL = "1'b1" *) (* RST_ACTIVE_HIGH = "1" *) (* VERSION = "0" *) 
(* XPM_MODULE = "TRUE" *) (* is_du_within_envelope = "true" *) (* keep_hierarchy = "true" *) 
(* xpm_cdc = "ASYNC_RST" *) 
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_xpm_cdc_async_rst
   (src_arst,
    dest_clk,
    dest_arst);
  input src_arst;
  input dest_clk;
  output dest_arst;

  (* RTL_KEEP = "true" *) (* async_reg = "true" *) (* xpm_cdc = "ASYNC_RST" *) wire [1:0]arststages_ff;
  wire dest_clk;
  wire src_arst;

  assign dest_arst = arststages_ff[1];
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ASYNC_RST" *) 
  FDPE #(
    .INIT(1'b0)) 
    \arststages_ff_reg[0] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(1'b0),
        .PRE(src_arst),
        .Q(arststages_ff[0]));
  (* ASYNC_REG *) 
  (* KEEP = "true" *) 
  (* XPM_CDC = "ASYNC_RST" *) 
  FDPE #(
    .INIT(1'b0)) 
    \arststages_ff_reg[1] 
       (.C(dest_clk),
        .CE(1'b1),
        .D(arststages_ff[0]),
        .PRE(src_arst),
        .Q(arststages_ff[1]));
endmodule
`pragma protect begin_protected
`pragma protect version = 1
`pragma protect encrypt_agent = "XILINX"
`pragma protect encrypt_agent_info = "Xilinx Encryption Tool 2020.2"
`pragma protect key_keyowner="Cadence Design Systems.", key_keyname="cds_rsa_key", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=64)
`pragma protect key_block
SFoQ2tXDMrL2nCJbfpmHXuteJlKaWDWl3o9OY1miFvmYb8EDywmDpLUHQktJ/VoW+17fK5WHgFVI
FZV1B91GDQ==

`pragma protect key_keyowner="Synopsys", key_keyname="SNPS-VCS-RSA-2", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=128)
`pragma protect key_block
mxGWDRjEAsKmBqldxevT1RKZvqK7vn0KlTODVXNGlRcGf9zOAmj0Z7Ppu79POBDb8oNQyCY+2q1q
BddzhQfh5WLIVX9BNUMIF6M6IF0elM4GMSLHGeYEwqSaMPC+thuR8FGj1J7z6rH+43gDYhtIeyY+
ZuZUz/Pqg8Lu63Xwe+0=

`pragma protect key_keyowner="Aldec", key_keyname="ALDEC15_001", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
HLwPjQzkuqv5FEDBriEJS2DikBeIHB/bWuVWooHY5ChdoHatcmqCHpSvnGxVzLwObZWHFys2nR9y
P3zxywjtgtOWq/n3cYVa5li6eyiUmGXv2OE8nw1nLnAY1kzBvGd6VwQ45t6l4Hx5+oqpIfuU2KI2
7/Qpj2atiTN3Y+q5He/BMXLIxF9vWuU6XL/+HsxriGAumcZDuESdidlxOztbW1bFhYr1/qWwou2q
wynnRVKYHL41aWycgFdkDoDEFFxv8ft8+F5Ux+J5Hg5XdgRULJc6uUQE/lDG3zOqzPftlODB52zU
d0cm8gFOvSZ2nO8ZB8THnxoAGe33iIZJfMcefA==

`pragma protect key_keyowner="ATRENTA", key_keyname="ATR-SG-2015-RSA-3", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
jlR0iZ4fp9QXiFgaT07DMAK1YFLyBpsOGOOR9j2PWImFEh8oTBt4cvmGo+2z1Umbt9OMQwOhyepO
QIsKLFzUXYUba+SFFLBoCiaww24KICecbUfd3VV5sg2bEJjAdtYTT6mJqyc3vQRvBlONeBFdIGy2
AXqdK7QtXGLsLAIF/z4FG8cfG6nSD6e16gccBC6+kl5MoShdnmebKLyoo6UKFdMbDK88sHvTcD9S
LNCau6RK7FkTZg23FV0tf6cTP9Rray9YEcowm2AAh51Wldo2lGJ2W5iiDatRKH/W1bu7FGWZG+OT
+VZE+Ckiuf4T6cuu+G5IbrtMv6a4U93R0gtxXQ==

`pragma protect key_keyowner="Mentor Graphics Corporation", key_keyname="MGC-VELOCE-RSA", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=128)
`pragma protect key_block
p/kq+JjPPJbOTWT2SRiPJ99/iH6kkVGEiluRRXpuRN+j+cVPgJD1v4QVjw3zMWLlvTGB7OOqC+JG
Lc62Wiizd/BFfGj2JYkTZMatcOWok7A87HK+vRTjr4nZMApD2jKaneJdU1279KsIEeRfImCQ2uRl
QRNMH3PPdNGYCnOGgNk=

`pragma protect key_keyowner="Mentor Graphics Corporation", key_keyname="MGC-VERIF-SIM-RSA-2", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
kyyI/O29YYc5VBwhz19i7AV7MC75r43hHVKAOTBiGBhRu8zZxCwGGcNFqc2HgHcWC6nq4jCIbIXf
S3FDzPdasegnERlWvoob9/SXM88zKsyeTbUf+DRu5lB8SPROBMaIhnj375C5XLowL17MXZdmB6fV
X5ukCg7cNhCjssKt/bIJibWkfna7hvj4ye+CLWmi3LdEiix8KTwRoBS3ZJrjM4/N6FfZkXerVxs+
txkhdsmG9ga1g/xErhTRilhqrV2WetlpX86qH/64sRGVxrWeEfNoHhMZsqEK0jWDx4WavKt8XY7W
NDzMXLZ2m5Dv5HMiJWgFG+ntPwgiYYtBuwu7Eg==

`pragma protect key_keyowner="Real Intent", key_keyname="RI-RSA-KEY-1", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
tv6UL1ZWqo3dAIlhN5UTNGzJyqzdHpCqh217JPvIvHiWJgcFh2tw1n7HWnOPcK3VhCt31AGnCEFe
HpTiinXvHna65L2X2HhtNUrsgvZlUuh/oQR273wp5JPFDPD97NQ4ELkGI+w26HTYLgZ70K5rQo87
D4AkQNRuzTRS5G12yb4RU7ZYgmkYLuq1UyqjlxyN62Del4XoqZyivOGw5H+7wlfkNRu98iQwqq12
jthZbH/ue5wxZJUcb7NmEwL+3abpyDNmWs1qORHOFoE3t97/9XMmeSCpM2+KnSKJvsV5VbuoTCOT
964fsEh7ey4IVb4aum095gQjLCqTmDm8DWFmaw==

`pragma protect key_keyowner="Xilinx", key_keyname="xilinxt_2020_08", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
Oxo3AgNmVWgrXtMKDIThYfXr0YJfyFr7Bsjn2ge/G72mb25MA8Dbkd9ZZPtwqU1poazNnTng5Cx5
s8C1zMNEoo38jNY8zEUBjCCuasJgeMo5xsiha+3ZIBiuHS0KLrjLaPFIQZdsYevb44fg6J5YQLn5
jd1M6YdNMd1VwSezDxtbk9sN8ExPrmtwum/6L1ia9j9UlIzPTEaJ60Xz7tloPsgsbkborO2JLiIk
kIAY2q1b8tuhHzJ5DoXlvIo49wSDj75ncLrkwbAd26huob7aOmX1bS34pJLF17JzqYH0MoPJbHxb
RPdD+qUawXFsMSs2fOLnZrNxeG8L+TyAT0N8tQ==

`pragma protect key_keyowner="Metrics Technologies Inc.", key_keyname="DSim", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
CIR/vwxo0IBrPr5+bMp2YuBCQTNBRIIbqgEB18Oewkc8CuHzGCAgPyQUBUKaUG3bBy+KDOPVxBP5
cE/d3QYZAT11fyB1OMMTrjmEIZcr0Vk3nVTAnivoxxxkmdzPjkj0OcGcU9fMArPi3dfTgIsKdtCq
94+mV/70WeprgijzuZFWD7uH+gVioY/+rq/Wc1O6x1n949w8YGgSCTurUvhsobx2bonoC317J0Wm
IX17XRkSBIFgzqA8iC+GV5oCfxIGkihKmXxjIJbMamlOdCOycEkjkh3JYmm7TLNxmI65iffsabR0
t5+iI0l8eJxFhElzWeREqE43cnJYLaKZBUA+DA==

`pragma protect data_method = "AES128-CBC"
`pragma protect encoding = (enctype = "BASE64", line_length = 76, bytes = 70608)
`pragma protect data_block
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
`pragma protect end_protected
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
