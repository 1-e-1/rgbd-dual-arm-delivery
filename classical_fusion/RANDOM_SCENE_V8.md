# V8 有界随机几何采样

本版本修改场景生成和离线真值评估，不增加 ActiveTSDF 建模流程。
旧 v7 数据和旧模型不覆盖这些新形状，不能直接把旧评估成绩作为 v8 成绩。

## 默认随机范围

| 参数 | 提议分布与约束 |
| --- | --- |
| 数量 | 每个场景独立均匀整数采样 3–5 个；CLI 可配置 |
| 形状 | 每个物体独立、等概率抽取 cuboid / sphere / cylinder，不强制每场景凑齐三类 |
| 长方体尺寸 | 局部 x、y 分别 6–16 cm，z 为 8–24 cm，连续独立均匀采样 |
| 球体尺寸 | 直径 8–16 cm，连续均匀采样；不是三个轴独立缩放的椭球 |
| 圆柱体尺寸 | 直径 6–16 cm、高度 8–24 cm，连续独立均匀采样 |
| 位置 | 物体整体位于 x=[0.54,0.94] m、y=[-0.50,0.50] m 的工作域内，非固定网格 |
| 姿态 | roll、pitch 分别在 ±35° 内，yaw 在 ±180° 内连续均匀采样 |
| 高度 | 每个物体以 25% 概率接地；否则最低点高度在 6–38 cm 内连续均匀采样；不强制一定有接地或悬空物体 |
| 间距 | 每场景的保守表面净距下限在 6–12 cm 内均匀抽取；每个物体至少有一个邻居的实际表面净距 ≤45 cm；位置自由随机，不是等间距网格 |

姿态是有界 Euler 角采样，不宣称 SO(3) 上均匀。三个原生几何族是当前明确支持的
形状范围，不宣称已经包含任意凹体或任意 CAD 网格。

“随机”不等于所有最终参数仍相互独立：越界、相交、净距与轨迹可见性约束会改变
最终接受布局的联合分布。数量、形状、尺寸和净距下限在每个 episode 开始时固定，
拒绝布局后只重试摆放/观测，不重新抽到更少、更容易的物体。达到重试上限即明确失败。
`geometry_NNNN.json` 保留原始几何提议，包括未成功采集的场景，便于审计选择偏差。

球和倾斜圆柱使用各自真实的旋转包围范围计算接地高度；不能把它们都当作旋转长方体。
物体间最小距离使用保守 AABB 净距保证不穿透；上限使用各物体内切球间距作为
真实表面距离的保守上界。45 cm 限制最近邻，不要求所有物体两两都小于 45 cm，
也不宣称各对距离可以独立均匀采样。这个 AABB 仅用于采样约束，
不是建模算法输出的外框，也不传给主动视角选择器。

## 数据、控制及评估

- 观察仍是 depth [H,W]、K [3,3]、相机位姿 [7] 和关节角 [8]。
  TSDF 不接收物体数量、形状标签、尺寸或中心；无需事先知道物体数。
- 形状 ID、尺寸、姿态、`gt_object_points` 仅记录在评估元数据中。真值点在各物体
  表面按面积均匀采样，每个物体 1536 点，避免球/圆柱仍被错误评估为长方体。
- schema_version=8。旧数据目录或采样参数不一致时拒绝混合续采；镜头配置为修正后的
  focal_length=24，不能套用旧 v7 拟合出的 fx≈128。
- 默认 256×256、110° 请求 FOV、2 mm Gaussian 深度噪声、零重力、运动学关节控制，
  physics_dt=1/60 s、decimation=4、15 Hz 采集。相机内外参仍需独立标定验证。
- 不要求全部物体同时可见，也不强制每个物体在整段轨迹中都可见。仍要求至少一个
  初始物体可见；全空帧比例 ≤20%，连续全空帧 ≤15；有碰撞则拒绝。
- 采集沿用既有模拟器轨迹流程，并非新版 NBV 的在线闭环验证；端点无碰撞不等于
  全臂扫掠路径安全，更不是实机运行许可。

随机种子：形状/尺寸为 base_seed + episode×1000003 + 71；数量为
base_seed + episode×1000003 + 193；摆放重试与传感器噪声分别使用可追溯的 attempt 种子。
默认 base_seed=20260905（批量启动器）；每场景实际配置保存在 manifest 中。

## 验证与复现

已完成：44 项仓库测试全部通过，包括 1000 个随机布局的尺寸/姿态/净距/接地/工作域检查，
三类几何的解析距离与旋转表面真值检查，以及形状感知评估回归。

原生仿真烟雾测试数据：`data/random_shapes_v8_smoke/`，固定 seed=20260905，30 帧。
实际抽到 2 个圆柱和 1 个长方体，3 个都悬空；全空帧 0%，碰撞帧 0%，
最小保守表面净距 21.41 cm（采样下限 11.36 cm）。有物体整段未被看到仍可合法接受。
两视角 TSDF 兼容性回放可运行；该单场景不足以衡量新随机分布上的总体性能。
本轮未启动正式大规模重采集或训练。

从统一工程根目录 `D:\work_space\lunwen2` 运行：

```powershell
# 几何约束和解析评估测试，不启动 IsaacSim
& 'D:\conda_envs\env_isaaclab\python.exe' -B -m pytest tests/test_random_shapes.py -q -p no:cacheprovider

# 正式批量采集入口；执行此命令才会开始（本轮没有启动全量）
& 'D:\conda_envs\env_isaaclab\python.exe' -B -u scripts/run_random_geometry_collection.py --episodes 80 --frames 120 --seed 20260905 --objects_min 3 --objects_max 5 --shape_types cuboid sphere cylinder

# 新镜头诊断显式使用记录 K，不能冒充独立标定成绩
& 'D:\conda_envs\env_isaaclab\python.exe' -B -m classical_fusion.active_tsdf_experiment --data_dir data/random_shapes_v8_smoke --use_recorded_intrinsics --start_episode 0 --episodes 1 --view_budget 2 --modes tsdf_order tsdf_active --output_dir classical_fusion/outputs/random_shapes_v8_smoke_reconstruction
```

正式算法对照应锁定采样配置和互不重叠的标定/调参/测试 seed 集，比较相同视角预算下
体素融合、随机 TSDF、主动 TSDF，并按物体数量和形状组成分组报告覆盖率与精度。
不应用固定物体数、固定几何或单个成功样例代替随机环境评估。
