# 纯 RGB-D 仿真研究：复现与证据入口

本文件只覆盖最新测量权限路线。项目尚未全部完成；实物验证排除在当前任务外。旧 CAD baseline、神经网络建图训练和新的纯 RGB-D 控制结果不能混合统计。

## 已有证据

- `logs/pytest_project_full_48.xml`：当前全项目 479 项通过，含漂移模型子工程测试；一个 PyTorch nested-tensor 提示不影响通过。此前显式传入 `tests` 的命令只覆盖主目录，不能称为全项目测试。
- `reports/rgbd_task_sensing_budget_development_01`：6101/6103 两个复用开发场景均完成两个物体，任务、几何、反作用及原始时钟审计通过；两次第二物体都没有补观测，不能据此隔离预算重置的因果收益。
- `reports/rgbd_holdout_01`：已启动 24 个未见场景、set/pose/hold 共 72 次冻结试验。全部失败保留；运行过程中不调整源码/配置。当前没有最终成功率或优越性结论。
- `reports/rgbd_rendered_stress_development_01`：冻结新渲染名义、RGB-D 黑帧、RGB 遮断、0.3 s 延迟四次均完成；故障各触发一次实际 C2 制动并恢复。独立几何、反作用、4,339 份原始时钟以及故障前后图像哈希通过，详见 `RGBD_RENDERED_STRESS_PROTOCOL.md`。这是单个复用开发场景，不是保留集成功率。
- `reports/rgbd_euclidean_envelope_development_01`：6102 单物体完成，6101 第二次放置失败；两次整段几何与时钟审计通过。保存状态的误差球外包体积减少不能替代整任务成功。
- `reports/rgbd_spread_destination_development_01`：三开发场景已完成，6102/6103 成功、6101 首次放置失败，三个时钟审计均通过。该启发式已撤回默认路线。当前软件没有使用预知物体数量或环境 CAD 填补未知空间。
- `logs/pytest_full_mission_35.xml`：此前主测试目录 402 项通过，涵盖接触走廊连续界、精细块整组件分配、未知细图不回退到粗图自由、块版本/身份撤销、原始请求时钟独立审计。
- `reports/rgbd_contact_query_microbenchmark_01.json`：原接触许可的精确查询重排；两种次序都通过同一保存状态的提案，端点逐值一致，3.846 s 对 0.623 s 是单次有序局部微基准，不是全任务加速。旧字段 `counted_forbidden_cell_tests` 在未前置过滤组含原来会在后续被许可排除的单元；应理解为实际枚举查询量，且是 5 次完整 guard 调用的合计。
- `reports/rgbd_clock_support_development_01/run_6101_set/wall_clock_audit.json` 与同批次 6102：两次时钟审计通过，共 3,580 份在线原始记录。整场景任务均失败，不能把审计通过误报为任务成功。
- `reports/rgbd_refined_robot_probe_01/probe.json`：原始扫描重建的连杆精细块与完整保存地图提案，未执行。此提案朝向真实剩余目的地，非最后高度调整端点的零位移测试。

- `logs/pytest_full_mission_31.xml`：历史主目录 374 项通过；新增双时钟、运动前重观测、传感器异常时拒绝未执行轨迹、地图过期与接触柱交集检查。以下 `_29` 的 360 项是历史版本结果。
- `reports/rgbd_paired_mission_pilot_02/statistics.json/.md`：第二轮 9 次冻结开发配对，set 1/3、pose 2/3、hold 1/3；唯一共同成功场景中有力矩 RMS 收益，也有更高峰值和更长任务时间。不能删去不利指标或声称保留集收益。

