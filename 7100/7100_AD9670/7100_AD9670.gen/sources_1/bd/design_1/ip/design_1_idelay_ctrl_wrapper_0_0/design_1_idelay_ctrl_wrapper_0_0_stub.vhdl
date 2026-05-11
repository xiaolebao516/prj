-- Copyright 1986-2020 Xilinx, Inc. All Rights Reserved.
-- --------------------------------------------------------------------------------
-- Tool Version: Vivado v.2020.2 (win64) Build 3064766 Wed Nov 18 09:12:45 MST 2020
-- Date        : Sun Apr 19 15:52:56 2026
-- Host        : DESKTOP-MM8H4FT running 64-bit major release  (build 9200)
-- Command     : write_vhdl -force -mode synth_stub
--               c:/Study/7100_AD9670/7100_AD9670.gen/sources_1/bd/design_1/ip/design_1_idelay_ctrl_wrapper_0_0/design_1_idelay_ctrl_wrapper_0_0_stub.vhdl
-- Design      : design_1_idelay_ctrl_wrapper_0_0
-- Purpose     : Stub declaration of top-level module interface
-- Device      : xc7z100ffg900-2
-- --------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity design_1_idelay_ctrl_wrapper_0_0 is
  Port ( 
    ref_clk : in STD_LOGIC;
    rst : in STD_LOGIC;
    ready : out STD_LOGIC
  );

end design_1_idelay_ctrl_wrapper_0_0;

architecture stub of design_1_idelay_ctrl_wrapper_0_0 is
attribute syn_black_box : boolean;
attribute black_box_pad_pin : string;
attribute syn_black_box of stub : architecture is true;
attribute black_box_pad_pin of stub : architecture is "ref_clk,rst,ready";
attribute X_CORE_INFO : string;
attribute X_CORE_INFO of stub : architecture is "idelay_ctrl_wrapper,Vivado 2020.2";
begin
end;
