# 宇航员双臂视觉引导与磁吸递送

2026-09-12 当前主线已按要求恢复为 **PPO 之前的确定性 RGB-D 双臂协作**。
推荐入口：`python -m astronaut_delivery.rgbd_classical_mission`，默认 set 模式，
保留 16M 地图容量修复，不加载学习模型，不启用后续试验性恢复策略。
启动参数及历史结果边界见 [当前主线说明](research/ACTIVE_CLASSICAL_ROUTE.md)。
PPO 与果蝇实验原地归档保留，不属于当前运行路线；下方历史记录不代表本次重测。

当前要求已确定为 **环境避碰完全来自 RGB-D，允许原始 RGB-D 初始扫描**。见 [纯 RGB-D 路线与实际进展](research/RGBD_ONLY_ROUTE.md)。`research_delivery` 默认已改为纯测量启动门控；原 CAD 完整任务只能显式作为历史 baseline 运行，其成功率不证明新要求已完成。

当前纯 RGB-D 主入口为 `rgbd_mission`（已审计名义三物体连续递送），单物体入口为 `rgbd_transfer`。最新论文工作稿是 [RGB-D simulation manuscript](research/PAPER_RGBD_SIMULATION.md)，对应中文公式与伪代码见 [RGB-D 方法及协议](research/RGBD_METHOD_AND_PROTOCOL.md)。下面链接的旧完整技术路线与仿真论文属于不同历史权限边界，不能作为纯 RGB-D 版本的完整验收结论。

已验证：`reports/rgbd_nominal_multi_mission_08` 连续完成三物体抓取、搬运、放置、释放和撤离；169 段独立连续几何、12,933 个 wrench 样本全部通过，水平落点误差 0.607/0.857/0.690 mm，每个物体实际撤离 80 mm。三物体数量仅由独立审计读取，不输入控制器。完整场景一次成功不等于随机成功率；之前的停止记录继续保留。

新运行保存 `source_snapshot.zip`、源码/配置 SHA-256、运行参数和结束时源码一致性检查。随机配对开发入口为 `scripts/run_rgbd_paired_evaluation.py --mission --extended-scan --modes set pose hold`，每个随机场景独立采集 732 帧预扫描，三个策略共用该原始记录；目标仅由 RGB-D 自动选择，生成/启动失败也计入分母。开发种子 6101/6102/6103 共 9 次已完成，独立全场景成功为 set 0/3、pose 1/3、hold 1/3，源码一致性通过。当前随机可靠性不足；修复复测另存，不能混入原冻结批次。

当前全项目测试 479 项通过（`logs/pytest_project_full_48.xml`，含 `piaoyi_model/tests`）。第二轮冻结配对为 set 1/3、pose 2/3、hold 1/3，唯一共同成功场景有力矩 RMS 收益，也有更高峰值。竖直旋转行 IK 与欧氏误差外包未单独消除 6101 第二次放置失败。分散放置使该场景退化为首物体放置失败，已撤回默认策略。当前恢复最近可用实测标记，并对每个新观测物体分配同样的六次补观测提案预算；6101/6103 两个复用开发场景均完成两个物体，且全部原始时钟审计通过。两次第二物体均未用到补观测，因此不能把成功归因于预算重置。独立的[当前技术路线与数学计算](research/CURRENT_RGBD_TECHNICAL_ROUTE.md)不再混用旧 CAD 路线。

新渲染名义/黑帧/RGB 遮断/延迟四次整任务均已通过，三个故障均实际制动、恢复、释放和撤离；独立时钟审计核验 4,339 份在线原始记录。详见 [受控传感流压力试验](research/RGBD_RENDERED_STRESS_PROTOCOL.md)。每条件只有一个复用开发场景，不代表总体鲁棒性，也不是外界物体真实动态遮挡试验。

名义任务原始记录的 180 次反馈故障回放和 32 次质量属性开环扰动仍保留，不能冒充重新渲染的闭环压力实验。完整名义任务墙钟 766.081 s 对应运动 215.308 s，不能把流式 P99 4.437 ms 写成全流程实时。阶段成果不混入旧版冻结批次。24 场景、三个策略共 72 次正式保留试验正在 `reports/rgbd_holdout_01` 运行；失败保留，源码冻结。实物之外仍有未完成工作。