- `reports/rgbd_nominal_multi_mission_08/mission_audit.json`：名义三物体全部释放及撤离通过；169 段，12,933 个反作用样本，水平误差 0.607/0.857/0.690 mm。每物体撤离按实际执行关节复算，不能沿用上一物体完成标志。
- 同目录 `rgbd_guidance.gif` / `rgbd_guidance_summary.png`：96 帧同步测量/引导/六轴合成反作用演示，PNG 已视觉核对；初始灰色表面不是最终融合地图。`rgbd_surface_comparison.png` 单独展示同一最终状态的真实测量三角面与覆盖率。
- 同目录 `feedback_replay_causal.json`：180 次实际控制器的记录反馈故障回放，通过因果时间、检测时限、路径内制动与 C2 静止检查；不是重新渲染的恢复试验。`inertia_sensitivity.json`：同一全部运动样本的 32 次质量属性开环扰动，不是闭环鲁棒性证明。
- `logs/pytest_full_mission_29.xml`：历史主目录 360 项通过；覆盖形状相关证据撤销、NumPy 双精度 ROI、持物凸包自碰撞细化、接触边界交集认证、相机回退、认证解码缓存、宽图静态区间查询和配对输入协议。`_14` 的 4 项新回放测试失败已保留，原因是回放器将微小未来时间四舍五入进当前观测；`_16` 的两项测试缺少新实际接触间距所需的右臂 FK 测试替身，补齐并增加回归测试后通过。新增测试前的数值类型和粗球误报复现日志也保留。
- `reports/rgbd_pilot_repair_08_6102_set/mission_audit.json`：开发场景唯一物体完整释放与撤离，57 段、4,407 个反作用样本通过；水平误差 1.555 mm、支撑间隙 1.112 mm、实际撤离 80 mm。该次未触发相机回退，不是回退有效性的证据。
- `reports/rgbd_replay_cache_benchmark_01.json`：同一 732 帧原始记录和 9,203,205 单元细图，暖解码缓存重放 14.793 s、认证流式重放 31.416 s，全部地图字段逐字节一致；冷缓存 23.213 s。每次仍验证全部原始文件哈希，不缓存可直接授权运动的自由图。单次有序微基准，不代表完整任务加速。
- `reports/rgbd_pilot_repair_03_6103_set/mission_audit.json`：重复开发场景修复验证，两个物体全部完成，123 段、9,196 个反作用样本独立复核通过，水平误差 1.564/1.946 mm。不是未见场景，不与旧源码基线构成配对。
- `reports/rgbd_paired_mission_pilot_01`：冻结开发种子 6101/6102/6103 的 9 次全任务配对全部完成。独立全场景成功为 set 0/3、pose 1/3、hold 1/3；没有与 set 共同成功的场景，因此不计算完整任务反作用优势。统计见 `statistics.json/.md`。这是开发数据，不是保留集。

- `reports/rgbd_nominal_multi_mission_07/mission_audit.json`：连续两个物体完成释放和撤离，水平误差 0.772/1.054 mm；136 段几何、10,586 个反作用样本通过。第三个物体接近失败，因此整场景失败。包含初始化的任务墙钟 643.111 s，不等于 176.122 s 的运动时间，也不等于流式计算 P99 4.422 ms。
- `logs/pytest_full_mission_12.xml`：此前版本 292 项通过，新增接近步长回退、纯 RGB-D 证据索引隔离和失败计入的场景级统计测试。

