-- Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
-- --------------------------------------------------------------------------------
-- Tool Version: Vivado v.2020.2 (win64) Build 3064766 Wed Nov 18 09:12:45 MST 2020
-- Date        : Sun Apr 19 15:52:58 2026
-- Host        : DESKTOP-MM8H4FT running 64-bit major release  (build 9200)
-- Command     : write_vhdl -force -mode funcsim -rename_top decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix -prefix
--               decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_ design_1_AdcLVDS_Top_0_0_sim_netlist.vhdl
-- Design      : design_1_AdcLVDS_Top_0_0
-- Purpose     : This VHDL netlist is a functional simulation representation of the design and should not be modified or
--               synthesized. This netlist cannot be used for SDF annotated simulation.
-- Device      : xc7z100ffg900-2
-- --------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcClock is
  port (
    clk_bit : out STD_LOGIC;
    clk_word : out STD_LOGIC;
    lvds_dco_p : in STD_LOGIC;
    lvds_dco_n : in STD_LOGIC
  );
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcClock;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcClock is
  signal dco_mr_out : STD_LOGIC;
  signal dco_out : STD_LOGIC;
  attribute BOX_TYPE : string;
  attribute BOX_TYPE of i_bufio : label is "PRIMITIVE";
  attribute BOX_TYPE of i_bufmr : label is "PRIMITIVE";
  attribute BOX_TYPE of i_bufr : label is "PRIMITIVE";
  attribute BOX_TYPE of i_ibufgds_dco : label is "PRIMITIVE";
  attribute CAPACITANCE : string;
  attribute CAPACITANCE of i_ibufgds_dco : label is "DONT_CARE";
  attribute IBUF_DELAY_VALUE : string;
  attribute IBUF_DELAY_VALUE of i_ibufgds_dco : label is "0";
  attribute XILINX_LEGACY_PRIM : string;
  attribute XILINX_LEGACY_PRIM of i_ibufgds_dco : label is "IBUFGDS";
begin
i_bufio: unisim.vcomponents.BUFIO
     port map (
      I => dco_mr_out,
      O => clk_bit
    );
i_bufmr: unisim.vcomponents.BUFMR
     port map (
      I => dco_out,
      O => dco_mr_out
    );
i_bufr: unisim.vcomponents.BUFR
    generic map(
      BUFR_DIVIDE => "7",
      SIM_DEVICE => "7SERIES"
    )
        port map (
      CE => '1',
      CLR => '0',
      I => dco_mr_out,
      O => clk_word
    );
