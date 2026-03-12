# README  

## 配置及移植  

**PL端配置**  
PL端配置参考zynq linux教程实验十
具体配置如下：  

**芯片型号** XC7Z020-CLG484-2
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
### PS端
**vitis运行配置**：  
1. 连接 JTAG 下载器，给开发板上电。
2. 先右键**platform**和**system**分别**build**，然后再设置**Run configuration**
3. 在弹出窗口左侧，双击 **`Single Application Debug`**，系统会自动新建一个运行配置。
4. ⚠️ **【极其重要的配置】** 切换到 **`Target Setup`** 选项卡，**必须且确认**勾选以下三项（默认是都勾选的）：
   - [x] **Reset entire system**：防止上一次运行的残余状态干扰。
   - [x] **Program FPGA**：将 Vivado 生成的 `.bit` 文件下载到 PL 端（不勾选的话屏幕不会亮）。
   - [x] **Run psu_init / ps7_init**：**最关键的一步！** 负责初始化 Zynq 的时钟系统和 DDR，如果不勾选，程序加载进 DDR 时会直接卡死，串口也毫无输出。
5. 点击右下角的 **`Run`** 即可。以后修改了代码，直接点工具栏的绿色运行按钮就会默认使用这个配置。  

**Vitis 包含路径配置 (Include Paths)（极其重要⚠️）**
由于本项目采用了高度解耦的分层架构，直接使用 `#include "xxx.h"` 需要 IDE 的路径索引支持。当你导入本工程或新增了包含头文件的文件夹时，必须进行以下检查或配置：

右键点击 **Application** 工程（注意是带有 C 图标的 app 工程，不是 system 工程），选择 Properties。  

依次展开 **C/C++ General -> Paths and Symbols**。  

切换到 Includes 选项卡，语言选择 **GNU C**。  

点击右侧的 **Add...**，务必勾选 **Is a workspace path**，然后点击 Workspace... 按钮。  



将以下存放了 .h 文件的目录依次添加进去：
```text
/app/src/app
/app/src/gui
/app/src/bsp
/app/src/hal
/app/src/middleware/lvgl
...
```
点击 **Apply and Close**，允许 Vitis 重建索引即可消除所有的头文件报错。  



**PS端导出导入格式**  
vitis做完之后，在左上角file->export导出为zip文件供其他人导入，另外还需要准备.xsa文件  

拿到zip文件后，在vitis的file->import->第一个选项，然后在platform右键update hardware specification，选择.xsa文件  

## 文件结构及功能  
**文件结构**  
本项目在 Zynq 平台上严格遵守高内聚、低耦合的设计规范：  
```text
/zynq7020
├─archive                   //存放vitis导出的zip
├─hw                        //存放vivado导出的.xsa文件
├─lvgl                      //存放lvgl模拟器和移植的内容
│  └─simulator              //lvgl模拟器
├─vitis_workspace           //vitis workspace
├─vivado                    //vivado工程
│   └─lcd
└─doc                       //文档  

zynq内部的文件结构
src/
├── app/                      // 控制层 (Controller)
│   ├── app_main.c/.h         // 系统装配店长：封装底层硬件、中断与网络的初始化细节
│   └── app_monitor.c/.h      // 业务大管家：负责 ADC 数据的清洗、网络发送及 UI 状态联动
├── gui/                      // 视图层 (View)
│   └── ui_main.c/.h          // 纯 UI 绘制层：仅暴露数据刷新接口，没有硬件和业务概念
├── middleware/               // 中间件 (Middleware)
│   ├── lvgl/                 // LVGL 官方图形库源码
│   └── lv_conf.h             // LVGL 配置文件 (开启了 16bit 色深和 SPI 大端翻转)
├── bsp/                      // 板级支持包 (Board Support Package)
│   ├── lcd_drv.c/.h          // 针对当前 ST7789 屏幕的专属 SPI 驱动
│   ├── sys_timer.c/.h        // SCUTIMER 定时器封装 (为 LVGL 提供心跳)
│   └── platform/             // Xilinx 自动生成的 Cache 和基本环境配置
├── hal/                      // 硬件抽象层 (Hardware Abstraction Layer)
│   ├── adc_dma.c/.h          // Zynq 内部 XADC 采样逻辑及 AXI DMA 传输控制
│   ├── zynq_int.c/.h         // SCUGIC 中断控制器封装
│   └── dma_bd/               // DMA 散列采集模式 (SG) 描述符链表管理
└── main.c                    // 极简主入口：仅保留 Init 调用和 while(1) **核心调度循环**
```

**核心文件功能说明**：  

- **`lcd_drv.c / .h`**
  - **功能**: LCD 硬件驱动层。
  - **描述**: 负责配置 Zynq 的 SPI 和 GPIO 控制器，执行 ST7789 屏幕的初始化序列（指令与 Gamma 校准），并提供 LVGL 所需的 `disp_flush` 高速 SPI 批量传输回调接口。
- **`sys_timer.c / .h`**
  - **功能**: 系统时基层（心跳引擎）。
  - **描述**: 负责配置 Zynq 内部私有定时器 (SCUTIMER) 及中断控制器 (SCUGIC)，产生 1ms 的周期性硬件中断，调用 `lv_tick_inc()` 为 LVGL 提供时间基准。
- **`lv_conf.h`**
  - **功能**: LVGL 全局配置文件。
  - **描述**: 配置了 `LV_COLOR_DEPTH 16` (RGB565) 以及 `LV_COLOR_16_SWAP 1` (适配 SPI 屏的大端模式)，并开启了 FPS 性能监视器。  











