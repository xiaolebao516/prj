-- Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
-- --------------------------------------------------------------------------------
-- Tool Version: Vivado v.2020.2 (win64) Build 3064766 Wed Nov 18 09:12:45 MST 2020
-- Date        : Sun Apr 19 15:52:58 2026
-- Host        : DESKTOP-MM8H4FT running 64-bit major release  (build 9200)
-- Command     : write_vhdl -force -mode synth_stub -rename_top decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix -prefix
--               decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_ design_1_AdcLVDS_Top_0_0_stub.vhdl
-- Design      : design_1_AdcLVDS_Top_0_0
-- Purpose     : Stub declaration of top-level module interface
-- Device      : xc7z100ffg900-2
-- --------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix is
  Port ( 
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

end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix;

architecture stub of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix is
attribute syn_black_box : boolean;
attribute black_box_pad_pin : string;
attribute syn_black_box of stub : architecture is true;
attribute black_box_pad_pin of stub : architecture is "lvds_dco_p,lvds_dco_n,lvds_fco_p,lvds_fco_n,lvds_dat_p[7:0],lvds_dat_n[7:0],sys_rst,m_axis_aclk,m_axis_tvalid,m_axis_tdata[127:0],m_axis_tlast";
attribute X_CORE_INFO : string;
attribute X_CORE_INFO of stub : architecture is "AdcLVDS_Top,Vivado 2020.2";
begin
end;