i_ibufgds_dco: unisim.vcomponents.IBUFDS
    generic map(
      IOSTANDARD => "DEFAULT"
    )
        port map (
      I => lvds_dco_p,
      IB => lvds_dco_n,
      O => dco_out
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR is
  port (
    rFrmAlignDone_reg : out STD_LOGIC;
    sys_rst_0 : out STD_LOGIC;
    lvds_fco_p : in STD_LOGIC;
    lvds_fco_n : in STD_LOGIC;
    bitslip_cmd : in STD_LOGIC;
    clk_bit : in STD_LOGIC;
    clkb : in STD_LOGIC;
    clk_word : in STD_LOGIC;
    sys_rst : in STD_LOGIC;
    rFrmAlignDone : in STD_LOGIC;
    m_axis_tvalid : in STD_LOGIC
  );
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR is
  signal ISERDESE2_inst_master_n_1 : STD_LOGIC;
  signal ISERDESE2_inst_master_n_2 : STD_LOGIC;
  signal ISERDESE2_inst_master_n_3 : STD_LOGIC;
  signal ISERDESE2_inst_master_n_4 : STD_LOGIC;
  signal ISERDESE2_inst_master_n_5 : STD_LOGIC;
  signal ISERDESE2_inst_master_n_6 : STD_LOGIC;
  signal ISERDESE2_inst_master_n_7 : STD_LOGIC;
  signal ISERDESE2_inst_master_n_8 : STD_LOGIC;
  signal ISERDESE2_inst_slave_n_3 : STD_LOGIC;
  signal ISERDESE2_inst_slave_n_4 : STD_LOGIC;
  signal ISERDESE2_inst_slave_n_5 : STD_LOGIC;
  signal ISERDESE2_inst_slave_n_6 : STD_LOGIC;
  signal ISERDESE2_inst_slave_n_7 : STD_LOGIC;
  signal ISERDESE2_inst_slave_n_8 : STD_LOGIC;
  signal SHIFTOUT1 : STD_LOGIC;
  signal SHIFTOUT2 : STD_LOGIC;
  signal data_in_single : STD_LOGIC;
  signal rFrmAlignDone_i_2_n_0 : STD_LOGIC;
  signal rFrmAlignDone_i_3_n_0 : STD_LOGIC;
  signal rFrmAlignDone_i_4_n_0 : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_O_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_OFB_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_D_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_O_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED : STD_LOGIC;
  attribute BOX_TYPE : string;
  attribute BOX_TYPE of ISERDESE2_inst_master : label is "PRIMITIVE";
  attribute OPT_MODIFIED : string;
  attribute OPT_MODIFIED of ISERDESE2_inst_master : label is "MLO";
  attribute BOX_TYPE of ISERDESE2_inst_slave : label is "PRIMITIVE";
  attribute OPT_MODIFIED of ISERDESE2_inst_slave : label is "MLO";
  attribute BOX_TYPE of ibufds_inst : label is "PRIMITIVE";
  attribute CAPACITANCE : string;
  attribute CAPACITANCE of ibufds_inst : label is "DONT_CARE";
  attribute IBUF_DELAY_VALUE : string;
  attribute IBUF_DELAY_VALUE of ibufds_inst : label is "0";
  attribute IFD_DELAY_VALUE : string;
  attribute IFD_DELAY_VALUE of ibufds_inst : label is "AUTO";
begin
ISERDESE2_inst_master: unisim.vcomponents.ISERDESE2
    generic map(
      DATA_RATE => "DDR",
      DATA_WIDTH => 14,
      DYN_CLKDIV_INV_EN => "FALSE",
      DYN_CLK_INV_EN => "FALSE",
      INIT_Q1 => '0',
      INIT_Q2 => '0',
      INIT_Q3 => '0',
      INIT_Q4 => '0',
      INTERFACE_TYPE => "NETWORKING",
      IOBDELAY => "NONE",
      IS_CLKB_INVERTED => '1',
      IS_CLKDIVP_INVERTED => '0',
      IS_CLKDIV_INVERTED => '0',
      IS_CLK_INVERTED => '0',
      IS_D_INVERTED => '0',
      IS_OCLKB_INVERTED => '0',
      IS_OCLK_INVERTED => '0',
      NUM_CE => 2,
      OFB_USED => "FALSE",
      SERDES_MODE => "MASTER",
      SRVAL_Q1 => '0',
      SRVAL_Q2 => '0',
      SRVAL_Q3 => '0',
      SRVAL_Q4 => '0'
    )
        port map (
      BITSLIP => bitslip_cmd,
      CE1 => '1',
      CE2 => '1',
      CLK => clk_bit,
      CLKB => clkb,
      CLKDIV => clk_word,
      CLKDIVP => '0',
      D => data_in_single,
      DDLY => NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED,
      DYNCLKDIVSEL => '0',
      DYNCLKSEL => '0',
      O => NLW_ISERDESE2_inst_master_O_UNCONNECTED,
      OCLK => '0',
      OCLKB => NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED,
      OFB => NLW_ISERDESE2_inst_master_OFB_UNCONNECTED,
      Q1 => ISERDESE2_inst_master_n_1,
      Q2 => ISERDESE2_inst_master_n_2,
      Q3 => ISERDESE2_inst_master_n_3,
      Q4 => ISERDESE2_inst_master_n_4,
      Q5 => ISERDESE2_inst_master_n_5,
      Q6 => ISERDESE2_inst_master_n_6,
      Q7 => ISERDESE2_inst_master_n_7,
      Q8 => ISERDESE2_inst_master_n_8,
      RST => sys_rst,
      SHIFTIN1 => NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED,
      SHIFTIN2 => NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED,
      SHIFTOUT1 => SHIFTOUT1,
      SHIFTOUT2 => SHIFTOUT2
    );
ISERDESE2_inst_slave: unisim.vcomponents.ISERDESE2
    generic map(
      DATA_RATE => "DDR",
      DATA_WIDTH => 14,
      DYN_CLKDIV_INV_EN => "FALSE",
      DYN_CLK_INV_EN => "FALSE",
      INIT_Q1 => '0',
      INIT_Q2 => '0',
      INIT_Q3 => '0',
      INIT_Q4 => '0',
      INTERFACE_TYPE => "NETWORKING",
      IOBDELAY => "NONE",
      IS_CLKB_INVERTED => '1',
      IS_CLKDIVP_INVERTED => '0',
      IS_CLKDIV_INVERTED => '0',
      IS_CLK_INVERTED => '0',
      IS_D_INVERTED => '0',
      IS_OCLKB_INVERTED => '0',
      IS_OCLK_INVERTED => '0',
      NUM_CE => 2,
      OFB_USED => "FALSE",
      SERDES_MODE => "SLAVE",
      SRVAL_Q1 => '0',
      SRVAL_Q2 => '0',
      SRVAL_Q3 => '0',
      SRVAL_Q4 => '0'
    )
        port map (
      BITSLIP => bitslip_cmd,
      CE1 => '1',
      CE2 => '1',
      CLK => clk_bit,
      CLKB => clkb,
      CLKDIV => clk_word,
      CLKDIVP => '0',
      D => NLW_ISERDESE2_inst_slave_D_UNCONNECTED,
      DDLY => NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED,
      DYNCLKDIVSEL => '0',
      DYNCLKSEL => '0',
      O => NLW_ISERDESE2_inst_slave_O_UNCONNECTED,
      OCLK => '0',
      OCLKB => '0',
      OFB => '0',
      Q1 => NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED,
      Q2 => NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED,
      Q3 => ISERDESE2_inst_slave_n_3,
      Q4 => ISERDESE2_inst_slave_n_4,
      Q5 => ISERDESE2_inst_slave_n_5,
      Q6 => ISERDESE2_inst_slave_n_6,
      Q7 => ISERDESE2_inst_slave_n_7,
      Q8 => ISERDESE2_inst_slave_n_8,
      RST => sys_rst,
      SHIFTIN1 => SHIFTOUT1,
      SHIFTIN2 => SHIFTOUT2,
      SHIFTOUT1 => NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED,
      SHIFTOUT2 => NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED
    );
ibufds_inst: unisim.vcomponents.IBUFDS
     port map (
      I => lvds_fco_p,
      IB => lvds_fco_n,
      O => data_in_single
    );
rFrmAlignDone_i_1: unisim.vcomponents.LUT5
    generic map(
      INIT => X"01FF0100"
    )
        port map (
      I0 => rFrmAlignDone_i_2_n_0,
      I1 => rFrmAlignDone_i_3_n_0,
      I2 => rFrmAlignDone_i_4_n_0,
      I3 => rFrmAlignDone,
      I4 => m_axis_tvalid,
      O => rFrmAlignDone_reg
    );
rFrmAlignDone_i_2: unisim.vcomponents.LUT6
    generic map(
      INIT => X"FFFFFFFFFFFF7FFF"
    )
        port map (
      I0 => ISERDESE2_inst_slave_n_7,
      I1 => ISERDESE2_inst_slave_n_8,
      I2 => ISERDESE2_inst_slave_n_5,
      I3 => ISERDESE2_inst_slave_n_6,
      I4 => ISERDESE2_inst_master_n_2,
      I5 => ISERDESE2_inst_master_n_1,
      O => rFrmAlignDone_i_2_n_0
    );
rFrmAlignDone_i_3: unisim.vcomponents.LUT4
    generic map(
      INIT => X"FFFE"
    )
        port map (
      I0 => ISERDESE2_inst_master_n_4,
      I1 => ISERDESE2_inst_master_n_3,
      I2 => ISERDESE2_inst_master_n_6,
      I3 => ISERDESE2_inst_master_n_5,
      O => rFrmAlignDone_i_3_n_0
    );
rFrmAlignDone_i_4: unisim.vcomponents.LUT4
    generic map(
      INIT => X"DFFF"
    )
        port map (
      I0 => ISERDESE2_inst_master_n_8,
      I1 => ISERDESE2_inst_master_n_7,
      I2 => ISERDESE2_inst_slave_n_4,
      I3 => ISERDESE2_inst_slave_n_3,
      O => rFrmAlignDone_i_4_n_0
    );
rFrmBitslip_i_1: unisim.vcomponents.LUT5
    generic map(
      INIT => X"00FE0000"
    )
        port map (
      I0 => rFrmAlignDone_i_2_n_0,
      I1 => rFrmAlignDone_i_3_n_0,
      I2 => rFrmAlignDone_i_4_n_0,
      I3 => sys_rst,
      I4 => rFrmAlignDone,
      O => sys_rst_0
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_10 is
  port (
    D : out STD_LOGIC_VECTOR ( 13 downto 0 );
    lvds_dat_p : in STD_LOGIC_VECTOR ( 0 to 0 );
    lvds_dat_n : in STD_LOGIC_VECTOR ( 0 to 0 );
    bitslip_cmd : in STD_LOGIC;
    clk_bit : in STD_LOGIC;
    clkb : in STD_LOGIC;
    clk_word : in STD_LOGIC;
    sys_rst : in STD_LOGIC
  );
  attribute ORIG_REF_NAME : string;
  attribute ORIG_REF_NAME of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_10 : entity is "Serdes_1x14_DDR";
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_10;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_10 is
  signal SHIFTOUT1 : STD_LOGIC;
  signal SHIFTOUT2 : STD_LOGIC;
  signal data_in_single : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_O_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_OFB_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_D_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_O_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED : STD_LOGIC;
  attribute BOX_TYPE : string;
  attribute BOX_TYPE of ISERDESE2_inst_master : label is "PRIMITIVE";
  attribute OPT_MODIFIED : string;
  attribute OPT_MODIFIED of ISERDESE2_inst_master : label is "MLO";
  attribute BOX_TYPE of ISERDESE2_inst_slave : label is "PRIMITIVE";
  attribute OPT_MODIFIED of ISERDESE2_inst_slave : label is "MLO";
  attribute BOX_TYPE of ibufds_inst : label is "PRIMITIVE";
  attribute CAPACITANCE : string;
  attribute CAPACITANCE of ibufds_inst : label is "DONT_CARE";
  attribute IBUF_DELAY_VALUE : string;
  attribute IBUF_DELAY_VALUE of ibufds_inst : label is "0";
  attribute IFD_DELAY_VALUE : string;
  attribute IFD_DELAY_VALUE of ibufds_inst : label is "AUTO";
begin
ISERDESE2_inst_master: unisim.vcomponents.ISERDESE2
    generic map(
      DATA_RATE => "DDR",
      DATA_WIDTH => 14,
      DYN_CLKDIV_INV_EN => "FALSE",
      DYN_CLK_INV_EN => "FALSE",
      INIT_Q1 => '0',
      INIT_Q2 => '0',
      INIT_Q3 => '0',
      INIT_Q4 => '0',
      INTERFACE_TYPE => "NETWORKING",
      IOBDELAY => "NONE",
      IS_CLKB_INVERTED => '1',
      IS_CLKDIVP_INVERTED => '0',
      IS_CLKDIV_INVERTED => '0',
      IS_CLK_INVERTED => '0',
      IS_D_INVERTED => '0',
      IS_OCLKB_INVERTED => '0',
      IS_OCLK_INVERTED => '0',
      NUM_CE => 2,
      OFB_USED => "FALSE",
      SERDES_MODE => "MASTER",
      SRVAL_Q1 => '0',
      SRVAL_Q2 => '0',
      SRVAL_Q3 => '0',
      SRVAL_Q4 => '0'
    )
        port map (
      BITSLIP => bitslip_cmd,
      CE1 => '1',
      CE2 => '1',
      CLK => clk_bit,
      CLKB => clkb,
      CLKDIV => clk_word,
      CLKDIVP => '0',
      D => data_in_single,
      DDLY => NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED,
      DYNCLKDIVSEL => '0',
      DYNCLKSEL => '0',
      O => NLW_ISERDESE2_inst_master_O_UNCONNECTED,
      OCLK => '0',
      OCLKB => NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED,
      OFB => NLW_ISERDESE2_inst_master_OFB_UNCONNECTED,
      Q1 => D(0),
      Q2 => D(1),
      Q3 => D(2),
      Q4 => D(3),
      Q5 => D(4),
      Q6 => D(5),
      Q7 => D(6),
      Q8 => D(7),
      RST => sys_rst,
      SHIFTIN1 => NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED,
      SHIFTIN2 => NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED,
      SHIFTOUT1 => SHIFTOUT1,
      SHIFTOUT2 => SHIFTOUT2
    );
ISERDESE2_inst_slave: unisim.vcomponents.ISERDESE2
    generic map(
      DATA_RATE => "DDR",
      DATA_WIDTH => 14,
      DYN_CLKDIV_INV_EN => "FALSE",
      DYN_CLK_INV_EN => "FALSE",
      INIT_Q1 => '0',
      INIT_Q2 => '0',
      INIT_Q3 => '0',
      INIT_Q4 => '0',
      INTERFACE_TYPE => "NETWORKING",
      IOBDELAY => "NONE",
      IS_CLKB_INVERTED => '1',
      IS_CLKDIVP_INVERTED => '0',
      IS_CLKDIV_INVERTED => '0',
      IS_CLK_INVERTED => '0',
      IS_D_INVERTED => '0',
      IS_OCLKB_INVERTED => '0',
      IS_OCLK_INVERTED => '0',
      NUM_CE => 2,
      OFB_USED => "FALSE",
      SERDES_MODE => "SLAVE",
      SRVAL_Q1 => '0',
      SRVAL_Q2 => '0',
      SRVAL_Q3 => '0',
      SRVAL_Q4 => '0'
    )
        port map (
      BITSLIP => bitslip_cmd,
      CE1 => '1',
      CE2 => '1',
      CLK => clk_bit,
      CLKB => clkb,
      CLKDIV => clk_word,
      CLKDIVP => '0',
      D => NLW_ISERDESE2_inst_slave_D_UNCONNECTED,
      DDLY => NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED,
      DYNCLKDIVSEL => '0',
      DYNCLKSEL => '0',
      O => NLW_ISERDESE2_inst_slave_O_UNCONNECTED,
      OCLK => '0',
      OCLKB => '0',
      OFB => '0',
      Q1 => NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED,
      Q2 => NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED,
      Q3 => D(8),
      Q4 => D(9),
      Q5 => D(10),
      Q6 => D(11),
      Q7 => D(12),
      Q8 => D(13),
      RST => sys_rst,
      SHIFTIN1 => SHIFTOUT1,
      SHIFTIN2 => SHIFTOUT2,
      SHIFTOUT1 => NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED,
      SHIFTOUT2 => NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED
    );
ibufds_inst: unisim.vcomponents.IBUFDS
     port map (
      I => lvds_dat_p(0),
      IB => lvds_dat_n(0),
      O => data_in_single
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_11 is
  port (
    D : out STD_LOGIC_VECTOR ( 13 downto 0 );
    lvds_dat_p : in STD_LOGIC_VECTOR ( 0 to 0 );
    lvds_dat_n : in STD_LOGIC_VECTOR ( 0 to 0 );
    bitslip_cmd : in STD_LOGIC;
    clk_bit : in STD_LOGIC;
    clkb : in STD_LOGIC;
    clk_word : in STD_LOGIC;
    sys_rst : in STD_LOGIC
  );
  attribute ORIG_REF_NAME : string;
  attribute ORIG_REF_NAME of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_11 : entity is "Serdes_1x14_DDR";
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_11;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_11 is
  signal SHIFTOUT1 : STD_LOGIC;
  signal SHIFTOUT2 : STD_LOGIC;
  signal data_in_single : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_O_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_OFB_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_D_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_O_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED : STD_LOGIC;
  attribute BOX_TYPE : string;
  attribute BOX_TYPE of ISERDESE2_inst_master : label is "PRIMITIVE";
  attribute OPT_MODIFIED : string;
  attribute OPT_MODIFIED of ISERDESE2_inst_master : label is "MLO";
  attribute BOX_TYPE of ISERDESE2_inst_slave : label is "PRIMITIVE";
  attribute OPT_MODIFIED of ISERDESE2_inst_slave : label is "MLO";
  attribute BOX_TYPE of ibufds_inst : label is "PRIMITIVE";
  attribute CAPACITANCE : string;
  attribute CAPACITANCE of ibufds_inst : label is "DONT_CARE";
  attribute IBUF_DELAY_VALUE : string;
  attribute IBUF_DELAY_VALUE of ibufds_inst : label is "0";
  attribute IFD_DELAY_VALUE : string;
  attribute IFD_DELAY_VALUE of ibufds_inst : label is "AUTO";
begin
ISERDESE2_inst_master: unisim.vcomponents.ISERDESE2
    generic map(
      DATA_RATE => "DDR",
      DATA_WIDTH => 14,
      DYN_CLKDIV_INV_EN => "FALSE",
      DYN_CLK_INV_EN => "FALSE",
      INIT_Q1 => '0',
      INIT_Q2 => '0',
      INIT_Q3 => '0',
      INIT_Q4 => '0',
      INTERFACE_TYPE => "NETWORKING",
      IOBDELAY => "NONE",
      IS_CLKB_INVERTED => '1',
      IS_CLKDIVP_INVERTED => '0',
      IS_CLKDIV_INVERTED => '0',
      IS_CLK_INVERTED => '0',
      IS_D_INVERTED => '0',
      IS_OCLKB_INVERTED => '0',
      IS_OCLK_INVERTED => '0',
      NUM_CE => 2,
      OFB_USED => "FALSE",
      SERDES_MODE => "MASTER",
      SRVAL_Q1 => '0',
      SRVAL_Q2 => '0',
      SRVAL_Q3 => '0',
      SRVAL_Q4 => '0'
    )
        port map (
      BITSLIP => bitslip_cmd,
      CE1 => '1',
      CE2 => '1',
      CLK => clk_bit,
      CLKB => clkb,
      CLKDIV => clk_word,
      CLKDIVP => '0',
      D => data_in_single,
      DDLY => NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED,
      DYNCLKDIVSEL => '0',
      DYNCLKSEL => '0',
      O => NLW_ISERDESE2_inst_master_O_UNCONNECTED,
      OCLK => '0',
      OCLKB => NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED,
      OFB => NLW_ISERDESE2_inst_master_OFB_UNCONNECTED,
      Q1 => D(0),
      Q2 => D(1),
      Q3 => D(2),
      Q4 => D(3),
      Q5 => D(4),
      Q6 => D(5),
      Q7 => D(6),
      Q8 => D(7),
      RST => sys_rst,
      SHIFTIN1 => NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED,
      SHIFTIN2 => NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED,
      SHIFTOUT1 => SHIFTOUT1,
      SHIFTOUT2 => SHIFTOUT2
    );
ISERDESE2_inst_slave: unisim.vcomponents.ISERDESE2
    generic map(
      DATA_RATE => "DDR",
      DATA_WIDTH => 14,
      DYN_CLKDIV_INV_EN => "FALSE",
      DYN_CLK_INV_EN => "FALSE",
      INIT_Q1 => '0',
      INIT_Q2 => '0',
      INIT_Q3 => '0',
      INIT_Q4 => '0',
      INTERFACE_TYPE => "NETWORKING",
      IOBDELAY => "NONE",
      IS_CLKB_INVERTED => '1',
      IS_CLKDIVP_INVERTED => '0',
      IS_CLKDIV_INVERTED => '0',
      IS_CLK_INVERTED => '0',
      IS_D_INVERTED => '0',
      IS_OCLKB_INVERTED => '0',
      IS_OCLK_INVERTED => '0',
      NUM_CE => 2,
      OFB_USED => "FALSE",
      SERDES_MODE => "SLAVE",
      SRVAL_Q1 => '0',
      SRVAL_Q2 => '0',
      SRVAL_Q3 => '0',
      SRVAL_Q4 => '0'
    )
        port map (
      BITSLIP => bitslip_cmd,
      CE1 => '1',
      CE2 => '1',
      CLK => clk_bit,
      CLKB => clkb,
      CLKDIV => clk_word,
      CLKDIVP => '0',
      D => NLW_ISERDESE2_inst_slave_D_UNCONNECTED,
      DDLY => NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED,
      DYNCLKDIVSEL => '0',
      DYNCLKSEL => '0',
      O => NLW_ISERDESE2_inst_slave_O_UNCONNECTED,
      OCLK => '0',
      OCLKB => '0',
      OFB => '0',
      Q1 => NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED,
      Q2 => NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED,
      Q3 => D(8),
      Q4 => D(9),
      Q5 => D(10),
      Q6 => D(11),
      Q7 => D(12),
      Q8 => D(13),
      RST => sys_rst,
      SHIFTIN1 => SHIFTOUT1,
      SHIFTIN2 => SHIFTOUT2,
      SHIFTOUT1 => NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED,
      SHIFTOUT2 => NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED
    );
ibufds_inst: unisim.vcomponents.IBUFDS
     port map (
      I => lvds_dat_p(0),
      IB => lvds_dat_n(0),
      O => data_in_single
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_12 is
  port (
    D : out STD_LOGIC_VECTOR ( 13 downto 0 );
    lvds_dat_p : in STD_LOGIC_VECTOR ( 0 to 0 );
    lvds_dat_n : in STD_LOGIC_VECTOR ( 0 to 0 );
    bitslip_cmd : in STD_LOGIC;
    clk_bit : in STD_LOGIC;
    clkb : in STD_LOGIC;
    clk_word : in STD_LOGIC;
    sys_rst : in STD_LOGIC
  );
  attribute ORIG_REF_NAME : string;
  attribute ORIG_REF_NAME of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_12 : entity is "Serdes_1x14_DDR";
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_12;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_12 is
  signal SHIFTOUT1 : STD_LOGIC;
  signal SHIFTOUT2 : STD_LOGIC;
  signal data_in_single : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_O_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_OFB_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_D_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_O_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED : STD_LOGIC;
  attribute BOX_TYPE : string;
  attribute BOX_TYPE of ISERDESE2_inst_master : label is "PRIMITIVE";
  attribute OPT_MODIFIED : string;
  attribute OPT_MODIFIED of ISERDESE2_inst_master : label is "MLO";
  attribute BOX_TYPE of ISERDESE2_inst_slave : label is "PRIMITIVE";
  attribute OPT_MODIFIED of ISERDESE2_inst_slave : label is "MLO";
  attribute BOX_TYPE of ibufds_inst : label is "PRIMITIVE";
  attribute CAPACITANCE : string;
  attribute CAPACITANCE of ibufds_inst : label is "DONT_CARE";
  attribute IBUF_DELAY_VALUE : string;
  attribute IBUF_DELAY_VALUE of ibufds_inst : label is "0";
  attribute IFD_DELAY_VALUE : string;
  attribute IFD_DELAY_VALUE of ibufds_inst : label is "AUTO";
begin
ISERDESE2_inst_master: unisim.vcomponents.ISERDESE2
    generic map(
      DATA_RATE => "DDR",
      DATA_WIDTH => 14,
      DYN_CLKDIV_INV_EN => "FALSE",
      DYN_CLK_INV_EN => "FALSE",
      INIT_Q1 => '0',
      INIT_Q2 => '0',
      INIT_Q3 => '0',
      INIT_Q4 => '0',
      INTERFACE_TYPE => "NETWORKING",
      IOBDELAY => "NONE",
      IS_CLKB_INVERTED => '1',
      IS_CLKDIVP_INVERTED => '0',
      IS_CLKDIV_INVERTED => '0',
      IS_CLK_INVERTED => '0',
      IS_D_INVERTED => '0',
      IS_OCLKB_INVERTED => '0',
      IS_OCLK_INVERTED => '0',
      NUM_CE => 2,
      OFB_USED => "FALSE",
      SERDES_MODE => "MASTER",
      SRVAL_Q1 => '0',
      SRVAL_Q2 => '0',
      SRVAL_Q3 => '0',
      SRVAL_Q4 => '0'
    )
        port map (
      BITSLIP => bitslip_cmd,
      CE1 => '1',
      CE2 => '1',
      CLK => clk_bit,
      CLKB => clkb,
      CLKDIV => clk_word,
      CLKDIVP => '0',
      D => data_in_single,
      DDLY => NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED,
      DYNCLKDIVSEL => '0',
      DYNCLKSEL => '0',
      O => NLW_ISERDESE2_inst_master_O_UNCONNECTED,
      OCLK => '0',
      OCLKB => NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED,
      OFB => NLW_ISERDESE2_inst_master_OFB_UNCONNECTED,
      Q1 => D(0),
      Q2 => D(1),
      Q3 => D(2),
      Q4 => D(3),
      Q5 => D(4),
      Q6 => D(5),
      Q7 => D(6),
      Q8 => D(7),
      RST => sys_rst,
      SHIFTIN1 => NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED,
      SHIFTIN2 => NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED,
      SHIFTOUT1 => SHIFTOUT1,
      SHIFTOUT2 => SHIFTOUT2
    );
ISERDESE2_inst_slave: unisim.vcomponents.ISERDESE2
    generic map(
      DATA_RATE => "DDR",
      DATA_WIDTH => 14,
      DYN_CLKDIV_INV_EN => "FALSE",
      DYN_CLK_INV_EN => "FALSE",
      INIT_Q1 => '0',
      INIT_Q2 => '0',
      INIT_Q3 => '0',
      INIT_Q4 => '0',
      INTERFACE_TYPE => "NETWORKING",
      IOBDELAY => "NONE",
      IS_CLKB_INVERTED => '1',
      IS_CLKDIVP_INVERTED => '0',
      IS_CLKDIV_INVERTED => '0',
      IS_CLK_INVERTED => '0',
      IS_D_INVERTED => '0',
      IS_OCLKB_INVERTED => '0',
      IS_OCLK_INVERTED => '0',
      NUM_CE => 2,
      OFB_USED => "FALSE",
      SERDES_MODE => "SLAVE",
      SRVAL_Q1 => '0',
      SRVAL_Q2 => '0',
      SRVAL_Q3 => '0',
      SRVAL_Q4 => '0'
    )
        port map (
      BITSLIP => bitslip_cmd,
      CE1 => '1',
      CE2 => '1',
      CLK => clk_bit,
      CLKB => clkb,
      CLKDIV => clk_word,
      CLKDIVP => '0',
      D => NLW_ISERDESE2_inst_slave_D_UNCONNECTED,
      DDLY => NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED,
      DYNCLKDIVSEL => '0',
      DYNCLKSEL => '0',
      O => NLW_ISERDESE2_inst_slave_O_UNCONNECTED,
      OCLK => '0',
      OCLKB => '0',
      OFB => '0',
      Q1 => NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED,
      Q2 => NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED,
      Q3 => D(8),
      Q4 => D(9),
      Q5 => D(10),
      Q6 => D(11),
      Q7 => D(12),
      Q8 => D(13),
      RST => sys_rst,
      SHIFTIN1 => SHIFTOUT1,
      SHIFTIN2 => SHIFTOUT2,
      SHIFTOUT1 => NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED,
      SHIFTOUT2 => NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED
    );
ibufds_inst: unisim.vcomponents.IBUFDS
     port map (
      I => lvds_dat_p(0),
      IB => lvds_dat_n(0),
      O => data_in_single
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_13 is
  port (
    D : out STD_LOGIC_VECTOR ( 13 downto 0 );
    lvds_dat_p : in STD_LOGIC_VECTOR ( 0 to 0 );
    lvds_dat_n : in STD_LOGIC_VECTOR ( 0 to 0 );
    bitslip_cmd : in STD_LOGIC;
    clk_bit : in STD_LOGIC;
    clkb : in STD_LOGIC;
    clk_word : in STD_LOGIC;
    sys_rst : in STD_LOGIC
  );
  attribute ORIG_REF_NAME : string;
  attribute ORIG_REF_NAME of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_13 : entity is "Serdes_1x14_DDR";
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_13;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_13 is
  signal SHIFTOUT1 : STD_LOGIC;
  signal SHIFTOUT2 : STD_LOGIC;
  signal data_in_single : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_O_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_OFB_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_D_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_O_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED : STD_LOGIC;
  attribute BOX_TYPE : string;
  attribute BOX_TYPE of ISERDESE2_inst_master : label is "PRIMITIVE";
  attribute OPT_MODIFIED : string;
  attribute OPT_MODIFIED of ISERDESE2_inst_master : label is "MLO";
  attribute BOX_TYPE of ISERDESE2_inst_slave : label is "PRIMITIVE";
  attribute OPT_MODIFIED of ISERDESE2_inst_slave : label is "MLO";
  attribute BOX_TYPE of ibufds_inst : label is "PRIMITIVE";
  attribute CAPACITANCE : string;
  attribute CAPACITANCE of ibufds_inst : label is "DONT_CARE";
  attribute IBUF_DELAY_VALUE : string;
  attribute IBUF_DELAY_VALUE of ibufds_inst : label is "0";
  attribute IFD_DELAY_VALUE : string;
  attribute IFD_DELAY_VALUE of ibufds_inst : label is "AUTO";
begin
ISERDESE2_inst_master: unisim.vcomponents.ISERDESE2
    generic map(
      DATA_RATE => "DDR",
      DATA_WIDTH => 14,
      DYN_CLKDIV_INV_EN => "FALSE",
      DYN_CLK_INV_EN => "FALSE",
      INIT_Q1 => '0',
      INIT_Q2 => '0',
      INIT_Q3 => '0',
      INIT_Q4 => '0',
      INTERFACE_TYPE => "NETWORKING",
      IOBDELAY => "NONE",
      IS_CLKB_INVERTED => '1',
      IS_CLKDIVP_INVERTED => '0',
      IS_CLKDIV_INVERTED => '0',
      IS_CLK_INVERTED => '0',
      IS_D_INVERTED => '0',
      IS_OCLKB_INVERTED => '0',
      IS_OCLK_INVERTED => '0',
      NUM_CE => 2,
      OFB_USED => "FALSE",
      SERDES_MODE => "MASTER",
      SRVAL_Q1 => '0',
      SRVAL_Q2 => '0',
      SRVAL_Q3 => '0',
      SRVAL_Q4 => '0'
    )
        port map (
      BITSLIP => bitslip_cmd,
      CE1 => '1',
      CE2 => '1',
      CLK => clk_bit,
      CLKB => clkb,
      CLKDIV => clk_word,
      CLKDIVP => '0',
      D => data_in_single,
      DDLY => NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED,
      DYNCLKDIVSEL => '0',
      DYNCLKSEL => '0',
      O => NLW_ISERDESE2_inst_master_O_UNCONNECTED,
      OCLK => '0',
      OCLKB => NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED,
      OFB => NLW_ISERDESE2_inst_master_OFB_UNCONNECTED,
      Q1 => D(0),
      Q2 => D(1),
      Q3 => D(2),
      Q4 => D(3),
      Q5 => D(4),
      Q6 => D(5),
      Q7 => D(6),
      Q8 => D(7),
      RST => sys_rst,
      SHIFTIN1 => NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED,
      SHIFTIN2 => NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED,
      SHIFTOUT1 => SHIFTOUT1,
      SHIFTOUT2 => SHIFTOUT2
    );
ISERDESE2_inst_slave: unisim.vcomponents.ISERDESE2
    generic map(
      DATA_RATE => "DDR",
      DATA_WIDTH => 14,
      DYN_CLKDIV_INV_EN => "FALSE",
      DYN_CLK_INV_EN => "FALSE",
      INIT_Q1 => '0',
      INIT_Q2 => '0',
      INIT_Q3 => '0',
      INIT_Q4 => '0',
      INTERFACE_TYPE => "NETWORKING",
      IOBDELAY => "NONE",
      IS_CLKB_INVERTED => '1',
      IS_CLKDIVP_INVERTED => '0',
      IS_CLKDIV_INVERTED => '0',
      IS_CLK_INVERTED => '0',
      IS_D_INVERTED => '0',
      IS_OCLKB_INVERTED => '0',
      IS_OCLK_INVERTED => '0',
      NUM_CE => 2,
      OFB_USED => "FALSE",
      SERDES_MODE => "SLAVE",
      SRVAL_Q1 => '0',
      SRVAL_Q2 => '0',
      SRVAL_Q3 => '0',
      SRVAL_Q4 => '0'
    )
        port map (
      BITSLIP => bitslip_cmd,
      CE1 => '1',
      CE2 => '1',
      CLK => clk_bit,
      CLKB => clkb,
      CLKDIV => clk_word,
      CLKDIVP => '0',
      D => NLW_ISERDESE2_inst_slave_D_UNCONNECTED,
      DDLY => NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED,
      DYNCLKDIVSEL => '0',
      DYNCLKSEL => '0',
      O => NLW_ISERDESE2_inst_slave_O_UNCONNECTED,
      OCLK => '0',
      OCLKB => '0',
      OFB => '0',
      Q1 => NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED,
      Q2 => NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED,
      Q3 => D(8),
      Q4 => D(9),
      Q5 => D(10),
      Q6 => D(11),
      Q7 => D(12),
      Q8 => D(13),
      RST => sys_rst,
      SHIFTIN1 => SHIFTOUT1,
      SHIFTIN2 => SHIFTOUT2,
      SHIFTOUT1 => NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED,
      SHIFTOUT2 => NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED
    );
ibufds_inst: unisim.vcomponents.IBUFDS
     port map (
      I => lvds_dat_p(0),
      IB => lvds_dat_n(0),
      O => data_in_single
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_14 is
  port (
    D : out STD_LOGIC_VECTOR ( 13 downto 0 );
    lvds_dat_p : in STD_LOGIC_VECTOR ( 0 to 0 );
    lvds_dat_n : in STD_LOGIC_VECTOR ( 0 to 0 );
    bitslip_cmd : in STD_LOGIC;
    clk_bit : in STD_LOGIC;
    clkb : in STD_LOGIC;
    clk_word : in STD_LOGIC;
    sys_rst : in STD_LOGIC
  );
  attribute ORIG_REF_NAME : string;
  attribute ORIG_REF_NAME of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_14 : entity is "Serdes_1x14_DDR";
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_14;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_14 is
  signal SHIFTOUT1 : STD_LOGIC;
  signal SHIFTOUT2 : STD_LOGIC;
  signal data_in_single : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_O_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_OFB_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_D_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_O_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED : STD_LOGIC;
  attribute BOX_TYPE : string;
  attribute BOX_TYPE of ISERDESE2_inst_master : label is "PRIMITIVE";
  attribute OPT_MODIFIED : string;
  attribute OPT_MODIFIED of ISERDESE2_inst_master : label is "MLO";
  attribute BOX_TYPE of ISERDESE2_inst_slave : label is "PRIMITIVE";
  attribute OPT_MODIFIED of ISERDESE2_inst_slave : label is "MLO";
  attribute BOX_TYPE of ibufds_inst : label is "PRIMITIVE";
  attribute CAPACITANCE : string;
  attribute CAPACITANCE of ibufds_inst : label is "DONT_CARE";
  attribute IBUF_DELAY_VALUE : string;
  attribute IBUF_DELAY_VALUE of ibufds_inst : label is "0";
  attribute IFD_DELAY_VALUE : string;
  attribute IFD_DELAY_VALUE of ibufds_inst : label is "AUTO";
begin
ISERDESE2_inst_master: unisim.vcomponents.ISERDESE2
    generic map(
      DATA_RATE => "DDR",
      DATA_WIDTH => 14,
      DYN_CLKDIV_INV_EN => "FALSE",
      DYN_CLK_INV_EN => "FALSE",
      INIT_Q1 => '0',
      INIT_Q2 => '0',
      INIT_Q3 => '0',
      INIT_Q4 => '0',
      INTERFACE_TYPE => "NETWORKING",
      IOBDELAY => "NONE",
      IS_CLKB_INVERTED => '1',
      IS_CLKDIVP_INVERTED => '0',
      IS_CLKDIV_INVERTED => '0',
      IS_CLK_INVERTED => '0',
      IS_D_INVERTED => '0',
      IS_OCLKB_INVERTED => '0',
      IS_OCLK_INVERTED => '0',
      NUM_CE => 2,
      OFB_USED => "FALSE",
      SERDES_MODE => "MASTER",
      SRVAL_Q1 => '0',
      SRVAL_Q2 => '0',
      SRVAL_Q3 => '0',
      SRVAL_Q4 => '0'
    )
        port map (
      BITSLIP => bitslip_cmd,
      CE1 => '1',
      CE2 => '1',
      CLK => clk_bit,
      CLKB => clkb,
      CLKDIV => clk_word,
      CLKDIVP => '0',
      D => data_in_single,
      DDLY => NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED,
      DYNCLKDIVSEL => '0',
      DYNCLKSEL => '0',
      O => NLW_ISERDESE2_inst_master_O_UNCONNECTED,
      OCLK => '0',
      OCLKB => NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED,
      OFB => NLW_ISERDESE2_inst_master_OFB_UNCONNECTED,
      Q1 => D(0),
      Q2 => D(1),
      Q3 => D(2),
      Q4 => D(3),
      Q5 => D(4),
      Q6 => D(5),
      Q7 => D(6),
      Q8 => D(7),
      RST => sys_rst,
      SHIFTIN1 => NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED,
      SHIFTIN2 => NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED,
      SHIFTOUT1 => SHIFTOUT1,
      SHIFTOUT2 => SHIFTOUT2
    );
ISERDESE2_inst_slave: unisim.vcomponents.ISERDESE2
    generic map(
      DATA_RATE => "DDR",
      DATA_WIDTH => 14,
      DYN_CLKDIV_INV_EN => "FALSE",
      DYN_CLK_INV_EN => "FALSE",
      INIT_Q1 => '0',
      INIT_Q2 => '0',
      INIT_Q3 => '0',
      INIT_Q4 => '0',
      INTERFACE_TYPE => "NETWORKING",
      IOBDELAY => "NONE",
      IS_CLKB_INVERTED => '1',
      IS_CLKDIVP_INVERTED => '0',
      IS_CLKDIV_INVERTED => '0',
      IS_CLK_INVERTED => '0',
      IS_D_INVERTED => '0',
      IS_OCLKB_INVERTED => '0',
      IS_OCLK_INVERTED => '0',
      NUM_CE => 2,
      OFB_USED => "FALSE",
      SERDES_MODE => "SLAVE",
      SRVAL_Q1 => '0',
      SRVAL_Q2 => '0',
      SRVAL_Q3 => '0',
      SRVAL_Q4 => '0'
    )
        port map (
      BITSLIP => bitslip_cmd,
      CE1 => '1',
      CE2 => '1',
      CLK => clk_bit,
      CLKB => clkb,
      CLKDIV => clk_word,
      CLKDIVP => '0',
      D => NLW_ISERDESE2_inst_slave_D_UNCONNECTED,
      DDLY => NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED,
      DYNCLKDIVSEL => '0',
      DYNCLKSEL => '0',
      O => NLW_ISERDESE2_inst_slave_O_UNCONNECTED,
      OCLK => '0',
      OCLKB => '0',
      OFB => '0',
      Q1 => NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED,
      Q2 => NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED,
      Q3 => D(8),
      Q4 => D(9),
      Q5 => D(10),
      Q6 => D(11),
      Q7 => D(12),
      Q8 => D(13),
      RST => sys_rst,
      SHIFTIN1 => SHIFTOUT1,
      SHIFTIN2 => SHIFTOUT2,
      SHIFTOUT1 => NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED,
      SHIFTOUT2 => NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED
    );
ibufds_inst: unisim.vcomponents.IBUFDS
     port map (
      I => lvds_dat_p(0),
      IB => lvds_dat_n(0),
      O => data_in_single
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_7 is
  port (
    D : out STD_LOGIC_VECTOR ( 13 downto 0 );
    lvds_dat_p : in STD_LOGIC_VECTOR ( 0 to 0 );
    lvds_dat_n : in STD_LOGIC_VECTOR ( 0 to 0 );
    bitslip_cmd : in STD_LOGIC;
    clk_bit : in STD_LOGIC;
    clkb : in STD_LOGIC;
    clk_word : in STD_LOGIC;
    sys_rst : in STD_LOGIC
  );
  attribute ORIG_REF_NAME : string;
  attribute ORIG_REF_NAME of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_7 : entity is "Serdes_1x14_DDR";
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_7;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_7 is
  signal SHIFTOUT1 : STD_LOGIC;
  signal SHIFTOUT2 : STD_LOGIC;
  signal data_in_single : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_O_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_OFB_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_D_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_O_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED : STD_LOGIC;
  attribute BOX_TYPE : string;
  attribute BOX_TYPE of ISERDESE2_inst_master : label is "PRIMITIVE";
  attribute OPT_MODIFIED : string;
  attribute OPT_MODIFIED of ISERDESE2_inst_master : label is "MLO";
  attribute BOX_TYPE of ISERDESE2_inst_slave : label is "PRIMITIVE";
  attribute OPT_MODIFIED of ISERDESE2_inst_slave : label is "MLO";
  attribute BOX_TYPE of ibufds_inst : label is "PRIMITIVE";
  attribute CAPACITANCE : string;
  attribute CAPACITANCE of ibufds_inst : label is "DONT_CARE";
  attribute IBUF_DELAY_VALUE : string;
  attribute IBUF_DELAY_VALUE of ibufds_inst : label is "0";
  attribute IFD_DELAY_VALUE : string;
  attribute IFD_DELAY_VALUE of ibufds_inst : label is "AUTO";
begin
ISERDESE2_inst_master: unisim.vcomponents.ISERDESE2
    generic map(
      DATA_RATE => "DDR",
      DATA_WIDTH => 14,
      DYN_CLKDIV_INV_EN => "FALSE",
      DYN_CLK_INV_EN => "FALSE",
      INIT_Q1 => '0',
      INIT_Q2 => '0',
      INIT_Q3 => '0',
      INIT_Q4 => '0',
      INTERFACE_TYPE => "NETWORKING",
      IOBDELAY => "NONE",
      IS_CLKB_INVERTED => '1',
      IS_CLKDIVP_INVERTED => '0',
      IS_CLKDIV_INVERTED => '0',
      IS_CLK_INVERTED => '0',
      IS_D_INVERTED => '0',
      IS_OCLKB_INVERTED => '0',
      IS_OCLK_INVERTED => '0',
      NUM_CE => 2,
      OFB_USED => "FALSE",
      SERDES_MODE => "MASTER",
      SRVAL_Q1 => '0',
      SRVAL_Q2 => '0',
      SRVAL_Q3 => '0',
      SRVAL_Q4 => '0'
    )
        port map (
      BITSLIP => bitslip_cmd,
      CE1 => '1',
      CE2 => '1',
      CLK => clk_bit,
      CLKB => clkb,
      CLKDIV => clk_word,
      CLKDIVP => '0',
      D => data_in_single,
      DDLY => NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED,
      DYNCLKDIVSEL => '0',
      DYNCLKSEL => '0',
      O => NLW_ISERDESE2_inst_master_O_UNCONNECTED,
      OCLK => '0',
      OCLKB => NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED,
      OFB => NLW_ISERDESE2_inst_master_OFB_UNCONNECTED,
      Q1 => D(0),
      Q2 => D(1),
      Q3 => D(2),
      Q4 => D(3),
      Q5 => D(4),
      Q6 => D(5),
      Q7 => D(6),
      Q8 => D(7),
      RST => sys_rst,
      SHIFTIN1 => NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED,
      SHIFTIN2 => NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED,
      SHIFTOUT1 => SHIFTOUT1,
      SHIFTOUT2 => SHIFTOUT2
    );
ISERDESE2_inst_slave: unisim.vcomponents.ISERDESE2
    generic map(
      DATA_RATE => "DDR",
      DATA_WIDTH => 14,
      DYN_CLKDIV_INV_EN => "FALSE",
      DYN_CLK_INV_EN => "FALSE",
      INIT_Q1 => '0',
      INIT_Q2 => '0',
      INIT_Q3 => '0',
      INIT_Q4 => '0',
      INTERFACE_TYPE => "NETWORKING",
      IOBDELAY => "NONE",
      IS_CLKB_INVERTED => '1',
      IS_CLKDIVP_INVERTED => '0',
      IS_CLKDIV_INVERTED => '0',
      IS_CLK_INVERTED => '0',
      IS_D_INVERTED => '0',
      IS_OCLKB_INVERTED => '0',
      IS_OCLK_INVERTED => '0',
      NUM_CE => 2,
      OFB_USED => "FALSE",
      SERDES_MODE => "SLAVE",
      SRVAL_Q1 => '0',
      SRVAL_Q2 => '0',
      SRVAL_Q3 => '0',
      SRVAL_Q4 => '0'
    )
        port map (
      BITSLIP => bitslip_cmd,
      CE1 => '1',
      CE2 => '1',
      CLK => clk_bit,
      CLKB => clkb,
      CLKDIV => clk_word,
      CLKDIVP => '0',
      D => NLW_ISERDESE2_inst_slave_D_UNCONNECTED,
      DDLY => NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED,
      DYNCLKDIVSEL => '0',
      DYNCLKSEL => '0',
      O => NLW_ISERDESE2_inst_slave_O_UNCONNECTED,
      OCLK => '0',
      OCLKB => '0',
      OFB => '0',
      Q1 => NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED,
      Q2 => NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED,
      Q3 => D(8),
      Q4 => D(9),
      Q5 => D(10),
      Q6 => D(11),
      Q7 => D(12),
      Q8 => D(13),
      RST => sys_rst,
      SHIFTIN1 => SHIFTOUT1,
      SHIFTIN2 => SHIFTOUT2,
      SHIFTOUT1 => NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED,
      SHIFTOUT2 => NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED
    );
ibufds_inst: unisim.vcomponents.IBUFDS
     port map (
      I => lvds_dat_p(0),
      IB => lvds_dat_n(0),
      O => data_in_single
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_8 is
  port (
    D : out STD_LOGIC_VECTOR ( 13 downto 0 );
    lvds_dat_p : in STD_LOGIC_VECTOR ( 0 to 0 );
    lvds_dat_n : in STD_LOGIC_VECTOR ( 0 to 0 );
    bitslip_cmd : in STD_LOGIC;
    clk_bit : in STD_LOGIC;
    clkb : in STD_LOGIC;
    clk_word : in STD_LOGIC;
    sys_rst : in STD_LOGIC
  );
  attribute ORIG_REF_NAME : string;
  attribute ORIG_REF_NAME of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_8 : entity is "Serdes_1x14_DDR";
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_8;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_8 is
  signal SHIFTOUT1 : STD_LOGIC;
  signal SHIFTOUT2 : STD_LOGIC;
  signal data_in_single : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_O_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_OFB_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_D_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_O_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED : STD_LOGIC;
  attribute BOX_TYPE : string;
  attribute BOX_TYPE of ISERDESE2_inst_master : label is "PRIMITIVE";
  attribute OPT_MODIFIED : string;
  attribute OPT_MODIFIED of ISERDESE2_inst_master : label is "MLO";
  attribute BOX_TYPE of ISERDESE2_inst_slave : label is "PRIMITIVE";
  attribute OPT_MODIFIED of ISERDESE2_inst_slave : label is "MLO";
  attribute BOX_TYPE of ibufds_inst : label is "PRIMITIVE";
  attribute CAPACITANCE : string;
  attribute CAPACITANCE of ibufds_inst : label is "DONT_CARE";
  attribute IBUF_DELAY_VALUE : string;
  attribute IBUF_DELAY_VALUE of ibufds_inst : label is "0";
  attribute IFD_DELAY_VALUE : string;
  attribute IFD_DELAY_VALUE of ibufds_inst : label is "AUTO";
begin
ISERDESE2_inst_master: unisim.vcomponents.ISERDESE2
    generic map(
      DATA_RATE => "DDR",
      DATA_WIDTH => 14,
      DYN_CLKDIV_INV_EN => "FALSE",
      DYN_CLK_INV_EN => "FALSE",
      INIT_Q1 => '0',
      INIT_Q2 => '0',
      INIT_Q3 => '0',
      INIT_Q4 => '0',
      INTERFACE_TYPE => "NETWORKING",
      IOBDELAY => "NONE",
      IS_CLKB_INVERTED => '1',
      IS_CLKDIVP_INVERTED => '0',
      IS_CLKDIV_INVERTED => '0',
      IS_CLK_INVERTED => '0',
      IS_D_INVERTED => '0',
      IS_OCLKB_INVERTED => '0',
      IS_OCLK_INVERTED => '0',
      NUM_CE => 2,
      OFB_USED => "FALSE",
      SERDES_MODE => "MASTER",
      SRVAL_Q1 => '0',
      SRVAL_Q2 => '0',
      SRVAL_Q3 => '0',
      SRVAL_Q4 => '0'
    )
        port map (
      BITSLIP => bitslip_cmd,
      CE1 => '1',
      CE2 => '1',
      CLK => clk_bit,
      CLKB => clkb,
      CLKDIV => clk_word,
      CLKDIVP => '0',
      D => data_in_single,
      DDLY => NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED,
      DYNCLKDIVSEL => '0',
      DYNCLKSEL => '0',
      O => NLW_ISERDESE2_inst_master_O_UNCONNECTED,
      OCLK => '0',
      OCLKB => NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED,
      OFB => NLW_ISERDESE2_inst_master_OFB_UNCONNECTED,
      Q1 => D(0),
      Q2 => D(1),
      Q3 => D(2),
      Q4 => D(3),
      Q5 => D(4),
      Q6 => D(5),
      Q7 => D(6),
      Q8 => D(7),
      RST => sys_rst,
      SHIFTIN1 => NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED,
      SHIFTIN2 => NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED,
      SHIFTOUT1 => SHIFTOUT1,
      SHIFTOUT2 => SHIFTOUT2
    );
ISERDESE2_inst_slave: unisim.vcomponents.ISERDESE2
    generic map(
      DATA_RATE => "DDR",
      DATA_WIDTH => 14,
      DYN_CLKDIV_INV_EN => "FALSE",
      DYN_CLK_INV_EN => "FALSE",
      INIT_Q1 => '0',
      INIT_Q2 => '0',
      INIT_Q3 => '0',
      INIT_Q4 => '0',
      INTERFACE_TYPE => "NETWORKING",
      IOBDELAY => "NONE",
      IS_CLKB_INVERTED => '1',
      IS_CLKDIVP_INVERTED => '0',
      IS_CLKDIV_INVERTED => '0',
      IS_CLK_INVERTED => '0',
      IS_D_INVERTED => '0',
      IS_OCLKB_INVERTED => '0',
      IS_OCLK_INVERTED => '0',
      NUM_CE => 2,
      OFB_USED => "FALSE",
      SERDES_MODE => "SLAVE",
      SRVAL_Q1 => '0',
      SRVAL_Q2 => '0',
      SRVAL_Q3 => '0',
      SRVAL_Q4 => '0'
    )
        port map (
      BITSLIP => bitslip_cmd,
      CE1 => '1',
      CE2 => '1',
      CLK => clk_bit,
      CLKB => clkb,
      CLKDIV => clk_word,
      CLKDIVP => '0',
      D => NLW_ISERDESE2_inst_slave_D_UNCONNECTED,
      DDLY => NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED,
      DYNCLKDIVSEL => '0',
      DYNCLKSEL => '0',
      O => NLW_ISERDESE2_inst_slave_O_UNCONNECTED,
      OCLK => '0',
      OCLKB => '0',
      OFB => '0',
      Q1 => NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED,
      Q2 => NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED,
      Q3 => D(8),
      Q4 => D(9),
      Q5 => D(10),
      Q6 => D(11),
      Q7 => D(12),
      Q8 => D(13),
      RST => sys_rst,
      SHIFTIN1 => SHIFTOUT1,
      SHIFTIN2 => SHIFTOUT2,
      SHIFTOUT1 => NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED,
      SHIFTOUT2 => NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED
    );
ibufds_inst: unisim.vcomponents.IBUFDS
     port map (
      I => lvds_dat_p(0),
      IB => lvds_dat_n(0),
      O => data_in_single
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_9 is
  port (
    lvds_dat_p : in STD_LOGIC_VECTOR ( 0 to 0 );
    lvds_dat_n : in STD_LOGIC_VECTOR ( 0 to 0 );
    bitslip_cmd : in STD_LOGIC;
    clk_bit : in STD_LOGIC;
    clkb : in STD_LOGIC;
    clk_word : in STD_LOGIC;
    sys_rst : in STD_LOGIC
  );
  attribute ORIG_REF_NAME : string;
  attribute ORIG_REF_NAME of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_9 : entity is "Serdes_1x14_DDR";
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_9;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_9 is
  signal ADCDataLine : STD_LOGIC_VECTOR ( 13 downto 0 );
  signal SHIFTOUT1 : STD_LOGIC;
  signal SHIFTOUT2 : STD_LOGIC;
  signal data_in_single : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_O_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_OFB_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_D_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_O_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED : STD_LOGIC;
  signal NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED : STD_LOGIC;
  attribute BOX_TYPE : string;
  attribute BOX_TYPE of ISERDESE2_inst_master : label is "PRIMITIVE";
  attribute OPT_MODIFIED : string;
  attribute OPT_MODIFIED of ISERDESE2_inst_master : label is "MLO";
  attribute BOX_TYPE of ISERDESE2_inst_slave : label is "PRIMITIVE";
  attribute OPT_MODIFIED of ISERDESE2_inst_slave : label is "MLO";
  attribute BOX_TYPE of ibufds_inst : label is "PRIMITIVE";
  attribute CAPACITANCE : string;
  attribute CAPACITANCE of ibufds_inst : label is "DONT_CARE";
  attribute IBUF_DELAY_VALUE : string;
  attribute IBUF_DELAY_VALUE of ibufds_inst : label is "0";
  attribute IFD_DELAY_VALUE : string;
  attribute IFD_DELAY_VALUE of ibufds_inst : label is "AUTO";
begin
ISERDESE2_inst_master: unisim.vcomponents.ISERDESE2
    generic map(
      DATA_RATE => "DDR",
      DATA_WIDTH => 14,
      DYN_CLKDIV_INV_EN => "FALSE",
      DYN_CLK_INV_EN => "FALSE",
      INIT_Q1 => '0',
      INIT_Q2 => '0',
      INIT_Q3 => '0',
      INIT_Q4 => '0',
      INTERFACE_TYPE => "NETWORKING",
      IOBDELAY => "NONE",
      IS_CLKB_INVERTED => '1',
      IS_CLKDIVP_INVERTED => '0',
      IS_CLKDIV_INVERTED => '0',
      IS_CLK_INVERTED => '0',
      IS_D_INVERTED => '0',
      IS_OCLKB_INVERTED => '0',
      IS_OCLK_INVERTED => '0',
      NUM_CE => 2,
      OFB_USED => "FALSE",
      SERDES_MODE => "MASTER",
      SRVAL_Q1 => '0',
      SRVAL_Q2 => '0',
      SRVAL_Q3 => '0',
      SRVAL_Q4 => '0'
    )
        port map (
      BITSLIP => bitslip_cmd,
      CE1 => '1',
      CE2 => '1',
      CLK => clk_bit,
      CLKB => clkb,
      CLKDIV => clk_word,
      CLKDIVP => '0',
      D => data_in_single,
      DDLY => NLW_ISERDESE2_inst_master_DDLY_UNCONNECTED,
      DYNCLKDIVSEL => '0',
      DYNCLKSEL => '0',
      O => NLW_ISERDESE2_inst_master_O_UNCONNECTED,
      OCLK => '0',
      OCLKB => NLW_ISERDESE2_inst_master_OCLKB_UNCONNECTED,
      OFB => NLW_ISERDESE2_inst_master_OFB_UNCONNECTED,
      Q1 => ADCDataLine(0),
      Q2 => ADCDataLine(1),
      Q3 => ADCDataLine(2),
      Q4 => ADCDataLine(3),
      Q5 => ADCDataLine(4),
      Q6 => ADCDataLine(5),
      Q7 => ADCDataLine(6),
      Q8 => ADCDataLine(7),
      RST => sys_rst,
      SHIFTIN1 => NLW_ISERDESE2_inst_master_SHIFTIN1_UNCONNECTED,
      SHIFTIN2 => NLW_ISERDESE2_inst_master_SHIFTIN2_UNCONNECTED,
      SHIFTOUT1 => SHIFTOUT1,
      SHIFTOUT2 => SHIFTOUT2
    );
ISERDESE2_inst_slave: unisim.vcomponents.ISERDESE2
    generic map(
      DATA_RATE => "DDR",
      DATA_WIDTH => 14,
      DYN_CLKDIV_INV_EN => "FALSE",
      DYN_CLK_INV_EN => "FALSE",
      INIT_Q1 => '0',
      INIT_Q2 => '0',
      INIT_Q3 => '0',
      INIT_Q4 => '0',
      INTERFACE_TYPE => "NETWORKING",
      IOBDELAY => "NONE",
      IS_CLKB_INVERTED => '1',
      IS_CLKDIVP_INVERTED => '0',
      IS_CLKDIV_INVERTED => '0',
      IS_CLK_INVERTED => '0',
      IS_D_INVERTED => '0',
      IS_OCLKB_INVERTED => '0',
      IS_OCLK_INVERTED => '0',
      NUM_CE => 2,
      OFB_USED => "FALSE",
      SERDES_MODE => "SLAVE",
      SRVAL_Q1 => '0',
      SRVAL_Q2 => '0',
      SRVAL_Q3 => '0',
      SRVAL_Q4 => '0'
    )
        port map (
      BITSLIP => bitslip_cmd,
      CE1 => '1',
      CE2 => '1',
      CLK => clk_bit,
      CLKB => clkb,
      CLKDIV => clk_word,
      CLKDIVP => '0',
      D => NLW_ISERDESE2_inst_slave_D_UNCONNECTED,
      DDLY => NLW_ISERDESE2_inst_slave_DDLY_UNCONNECTED,
      DYNCLKDIVSEL => '0',
      DYNCLKSEL => '0',
      O => NLW_ISERDESE2_inst_slave_O_UNCONNECTED,
      OCLK => '0',
      OCLKB => '0',
      OFB => '0',
      Q1 => NLW_ISERDESE2_inst_slave_Q1_UNCONNECTED,
      Q2 => NLW_ISERDESE2_inst_slave_Q2_UNCONNECTED,
      Q3 => ADCDataLine(8),
      Q4 => ADCDataLine(9),
      Q5 => ADCDataLine(10),
      Q6 => ADCDataLine(11),
      Q7 => ADCDataLine(12),
      Q8 => ADCDataLine(13),
      RST => sys_rst,
      SHIFTIN1 => SHIFTOUT1,
      SHIFTIN2 => SHIFTOUT2,
      SHIFTOUT1 => NLW_ISERDESE2_inst_slave_SHIFTOUT1_UNCONNECTED,
      SHIFTOUT2 => NLW_ISERDESE2_inst_slave_SHIFTOUT2_UNCONNECTED
    );
ibufds_inst: unisim.vcomponents.IBUFDS
     port map (
      I => lvds_dat_p(0),
      IB => lvds_dat_n(0),
      O => data_in_single
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcFrame is
  port (
    bitslip_cmd : out STD_LOGIC;
    m_axis_tvalid : out STD_LOGIC;
    lvds_fco_p : in STD_LOGIC;
    lvds_fco_n : in STD_LOGIC;
    clk_bit : in STD_LOGIC;
    clkb : in STD_LOGIC;
    clk_word : in STD_LOGIC;
    sys_rst : in STD_LOGIC
  );
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcFrame;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcFrame is
  signal \^bitslip_cmd\ : STD_LOGIC;
  signal inst_Serdes_1x14_DDR_Data_Line_n_0 : STD_LOGIC;
  signal inst_Serdes_1x14_DDR_Data_Line_n_1 : STD_LOGIC;
  signal \^m_axis_tvalid\ : STD_LOGIC;
  signal p_1_in : STD_LOGIC_VECTOR ( 4 downto 0 );
  signal \rBitslipCnt_reg_n_0_[0]\ : STD_LOGIC;
  signal \rBitslipCnt_reg_n_0_[1]\ : STD_LOGIC;
  signal \rBitslipCnt_reg_n_0_[2]\ : STD_LOGIC;
  signal \rBitslipCnt_reg_n_0_[3]\ : STD_LOGIC;
  signal \rBitslipCnt_reg_n_0_[4]\ : STD_LOGIC;
  signal rFrmAlignDone : STD_LOGIC;
  attribute SOFT_HLUTNM : string;
  attribute SOFT_HLUTNM of \rBitslipCnt[0]_i_1\ : label is "soft_lutpair2";
  attribute SOFT_HLUTNM of \rBitslipCnt[1]_i_1\ : label is "soft_lutpair2";
  attribute SOFT_HLUTNM of \rBitslipCnt[2]_i_1\ : label is "soft_lutpair1";
  attribute SOFT_HLUTNM of \rBitslipCnt[3]_i_1\ : label is "soft_lutpair1";
  attribute SOFT_HLUTNM of \rBitslipCnt[4]_i_1\ : label is "soft_lutpair0";
  attribute SOFT_HLUTNM of rFrmAlignDone_i_5 : label is "soft_lutpair0";
begin
  bitslip_cmd <= \^bitslip_cmd\;
  m_axis_tvalid <= \^m_axis_tvalid\;
inst_Serdes_1x14_DDR_Data_Line: entity work.decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR
     port map (
      bitslip_cmd => \^bitslip_cmd\,
      clk_bit => clk_bit,
      clk_word => clk_word,
      clkb => clkb,
      lvds_fco_n => lvds_fco_n,
      lvds_fco_p => lvds_fco_p,
      m_axis_tvalid => \^m_axis_tvalid\,
      rFrmAlignDone => rFrmAlignDone,
      rFrmAlignDone_reg => inst_Serdes_1x14_DDR_Data_Line_n_0,
      sys_rst => sys_rst,
      sys_rst_0 => inst_Serdes_1x14_DDR_Data_Line_n_1
    );
\rBitslipCnt[0]_i_1\: unisim.vcomponents.LUT1
    generic map(
      INIT => X"1"
    )
        port map (
      I0 => \rBitslipCnt_reg_n_0_[0]\,
      O => p_1_in(0)
    );
\rBitslipCnt[1]_i_1\: unisim.vcomponents.LUT2
    generic map(
      INIT => X"6"
    )
        port map (
      I0 => \rBitslipCnt_reg_n_0_[0]\,
      I1 => \rBitslipCnt_reg_n_0_[1]\,
      O => p_1_in(1)
    );
\rBitslipCnt[2]_i_1\: unisim.vcomponents.LUT3
    generic map(
      INIT => X"78"
    )
        port map (
      I0 => \rBitslipCnt_reg_n_0_[1]\,
      I1 => \rBitslipCnt_reg_n_0_[0]\,
      I2 => \rBitslipCnt_reg_n_0_[2]\,
      O => p_1_in(2)
    );
\rBitslipCnt[3]_i_1\: unisim.vcomponents.LUT4
    generic map(
      INIT => X"7F80"
    )
        port map (
      I0 => \rBitslipCnt_reg_n_0_[2]\,
      I1 => \rBitslipCnt_reg_n_0_[0]\,
      I2 => \rBitslipCnt_reg_n_0_[1]\,
      I3 => \rBitslipCnt_reg_n_0_[3]\,
      O => p_1_in(3)
    );
\rBitslipCnt[4]_i_1\: unisim.vcomponents.LUT5
    generic map(
      INIT => X"7FFF8000"
    )
        port map (
      I0 => \rBitslipCnt_reg_n_0_[3]\,
      I1 => \rBitslipCnt_reg_n_0_[1]\,
      I2 => \rBitslipCnt_reg_n_0_[0]\,
      I3 => \rBitslipCnt_reg_n_0_[2]\,
      I4 => \rBitslipCnt_reg_n_0_[4]\,
      O => p_1_in(4)
    );
\rBitslipCnt_reg[0]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => p_1_in(0),
      Q => \rBitslipCnt_reg_n_0_[0]\,
      R => sys_rst
    );
\rBitslipCnt_reg[1]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => p_1_in(1),
      Q => \rBitslipCnt_reg_n_0_[1]\,
      R => sys_rst
    );
\rBitslipCnt_reg[2]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => p_1_in(2),
      Q => \rBitslipCnt_reg_n_0_[2]\,
      R => sys_rst
    );
\rBitslipCnt_reg[3]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => p_1_in(3),
      Q => \rBitslipCnt_reg_n_0_[3]\,
      R => sys_rst
    );
\rBitslipCnt_reg[4]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => p_1_in(4),
      Q => \rBitslipCnt_reg_n_0_[4]\,
      R => sys_rst
    );
rFrmAlignDone_i_5: unisim.vcomponents.LUT5
    generic map(
      INIT => X"00000001"
    )
        port map (
      I0 => \rBitslipCnt_reg_n_0_[1]\,
      I1 => \rBitslipCnt_reg_n_0_[0]\,
      I2 => \rBitslipCnt_reg_n_0_[2]\,
      I3 => \rBitslipCnt_reg_n_0_[3]\,
      I4 => \rBitslipCnt_reg_n_0_[4]\,
      O => rFrmAlignDone
    );
rFrmAlignDone_reg: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => inst_Serdes_1x14_DDR_Data_Line_n_0,
      Q => \^m_axis_tvalid\,
      R => sys_rst
    );
rFrmBitslip_reg: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => inst_Serdes_1x14_DDR_Data_Line_n_1,
      Q => \^bitslip_cmd\,
      R => '0'
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane is
  port (
    m_axis_tdata : out STD_LOGIC_VECTOR ( 13 downto 0 );
    lvds_dat_p : in STD_LOGIC_VECTOR ( 0 to 0 );
    lvds_dat_n : in STD_LOGIC_VECTOR ( 0 to 0 );
    bitslip_cmd : in STD_LOGIC;
    clk_bit : in STD_LOGIC;
    clkb : in STD_LOGIC;
    clk_word : in STD_LOGIC;
    sys_rst : in STD_LOGIC
  );
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane is
  signal ADCDataLine : STD_LOGIC_VECTOR ( 13 downto 0 );
begin
\ADCData_reg[0]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(0),
      Q => m_axis_tdata(0),
      R => '0'
    );
\ADCData_reg[10]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(10),
      Q => m_axis_tdata(10),
      R => '0'
    );
\ADCData_reg[11]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(11),
      Q => m_axis_tdata(11),
      R => '0'
    );
\ADCData_reg[12]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(12),
      Q => m_axis_tdata(12),
      R => '0'
    );
