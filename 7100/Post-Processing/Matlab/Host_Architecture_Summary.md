# Open Image 上位机 MATLAB 工程架构文档

## 1. 目录结构图

```
Post-Processing/Matlab/
├── openimage/                    # 核心类库（面向对象架构）
│   ├── openimage.m              # 主通信类 - TCP 连接与指令收发
│   ├── oi_frame.m              # 帧结构定义 - 包含多个 Shot
│   ├── oi_shot.m               # 单次发射-接收过程配置
│   ├── oi_tx.m                # 发射参数配置
│   ├── oi_rx.m                # 接收参数配置（采样、TGC、增益、滤波）
│   ├── oi_tx_channel.m        # 发射通道级配置
│   └── oi_rx_channel.m        # 接收通道级配置
│
└── scripts/                    # 应用脚本（执行入口与算法实现）
    ├── oi_example_script.m     # 典型示例脚本（新手入门）
    ├── oi_troubleshooting_script.m  # 故障排查脚本
    ├── delayAndSum.m          # 波束成形算法（延迟求和）
    ├── alignOiData.m          # 信号对齐算法
    ├── alignAndAverage.m       # 对齐平均算法
    ├── stitch_phantom_image.m  # 拼接 phantom 图像
    ├── anaToneTest.m          # 模拟音调测试
    ├── deadbeef.m             # 测试数据标记
    └── water_reflection.mat   # 测试数据文件
```

### 目录职责说明

| 目录 | 职责 |
|------|------|
| `openimage/` | 核心类库，封装与下位机的通信协议和配置结构 |
| `scripts/` | 应用层脚本，提供数据采集、信号处理和图像重建的完整流程 |

---

## 2. 类关系拓扑

### 2.1 类层次结构

```
openimage (handle 类)
    │
    ├── 属性: N_CHAN = 16 (常量)
    ├── 方法: open(), close(), queue_frame(), get_frame(), get_status()
    │
    │
    └── oi_frame (handle 类)
            │
            ├── 属性: nShots, shots[]
            │
            └── oi_shot
                    │
                    ├── 属性: tx, rx
                    │
                    ├── oi_tx
                    │       │
                    │       └── oi_tx_channel[] (16个通道)
                    │               - enable
                    │               - levelSequence
                    │
                    └── oi_rx
                            │
                            ├── 常量: fs=80MHz, TGC_FS=1MHz, N_CHAN_PER_CHIP=8, N_CHIPS=2
                            ├── 属性: N, tgc, testMode, lna, pga, lpf, hpf_divisor
                            │
                            └── oi_rx_channel[] (16个通道)
                                    - enable
```

### 2.2 类关系详解

| 类名 | 类型 | 父类/包含关系 | 核心职责 |
|------|------|---------------|----------|
| `openimage` | handle 类 | 无 | 与下位机建立 TCP 连接，发送命令和接收数据 |
| `oi_frame` | handle 类 | 被 openimage 调用 | 代表一帧图像的配置容器，包含多个 Shot |
| `oi_shot` | 普通类 | 被 oi_frame 包含 | 单次发射-接收过程，包含发射和接收配置 |
| `oi_tx` | handle 类 | 被 oi_shot 包含 | 发射参数配置，管理 16 个发射通道 |
| `oi_rx` | handle 类 | 被 oi_shot 包含 | 接收参数配置（采样点数、TGC、增益、滤波） |
| `oi_tx_channel` | 普通类 | 被 oi_tx 包含 | 单通道发射波形和使能控制 |
| `oi_rx_channel` | handle 类 | 被 oi_rx 包含 | 单通道接收使能控制 |

### 2.3 数据组合关系

```
Frame (帧)
    │
    └── shots[] (Shot 数组，长度由 nShots 指定)
            │
            ├── tx (oi_tx)
            │       │
            │       └── channels[] (16 个 oi_tx_channel)
            │               ├── enable: true/false
            │               └── levelSequence: 波形序列
            │
            └── rx (oi_rx)
                    │
                    ├── N: 采样点数
                    ├── tgc: TGC 衰减曲线 (2×N double)
                    ├── testMode: 测试模式
                    ├── lna: 低噪声放大器增益 [15.6/17.9/21.6] dB
                    ├── pga: 可编程增益放大器 [21/24/27/30] dB
                    ├── lpf: 低通滤波频率 8-30 MHz
                    ├── hpf_divisor: 高通滤波截止频率除数
                    │
                    └── channels[] (16 个 oi_rx_channel)
                            └── enable: true/false
```

---

## 3. 模块功能表

### 3.1 openimage/ 核心类库