- `reports/rgbd_measured_transfer_15/transfer_audit.json`：首个单物体释放及撤离全部通过的独立审计，56 段、4,130 个实际反作用样本；水平放置误差 0.384 mm。这仍不是随机场景成功率或多物体任务验收。
- `reports/rgbd_measured_transfer_09/transfer_audit.json`：单物体释放独立审计通过，不含撤离。
- 同目录 `rgbd_guidance.gif` / `rgbd_guidance_summary.png`：64 帧 Matplotlib 演示及末帧概览，已检查 PNG。
- 同目录 `reaction_sharing.json` / `.npz`：实际轨迹双臂反作用分解，不是独立 baseline。
- `reports/rgbd_measured_transfer_10`、`_11`：名义场景重复开发失败，原始帧、拒绝诊断和源码快照保留。后者 45 段独立几何通过，但没有释放。
- `logs/pytest_full_mission_02.xml`：270 项通过，0 失败/错误/跳过；后续新增功能另有测试日志。
- `reports/rgbd_random_6100_mission_03/mission_audit.json`：随机开发场景实际 1 个物体，控制器不读取数量；完整循环通过 50 段几何及 3,853 个 wrench 样本复核，放置水平误差 1.060 mm。该种子参与了开发，不能当作未见保留集。
- `reports/rgbd_matched_cycle_15.json` / `.md`：同源码、场景、扫描、初态及任务的名义配对；两者释放与撤离均通过。集合策略力矩 RMS 降 8.16%，力 RMS 升 1.34%，力峰值升 11.05%；保留全部不利结果。
- `reports/rgbd_evidence_index.json` / `.md`：包含失败的开发记录目录，不是跨场景成功率。
- `data/rgbd_development_scene_6100` 与 `data/rgbd_development_scan_6100/recording.json`：随机场景及其独立 516 帧模拟预部署扫描；不是实物采集。

## 名义场景复跑

USD 组合依赖核对：`astronaut_delivery_rgbd_cabin.usda` 的 12 个实际使用层全部位于当前工程的 `isaacsim_model` 内，包含左右臂、宇航员工作姿态、C 件、磁吸、D435i 和工作台。没有指向此前外部工程目录的 USD 层依赖；这项检查不等于已经验证任意新机器上的驱动/依赖安装。

当前已安装环境核对：NumPy 1.26.0、SciPy 1.15.3、PyTorch 2.7.0+cu128、Warp 1.15.0、usd-core 26.8、OSQP 0.6.7.post3、Matplotlib 3.10.3；GPU 为 RTX 5070 Ti 16 GB。这里记录的是实际运行环境，不是重新安装或升级依赖的指令。

工作目录为工程根目录。每次选一个不存在的新输出目录；不要覆盖旧实验。

```powershell
& D:\conda_envs\env_isaaclab\python.exe -m astronaut_delivery.rgbd_transfer `
  --output reports/my_rgbd_transfer `
  --scene isaacsim_model/astronaut_delivery_rgbd_cabin.usda `
  --initial-scan data/rgbd_initial_full_corridor_01.json `
  --scene-token astronaut_delivery_rgbd_cabin `
  --calibration-id 660ece523b85804d9a24fc37be455e4dbcfe9b6a178be2e0bd4254e2488e05f1 `
  --assume-unchanged-scene
```

当前工作区代码可能已超过 `_09` 的版本，因此复跑不等于历史结果逐位重现。`_10` 起保存实际源码 ZIP，`_11` 起也包含配置；运行末尾检查被记录源码是否变化。异步采集存在调度差异；保存的原始帧和接受时刻是该次运行的证据，不宣称仅凭随机种子可逐帧重现。

```powershell
& D:\conda_envs\env_isaaclab\python.exe -m astronaut_delivery.rgbd_transfer_audit --run reports/my_rgbd_transfer --scene isaacsim_model/astronaut_delivery_rgbd_cabin.usda
& D:\conda_envs\env_isaaclab\python.exe -m astronaut_delivery.rgbd_dashboard --run reports/my_rgbd_transfer --scene isaacsim_model/astronaut_delivery_rgbd_cabin.usda --frames 64
& D:\conda_envs\env_isaaclab\python.exe scripts/analyze_rgbd_reaction_sharing.py --run reports/my_rgbd_transfer --scene isaacsim_model/astronaut_delivery_rgbd_cabin.usda
```

审计仅适用于确有吸附事件的日志；启动失败不伪造载荷审计。放置与撤离标志分别检查。环境 CAD 只在独立审计侧可用，不能从审计结果直接修改控制器的地图单元。

## 随机配对开发协议

```powershell
& D:\conda_envs\env_isaaclab\python.exe scripts/run_rgbd_paired_evaluation.py --output reports/my_rgbd_paired_pilot --seeds 6101 6102 6103
```