\ADCData_reg[13]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(13),
      Q => m_axis_tdata(13),
      R => '0'
    );
\ADCData_reg[1]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(1),
      Q => m_axis_tdata(1),
      R => '0'
    );
\ADCData_reg[2]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(2),
      Q => m_axis_tdata(2),
      R => '0'
    );
\ADCData_reg[3]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(3),
      Q => m_axis_tdata(3),
      R => '0'
    );
\ADCData_reg[4]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(4),
      Q => m_axis_tdata(4),
      R => '0'
    );
\ADCData_reg[5]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(5),
      Q => m_axis_tdata(5),
      R => '0'
    );
\ADCData_reg[6]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(6),
      Q => m_axis_tdata(6),
      R => '0'
    );
\ADCData_reg[7]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(7),
      Q => m_axis_tdata(7),
      R => '0'
    );
\ADCData_reg[8]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(8),
      Q => m_axis_tdata(8),
      R => '0'
    );
\ADCData_reg[9]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(9),
      Q => m_axis_tdata(9),
      R => '0'
    );
inst_Serdes_1x14_DDR_Data_Line: entity work.decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_14
     port map (
      D(13 downto 0) => ADCDataLine(13 downto 0),
      bitslip_cmd => bitslip_cmd,
      clk_bit => clk_bit,
      clk_word => clk_word,
      clkb => clkb,
      lvds_dat_n(0) => lvds_dat_n(0),
      lvds_dat_p(0) => lvds_dat_p(0),
      sys_rst => sys_rst
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_0 is
  port (
    m_axis_tdata : out STD_LOGIC_VECTOR ( 13 downto 0 );
    lvds_dat_p : in STD_LOGIC_VECTOR ( 0 to 0 );
    lvds_dat_n : in STD_LOGIC_VECTOR ( 0 to 0 );
    bitslip_cmd : in STD_LOGIC;
    clk_bit : in STD_LOGIC;
    clkb : in STD_LOGIC;
    clk_word : in STD_LOGIC;
    sys_rst : in STD_LOGIC
  );
  attribute ORIG_REF_NAME : string;
  attribute ORIG_REF_NAME of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_0 : entity is "AdcLane";
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_0;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_0 is
  signal ADCDataLine : STD_LOGIC_VECTOR ( 13 downto 0 );
begin
\ADCData_reg[0]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(0),
      Q => m_axis_tdata(0),
      R => '0'
    );
