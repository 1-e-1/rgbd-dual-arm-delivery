# 无神经网络主动建图：Planning v3

唯一保留的生产路线：已标定深度 / 位姿 → 鲁棒 4 mm TSDF + 自由空间证据 →
信息增益 NBV、测量表面网格、保守欧氏净距与覆盖球连续扫掠检查。

- `active_tsdf.py`：共享 TSDF 核心，零交叉表面、Surface Nets、一阶/二阶导数、NBV。
- `planning_map.py`：Planning v3 默认配置、不同视角自由证据、EDT 快照与轨迹查询。
- `active_tsdf_servo.py`：同步观测与运动学控制接口，依赖外部全臂路径安全回调。
- `active_tsdf_experiment.py`：当前路线评估；order/random/active 只比较视角策略。
- `planning_audit.py`：离线真值探针验证，真值不输入建图或选视角。

这里的 PyTorch 仅作数值计算，没有神经网络训练、权重、PPO 或 World Model。
旧的传统点云融合 + 神经世界模型 / PPO 分支已移除。

完整公式、参数和实验结果见 [PLANNING_V3.md](PLANNING_V3.md)。
随机几何采集协议见 [RANDOM_SCENE_V8.md](RANDOM_SCENE_V8.md)。
历史有效指标保存在根目录 `reports/planning_v3/`。展示图和临时网格样例已删除。
原始采集样例在 `data/planning_shapes_512_full_depth/` 等目录，未删除。

未观测区域不闭合、不当作自由空间。该路线不是完整机械臂或实机安全认证。