脚本先冻结代码，逐场景生成、独立采集原始扫描，再运行两个策略。试验期间不得修改被记录的源码/配置。每次仅自动选择当前观察到的一个磁吸接口，不输入物体数量、位置或尺寸真值；失败场景仍占两个策略各一次试验。该命令是开发 pilot，不预先声称统计显著性、保留集性能或全物体递送完成。

物体数量 1–3，平面位置、尺寸、形状、平面内 yaw、间距及台面高度随机；接口保持水平，放置标记保持世界轴对齐。不包含任意 SO(3) 倾斜接口、未标记物体或任意语义抓取。

连续多物体/更密预扫描/无反作用优化消融的开发命令为：

```powershell
& D:\conda_envs\env_isaaclab\python.exe scripts/run_rgbd_paired_evaluation.py --output reports/my_full_mission_pilot --seeds 6101 6102 --mission --extended-scan --modes set pose hold
```

`hold` 表示普通任务增量不进行左臂反作用优化；仍允许安全约束下的必要视角重观测，不伪装成禁止视觉恢复的弱对照。该策略刚加入，实现和试验结果须分别看待。`--extended-scan` 统一为每场景 732 帧外部预部署扫描；每个策略共享本场景原始记录，不能把名义地图复用到随机物体场景。

## 尚缺的非实物验收

当前工作区全量测试：`logs/pytest_project_full_48.xml`，479 项通过，0 失败/错误/跳过，含 `piaoyi_model/tests`。新增覆盖竖直行 IK、欧氏误差外包、新渲染故障记录与 C2 制动复算、被否决的实测摆放分散候选、每观测物体有限恢复预算，以及正式统计强制原始时钟审计和未完成批次拒绝后处理。`pytest_project_full_43.xml` 因系统临时目录权限导致部分 fixture 错误，不是通过的记录；改用工程内独立临时目录后完整重跑，保留失败日志。命令使用工程内独立临时目录：

```powershell
& D:\conda_envs\env_isaaclab\python.exe -m pytest -q --basetemp logs/my_unique_pytest_tmp --junitxml=logs/my_unique_pytest.xml
```

`pytest_full_mission_06` 使用系统临时目录触发权限错误，不是通过的测试记录；保留该日志，不混同于新测试结果。

名义连续三物体任务现已通过独立审计。仍缺随机条件下的稳定性验证、冻结保留集及足量配对/消融、全栈在线时序与计算预算、最终论文定稿和相应复现包。不得把这些缺项归入“等待实物验证”。

## 正式批次与可恢复审计

当前正式批次的实际启动命令为：

```powershell
& D:\conda_envs\env_isaaclab\python.exe scripts/run_rgbd_paper_validation.py --output reports/rgbd_holdout_01
```

请勿并发重复运行这条命令。它先执行所有 72 次串行任务，再逐次完成原始双时钟、宽/细图和保留实测表面审计，最后生成 `statistics.json/.md`。`evaluation.json` 中单次任务的临时成功字段并不代替最终时钟审计。后处理要求全部试验结束、源码在全批次中未变化，而且当前源码仍与冻结清单一致。

如果任务执行已经全部结束，但后处理被外部中断，可在同一冻结源码下使用：

```powershell
& D:\conda_envs\env_isaaclab\python.exe scripts/run_rgbd_paper_validation.py --output reports/rgbd_holdout_01 --postprocess-only
```

它跳过已经存在的审计报告，不重新抽场景或重跑失败任务；缺失/失败报告会显式保留，不能把文件存在当作审计通过。不同源码的开发修订不得使用这个入口伪装成同一正式批次。正式计时期间不运行无关 GPU/CPU 基准或全套测试。

当前另有交付准备进程等待该批次，状态文件为 `output/repro/rgbd_holdout_delivery_01/preparation.json`。它不会启动第二份实验；在正式后处理结束后才运行出版表格汇总、带实际论文图片的源码包、迁移目录全量测试和 32 帧新渲染采集。每步检查返回码和产物，失败则停止后续准备步骤。它不会自动把文稿标为定稿；结果解释、尚缺消融和新版 PDF 逐页视觉审核仍需完成。等待和打包状态不能当作试验成功或论文完成证据。