\ADCData_reg[10]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(10),
      Q => m_axis_tdata(10),
      R => '0'
    );
\ADCData_reg[11]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(11),
      Q => m_axis_tdata(11),
      R => '0'
    );
\ADCData_reg[12]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(12),
      Q => m_axis_tdata(12),
      R => '0'
    );
\ADCData_reg[13]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(13),
      Q => m_axis_tdata(13),
      R => '0'
    );
\ADCData_reg[1]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(1),
      Q => m_axis_tdata(1),
      R => '0'
    );
\ADCData_reg[2]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(2),
      Q => m_axis_tdata(2),
      R => '0'
    );
\ADCData_reg[3]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(3),
      Q => m_axis_tdata(3),
      R => '0'
    );
\ADCData_reg[4]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(4),
      Q => m_axis_tdata(4),
      R => '0'
    );
\ADCData_reg[5]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(5),
      Q => m_axis_tdata(5),
      R => '0'
    );
\ADCData_reg[6]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(6),
      Q => m_axis_tdata(6),
      R => '0'
    );
\ADCData_reg[7]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(7),
      Q => m_axis_tdata(7),
      R => '0'
    );
\ADCData_reg[8]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(8),
      Q => m_axis_tdata(8),
      R => '0'
    );
\ADCData_reg[9]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(9),
      Q => m_axis_tdata(9),
      R => '0'
    );
