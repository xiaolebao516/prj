-- Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
-- --------------------------------------------------------------------------------
-- Tool Version: Vivado v.2020.2 (win64) Build 3064766 Wed Nov 18 09:12:45 MST 2020
-- Date        : Sun Apr 19 15:52:56 2026
-- Host        : DESKTOP-MM8H4FT running 64-bit major release  (build 9200)
-- Command     : write_vhdl -force -mode funcsim
--               c:/Study/7100_AD9670/7100_AD9670.gen/sources_1/bd/design_1/ip/design_1_idelay_ctrl_wrapper_0_0/design_1_idelay_ctrl_wrapper_0_0_sim_netlist.vhdl
-- Design      : design_1_idelay_ctrl_wrapper_0_0
-- Purpose     : This VHDL netlist is a functional simulation representation of the design and should not be modified or
--               synthesized. This netlist cannot be used for SDF annotated simulation.
-- Device      : xc7z100ffg900-2
-- --------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity design_1_idelay_ctrl_wrapper_0_0_idelay_ctrl_wrapper is
  port (
    ready : out STD_LOGIC;
    ref_clk : in STD_LOGIC;
    rst : in STD_LOGIC
  );
  attribute ORIG_REF_NAME : string;
  attribute ORIG_REF_NAME of design_1_idelay_ctrl_wrapper_0_0_idelay_ctrl_wrapper : entity is "idelay_ctrl_wrapper";
end design_1_idelay_ctrl_wrapper_0_0_idelay_ctrl_wrapper;

architecture STRUCTURE of design_1_idelay_ctrl_wrapper_0_0_idelay_ctrl_wrapper is
  attribute BOX_TYPE : string;
  attribute BOX_TYPE of IDELAYCTRL_inst : label is "PRIMITIVE";
begin
IDELAYCTRL_inst: unisim.vcomponents.IDELAYCTRL
    generic map(
      SIM_DEVICE => "7SERIES"
    )
        port map (
      RDY => ready,
      REFCLK => ref_clk,
      RST => rst
    );
end STRUCTURE;
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
library UNISIM;
use UNISIM.VCOMPONENTS.ALL;
entity design_1_idelay_ctrl_wrapper_0_0 is
  port (
    ref_clk : in STD_LOGIC;
    rst : in STD_LOGIC;
    ready : out STD_LOGIC
  );
  attribute NotValidForBitStream : boolean;
  attribute NotValidForBitStream of design_1_idelay_ctrl_wrapper_0_0 : entity is true;
  attribute CHECK_LICENSE_TYPE : string;
  attribute CHECK_LICENSE_TYPE of design_1_idelay_ctrl_wrapper_0_0 : entity is "design_1_idelay_ctrl_wrapper_0_0,idelay_ctrl_wrapper,{}";
  attribute DowngradeIPIdentifiedWarnings : string;
  attribute DowngradeIPIdentifiedWarnings of design_1_idelay_ctrl_wrapper_0_0 : entity is "yes";
  attribute IP_DEFINITION_SOURCE : string;
  attribute IP_DEFINITION_SOURCE of design_1_idelay_ctrl_wrapper_0_0 : entity is "module_ref";
  attribute X_CORE_INFO : string;
  attribute X_CORE_INFO of design_1_idelay_ctrl_wrapper_0_0 : entity is "idelay_ctrl_wrapper,Vivado 2020.2";
end design_1_idelay_ctrl_wrapper_0_0;

architecture STRUCTURE of design_1_idelay_ctrl_wrapper_0_0 is
  attribute X_INTERFACE_INFO : string;
  attribute X_INTERFACE_INFO of ref_clk : signal is "xilinx.com:signal:clock:1.0 ref_clk CLK";
  attribute X_INTERFACE_PARAMETER : string;
  attribute X_INTERFACE_PARAMETER of ref_clk : signal is "XIL_INTERFACENAME ref_clk, ASSOCIATED_RESET rst, FREQ_HZ 200000000, FREQ_TOLERANCE_HZ 0, PHASE 0.000, CLK_DOMAIN design_1_processing_system7_0_0_FCLK_CLK1, INSERT_VIP 0";
  attribute X_INTERFACE_INFO of rst : signal is "xilinx.com:signal:reset:1.0 rst RST";
  attribute X_INTERFACE_PARAMETER of rst : signal is "XIL_INTERFACENAME rst, POLARITY ACTIVE_HIGH, INSERT_VIP 0";
begin
inst: entity work.design_1_idelay_ctrl_wrapper_0_0_idelay_ctrl_wrapper
     port map (
      ready => ready,
      ref_clk => ref_clk,
      rst => rst
    );
end STRUCTURE;