当前双臂同时运动、局部地图更新及反作用控制的说明见 [完整技术路线](COMPLETE_TECHNICAL_ROUTE.md) 与 [理论和独立验证](research/THEORY_AND_VALIDATION.md)。本页下方保留的是早期顺序控制版本记录，不应将其中“左臂保持”“65 项测试”等历史描述当作当前实现。

最新可运行异步集成入口、演示及未通过的非实物验收项见 [仿真交付说明](research/SIMULATION_RELEASE.md)。英文论文草稿见 [Simulation manuscript](research/PAPER_SIMULATION_STUDY.md)。研究目标未被证实的部分明确保留，不以测试通过代替科学结论。

本轮非实物验证入口：`formal_benchmark`（冻结随机配对）、`formal_statistics`（场景级统计）、`high_resolution_delivery`（4 mm 地图与可选 `--adaptive-free`）、`surface_accuracy_audit`（独立表面误差/覆盖）、`reaction_feasibility_experiment`（有界反作用残余）、`inertia_sensitivity`（惯性参数扰动）、`sensor_stress_delivery`（噪声/缺帧）。正在执行的实验不等于已完成验收，具体以各新目录的报告为准。

这是已有静态宇航员装配之上的**运动学原型**，不是训练任务，也不是实机安全认证。原装配和原始模型不修改。

本轮验证：固定场景 3/3 完成，80.32 s 仿真动作、1,063 次 RGB-D 观测，完整阶段连续审计开启；全工程 65 项测试通过。磁吸确认距离分别约 3.0、5.6、6.3 mm。原生 RTX 加载/出图通过但存在本机 Kit 退出告警，结果为 `PASS_WITH_ENGINE_WARNING`。最终复跑与原生出图所用轨迹的 SHA-256 完全相同，复用依据保存在 `native_render_reuse.json`。这些不是跨场景成功率或实机测量结果。

## 文件与执行

```text
configs/astronaut_delivery.json               可复现参数、三种零件和放置任务
astronaut_delivery/
  scene.py                                   台面、仪表台、零件 USD 构建
  motion.py                                  从 USD 关节读取 FK、IK、几何避碰、RRT
  vision.py / ray_kernel.py                   RGB-D 网格射线传感器、TSDF、视觉目标估计
  magnet.py                                  近距离吸附、携带、释放
  run.py                                     顺序控制、视觉反馈、日志与停止条件
  preview.py                                 真实几何预览、运动回放导出
isaacsim_model/
  astronaut_delivery_workcell.usd             新场景入口，引用原有完整装配
  astronaut_delivery_demo.usd                 执行日志生成的时间采样回放
reports/astronaut_delivery_verified/
  events.json / trajectory.json / result.json
  measured_map.npz / final_state.usdc
  left_camera.gif / delivery_overview.gif
  workcell_overview.png / workcell_final.png
  evaluation.json / native_render_result.json
tests/test_astronaut_delivery.py
```

在 `D:\work_space\lunwen2` 的 PowerShell 执行：

```powershell
& D:\conda_envs\env_isaaclab\python.exe -m astronaut_delivery.scene
& D:\conda_envs\env_isaaclab\python.exe -m pytest tests/test_astronaut_delivery.py -q
& D:\conda_envs\env_isaaclab\python.exe -m astronaut_delivery.run --parts 3
& D:\conda_envs\env_isaaclab\python.exe -m astronaut_delivery.evaluate
& D:\conda_envs\env_isaaclab\python.exe -m astronaut_delivery.preview --frames 100
```

`--sampled-debug` 仅用于快速调试，不能当作连续轨迹验证。正式结果以 `result.json` 的 `status`、`completed`、`continuous_audit` 三项共同判断。错误时保存现场并抛异常，不会把部分完成报告为成功。

在 Isaac Sim 中直接打开 `astronaut_delivery_workcell.usd` 查看静态工作区，打开 `astronaut_delivery_demo.usd` 查看时间轴回放。回放是日志的几何动画，不在 Isaac Sim 内重新运行规划器；执行已审计的关节轨迹应使用控制程序，而非 USD 矩阵插值。当前传感器使用环境内 NVIDIA Warp 对实际 USD 网格投射射线，不是 RTX 图像，不加载旧 PPO 权重、不伪装成 IsaacLab RLTask。后续如需训练，再单独接入原生 IsaacLab 任务。

## 工作区与输入输出

坐标：+X 宇航员前方、+Y 宇航员左侧、+Z 上方，米/rad。两张台面位于前方偏右的右臂安全可达区：近侧为操作台，远侧为摆放台。台面高 0.760 m，垫面高 0.762 m。静态宇航员、固定 C 形件、原始 8+8 个关节和右侧磁吸组件继续复用。