inst_Serdes_1x14_DDR_Data_Line: entity work.decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_13
     port map (
      D(13 downto 0) => ADCDataLine(13 downto 0),
      bitslip_cmd => bitslip_cmd,
      clk_bit => clk_bit,
      clk_word => clk_word,
      clkb => clkb,
      lvds_dat_n(0) => lvds_dat_n(0),
      lvds_dat_p(0) => lvds_dat_p(0),
      sys_rst => sys_rst
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_1 is
  port (
    m_axis_tdata : out STD_LOGIC_VECTOR ( 13 downto 0 );
    lvds_dat_p : in STD_LOGIC_VECTOR ( 0 to 0 );
    lvds_dat_n : in STD_LOGIC_VECTOR ( 0 to 0 );
    bitslip_cmd : in STD_LOGIC;
    clk_bit : in STD_LOGIC;
    clkb : in STD_LOGIC;
    clk_word : in STD_LOGIC;
    sys_rst : in STD_LOGIC
  );
  attribute ORIG_REF_NAME : string;
  attribute ORIG_REF_NAME of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_1 : entity is "AdcLane";
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_1;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_1 is
  signal ADCDataLine : STD_LOGIC_VECTOR ( 13 downto 0 );
begin
\ADCData_reg[0]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(0),
      Q => m_axis_tdata(0),
      R => '0'
    );
