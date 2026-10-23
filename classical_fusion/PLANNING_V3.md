# 面向轨迹规划的视觉地图 v3

## 结论与可执行流程

仍然只有一张测量地图：RGB-D/相机位姿 → TSDF 更新与自由空间证据 → 下一视角。
表面点、三角面和一阶/二阶导数来自同一 TSDF。规划时对其自由空间证据做一次
欧氏距离变换，形成可缓存的碰撞查询快照，不再增加生成模型或学习阶段。

已实现保守的**仿真环境碰撞查询模块**，但没有证明整套系统已经满足实机轨迹执行要求。
特别是：表面很准不等于环境完整；开放三角面不能直接当作封闭障碍实体。

核心代码：`planning_map.py`，评估：`planning_audit.py`、`active_tsdf_experiment.py`。

## 精度改进

1. 原先仅校正焦距，主点仍固定在 (128,128)。仅用 v7 标定场景 0/1 重新拟合
   fx、fy、cx、cy，得到约 (128.0036,128.0023,127.5002,127.4996)。
2. 独立原生仿真验证：512×512、110°、focal_length=24 的三个无物体平面位姿，
   对整数像素索引使用 cx=cy=255.5，平面投影误差降至浮点数值误差量级。
   这只是无噪声平面的一致性检查，不是实际传感器的纳米级精度声明。
3. 原生相机读数通过 `camera_metric_intrinsics` 统一转换。物理镜头参数不移动，
   只转换像素坐标约定；不能对已标定 K 再减一次 0.5。旧 v7 数据必须使用自己的
   完整拟合 K，不能使用新镜头的焦距。采集续写会检查像素约定，拒绝新旧混合。
4. 规划配置采用 4 mm 体素、12 mm 截断带、至少 2 权重点和 6 mm Huber 更新尺度，
   降低异常深度对边界的影响。实验预算提升为 8 视角×5 帧；更改预算单独列出，
   不把更多数据的收益全部归因于算法。
5. 保留 4 m 内的有效背景深度作为自由空间证据，而非将 2 m 外的测量全置零。
   TSDF 表面范围仍为 2 m；远处墙面仅用于确认本地工作域的射线前方没有障碍。
   无效深度/无回波不能当作自由空间。

## Obs / State / Action / Reward / Loss

- Obs：深度 [H,W]、整数像素索引 K [3,3]、相机世界位姿 xyz+wxyz [7]、关节角 [8]。
  默认新采集分辨率在本次样例中为 512×512；标定、深度和位姿必须同步。
- State：F、W、独立视角自由证据 C、近表面否决标记 B，均为 [150,325,183]。
  所有网格按 x,y,z 排列。没有物体数量、形状类别或 GT 几何输入。
- Action：沿用运动学候选关节目标 [C,8] → FK 相机位姿 [C,7]；需要外部全臂路径
  检查回调。每控制步限幅 0.03 rad。15 Hz 为目标控制频率，NBV 可较低频调用。
- Reward/选视角分数：观测置信度增益＋未知区域探索收益－关节运动代价。
  新的未知区域项权重为 0.15，不再只关注已见物体附近；不是全局最优探索保证。
- Loss：TSDF 的投影距离残差；旧权重≥2 时使用 Huber IRLS 权重
  min(1,0.006/|d−F|)，无需神经网络训练。PPO/World Model 本轮未训练。

## 规划净距与连续碰撞检查

TSDF 是投影截断距离，不能直接作为欧氏净距。自由判定要求：

- 至少两个不同相机位姿的有效深度证据，阈值为平移 4 cm 或旋转 5°；重复帧和
  A→B→A 回访不重复计票。当前每个独立位姿仅使用一次自由证据，偏保守。
- 把体素体积、位姿误差和像素误差投影到深度图，用整个对应像素窗口的最小有效
  深度验证“体素整体都在表面前方”。无效像素使这条自由证据失效。
- 任何近表面观测都保留否决标记；未知、证据不足、存在矛盾和域外均不可通行。

