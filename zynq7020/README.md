# README  

**文件结构**  
```text
/zynq7020
├─archive                   //存放vitis导出的zip
├─hw                        //存放vivado导出的.xsa文件
├─lvgl                      //存放lvgl模拟器和移植的内容
│  └─simulator              //lvgl模拟器
├─vitis_workspace           //vitis workspace
└─vivado                    //vivado工程
    └─lcd
```

**导出导入格式**  
vitis做完之后，在左上角file->export导出为zip文件供其他人导入，另外还需要准备.xsa文件  

拿到zip文件后，在vitis的file->import->第一个选项，然后在platform右键update hardware specification，选择.xsa文件  

**PL端配置**  
PL端配置参考zynq linux教程实验十
具体配置如下：
芯片型号 XC7Z020-CLG484-2
vivado DDR3型号 MT41K256M16 RE-125 32bit

lcd引脚分配：
SDA U19
SCL V18 
RST AA18
DC W13
LED Y13
CS AA13
其中要删掉.v里部分的声明和iobuf（按教程操作即可）

约束：（我改了名字没用默认名字）
```verilog
set_property IOSTANDARD LVCMOS33 [get_ports {LCD_GPIO_tri_io[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {LCD_GPIO_tri_io[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {LCD_GPIO_tri_io[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports LCD_SPI_io0_io]
set_property IOSTANDARD LVCMOS33 [get_ports LCD_SPI_sck_io]
set_property IOSTANDARD LVCMOS33 [get_ports LCD_SPI_ss_io]
set_property PACKAGE_PIN Y13 [get_ports {LCD_GPIO_tri_io[2]}]
set_property PACKAGE_PIN AA18 [get_ports {LCD_GPIO_tri_io[1]}]
set_property PACKAGE_PIN W13 [get_ports {LCD_GPIO_tri_io[0]}]
set_property PACKAGE_PIN U19 [get_ports LCD_SPI_io0_io]
set_property PACKAGE_PIN V18 [get_ports LCD_SPI_sck_io]
set_property PACKAGE_PIN AA13 [get_ports LCD_SPI_ss_io]
```



