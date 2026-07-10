# 实施进度

## 2026-07-10：阶段 A

- 已创建 `codex/feature-complete` 分支。
- 基线 Qt Debug 构建通过。
- 已确认：现有工作区包含未提交的 USB 热拔插稳定性修改；本轮保留该修改，不做回退。
- 硬件、串口和真实测量结果尚未进行现场验证。

## 2026-07-10：阶段 B

- 新增 `PatientStore`：患者档案写入 `patients.xml`（version 2），检测历史使用独立 `measurements.xml`。
- 旧版 `patients.xml` 在首次迁移前复制为 `patients.xml.bak`；迁移或写入失败会保留原文件。
- XML 使用 `QSaveFile` 原子写入。
- Qt Debug 构建通过；旧 XML 实际迁移、真实历史数据仍未验证。
