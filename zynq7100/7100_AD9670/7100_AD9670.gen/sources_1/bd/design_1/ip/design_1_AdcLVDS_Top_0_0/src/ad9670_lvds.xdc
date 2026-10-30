# ============================================================================
# 1. 时钟定义 (Timing Constraints)
# 假设 AD9670 DCO 为 80MHz，周期 12.5ns
# ============================================================================
create_clock -period 12.500 -name lvds_dco_clk [get_ports lvds_dco_p]

# ============================================================================
# 2. IO 电平标准 (IO Standards)
# 针对 Bank 12 (HR Bank)，必须使用 LVDS_25 关键字
# 即使物理电平是 1.8V，在 7 系列 HR Bank 的约束中也必须写为 LVDS_25
# ============================================================================

# 数据时钟 DCO
set_property IOSTANDARD LVDS_25 [get_ports lvds_dco_p]
set_property IOSTANDARD LVDS_25 [get_ports lvds_dco_n]
set_property DIFF_TERM TRUE [get_ports lvds_dco_p]

# 帧时钟 FCO
set_property IOSTANDARD LVDS_25 [get_ports lvds_fco_p]
set_property IOSTANDARD LVDS_25 [get_ports lvds_fco_n]
set_property DIFF_TERM TRUE [get_ports lvds_fco_p]

# 8 路数据通道
set_property IOSTANDARD LVDS_25 [get_ports {lvds_dat_p[*]}]
set_property IOSTANDARD LVDS_25 [get_ports {lvds_dat_n[*]}]
set_property DIFF_TERM TRUE [get_ports {lvds_dat_p[*]}]

# ============================================================================
# 3. 物理引脚分配 (Package Pins)
# 严格对应你提供的 FMC LA 映射表
# ============================================================================

# DCO -> LA00 (Clock Capable)
set_property PACKAGE_PIN U26 [get_ports lvds_dco_p]

# FCO -> LA01 (Clock Capable)
set_property PACKAGE_PIN AB27 [get_ports lvds_fco_p]

# Data Lanes -> LA02 to LA09
set_property PACKAGE_PIN AK27 [get_ports {lvds_dat_p[0]}]
set_property PACKAGE_PIN AJ28 [get_ports {lvds_dat_p[1]}]
set_property PACKAGE_PIN AH26 [get_ports {lvds_dat_p[2]}]
set_property PACKAGE_PIN AG26 [get_ports {lvds_dat_p[3]}]
set_property PACKAGE_PIN AJ30 [get_ports {lvds_dat_p[4]}]
set_property PACKAGE_PIN AD30 [get_ports {lvds_dat_p[5]}]
set_property PACKAGE_PIN AF30 [get_ports {lvds_dat_p[6]}]
set_property PACKAGE_PIN AF29 [get_ports {lvds_dat_p[7]}]

# ============================================================================
# 4. 延迟控制组 (IDELAY Group)
# ============================================================================
# set_property IODELAY_GROUP AD9670_DELAY_GRP [get_cells -hier *idelay_inst*]

# ============================================================================
# 5. 其他电压约束 (针对部分板卡的电压警告排除)
# ============================================================================
set_property BITSTREAM.CONFIG.OVERTEMPPOWERDOWN ENABLE [current_design]