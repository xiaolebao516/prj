# 静谧仪器（Quiet Instrument）UI 方案 · 2026-10-09

视觉设计稿（4 个画板：主界面 · 检测中 / 设计语言 / 登录 / 档案）：
https://claude.ai/artifact/2uCEhjTp7ZrjxnXjbEsaYV （私有，需在页面 Share 菜单里共享后他人才能打开）

## 1. 参考与结论

| 来源 | 借鉴点 |
| --- | --- |
| BeamMed Sunlight MiniOmni / Omnisense（桡骨 SOS 同类产品） | 结果以 m/s + T/Z 呈现，配绿 / 黄 / 红分区图；软件内置操作教学视频 |
| Echolight EchoS（REMS） | 报告与界面都以 T 值分区色带定位被测者 |
| Philips IntelliVue / 6000 系列 | 波形与对应数值同色编码；把注意力集中到关键数据区 |
| GE HealthCare Edison 设计系统 | 统一组件、患者信息横幅（patient banner）、WCAG 2.1 AA 对比度 |
| 临床看板设计经验 | 只有一个“临床锚点”（本次结果 / 实时状态），其余信息退后；红黄只给异常 |
| Qt Widgets 实践 | 设计令牌单一来源生成 QSS；动态属性做变体；QSS 做不到的（阴影、仪表、图标）用 QPainter；固定基础样式（Fusion）+ 调色板；不给实时刷新的控件加 QGraphicsEffect |

## 2. 视觉语言

- 中性色承载界面：canvas `#F4F5F7`，surface `#FFFFFF`，sunken `#FAFBFC`，line `#E6E8EC`，ink `#15191F / #3B4350 / #667085 / #98A2B3`。
- 蓝 `#2B5BD7` 只表示“可操作 / 当前焦点”；绿 `#12805C`、琥珀 `#B25E09`、红 `#C4321F` 只表示临床与设备状态，各配一个浅底色。
- 通道色 A 蓝 `#2B5BD7`、B 青 `#0E9384`、C 琥珀 `#D08A1E`、D 紫 `#7A55E8`，波形与标签一致。
- 字体：中文 Microsoft YaHei UI；数字 Segoe UI Semibold（等宽数字）；调试值 Consolas。层级 44 / 22 / 19 / 15 / 14 / 12。
- 卡片：12px 圆角、1px 发丝线、极浅阴影（由父层 `CardCanvas` 统一绘制）；卡片标题 15px + 12px 副标题，右侧放卡片自己的操作。
- 图表：无坐标轴线、虚线网格、10px 刻度；年龄 – SOS 图在屏幕上用柔和分区色和分区文字，“本次”用深色点 + 深色标注；报告打印仍用原配色。

## 3. 架构

```
resources/theme.qss          QSS 模板，颜色写成 @token，加载时替换
include/theme.h, src/theme.cpp
    Theme::tokens()           全部颜色 / 字号 / 圆角的唯一来源（C++ 与 QSS 共用）
    Theme::styleSheet()       读取模板并替换 @token
    Theme::installApplicationStyle()  Fusion + 调色板（只做一次）
    Theme::numberFont / monoFont / repolish / setTone
    Theme::styleChart / styleValueAxis   QtCharts 统一样式
include/uikit.h, src/uikit.cpp       只负责绘制、不含业务逻辑的小部件
    Icons::icon(Icon, color)  QIconEngine 矢量线性图标，任意 DPI 清晰，禁用态自动变灰
    BrandMark                 超声波纹品牌标
    CardCanvas                主体背景 + 卡片阴影（只在父层画，不影响实时图表）
    BrandPanel                登录页左侧品牌区
    AvatarBadge               姓名首字头像
    TScoreGauge               T 值分区条 + 指针（仅显示已有 T 值）
    RoundProgress             5 轮进度段（读取现有轮次计数，仅显示）
```

MainWindow 仍是同一套 `ui->` 控件对象，只在 `mainwindow_layout.cpp` / `mainwindow_display.cpp` 里重新组装和换样式；业务代码里散落的 `setStyleSheet("color:#…")` 改为 `Theme::setTone(widget, Tone::…)`，颜色只在 QSS 中出现。

## 4. 界面结构（保持单窗口三栏，不改信息架构）

- 顶栏：品牌标 + 名称 ｜ 端口下拉（带图标）+ 连接 / 断开 + 连接状态胶囊 ｜ 调试：获取波形、自动采集 ｜ 档案、报表、校准 ｜ 右侧：检测中状态胶囊 + 头像账号菜单。
- 左列：四通道波形（标题栏放唯一的增益滑条，四个通道共用）；声速趋势（三项数值条 + 调试值胶囊 + 曲线）。
- 中列：年龄 – SOS 参考（来源、图例移到标题栏）；检测过程（状态 + 5 轮进度段、有效值条、corrA / G 两张卡、辅助 D、提示）。
- 右列：被测者（头像、姓名、性别 / 年龄标签、编号 / 出生 / 身高 / 体重、主操作按钮）；测量结果（大号 SOS、骨强度标签、T 值分区条、四项指标、以往记录）；测量部位（圆角示意图）。
- 档案页、报表预览页、登录页、各对话框使用同一套令牌与组件。

## 5. 不改动的边界

测量算法与常数、闸门、串口协议与 80 ms 时序、数据文件格式、账号与权限逻辑、打印 / 导出报告的版式与配色、四通道 QChart 代码（只换样式，波形仍关闭抗锯齿）。

## 6. 验证

Debug 构建；主窗口安全测试、校准、档案、账号测试；用 `BONE_UI_CAPTURE_DIR` 离屏截图 1366×768、1600×900、1920×1080 逐张检查；`git diff --check`。真实设备、实际显示器 DPI 与打印仍需现场确认。