\ADCData_reg[10]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(10),
      Q => m_axis_tdata(10),
      R => '0'
    );
\ADCData_reg[11]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(11),
      Q => m_axis_tdata(11),
      R => '0'
    );
\ADCData_reg[12]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(12),
      Q => m_axis_tdata(12),
      R => '0'
    );
\ADCData_reg[13]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(13),
      Q => m_axis_tdata(13),
      R => '0'
    );
\ADCData_reg[1]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(1),
      Q => m_axis_tdata(1),
      R => '0'
    );
\ADCData_reg[2]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(2),
      Q => m_axis_tdata(2),
      R => '0'
    );
\ADCData_reg[3]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(3),
      Q => m_axis_tdata(3),
      R => '0'
    );
\ADCData_reg[4]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(4),
      Q => m_axis_tdata(4),
      R => '0'
    );
\ADCData_reg[5]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(5),
      Q => m_axis_tdata(5),
      R => '0'
    );
\ADCData_reg[6]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(6),
      Q => m_axis_tdata(6),
      R => '0'
    );
\ADCData_reg[7]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(7),
      Q => m_axis_tdata(7),
      R => '0'
    );
\ADCData_reg[8]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(8),
      Q => m_axis_tdata(8),
      R => '0'
    );
\ADCData_reg[9]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(9),
      Q => m_axis_tdata(9),
      R => '0'
    );