## 源码、资产路径迁移验证

`output/repro/rgbd_development_source_05.zip` 含当时 492 个源码/配置/测试/必要资产文件，SHA256 为 `fb872722ca155627b6a57627e5241b28a38568026a5376e9548e807fa0a37c0e`。逐文件原始/打包哈希与 USD 路径重写记录位于包内 `SOURCE_BUNDLE_MANIFEST.json`；仅打包副本的绝对 USD 引用改为相对路径，原始场景未改。

新路径 `tmp/rgbd_relocation_check_05` 已通过该快照全部 474 项测试，XML 在 `output/repro/rgbd_source_05_relocation_tests.xml`。12 个实际使用 USD 层均解析到新目录；该目录还运行了独立 32 帧 RGB-D 新渲染采集并成功编译本地 Warp 内核。它验证的是同一已安装解释器下的源码/资产迁移，不是全新依赖环境安装、完整任务复现或实物采集。源码包 05 尚不包含后来增加的正式后处理与五项测试；最终交付包须在正式批次结束后重新构建和验证。原始实验 RGB-D 大数据及 Python/CUDA 环境不在这个轻量源码包内。

独立中文算法说明见 `CURRENT_RGBD_TECHNICAL_ROUTE.md`；主文、条件证明和原始日志各自保留明确来源。期刊投稿所需的作者信息与资产公开再分发授权不能由工程脚本替用户声明。

## 连续观察任务入口

`python -m astronaut_delivery.rgbd_mission` 使用与单物体入口相同的场景、扫描和校验参数，自动从真实观测到的磁吸接口建立队列，不读取场景物体数量。完成一个释放及撤离后，不 reset 机器人或传感器场景，而是重观测下一个接口、选择未使用的实测放置标记，继续执行。新局部细图重放初始扫描后，必须再次应用全部历史局部撤销记录，才允许查询。

任务队列为空只记录 `observed_queue_exhausted`，不能据此声称场景内没有未知物体。`python -m astronaut_delivery.rgbd_mission_audit --run <run> --scene <scene>` 在执行结束之后独立读取场景数量，重放每个释放物体的实际碰撞位置，检查是否真的全部完成。此结果不返回控制器。

## 名义三物体结果的独立复核命令

以下 `<run>` 替换为新的完整运行目录。输出 JSON 应使用新的文件名，避免覆盖旧证据。

```text
python -m astronaut_delivery.rgbd_mission_audit --run <run> --scene isaacsim_model/astronaut_delivery_rgbd_cabin.usda
python -m astronaut_delivery.rgbd_environment_audit --run <run> --scene isaacsim_model/astronaut_delivery_rgbd_cabin.usda --motion-run <run>
python -m astronaut_delivery.rgbd_environment_audit --run <run> --scene isaacsim_model/astronaut_delivery_rgbd_cabin.usda --motion-run <run> --persistent-surface
python scripts/audit_rgbd_feedback_replay.py --run <run> --scene isaacsim_model/astronaut_delivery_rgbd_cabin.usda --output <new-feedback-report.json>
python scripts/audit_rgbd_inertia_sensitivity.py --run <run> --scene isaacsim_model/astronaut_delivery_rgbd_cabin.usda --draws 32 --output <new-inertia-report.json>
python scripts/plot_rgbd_surface_comparison.py --run <run>
python -m astronaut_delivery.rgbd_dashboard --run <run> --scene isaacsim_model/astronaut_delivery_rgbd_cabin.usda --frames 96
```

每个 `cycle_00x_local_map` 可单独传给环境审计并加 `--static-interval-fusion`；该文件夹的 `scene_state_at_export.json` 优先于最终任务状态。对不同随机场景必须传相应随机 USD，不能沿用名义场景几何。故障回放只接受已有全任务独立审计的测量日志，不会输出虚构恢复率。
