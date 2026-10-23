# 当前主线：恢复 PPO 之前的确定性 RGB-D 协作

> 2026-09-16 阶段调整：用户明确要求暂不考虑基座反作用，优先研究视觉建图与引导抓放，允许神经网络，目标改为随机整场成功率 99%。本页以下保留为历史确定性基线，不再代表新阶段方法限制。新阶段见 `visual_guidance_lab/README.md` 和 `protocol.json`；旧代码、权重和失败记录均保留，新训练尚未完成。

2026-09-12 按用户要求恢复非学习主线。这里不是退回早期 CFM、drifting、CAD 环境避碰或先左后右的顺序控制。

## 固定入口

```powershell
python -m astronaut_delivery.rgbd_classical_mission --output <新结果目录> --scene <仿真USD> --initial-scan <原始RGBD扫描目录> --scene-token <场景标识> --calibration-id <标定标识> --assume-unchanged-scene
```

以上尖括号是待替换参数，不是可直接执行的路径。只有确认扫描后环境未变化才可声明 `--assume-unchanged-scene`；初始地图必须带可核验的原始 RGB-D 记录。USD 供仿真渲染及独立审计使用，不为控制器提供环境真值几何。

此入口直接实例化原 `RGBDMission`，不加载 PPO、果蝇网络、世界模型、CFM 或 drifting 权重。默认 `set`：保持最低必要视觉质量后利用左臂剩余运动空间降低合成反作用。`pose`、`hold` 仅保留为对照选项。

保留：异步双臂协作、RGB-D 建图与局部更新、实测目标引导、右臂 IK、左臂 QP、连续轨迹避碰认证、视觉失效平滑制动。不得以不可观测区域为自由空间，也不得以降低反作用为理由放松安全边界。

关闭后续试验开关：`adaptive_visual_proof`、`quality_aware_handover`、`avoid_rejected_proposals`。原有限补观测/恢复机制仍保留。

保留已验证的地图容量修复：本入口默认 16M cells，仍为 2 mm 体素，ROI 和安全阈值不变。需要原 12M 设置时显式传 `--local-map-cell-budget 12000000`。因此这是固定技术路线并保留容量修复，不是逐字节恢复历史源码快照。

## 实验保留与证据边界

原通用入口 `astronaut_delivery.rgbd_mission` 保持兼容，仍默认非学习控制和 12M cells；新固定入口作为当前推荐入口。PPO 入口、训练权重、`flybrain_lab/`、各次实验结果均原地保留，仅退出当前主线，不删除、不覆盖。可选 VLM 上位机不属于本入口和本研究控制主干。

历史 24 场景保留集：set 16/24、pose 16/24、hold 18/24；这是旧冻结设置的数据，不能当作此次 16M 配置重测结果。82007 的容量修复完成 2/2 是复用开发场景证据，不更新保留集成功率。回退不等于已解决全部失败，也不建立零反冲或实机安全保证。

完整计算见 [技术路线与数学计算](CURRENT_RGBD_TECHNICAL_ROUTE.md)。本次入口测试检查配置隔离和兼容性，不代替全场景仿真重测。