inst_Serdes_1x14_DDR_Data_Line: entity work.decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_12
     port map (
      D(13 downto 0) => ADCDataLine(13 downto 0),
      bitslip_cmd => bitslip_cmd,
      clk_bit => clk_bit,
      clk_word => clk_word,
      clkb => clkb,
      lvds_dat_n(0) => lvds_dat_n(0),
      lvds_dat_p(0) => lvds_dat_p(0),
      sys_rst => sys_rst
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_2 is
  port (
    m_axis_tdata : out STD_LOGIC_VECTOR ( 13 downto 0 );
    lvds_dat_p : in STD_LOGIC_VECTOR ( 0 to 0 );
    lvds_dat_n : in STD_LOGIC_VECTOR ( 0 to 0 );
    bitslip_cmd : in STD_LOGIC;
    clk_bit : in STD_LOGIC;
    clkb : in STD_LOGIC;
    clk_word : in STD_LOGIC;
    sys_rst : in STD_LOGIC
  );
  attribute ORIG_REF_NAME : string;
  attribute ORIG_REF_NAME of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_2 : entity is "AdcLane";
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_2;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_2 is
  signal ADCDataLine : STD_LOGIC_VECTOR ( 13 downto 0 );
begin
\ADCData_reg[0]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(0),
      Q => m_axis_tdata(0),
      R => '0'
    );
\ADCData_reg[10]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(10),
      Q => m_axis_tdata(10),
      R => '0'
    );
\ADCData_reg[11]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(11),
      Q => m_axis_tdata(11),
      R => '0'
    );
\ADCData_reg[12]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(12),
      Q => m_axis_tdata(12),
      R => '0'
    );
\ADCData_reg[13]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(13),
      Q => m_axis_tdata(13),
      R => '0'
    );
\ADCData_reg[1]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(1),
      Q => m_axis_tdata(1),
      R => '0'
    );
\ADCData_reg[2]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(2),
      Q => m_axis_tdata(2),
      R => '0'
    );
\ADCData_reg[3]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(3),
      Q => m_axis_tdata(3),
      R => '0'
    );
\ADCData_reg[4]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(4),
      Q => m_axis_tdata(4),
      R => '0'
    );
\ADCData_reg[5]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(5),
      Q => m_axis_tdata(5),
      R => '0'
    );
\ADCData_reg[6]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(6),
      Q => m_axis_tdata(6),
      R => '0'
    );
\ADCData_reg[7]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(7),
      Q => m_axis_tdata(7),
      R => '0'
    );
\ADCData_reg[8]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(8),
      Q => m_axis_tdata(8),
      R => '0'
    );
\ADCData_reg[9]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(9),
      Q => m_axis_tdata(9),
      R => '0'
    );
inst_Serdes_1x14_DDR_Data_Line: entity work.decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_11
     port map (
      D(13 downto 0) => ADCDataLine(13 downto 0),
      bitslip_cmd => bitslip_cmd,
      clk_bit => clk_bit,
      clk_word => clk_word,
      clkb => clkb,
      lvds_dat_n(0) => lvds_dat_n(0),
      lvds_dat_p(0) => lvds_dat_p(0),
      sys_rst => sys_rst
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_3 is
  port (
    m_axis_tdata : out STD_LOGIC_VECTOR ( 13 downto 0 );
    lvds_dat_p : in STD_LOGIC_VECTOR ( 0 to 0 );
    lvds_dat_n : in STD_LOGIC_VECTOR ( 0 to 0 );
    bitslip_cmd : in STD_LOGIC;
    clk_bit : in STD_LOGIC;
    clkb : in STD_LOGIC;
    clk_word : in STD_LOGIC;
    sys_rst : in STD_LOGIC
  );
  attribute ORIG_REF_NAME : string;
  attribute ORIG_REF_NAME of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_3 : entity is "AdcLane";
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_3;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_3 is
  signal ADCDataLine : STD_LOGIC_VECTOR ( 13 downto 0 );
begin
\ADCData_reg[0]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(0),
      Q => m_axis_tdata(0),
      R => '0'
    );
\ADCData_reg[10]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(10),
      Q => m_axis_tdata(10),
      R => '0'
    );
\ADCData_reg[11]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(11),
      Q => m_axis_tdata(11),
      R => '0'
    );
\ADCData_reg[12]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(12),
      Q => m_axis_tdata(12),
      R => '0'
    );
\ADCData_reg[13]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(13),
      Q => m_axis_tdata(13),
      R => '0'
    );
\ADCData_reg[1]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(1),
      Q => m_axis_tdata(1),
      R => '0'
    );
\ADCData_reg[2]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(2),
      Q => m_axis_tdata(2),
      R => '0'
    );
\ADCData_reg[3]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(3),
      Q => m_axis_tdata(3),
      R => '0'
    );
\ADCData_reg[4]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(4),
      Q => m_axis_tdata(4),
      R => '0'
    );
\ADCData_reg[5]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(5),
      Q => m_axis_tdata(5),
      R => '0'
    );
\ADCData_reg[6]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(6),
      Q => m_axis_tdata(6),
      R => '0'
    );
\ADCData_reg[7]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(7),
      Q => m_axis_tdata(7),
      R => '0'
    );
\ADCData_reg[8]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(8),
      Q => m_axis_tdata(8),
      R => '0'
    );
\ADCData_reg[9]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(9),
      Q => m_axis_tdata(9),
      R => '0'
    );
inst_Serdes_1x14_DDR_Data_Line: entity work.decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_10
     port map (
      D(13 downto 0) => ADCDataLine(13 downto 0),
      bitslip_cmd => bitslip_cmd,
      clk_bit => clk_bit,
      clk_word => clk_word,
      clkb => clkb,
      lvds_dat_n(0) => lvds_dat_n(0),
      lvds_dat_p(0) => lvds_dat_p(0),
      sys_rst => sys_rst
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_4 is
  port (
    lvds_dat_p : in STD_LOGIC_VECTOR ( 0 to 0 );
    lvds_dat_n : in STD_LOGIC_VECTOR ( 0 to 0 );
    bitslip_cmd : in STD_LOGIC;
    clk_bit : in STD_LOGIC;
    clkb : in STD_LOGIC;
    clk_word : in STD_LOGIC;
    sys_rst : in STD_LOGIC
  );
  attribute ORIG_REF_NAME : string;
  attribute ORIG_REF_NAME of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_4 : entity is "AdcLane";
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_4;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_4 is
begin
inst_Serdes_1x14_DDR_Data_Line: entity work.decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_9
     port map (
      bitslip_cmd => bitslip_cmd,
      clk_bit => clk_bit,
      clk_word => clk_word,
      clkb => clkb,
      lvds_dat_n(0) => lvds_dat_n(0),
      lvds_dat_p(0) => lvds_dat_p(0),
      sys_rst => sys_rst
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_5 is
  port (
    m_axis_tdata : out STD_LOGIC_VECTOR ( 13 downto 0 );
    lvds_dat_p : in STD_LOGIC_VECTOR ( 0 to 0 );
    lvds_dat_n : in STD_LOGIC_VECTOR ( 0 to 0 );
    bitslip_cmd : in STD_LOGIC;
    clk_bit : in STD_LOGIC;
    clkb : in STD_LOGIC;
    clk_word : in STD_LOGIC;
    sys_rst : in STD_LOGIC
  );
  attribute ORIG_REF_NAME : string;
  attribute ORIG_REF_NAME of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_5 : entity is "AdcLane";
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_5;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_5 is
  signal ADCDataLine : STD_LOGIC_VECTOR ( 13 downto 0 );
begin
\ADCData_reg[0]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(0),
      Q => m_axis_tdata(0),
      R => '0'
    );
\ADCData_reg[10]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(10),
      Q => m_axis_tdata(10),
      R => '0'
    );
\ADCData_reg[11]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(11),
      Q => m_axis_tdata(11),
      R => '0'
    );
\ADCData_reg[12]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(12),
      Q => m_axis_tdata(12),
      R => '0'
    );
\ADCData_reg[13]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(13),
      Q => m_axis_tdata(13),
      R => '0'
    );
\ADCData_reg[1]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(1),
      Q => m_axis_tdata(1),
      R => '0'
    );
\ADCData_reg[2]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(2),
      Q => m_axis_tdata(2),
      R => '0'
    );
\ADCData_reg[3]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(3),
      Q => m_axis_tdata(3),
      R => '0'
    );
\ADCData_reg[4]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(4),
      Q => m_axis_tdata(4),
      R => '0'
    );
\ADCData_reg[5]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(5),
      Q => m_axis_tdata(5),
      R => '0'
    );
\ADCData_reg[6]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(6),
      Q => m_axis_tdata(6),
      R => '0'
    );
\ADCData_reg[7]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(7),
      Q => m_axis_tdata(7),
      R => '0'
    );
\ADCData_reg[8]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(8),
      Q => m_axis_tdata(8),
      R => '0'
    );
\ADCData_reg[9]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(9),
      Q => m_axis_tdata(9),
      R => '0'
    );
inst_Serdes_1x14_DDR_Data_Line: entity work.decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_8
     port map (
      D(13 downto 0) => ADCDataLine(13 downto 0),
      bitslip_cmd => bitslip_cmd,
      clk_bit => clk_bit,
      clk_word => clk_word,
      clkb => clkb,
      lvds_dat_n(0) => lvds_dat_n(0),
      lvds_dat_p(0) => lvds_dat_p(0),
      sys_rst => sys_rst
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_6 is
  port (
    m_axis_tdata : out STD_LOGIC_VECTOR ( 13 downto 0 );
    lvds_dat_p : in STD_LOGIC_VECTOR ( 0 to 0 );
    lvds_dat_n : in STD_LOGIC_VECTOR ( 0 to 0 );
    bitslip_cmd : in STD_LOGIC;
    clk_bit : in STD_LOGIC;
    clkb : in STD_LOGIC;
    clk_word : in STD_LOGIC;
    sys_rst : in STD_LOGIC
  );
  attribute ORIG_REF_NAME : string;
  attribute ORIG_REF_NAME of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_6 : entity is "AdcLane";
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_6;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_6 is
  signal ADCDataLine : STD_LOGIC_VECTOR ( 13 downto 0 );
begin
\ADCData_reg[0]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(0),
      Q => m_axis_tdata(0),
      R => '0'
    );
\ADCData_reg[10]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(10),
      Q => m_axis_tdata(10),
      R => '0'
    );
\ADCData_reg[11]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(11),
      Q => m_axis_tdata(11),
      R => '0'
    );
\ADCData_reg[12]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(12),
      Q => m_axis_tdata(12),
      R => '0'
    );
\ADCData_reg[13]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(13),
      Q => m_axis_tdata(13),
      R => '0'
    );
\ADCData_reg[1]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(1),
      Q => m_axis_tdata(1),
      R => '0'
    );
\ADCData_reg[2]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(2),
      Q => m_axis_tdata(2),
      R => '0'
    );
\ADCData_reg[3]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(3),
      Q => m_axis_tdata(3),
      R => '0'
    );
\ADCData_reg[4]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(4),
      Q => m_axis_tdata(4),
      R => '0'
    );
\ADCData_reg[5]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(5),
      Q => m_axis_tdata(5),
      R => '0'
    );
\ADCData_reg[6]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(6),
      Q => m_axis_tdata(6),
      R => '0'
    );
\ADCData_reg[7]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(7),
      Q => m_axis_tdata(7),
      R => '0'
    );
\ADCData_reg[8]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(8),
      Q => m_axis_tdata(8),
      R => '0'
    );
\ADCData_reg[9]\: unisim.vcomponents.FDRE
     port map (
      C => clk_word,
      CE => '1',
      D => ADCDataLine(9),
      Q => m_axis_tdata(9),
      R => '0'
    );