| 项目 | 当前设置 |
|---|---|
| Observation | RGB `[360,480,3]`、轴向深度 `[360,480]`、内参 `[3,3]`、OpenGL camera-to-world `[4,4]`、时间戳 |
| State | 本机两臂关节 `[2,8]`、FK、目标位置/协方差、TSDF、吸附对象、状态机阶段 |
| Action | 左或右臂关节目标 `[8]`，另一臂保持；吸附/释放为离散操作 |
| Reward / Loss | 不采用学习策略，无训练奖励、神经网络损失或新模型权重 |
| 控制步长 | 1/60 s；余弦时间缩放、段端速度为零；关节增量 ≤0.005 rad，速度 ≤0.3 rad/s |
| 观测频率 | 右臂运动期间 15 Hz（仿真时间）；左臂到位后观测 |
| 深度噪声 | 有效深度加独立 Gaussian 噪声，σ=1 mm，固定基础 seed 20260906 |
| 磁吸 | 接触点距离 ≤8 mm，方向满足朝向条件，仅允许单件携带 |
| 视觉闭环 | 最终接近每次重新观测，Cartesian 修正 ≤25 mm，最多 10 次 |
| 数据时效 | 视觉目标超过 0.25 s 不得继续视觉接近 |
| 建图 | 复用 `classical_fusion.PlanningTSDF`，本原型 12 mm voxel、36 mm truncation |
| 避碰 | 配置 margin 3 mm；部分窄相位使用两侧 margin，即更保守 |

这里的分辨率和体素用于快速功能验证，不代表前期高精度建图系统的精度上限，也不能把 12 mm 地图宣称为毫米级碰撞模型。真实视觉 RGB 的光照、材质与深度系统误差尚未覆盖；当前传感器 RGB 使用材质颜色，预览另加简易光照。已知三种颜色标记和近水平磁吸面是目标检测的明确前提；不能直接推广到任意无标记零件。

连续几何审计与规划是离线计算，60 Hz/15 Hz 指控制和观测的**仿真时间频率**，并不表示当前实现已经达到实时墙钟速度。当前 TSDF 未剔除机械臂的动态几何，运动中可能出现暂态残影；递送后清空重建，且它不被用作唯一安全规划地图。进一步用于未知环境规划前，需要加入基于 FK 的机器人自体过滤和更完整的多视角覆盖。

## 简洁技术路线

```text
reset -> 左臂选定可达观测位 -> RGB-D + TSDF + 彩色接触面估计
    对每个任务零件：
        右臂无碰撞接近 -> 分段视觉反馈修正 -> 近距离吸附确认
        携带几何加入碰撞模型 -> 抬升 -> 递送 -> 放置 -> 释放
        右臂退出 -> 清空旧静态地图 -> 重新观测
```

OpenGL 相机反投影（深度 z 为正轴向距离）：

`p_c = [(u-cx)z/fx, -(v-cy)z/fy, -z]`，`p_w = R_wc p_c + t_wc`。

在 RGB 连通区域内筛选顶部深度点，取稳健中心与协方差，得到磁吸面的世界坐标。静态待取零件的位置估计做 0.25 权重平滑；单次偏移超过 6 mm 或有效像素骤降至 40% 以下的测量不刷新目标，以抑制遮挡导致的中心漂移。视觉模块不读取 `/World/Parts` 位姿或模拟器物体 ID。已知目的位置属于任务输入；模拟器真值仅用于传感器渲染、磁吸物理近似、独立碰撞检查和评估/日志。

视觉引导以 `e = p_visual - p_TCP`，`Δp = e min(1, 0.025/||e||)` 给出小步目标，随后求带末端轴约束的 IK 并检查路径；执行后再次观测。这是停止—观测—再规划的分段位置式视觉伺服，不是已证明实时性的高速连续 IBVS。放置阶段使用已知台面目标和关节 FK，尚无独立视觉放置闭环。

吸附相对变换 `T_TCP_part = inverse(T_world_TCP) T_world_part`。携带时 `T_world_part = T_world_TCP T_TCP_part`，所以吸附和释放均不瞬移。创建 FixedJoint 表达连接关系，但动力学关节关闭，使用精确 FK 更新。未模拟磁场、吸力、接触摩擦或物件脱落。