| 文件名 | 类名 | 功能模块 | 关键方法/属性 |
|--------|------|----------|---------------|
| `openimage.m` | `openimage` | 网络通信层 | `open()` - 建立 TCP 连接 (IP: 10.11.34.215, Port: 26000)<br>`close()` - 关闭连接<br>`queue_frame()` - 发送帧配置<br>`get_frame()` - 接收原始数据<br>`get_status()` - 查询设备状态 |
| `oi_frame.m` | `oi_frame` | 帧配置容器 | `nShots` - Shot 数量<br>`shots[]` - Shot 对象数组<br>`resetHandle()` - 生成帧句柄<br>`fill()` - 序列化配置到字节缓冲 |
| `oi_shot.m` | `oi_shot` | 单次发射-接收 | 包含 `tx` 和 `rx` 两个配置对象<br>`fill()` - 委托给 tx.fill() 和 rx.fill() |
| `oi_tx.m` | `oi_tx` | 发射参数 | `CLOCK_RATE = 220e6` (常量)<br>`channels[]` - 16 个发射通道<br>`fill()` - 序列化发射配置 |
| `oi_rx.m` | `oi_rx` | 接收参数 | `fs = 80` MHz (采样率)<br>`TGC_FS = 1` MHz (TGC 更新率)<br>`N_CHAN_PER_CHIP = 8`<br>`N_CHIPS = 2`<br>`N` - 采样点数<br>`tgc` - TGC 曲线<br>`lna/pga/lpf/hpf` - 增益滤波设置 |
| `oi_tx_channel.m` | `oi_tx_channel` | 发射通道 | `enable` - 通道使能<br>`levelSequence` - 脉冲序列波形 |
| `oi_rx_channel.m` | `oi_rx_channel` | 接收通道 | `enable` - 通道使能 |

### 3.2 scripts/ 应用脚本

| 文件名 | 函数签名 | 功能模块 |
|--------|----------|----------|
| `oi_example_script.m` | 脚本文件 | **典型执行入口**：连接设备 → 配置帧 → 采集数据 → 信号处理 → 图像显示 |
| `oi_troubleshooting_script.m` | 脚本文件 | 故障排查和诊断脚本 |
| `delayAndSum.m` | `lineOut = delayAndSum(dataIn, channel, c, fs_Hz, pitch_mm)` | **波束成形算法**：延迟求和 (Delay-and-Sum) |
| `alignOiData.m` | `alignedSignalsOut = alignOiData(lineIn)` | 信号对齐算法 |
| `alignAndAverage.m` | `aveDataOut = alignAndAverage(frameData)` | 对齐后平均算法 |
| `stitch_phantom_image.m` | 脚本文件 | 拼接多个 phantom 图像 |
| `anaToneTest.m` | 脚本文件 | 模拟音调测试分析 |
| `deadbeef.m` | 脚本文件 | 测试数据标记（0xDEADBEEF） |

---

## 4. 通信协议概述

### 4.1 连接参数

| 参数 | 值 |
|------|-----|
| 协议 | TCP (Java SocketChannel) |
| IP 地址 | `10.11.34.215` (硬编码) |
| 端口 | `26000` |
| 字节序 | Little Endian |

### 4.2 指令码

| 指令 | 命令码 | 功能 |
|------|--------|------|
| `CMD_STATUS` | 0x01 | 查询设备状态 |
| `CMD_QUEUE_FRAME` | 0x11 | 发送帧配置 |
| `CMD_GET_FRAME` | 0x12 | 获取帧数据 |

### 4.3 响应码

| 响应 | 响应码 | 功能 |
|------|--------|------|
| `RES_ACK` | 0x80 | 命令确认 |
| `RES_STATUS` | 0x81 | 状态响应 |
| `RES_FRAME` | 0x92 | 帧数据响应 |
| `RES_NACK` | 0xFF | 命令拒绝/错误 |

---

## 5. 执行流程简述

```
oi_example_script.m 执行流程：

1. 连接阶段
   oi = openimage
   oi.open()           → 建立 TCP 连接
   oi.get_status()     → 确认设备就绪

2. 配置阶段
   fr = oi_frame       → 创建帧对象
   fr.nShots = 4       → 设置 4 个 Shot

   配置发射波形:
   wave = sin(2π·f·t)  → 生成正弦波形
   配置到所有通道

   配置接收参数:
   rx.N = ...          → 采样点数
   rx.tgc = {...}      → TGC 曲线
   rx.lna/pga/lpf      → 增益滤波

3. 采集阶段
   oi.queue_frame(fr)  → 发送配置到下位机
   pause(1)           → 等待采集完成
   fd = oi.get_frame() → 接收原始数据 (int16, interleaved)

4. 处理阶段
   reshape(fd, 16, [], nShots)  → 重排为 [16通道 × 采样点 × Shot]
   mean(fd2, 3)                  → 多帧平均提高 SNR
   highpass filter               → 去除直流分量
   hilbert()                     → 包络提取
   delayAndSum()                 → 波束成形

5. 显示阶段
   imagesc()                     → B 超图像显示
   colormap(gray)               → 灰度映射
```

---

## 6. 文件清单

```
Post-Processing/Matlab/
├── openimage/
│   ├── openimage.m
│   ├── oi_frame.m
│   ├── oi_shot.m
│   ├── oi_tx.m
│   ├── oi_rx.m
│   ├── oi_tx_channel.m
│   └── oi_rx_channel.m
│
└── scripts/
    ├── oi_example_script.m
    ├── oi_troubleshooting_script.m
    ├── delayAndSum.m
    ├── alignOiData.m
    ├── alignAndAverage.m
    ├── stitch_phantom_image.m
    ├── anaToneTest.m
    ├── deadbeef.m
    └── water_reflection.mat
```

---

*文档生成日期: 2026-04-18*
*项目: Open Image 超声系统上位机分析*