inst_Serdes_1x14_DDR_Data_Line: entity work.decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_Serdes_1x14_DDR_7
     port map (
      D(13 downto 0) => ADCDataLine(13 downto 0),
      bitslip_cmd => bitslip_cmd,
      clk_bit => clk_bit,
      clk_word => clk_word,
      clkb => clkb,
      lvds_dat_n(0) => lvds_dat_n(0),
      lvds_dat_p(0) => lvds_dat_p(0),
      sys_rst => sys_rst
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLVDS_Top is
  port (
    lvds_dco_p : in STD_LOGIC;
    lvds_dco_n : in STD_LOGIC;
    lvds_fco_p : in STD_LOGIC;
    lvds_fco_n : in STD_LOGIC;
    lvds_dat_p : in STD_LOGIC_VECTOR ( 7 downto 0 );
    lvds_dat_n : in STD_LOGIC_VECTOR ( 7 downto 0 );
    sys_rst : in STD_LOGIC;
    m_axis_aclk : in STD_LOGIC;
    m_axis_tvalid : out STD_LOGIC;
    m_axis_tdata : out STD_LOGIC_VECTOR ( 127 downto 0 );
    m_axis_tlast : out STD_LOGIC
  );
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLVDS_Top;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLVDS_Top is
  signal \<const0>\ : STD_LOGIC;
  signal bitslip_cmd : STD_LOGIC;
  signal clk_f : STD_LOGIC;
  signal clk_s : STD_LOGIC;
  signal \^m_axis_tdata\ : STD_LOGIC_VECTOR ( 109 downto 0 );
begin
  m_axis_tdata(127) <= \^m_axis_tdata\(93);
  m_axis_tdata(126) <= \^m_axis_tdata\(93);
  m_axis_tdata(125 downto 112) <= \^m_axis_tdata\(93 downto 80);
  m_axis_tdata(111) <= \^m_axis_tdata\(109);
  m_axis_tdata(110) <= \^m_axis_tdata\(109);
  m_axis_tdata(109 downto 96) <= \^m_axis_tdata\(109 downto 96);
  m_axis_tdata(95) <= \^m_axis_tdata\(93);
  m_axis_tdata(94) <= \^m_axis_tdata\(93);
  m_axis_tdata(93 downto 80) <= \^m_axis_tdata\(93 downto 80);
  m_axis_tdata(79) <= \^m_axis_tdata\(77);
  m_axis_tdata(78) <= \^m_axis_tdata\(77);
  m_axis_tdata(77 downto 64) <= \^m_axis_tdata\(77 downto 64);
  m_axis_tdata(63) <= \^m_axis_tdata\(61);
  m_axis_tdata(62) <= \^m_axis_tdata\(61);
  m_axis_tdata(61 downto 48) <= \^m_axis_tdata\(61 downto 48);
  m_axis_tdata(47) <= \^m_axis_tdata\(45);
  m_axis_tdata(46) <= \^m_axis_tdata\(45);
  m_axis_tdata(45 downto 32) <= \^m_axis_tdata\(45 downto 32);
  m_axis_tdata(31) <= \^m_axis_tdata\(29);
  m_axis_tdata(30) <= \^m_axis_tdata\(29);
  m_axis_tdata(29 downto 16) <= \^m_axis_tdata\(29 downto 16);
  m_axis_tdata(15) <= \^m_axis_tdata\(13);
  m_axis_tdata(14) <= \^m_axis_tdata\(13);
  m_axis_tdata(13 downto 0) <= \^m_axis_tdata\(13 downto 0);
  m_axis_tlast <= \<const0>\;
GND: unisim.vcomponents.GND
     port map (
      G => \<const0>\
    );
\lane_gen[0].u_lane\: entity work.decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane
     port map (
      bitslip_cmd => bitslip_cmd,
      clk_bit => clk_f,
      clk_word => clk_s,
      clkb => clk_f,
      lvds_dat_n(0) => lvds_dat_n(0),
      lvds_dat_p(0) => lvds_dat_p(0),
      m_axis_tdata(13 downto 0) => \^m_axis_tdata\(13 downto 0),
      sys_rst => sys_rst
    );
\lane_gen[1].u_lane\: entity work.decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_0
     port map (
      bitslip_cmd => bitslip_cmd,
      clk_bit => clk_f,
      clk_word => clk_s,
      clkb => clk_f,
      lvds_dat_n(0) => lvds_dat_n(1),
      lvds_dat_p(0) => lvds_dat_p(1),
      m_axis_tdata(13 downto 0) => \^m_axis_tdata\(29 downto 16),
      sys_rst => sys_rst
    );
\lane_gen[2].u_lane\: entity work.decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_1
     port map (
      bitslip_cmd => bitslip_cmd,
      clk_bit => clk_f,
      clk_word => clk_s,
      clkb => clk_f,
      lvds_dat_n(0) => lvds_dat_n(2),
      lvds_dat_p(0) => lvds_dat_p(2),
      m_axis_tdata(13 downto 0) => \^m_axis_tdata\(45 downto 32),
      sys_rst => sys_rst
    );
\lane_gen[3].u_lane\: entity work.decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_2
     port map (
      bitslip_cmd => bitslip_cmd,
      clk_bit => clk_f,
      clk_word => clk_s,
      clkb => clk_f,
      lvds_dat_n(0) => lvds_dat_n(3),
      lvds_dat_p(0) => lvds_dat_p(3),
      m_axis_tdata(13 downto 0) => \^m_axis_tdata\(61 downto 48),
      sys_rst => sys_rst
    );
\lane_gen[4].u_lane\: entity work.decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_3
     port map (
      bitslip_cmd => bitslip_cmd,
      clk_bit => clk_f,
      clk_word => clk_s,
      clkb => clk_f,
      lvds_dat_n(0) => lvds_dat_n(4),
      lvds_dat_p(0) => lvds_dat_p(4),
      m_axis_tdata(13 downto 0) => \^m_axis_tdata\(77 downto 64),
      sys_rst => sys_rst
    );
\lane_gen[5].u_lane\: entity work.decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_4
     port map (
      bitslip_cmd => bitslip_cmd,
      clk_bit => clk_f,
      clk_word => clk_s,
      clkb => clk_f,
      lvds_dat_n(0) => lvds_dat_n(5),
      lvds_dat_p(0) => lvds_dat_p(5),
      sys_rst => sys_rst
    );
\lane_gen[6].u_lane\: entity work.decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_5
     port map (
      bitslip_cmd => bitslip_cmd,
      clk_bit => clk_f,
      clk_word => clk_s,
      clkb => clk_f,
      lvds_dat_n(0) => lvds_dat_n(6),
      lvds_dat_p(0) => lvds_dat_p(6),
      m_axis_tdata(13 downto 0) => \^m_axis_tdata\(109 downto 96),
      sys_rst => sys_rst
    );
\lane_gen[7].u_lane\: entity work.decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLane_6
     port map (
      bitslip_cmd => bitslip_cmd,
      clk_bit => clk_f,
      clk_word => clk_s,
      clkb => clk_f,
      lvds_dat_n(0) => lvds_dat_n(7),
      lvds_dat_p(0) => lvds_dat_p(7),
      m_axis_tdata(13 downto 0) => \^m_axis_tdata\(93 downto 80),
      sys_rst => sys_rst
    );
u_clock: entity work.decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcClock
     port map (
      clk_bit => clk_f,
      clk_word => clk_s,
      lvds_dco_n => lvds_dco_n,
      lvds_dco_p => lvds_dco_p
    );
u_frame: entity work.decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcFrame
     port map (
      bitslip_cmd => bitslip_cmd,
      clk_bit => clk_f,
      clk_word => clk_s,
      clkb => clk_f,
      lvds_fco_n => lvds_fco_n,
      lvds_fco_p => lvds_fco_p,
      m_axis_tvalid => m_axis_tvalid,
      sys_rst => sys_rst
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix is
  port (
    lvds_dco_p : in STD_LOGIC;
    lvds_dco_n : in STD_LOGIC;
    lvds_fco_p : in STD_LOGIC;
    lvds_fco_n : in STD_LOGIC;
    lvds_dat_p : in STD_LOGIC_VECTOR ( 7 downto 0 );
    lvds_dat_n : in STD_LOGIC_VECTOR ( 7 downto 0 );
    sys_rst : in STD_LOGIC;
    m_axis_aclk : in STD_LOGIC;
    m_axis_tvalid : out STD_LOGIC;
    m_axis_tdata : out STD_LOGIC_VECTOR ( 127 downto 0 );
    m_axis_tlast : out STD_LOGIC
  );
  attribute NotValidForBitStream : boolean;
  attribute NotValidForBitStream of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix : entity is true;
  attribute CHECK_LICENSE_TYPE : string;
  attribute CHECK_LICENSE_TYPE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix : entity is "design_1_AdcLVDS_Top_0_0,AdcLVDS_Top,{}";
  attribute DowngradeIPIdentifiedWarnings : string;
  attribute DowngradeIPIdentifiedWarnings of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix : entity is "yes";
  attribute IP_DEFINITION_SOURCE : string;
  attribute IP_DEFINITION_SOURCE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix : entity is "package_project";
  attribute X_CORE_INFO : string;
  attribute X_CORE_INFO of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix : entity is "AdcLVDS_Top,Vivado 2020.2";
end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix;

architecture STRUCTURE of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix is
  signal \<const0>\ : STD_LOGIC;
  signal NLW_inst_m_axis_tlast_UNCONNECTED : STD_LOGIC;
  attribute X_INTERFACE_INFO : string;
  attribute X_INTERFACE_INFO of lvds_dco_n : signal is "xilinx.com:interface:diff_clock:1.0 lvds_dco CLK_N";
  attribute X_INTERFACE_PARAMETER : string;
  attribute X_INTERFACE_PARAMETER of lvds_dco_n : signal is "XIL_INTERFACENAME lvds_dco, CAN_DEBUG false, FREQ_HZ 100000000";
  attribute X_INTERFACE_INFO of lvds_dco_p : signal is "xilinx.com:interface:diff_clock:1.0 lvds_dco CLK_P";
  attribute X_INTERFACE_INFO of lvds_fco_n : signal is "xilinx.com:interface:diff_clock:1.0 lvds_fco CLK_N";
  attribute X_INTERFACE_PARAMETER of lvds_fco_n : signal is "XIL_INTERFACENAME lvds_fco, CAN_DEBUG false, FREQ_HZ 100000000";
  attribute X_INTERFACE_INFO of lvds_fco_p : signal is "xilinx.com:interface:diff_clock:1.0 lvds_fco CLK_P";
  attribute X_INTERFACE_INFO of m_axis_aclk : signal is "xilinx.com:signal:clock:1.0 m_axis_aclk CLK";
  attribute X_INTERFACE_PARAMETER of m_axis_aclk : signal is "XIL_INTERFACENAME m_axis_aclk, ASSOCIATED_BUSIF m_axis, FREQ_HZ 100000000, FREQ_TOLERANCE_HZ 0, PHASE 0.000, CLK_DOMAIN design_1_processing_system7_0_0_FCLK_CLK0, INSERT_VIP 0";
  attribute X_INTERFACE_INFO of m_axis_tlast : signal is "xilinx.com:interface:axis:1.0 m_axis TLAST";
  attribute X_INTERFACE_PARAMETER of m_axis_tlast : signal is "XIL_INTERFACENAME m_axis, TDATA_NUM_BYTES 16, TDEST_WIDTH 0, TID_WIDTH 0, TUSER_WIDTH 0, HAS_TREADY 0, HAS_TSTRB 0, HAS_TKEEP 0, HAS_TLAST 1, FREQ_HZ 100000000, PHASE 0.000, CLK_DOMAIN design_1_processing_system7_0_0_FCLK_CLK0, LAYERED_METADATA undef, INSERT_VIP 0";
  attribute X_INTERFACE_INFO of m_axis_tvalid : signal is "xilinx.com:interface:axis:1.0 m_axis TVALID";
  attribute X_INTERFACE_INFO of sys_rst : signal is "xilinx.com:signal:reset:1.0 sys_rst RST";
  attribute X_INTERFACE_PARAMETER of sys_rst : signal is "XIL_INTERFACENAME sys_rst, POLARITY ACTIVE_HIGH, INSERT_VIP 0";
  attribute X_INTERFACE_INFO of m_axis_tdata : signal is "xilinx.com:interface:axis:1.0 m_axis TDATA";
begin
  m_axis_tlast <= \<const0>\;
GND: unisim.vcomponents.GND
     port map (
      G => \<const0>\
    );
inst: entity work.decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_AdcLVDS_Top
     port map (
      lvds_dat_n(7 downto 0) => lvds_dat_n(7 downto 0),
      lvds_dat_p(7 downto 0) => lvds_dat_p(7 downto 0),
      lvds_dco_n => lvds_dco_n,
      lvds_dco_p => lvds_dco_p,
      lvds_fco_n => lvds_fco_n,
      lvds_fco_p => lvds_fco_p,
      m_axis_aclk => '0',
      m_axis_tdata(127 downto 0) => m_axis_tdata(127 downto 0),
      m_axis_tlast => NLW_inst_m_axis_tlast_UNCONNECTED,
      m_axis_tvalid => m_axis_tvalid,
      sys_rst => sys_rst
    );
end STRUCTURE;