对非自由体素做 EDT，查询扣除体素半对角线、查询点到体素中心距离和规划余量。
这是到“不允许通行体素集合”的保守净距下界，不是对未知物体的形状猜测。

`check_swept_spheres` 接受每个连杆/工具的保守覆盖球：起终点 [S,3]、半径 [S]。
沿球心线段采样，并额外膨胀半个采样间隔，因此不只检查起终点；狭窄通道会被拒绝。
对非线性关节路径，调用方必须给出 FK 曲线偏离球心弦线的上界。
该接口不包含机械臂自碰撞或机械臂之间碰撞，必须与各臂几何模型的检查器组合。

```python
from classical_fusion.planning_map import PlanningTSDF

mapper = PlanningTSDF(device="cuda")
# 用已同步、已标定的实测数据更新；每个新场景调用 reset()
mapper.integrate(depth, K, camera_world_pose)
# 获取足够的不同视角后，在静态观测阶段导出快照
snapshot = mapper.planning_snapshot()
result = snapshot.check_swept_spheres(
    link_sphere_centers_start, link_sphere_centers_end, link_sphere_radii,
    tracking_bound_m=measured_tracking_bound,
    interpolation_deviation_bound_m=verified_fk_curve_deviation,
)
# 只有 result.allowed 才满足这个环境地图的查询条件；它不是硬件执行许可。
```

任何地图更新、reset 或快照超时都会使旧快照查询失败。默认最大证据年龄为 5 s，
包含距离变换时间；它适用于静态建图后规划验证，不是动态障碍反应时限。
输入/输出全部使用同一个世界坐标系；各机械臂基座到世界的变换需单独标定。

## 实测与消融

固定随机种子 20260905；v7 场景 0/1 仅标定，20/21 用于实现检查，60–79 为20场景留出集。
下表只对应随机长方体 v7 数据，不代表所有混合形状。

| 方法 | 视角×帧 | 表面 MAE | 表面 P95 | P@5mm | R@10mm |
| --- | --- | ---: | ---: | ---: | ---: |
| 旧 v2 | 4×3 | 2.50 mm | 7.07 mm | 91.20% | 56.33% |
| 旧 TSDF＋完整标定＋更多观测 | 8×5 | 1.25 mm | 7.34 mm | 92.28% | 59.02% |
| v3 完整标定＋稳健细网格 | 8×5 | 0.84 mm | 3.66 mm | 96.60% | 59.00% |

MAE/P95 是预测点到真实表面的单向距离；P95 为每场景分位数再平均，不能替代完整性。
v3 的 P99 约7.97 mm，GT到预测点的距离 P95 仍约71.4 mm，暴露了未观测背面的缺失。
主要精度收益来自正确标定和稳健细网格，覆盖率提升仍然有限。

20场景×20000个固定随机探针：真实碰撞点62964个、半径20 mm球的真实碰撞探针
138665个，均未出现错误放行。这不是零风险证明。工作域中仅约12.49%的体素达到
自由证据要求；20 mm球在真实自由探针中的放行率约3.97%，非常保守，不能只报零漏检。
GT几何只参与事后探针标签计算，未参与建图或选视角。

单次混合形状原生样例：4个随机物体、512分辨率、120帧、8视角，
旧2m保存范围下的v3表面 MAE=1.63 mm、P95=3.98 mm、R@10mm=62.50%，
20mm球碰撞探针8924个未发现错误放行。它只是一个混合场景的验证，不是总体成绩。

保留4m深度的同场景补测与同一数据上人为截到2m的消融：表面点、选中视角和表面指标
完全一致；自由证据体素比例由23.68%提升至24.74%，20mm球在真实自由探针中的
放行率由10.97%提升至11.77%，仍未发现错误放行的碰撞探针。未观测区域依然拒绝通行。
另找到一条半径20mm球的28.49cm连续直线扫掠，扣除球半径和采样余量后最小净余量
18.79mm，记录在 `outputs/planning_map_v3_full_depth/example_sweep.json`。
这仅验证工具球直线，未计入实机跟踪误差，更不表示任意机械臂连杆都可沿该路线运动。