## 避碰与两臂协调

右臂检查：全臂非邻接连杆自碰撞；宇航员（含工作姿态双臂）、C 形件、台面/仪表台、其他零件；当前左臂几何；携带零件对自身、左臂及环境的碰撞。

左臂自身规划不设置两臂互碰撞目标，遵循本次要求；但系统级右臂安全监督仍会核验左臂的预期运动不会碰到保持中的右臂，不把“左臂不负责”解释成允许碰撞。控制器不允许左右臂同时移动：左臂调整时右臂空载保持，右臂执行时左臂保持并提供观测。若以后允许同时移动，必须对两条带时间轨迹联合复检，不能直接复用这里的静态另一臂假设。

宽相位使用覆盖每个完整网格三角形的球集合；窄相位使用凸包面的分离轴。少量形状缺少边叉乘分离轴，只会造成保守拒绝。C 形件分为三块，每个原始三角形完整归属至少一块；宇航员使用前期按工作姿态重建的 20 个覆盖凸包。

允许接触表显式排除同一固定连杆组与直接相邻组，C 形件仅排除安装相邻的第 0/1 组；磁吸时仅允许磁铁与指定零件接触。载物放置仅允许指定台面支撑接触，最高 2.5 mm 几何容差，不关闭其他碰撞检测。碰撞近似可能拒绝实际可行姿态；允许接触表也必须由真实机械设计复核。

RRT 搜索使用离散检测，但默认执行前对**整条阶段路径**额外做自适应连续包络审计。未通过的路径段先用更密采样重新规划并重新审计，最多三次，失败则保持当前位置。携带阶段 IK 优先选择与当前姿态有直接无碰撞连接的解，减少不必要的折叠。按 USD 全链平移长度与几何半径得到构型无关的杠杆长度上界 L。对线性关节段 a→b，任意时刻到最近端点的几何移动上界：

`δ ≤ (L/2) Σ_j |b_j-a_j|`。

用该 δ 膨胀两端几何。如果不能证明分离，则二分轨迹段；递归预算耗尽即停，不把“采样没撞”当作证明。携带零件会扩大 L。结论仅相对于覆盖几何、允许接触和固定场景假设成立。

自碰撞使用更紧的相对运动上界：公共上游刚体运动会同时作用于两个连杆，不改变它们的间距；仅将两组之间关节的角度变化，乘以 USD 子链长度和局部几何范围给出的 reach 上界。该优化不降低 margin。移动载物对左臂的包络同样包含运动膨胀；左臂移动时，右臂监督只膨胀变化的另一臂，不无谓膨胀静止右臂。

## 验证与边界

固定 seed、锁定深度噪声序列，日志保留每次视觉误差、磁吸距离、规划节点、状态切换、时间、关节与零件位姿。测试包含 USD/FK 一致性、NaN/限位、实际碰撞拒绝、过期视觉目标、单件吸附/距离门限/释放不瞬移、C 形件三角形全覆盖、采样点间窄碰撞、右臂互碰撞责任、载物检查和局部遮挡拒绝。完整验收需要 3/3 递送、无安全停止、连续审计开启，再复核回放。

原生 RTX 静态加载/出图检查：`python scripts/check_delivery_isaacsim.py`。它仅验证最终场景加载和 RGB 出图，不替代上述运动路径检查。独立子进程遇到本机已知的 Kit SharedMutex / 退出挂起时，监督进程只结束自己创建的子进程，并在 `native_render_result.json` 中明确标为 `PASS_WITH_ENGINE_WARNING`，不隐藏引擎问题。

下一轮定量实验应单独设置：单次视觉定位 vs 分段反馈；离散检查 vs 连续审计（仅离线）；忽略载物 vs 载物检查（仅离线）；深度噪声 0/1/3 mm；多布局与多 seed。当前没有完成这些消融，不能声称跨布局成功率或实机准确率。

工程边界：当前左臂先移动到一个可达观测位后保持，并非完整主动多视角策略；旧 TSDF 在物品搬动后重建以避免残影；右臂避碰依赖已知场景 CAD，尚未接入未知空间保守地图作为唯一约束。控制限速不等于已验证加速度/jerk 限制。上实机前还缺独立硬急停、厂商机械限位、通讯延迟/丢帧处理、TCP 与手眼标定、磁吸力/载荷验证、人体软结构安全裕量、跨布局噪声验证和低速分阶段试验。不能直接在人附近执行本原型轨迹。
