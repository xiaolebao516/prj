//Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
//--------------------------------------------------------------------------------
//Tool Version: Vivado v.2020.2 (win64) Build 3064766 Wed Nov 18 09:12:45 MST 2020
//Date        : Mon Jan 26 17:52:46 2026
//Host        : DESKTOP-8G1BU4I running 64-bit major release  (build 9200)
//Command     : generate_target lcd_wrapper.bd
//Design      : lcd_wrapper
//Purpose     : IP block netlist
//--------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

module lcd_wrapper
   (DDR_0_addr,
    DDR_0_ba,
    DDR_0_cas_n,
    DDR_0_ck_n,
    DDR_0_ck_p,
    DDR_0_cke,
    DDR_0_cs_n,
    DDR_0_dm,
    DDR_0_dq,
    DDR_0_dqs_n,
    DDR_0_dqs_p,
    DDR_0_odt,
    DDR_0_ras_n,
    DDR_0_reset_n,
    DDR_0_we_n,
    FIXED_IO_0_ddr_vrn,
    FIXED_IO_0_ddr_vrp,
    FIXED_IO_0_mio,
    FIXED_IO_0_ps_clk,
    FIXED_IO_0_ps_porb,
    FIXED_IO_0_ps_srstb,
    LCD_GPIO_tri_io,
    LCD_SPI_io0_io,
    LCD_SPI_sck_io,
    LCD_SPI_ss_io);
  inout [14:0]DDR_0_addr;
  inout [2:0]DDR_0_ba;
  inout DDR_0_cas_n;
  inout DDR_0_ck_n;
  inout DDR_0_ck_p;
  inout DDR_0_cke;
  inout DDR_0_cs_n;
  inout [3:0]DDR_0_dm;
  inout [31:0]DDR_0_dq;
  inout [3:0]DDR_0_dqs_n;
  inout [3:0]DDR_0_dqs_p;
  inout DDR_0_odt;
  inout DDR_0_ras_n;
  inout DDR_0_reset_n;
  inout DDR_0_we_n;
  inout FIXED_IO_0_ddr_vrn;
  inout FIXED_IO_0_ddr_vrp;
  inout [53:0]FIXED_IO_0_mio;
  inout FIXED_IO_0_ps_clk;
  inout FIXED_IO_0_ps_porb;
  inout FIXED_IO_0_ps_srstb;
  inout [2:0]LCD_GPIO_tri_io;
  inout LCD_SPI_io0_io;
  inout LCD_SPI_sck_io;
  inout LCD_SPI_ss_io;

  wire [14:0]DDR_0_addr;
  wire [2:0]DDR_0_ba;
  wire DDR_0_cas_n;
  wire DDR_0_ck_n;
  wire DDR_0_ck_p;
  wire DDR_0_cke;
  wire DDR_0_cs_n;
  wire [3:0]DDR_0_dm;
  wire [31:0]DDR_0_dq;
  wire [3:0]DDR_0_dqs_n;
  wire [3:0]DDR_0_dqs_p;
  wire DDR_0_odt;
  wire DDR_0_ras_n;
  wire DDR_0_reset_n;
  wire DDR_0_we_n;
  wire FIXED_IO_0_ddr_vrn;
  wire FIXED_IO_0_ddr_vrp;
  wire [53:0]FIXED_IO_0_mio;
  wire FIXED_IO_0_ps_clk;
  wire FIXED_IO_0_ps_porb;
  wire FIXED_IO_0_ps_srstb;
  wire [0:0]LCD_GPIO_tri_i_0;
  wire [1:1]LCD_GPIO_tri_i_1;
  wire [2:2]LCD_GPIO_tri_i_2;
  wire [0:0]LCD_GPIO_tri_io_0;
  wire [1:1]LCD_GPIO_tri_io_1;
  wire [2:2]LCD_GPIO_tri_io_2;
  wire [0:0]LCD_GPIO_tri_o_0;
  wire [1:1]LCD_GPIO_tri_o_1;
  wire [2:2]LCD_GPIO_tri_o_2;
  wire [0:0]LCD_GPIO_tri_t_0;
  wire [1:1]LCD_GPIO_tri_t_1;
  wire [2:2]LCD_GPIO_tri_t_2;
  wire LCD_SPI_io0_i;
  wire LCD_SPI_io0_io;
  wire LCD_SPI_io0_o;
  wire LCD_SPI_io0_t;
  wire LCD_SPI_io1_i;
  wire LCD_SPI_io1_io;
  wire LCD_SPI_io1_o;
  wire LCD_SPI_io1_t;
  wire LCD_SPI_sck_i;
  wire LCD_SPI_sck_io;
  wire LCD_SPI_sck_o;
  wire LCD_SPI_sck_t;
  wire LCD_SPI_ss1_o;
  wire LCD_SPI_ss2_o;
  wire LCD_SPI_ss_i;
  wire LCD_SPI_ss_io;
  wire LCD_SPI_ss_o;
  wire LCD_SPI_ss_t;

  IOBUF LCD_GPIO_tri_iobuf_0
       (.I(LCD_GPIO_tri_o_0),
        .IO(LCD_GPIO_tri_io[0]),
        .O(LCD_GPIO_tri_i_0),
        .T(LCD_GPIO_tri_t_0));
  IOBUF LCD_GPIO_tri_iobuf_1
       (.I(LCD_GPIO_tri_o_1),
        .IO(LCD_GPIO_tri_io[1]),
        .O(LCD_GPIO_tri_i_1),
        .T(LCD_GPIO_tri_t_1));
  IOBUF LCD_GPIO_tri_iobuf_2
       (.I(LCD_GPIO_tri_o_2),
        .IO(LCD_GPIO_tri_io[2]),
        .O(LCD_GPIO_tri_i_2),
        .T(LCD_GPIO_tri_t_2));
  IOBUF LCD_SPI_io0_iobuf
       (.I(LCD_SPI_io0_o),
        .IO(LCD_SPI_io0_io),
        .O(LCD_SPI_io0_i),
        .T(LCD_SPI_io0_t));
  IOBUF LCD_SPI_sck_iobuf
       (.I(LCD_SPI_sck_o),
        .IO(LCD_SPI_sck_io),
        .O(LCD_SPI_sck_i),
        .T(LCD_SPI_sck_t));
  IOBUF LCD_SPI_ss_iobuf
       (.I(LCD_SPI_ss_o),
        .IO(LCD_SPI_ss_io),
        .O(LCD_SPI_ss_i),
        .T(LCD_SPI_ss_t));
  lcd lcd_i
       (.DDR_0_addr(DDR_0_addr),
        .DDR_0_ba(DDR_0_ba),
        .DDR_0_cas_n(DDR_0_cas_n),
        .DDR_0_ck_n(DDR_0_ck_n),
        .DDR_0_ck_p(DDR_0_ck_p),
        .DDR_0_cke(DDR_0_cke),
        .DDR_0_cs_n(DDR_0_cs_n),
        .DDR_0_dm(DDR_0_dm),
        .DDR_0_dq(DDR_0_dq),
        .DDR_0_dqs_n(DDR_0_dqs_n),
        .DDR_0_dqs_p(DDR_0_dqs_p),
        .DDR_0_odt(DDR_0_odt),
        .DDR_0_ras_n(DDR_0_ras_n),
        .DDR_0_reset_n(DDR_0_reset_n),
        .DDR_0_we_n(DDR_0_we_n),
        .FIXED_IO_0_ddr_vrn(FIXED_IO_0_ddr_vrn),
        .FIXED_IO_0_ddr_vrp(FIXED_IO_0_ddr_vrp),
        .FIXED_IO_0_mio(FIXED_IO_0_mio),
        .FIXED_IO_0_ps_clk(FIXED_IO_0_ps_clk),
        .FIXED_IO_0_ps_porb(FIXED_IO_0_ps_porb),
        .FIXED_IO_0_ps_srstb(FIXED_IO_0_ps_srstb),
        .LCD_GPIO_tri_i({LCD_GPIO_tri_i_2,LCD_GPIO_tri_i_1,LCD_GPIO_tri_i_0}),
        .LCD_GPIO_tri_o({LCD_GPIO_tri_o_2,LCD_GPIO_tri_o_1,LCD_GPIO_tri_o_0}),
        .LCD_GPIO_tri_t({LCD_GPIO_tri_t_2,LCD_GPIO_tri_t_1,LCD_GPIO_tri_t_0}),
        .LCD_SPI_io0_i(LCD_SPI_io0_i),
        .LCD_SPI_io0_o(LCD_SPI_io0_o),
        .LCD_SPI_io0_t(LCD_SPI_io0_t),
        .LCD_SPI_io1_i(LCD_SPI_io1_i),
        .LCD_SPI_io1_o(LCD_SPI_io1_o),
        .LCD_SPI_io1_t(LCD_SPI_io1_t),
        .LCD_SPI_sck_i(LCD_SPI_sck_i),
        .LCD_SPI_sck_o(LCD_SPI_sck_o),
        .LCD_SPI_sck_t(LCD_SPI_sck_t),
        .LCD_SPI_ss1_o(LCD_SPI_ss1_o),
        .LCD_SPI_ss2_o(LCD_SPI_ss2_o),
        .LCD_SPI_ss_i(LCD_SPI_ss_i),
        .LCD_SPI_ss_o(LCD_SPI_ss_o),
        .LCD_SPI_ss_t(LCD_SPI_ss_t));
endmodule
