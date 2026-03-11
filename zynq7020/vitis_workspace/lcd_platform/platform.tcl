# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct C:\Users\Admin\workspace\lcd_platform\platform.tcl
# 
# OR launch xsct and run below command.
# source C:\Users\Admin\workspace\lcd_platform\platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {lcd_platform}\
-hw {C:\Users\Admin\Desktop\7020\lcd\vivado\lcd\lcd_wrapper.xsa}\
-proc {ps7_cortexa9_0} -os {standalone} -fsbl-target {psu_cortexa53_0} -out {C:/Users/Admin/workspace}

platform write
platform generate -domains 
platform active {lcd_platform}
bsp reload
domain active {zynq_fsbl}
bsp reload
platform generate
platform active {lcd_platform}
platform generate -domains 
platform generate -domains standalone_domain 
platform active {lcd_platform}
platform config -updatehw {D:/516/prj/zynq7020/vivado/lcd/lcd_wrapper.xsa}
platform active {lcd_platform}