53项仓库测试通过，包括独立视角计票、未知/域外拒绝、快照过期、净距下界、
粗采样下端点之间的碰撞检查、像素中心约定及有效远背景与无效深度的区别。
地图更新/reset/快照构建通过锁串行，开始更新即使旧快照失效。

RTX 5070 Ti 留出实验：地图融合约17.8 ms/帧，表面提取8.7 ms，NBV约32.1 ms/次，
保守距离快照构建约1.31 s，20000个净距查询约3.62 ms。不含机械臂运动与相机渲染。
快照构建不能放进要求每步完成的15 Hz控制回路。

## 风险与放行条件

默认深度误差预算8 mm、平移标定误差3 mm、旋转误差0.2°、像素误差1 px、额外余量5 mm
均是**待实际测量的工程假设**。Gaussian 噪声没有有限硬上界，重复观测也不消除系统偏差。
亚像素小障碍、透明/反光表面、未覆盖的畸变和错标定可能破坏自由证据的有效性。

当前假定环境物体静止。活动机械臂应由已知CAD/FK几何独立处理，不能把移动连杆直接
融合成静态环境，也不能依赖旧地图处理突然移动的物体。机械臂碰撞体必须覆盖全部连杆、
工具和线缆；跟踪误差、通信时延、急停距离、各臂基座标定须纳入余量与独立安全回路。

建议工程放行门槛：先验证所需规划区域的表面P95≤5 mm、P99≤10 mm；实际标定与
误差预算通过独立测试；轨迹每个连杆的扫掠体均处于证据充分的自由空间。当前表面精度
已在上述仿真留出集满足示例门槛，但完整性、真实传感器/全臂误差验证尚未满足。
所以不能宣布实机轨迹执行就绪，更不能把未看到的背面填平后当作安全障碍模型。

## 复现

2026-09-06 已归并到根目录 `D:\work_space\lunwen2`，仅保留 Planning v3。
已有表格指标移至 `reports/planning_v3/`。旧对照算法源码已清理，只保留指标用于溯源。
历史展示图、临时 mesh 和 sweep 样例已删除；下面最后两条命令可按需重新生成，不在清理时自动运行。
原生样例数据迁至 `data/planning_shapes_512_full_depth/`；53 项测试是整理前的历史记录。

```powershell
& 'D:\conda_envs\env_isaaclab\python.exe' -B -m pytest tests/test_planning_map.py -q -p no:cacheprovider
& 'D:\conda_envs\env_isaaclab\python.exe' -B -m classical_fusion.active_tsdf_experiment --planning --refine_calibration --start_episode 60 --episodes 20 --view_budget 8 --frames_per_view 5 --modes tsdf_active --output_dir classical_fusion/outputs/planning_map_v3_holdout
& 'D:\conda_envs\env_isaaclab\python.exe' -B -m classical_fusion.active_tsdf_experiment --data_dir data/planning_shapes_512_full_depth --calibration_json configs/camera_512_f110_integer_pixels.json --start_episode 0 --episodes 1 --modes tsdf_active --output_dir classical_fusion/outputs/planning_map_v3_full_depth
& 'D:\conda_envs\env_isaaclab\python.exe' -B -m classical_fusion.render_planning_result
```

上述表格/探针统计为本工程实测。TSDF与ESDF分开用于表面表达和规划距离查询的做法参见
[Voxblox规划文档](https://voxblox.readthedocs.io/en/latest/pages/Using-Voxblox-for-Planning.html)；
本实现采用[SciPy精确欧氏距离变换](https://docs.scipy.org/doc/scipy/reference/generated/scipy.ndimage.distance_transform_edt.html)。
独立视角阈值、误差预算和探索权重为本工程设计，不是文献给出的安全认证结论。
