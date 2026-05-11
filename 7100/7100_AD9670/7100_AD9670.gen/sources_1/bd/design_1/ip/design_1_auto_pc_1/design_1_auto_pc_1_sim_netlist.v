// Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2020.2 (win64) Build 3064766 Wed Nov 18 09:12:45 MST 2020
// Date        : Sun Apr 19 15:53:31 2026
// Host        : DESKTOP-MM8H4FT running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode funcsim
//               c:/Study/7100_AD9670/7100_AD9670.gen/sources_1/bd/design_1/ip/design_1_auto_pc_1/design_1_auto_pc_1_sim_netlist.v
// Design      : design_1_auto_pc_1
// Purpose     : This verilog netlist is a functional simulation representation of the design and should not be modified
//               or synthesized. This netlist cannot be used for SDF annotated simulation.
// Device      : xc7z100ffg900-2
// --------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

(* CHECK_LICENSE_TYPE = "design_1_auto_pc_1,axi_protocol_converter_v2_1_22_axi_protocol_converter,{}" *) (* DowngradeIPIdentifiedWarnings = "yes" *) (* X_CORE_INFO = "axi_protocol_converter_v2_1_22_axi_protocol_converter,Vivado 2020.2" *) 
(* NotValidForBitStream *)
module design_1_auto_pc_1
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
  design_1_auto_pc_1_axi_protocol_converter_v2_1_22_axi_protocol_converter inst
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

(* ORIG_REF_NAME = "axi_data_fifo_v2_1_21_axic_fifo" *) 
module design_1_auto_pc_1_axi_data_fifo_v2_1_21_axic_fifo
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
  design_1_auto_pc_1_axi_data_fifo_v2_1_21_fifo_gen inst
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

(* ORIG_REF_NAME = "axi_data_fifo_v2_1_21_fifo_gen" *) 
module design_1_auto_pc_1_axi_data_fifo_v2_1_21_fifo_gen
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
  design_1_auto_pc_1_fifo_generator_v13_2_5 fifo_gen_inst
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

(* ORIG_REF_NAME = "axi_protocol_converter_v2_1_22_a_axi3_conv" *) 
module design_1_auto_pc_1_axi_protocol_converter_v2_1_22_a_axi3_conv
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
  design_1_auto_pc_1_axi_data_fifo_v2_1_21_axic_fifo \USE_BURSTS.cmd_queue 
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

(* ORIG_REF_NAME = "axi_protocol_converter_v2_1_22_axi3_conv" *) 
module design_1_auto_pc_1_axi_protocol_converter_v2_1_22_axi3_conv
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

  design_1_auto_pc_1_axi_protocol_converter_v2_1_22_a_axi3_conv \USE_WRITE.write_addr_inst 
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
  design_1_auto_pc_1_axi_protocol_converter_v2_1_22_w_axi3_conv \USE_WRITE.write_data_inst 
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
(* C_TRANSLATION_MODE = "0" *) (* DowngradeIPIdentifiedWarnings = "yes" *) (* ORIG_REF_NAME = "axi_protocol_converter_v2_1_22_axi_protocol_converter" *) 
(* P_AXI3 = "1" *) (* P_AXI4 = "0" *) (* P_AXILITE = "2" *) 
(* P_AXILITE_SIZE = "3'b011" *) (* P_CONVERSION = "2" *) (* P_DECERR = "2'b11" *) 
(* P_INCR = "2'b01" *) (* P_PROTECTION = "1" *) (* P_SLVERR = "2'b10" *) 
module design_1_auto_pc_1_axi_protocol_converter_v2_1_22_axi_protocol_converter
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
  design_1_auto_pc_1_axi_protocol_converter_v2_1_22_axi3_conv \gen_axi4_axi3.axi3_conv_inst 
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

(* ORIG_REF_NAME = "axi_protocol_converter_v2_1_22_w_axi3_conv" *) 
module design_1_auto_pc_1_axi_protocol_converter_v2_1_22_w_axi3_conv
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

(* DEF_VAL = "1'b0" *) (* DEST_SYNC_FF = "2" *) (* INIT_SYNC_FF = "0" *) 
(* INV_DEF_VAL = "1'b1" *) (* ORIG_REF_NAME = "xpm_cdc_async_rst" *) (* RST_ACTIVE_HIGH = "1" *) 
(* VERSION = "0" *) (* XPM_MODULE = "TRUE" *) (* is_du_within_envelope = "true" *) 
(* keep_hierarchy = "true" *) (* xpm_cdc = "ASYNC_RST" *) 
module design_1_auto_pc_1_xpm_cdc_async_rst
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
`pragma protect encoding = (enctype = "BASE64", line_length = 76, bytes = 70528)
`pragma protect data_block
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
49HVSzEQxor/PkiyZtfn8kExRA==
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
