# 冻结随机开发批次：诊断记录

## 最新进展：第二批冻结完成与双时钟修订

`rgbd_paired_mission_pilot_02` 的 9 次全任务与审计已完成且源码一致：set 1/3、pose 2/3、hold 1/3。6103 三种策略均完成两个物体；6102 只有 pose 完成；6101 均失败。唯一共同成功场景不支持总体优越性：set 相对 hold 力矩 RMS −12.19%，力矩峰值 +40.86%；相对 pose 归一化均方反作用 +1.92%。完整指标和分母见该目录统计文件。

6101 set 第二物体抬升末段出现细图接触柱足迹边界单元，部分单元只有约 0.187 mm 越出足迹。新增有界交集检查保留禁止区域、整单元高度和原间距；保存地图的 3 mm 抬升提案通过，但没有执行，不能据此称整个场景成功。复查入口：`scripts/probe_rgbd_view_rollback.py --run reports/rgbd_paired_mission_pilot_02/run_6101_set --place-step`；输出注明真实阶段 lift，选项名沿用旧入口。

双时钟闭环批次 `rgbd_clock_support_development_01` 已完成，两个场景仍失败。6101 完成 washer 并通过第二物体抬升，后在第二物体搬运停下；127 段、9,005 个反作用样本复核通过。6102 放置失败，52 段、4,824 个反作用样本复核通过。两次 `wall_clock_audit.json` 均通过：共 3,580 份在线原始文件的哈希和请求时钟一致，正常运行最大目标年龄分别 0.249110 s 和 0.236771 s。制动阶段不冒充正常视觉权限。批次存在已披露的独立诊断计算重叠，不能作为无竞争实时性能实验。

6102 的另一误拒来自接触走廊以整段关节曲率上界直接拒绝，却没有像碰撞证书一样细分。采用同一原界的 127 区间/深度 8 检查后，旧批次和新墙钟批次保存的失败位置均找到完整通过的放置提案，未放宽 8 mm 横向、2 mm 底面条件。

6101 搬运末端 a7/b7 的粗体素没有足够自由证据。`rgbd_refined_robot_probe_01/probe.json` 使用同一 732 帧原始扫描和 3 次变化事件，在 1,268,064 个单元的 2 mm 连杆块中重建，朝真实剩余目的地的完整提案通过。这不是把高度逃逸最后端点当目的地的零位移检查；新脚本明确恢复剩余目的地 XY。它仍是未执行的保存地图诊断，不是闭环成功。

`rgbd_refinement_development_01` 已完成：6102 整场景成功，57 段、4,415 个反作用样本通过，水平误差 1.312 mm、支撑间隙 1.126 mm、实际撤离 80 mm；6101 完成首物体后，第二物体接近超时。两次时钟审计均通过，正常运动最大年龄分别 0.249791 s 和 0.197366 s。两次均未实际构建连杆精细块，因此不能用这次成功声称该块已经闭环验证。

6101 超时不是本次新地图块引起：它尚未进入第二次携带。记录显示磁吸标记仍有约 230 个有效像素、每帧都重新检测，但执行前视觉证书耗时约 140 ms（749 个区间），使起步测量年龄达到约 0.18–0.24 s。原逻辑又等运动第 4 tick 才提交下一帧，导致过期制动、再次观察、短距离运动反复消耗 48 步预算。新修订将视觉证书预算由 1023 降到 127（未通过者按原候选回退，不放松条件），并在开始运动前提交下一曝光请求。`pytest_full_mission_36.xml` 404 项通过；`rgbd_clock_pipeline_development_01` 单独复测 6101，结果待定。保留集仍未使用。

冻结批次 `reports/rgbd_paired_mission_pilot_01` 已完成 9/9 次试验，种子 6101/6102/6103，set/pose/hold，每场景 732 帧同源 RGB-D 初始扫描，源码未变化。独立全场景成功 set 0/3、pose 1/3、hold 1/3；两个基线仅在 6103 成功。没有 set 共同成功场景，不计算完整任务反作用对比。统计在批次 `statistics.json/.md`；修复复测不得混入该批次。

## 已从保存记录确认的问题

### 6101：局部地图 ROI 的前提过强

set 在 10.8 s、9 段后停止，错误 `no_fresh_measured_placement_marking`。此前三个放置标记都曾被真实 RGB-D 识别，末次年龄分别为 1.2/2.4/2.4 s。触发错误的操作仅是确定细地图的范围，还没有用放置位置授权运输或接触。

批次结束后已实现历史测量只作为 ROI/视线提案，原时间戳不变；真正运输仍经过既有的源接口到新鲜放置标记的交接门控。已使用槽位仍排除。单元测试覆盖提案与新鲜执行权限隔离，闭环改善尚待复测。

### 6101：运输局部规划被测量证据拒绝

hold/pose 都出现 `transfer/transport/no_measured_safe_IK`。hold 最后一次右腕拒绝在 b7 附近；保存宽图中的三个位点 `(99,57,109)、(99,58,108)、(100,58,109)` 各只有 **1** 个独立自由视角，要求是 **2**。其 TSDF 为正、无表面否决，不是基于该记录就能断定的实际碰撞。

宽图只使用 228/252 视角，所以这个失败不能归因于视角总预算耗尽。当前运输还完整锁定捕获时的右腕朝向。批次结束后可先在保存地图上比较固定朝向与有限世界竖直轴偏航候选；只得到候选也不算闭环成功。不得降低自由视角数、直接填空闲，或用审计 CAD 给这些单元放行。

保存地图完整持物诊断 `logs/rgbd_6101_transport_yaw_probe_01.log` 在 5 mm/+15° 得到通过原守卫的候选，固定朝向仍拒绝。因此加入运输末级小步失败后的有限偏航备选，所有原几何/视觉证书不变，只在执行成功后提交朝向，进入下一物体时重置。全量 `pytest_full_mission_19.xml` 为 320 项通过。`rgbd_pilot_repair_04_6101_set` 正用原场景与原扫描闭环复测，不能预先称改善。

几何依据是 `e_z^T R_z(ψ)=e_z^T`：对固定 TCP 高度，绕世界竖直轴的刚体偏航不改变附着物各点的高度。仍须认证完整腕部、持有物及双臂扫掠，不能由这条代数性质替代避碰。

### 6102：视觉过期制动之后没有尝试重新观测

pose/hold 运输中执行了 `visual_loss_smooth_brake`。原冻结版本只对 `visual_quality_smooth_brake` 做静止全幅重观测，过期情况直接结束任务。新版本统一要求有限数值的 C2 静止，取得新的全幅观测，再由后续循环重新通过原质量/几何门限。不吞掉未知异常，不刷新旧目标时间戳。单元测试通过，闭环改善尚待复测。

set 则在抬升阶段出现测量认证 IK 失败，需独立分析其保存地图，不能直接认定与前一场景同因。

### 6103：相机恢复错误使用名义工具间距

set 在捕获前 17 段后 `no_certified_operational_view_acquisition`。保存记录中实际 TCP 间距为 7.005 mm，图像归一化特征约为 `[0.5672,0.2954,1.0083,0.4557]`。恢复函数却以 100 mm 构造环境权限，错误撤销了已有的实测接触许可。改为 FK 实际间距后，同一保存地图上的完整相机恢复候选通过；原 100 mm 守卫仍拒绝磁吸末端。这只是非执行候选诊断。

全量回归 `logs/pytest_full_mission_18.xml` 为 313 项通过。闭环复测 `rgbd_pilot_repair_02_6103_set` 已越过原失败点，但捕获后的源目标略越出操作视域，交接恢复未处理此条件，因此停止；原始记录和安全审计保留。前一 `_01` 启动因手工录入 calibration ID 错误被完整性门禁拒绝，未执行运动。

补齐交接时的源视域恢复及见证目标还原后，`rgbd_pilot_repair_03_6103_set` 在原场景和原扫描上完整通过两个物体的独立全任务审计：123 段、9,196 个反作用样本，最大分量复核误差 `8.693e-7` SI，放置水平误差 1.564/1.946 mm，支撑间距 1.141/1.136 mm，两次实际撤离均约 80 mm。运动 153.169 s，包含启动的墙钟 530.302 s，流式 P99 4.504 ms；源码一致性通过。这是重复使用开发场景的修复验证，不是未见场景，也不能与旧版基线拼成新配对。

## 继续闭环暴露的几何近似问题

### 6101 释放后撤离：变化支撑 AABB 误撤销角部空域

`rgbd_pilot_repair_04_6101_set` 实际执行了 +15° 偏航并完成第一个物体释放，但尚未撤离：51 段/3,991 样本的独立几何与反作用审计通过，整场失败为 `transfer/retreat/no_measured_safe_IK`。阻挡磁吸的细图单元 `(65,115,83)` 位于释放包络 AABB 内，却与测量凸包整体分离至少约 2.117 mm。保存地图中其自由票为 0；原初始扫描独立重放表明原本有 26 票。

新形状相关变化更新用测量凸包面分离完整体素，保留 AABB 角部原自由证据，仍撤销真实可能相交单元及非正 TSDF 矛盾单元。`logs/rgbd_6101_change_support_probe_01.log` 显示诊断单元新更新前后均为 26 票；没有恢复失败运行的地图、没有增加票、没有改变接触范围。源码修订同时应用于捕获与释放。

`_05_6101_set` 和 `_05_6102_set` 均在捕获后的静止更新阶段遇到 Float/Double 类型不一致，未进入持物运动；两者已执行路径审计通过、整场失败。此前 326 项测试未覆盖 NumPy 双精度 ROI 边界。新增回归先复现失败（`pytest_change_dtype_before.log`），统一双精度几何支撑运算后 CPU/CUDA 测试和全量 `pytest_full_mission_23.xml` 328 项通过。原失败、原源码快照与部分更新证据全部保留，不将数值实现错误混同于物理不可行。

### 6101 第二物体：持物球包络误报自碰撞

`rgbd_pilot_repair_06_6101_set` 第一个 washer 已完成实际释放和 80 mm 撤离，水平放置误差 1.736 mm；第二个 bracket 已捕获，但停于抬升前的视域恢复。6,840 个反作用样本及已执行连续几何通过，最大 wrench 复核误差 `8.044e-7` SI，整场仍失败。

诊断发现，静止右臂与所持测量物体的球形粗包络已经触发 `payload_self`，所以无论左臂如何恢复都被拒绝。球半径 84.367 mm，疑似连杆 a7；测量凸包与机器人 a7 凸包在原双侧共 6 mm 裕量下有可验证分离轴。修订使球只作为 broad phase，随后用原凸包分离工具和原相对关节运动界认证，不删除自碰撞、不减小裕量。回归先复现误报，再检验真正重叠和运动裕量仍拒绝；全量 `pytest_full_mission_24.xml` 331 项通过。

### 6102 撤离：体素跨越接触边界，不等于工具越界

`rgbd_pilot_repair_06_6102_set` 已运输、释放唯一物体，但未知细体素阻止撤离。4,093 个 wrench 样本和已执行连续几何通过，最大复核误差 `7.453e-7` SI；未撤离故整场失败。

三个残余格分别为 `(50,82,80)`、`(51,78,80)`、`(52,75,80)`，确与释放测量包络相交，不能直接恢复自由票；与工具整体的距离下界也只有约 4.20/4.26/4.78 mm，不能降低原 5 mm 门槛放行。进一步按原允许接触脚印切分这些格后，工具与禁止部分的可验证分离为 5.632/5.682/6.702 mm。因此可以保留整格 UNKNOWN，只认证工具可能接触的部分属于原接触许可，详见条件性质 2.1。

`logs/rgbd_6102_release_boundary_probe_01.log` 保存非执行诊断：同一失败地图上的完整静止 broad/fine 环境查询通过，未修改地图或执行运动。新增细化只适用于 magnet、最多 32 个残余格，接触高度/半径仍检查整格，原运动余项和裕量全部保留。全量 `pytest_full_mission_25.xml` 338 项通过。`rgbd_pilot_repair_07_6101_set` 和 `_07_6102_set` 正从原始场景与扫描顺序闭环复测，不能由静止诊断提前认定完整成功。

`_06` 两次运行之间曾插入短时只读诊断，不用其计算耗时作为无竞争正式计时。`_07` 冻结期间不改运行源码、不并行运行其他数值实验。

### 07 复测：相机自身遮挡与全标记丢失

两个 07 试验均已结束、源码一致；6101 完成首个物体，第二个运输时在相机单臂补观测后丢失全部任务标记，停止于 146.503 s（106 段/8,814 tick）；6102 在放置时丢失当前目标，停止于 84.093 s（59 段/5,056 tick）。已执行连续几何与 wrench 审计通过，整场均失败。每个运行目录的 `visual_loss_inspection.png` 展示原始补观测前后 RGB-D，观察到机械臂自身遮挡明显增加；6102 的其他标记仍有新观测，不应误称其全部标记丢失。

新实现允许对刚执行的相机单臂补观测做最多两次有界回退：右臂不动，当前点须位于原关节线段，重新认证当前 RGB-D 图及持物/机器人整段几何，再返回该相机段起点。获取过程中不要求已丢失任务标记可见，但保留原始 RGB-D 帧年龄监测与传感器异常制动；右臂仍必须等真实新任务测量。不能反转普通搬运段或反转回退段，不能刷新旧时间戳。条件性质 3.1 明确了暂时不满足任务视觉集合的范围。

保存地图非执行诊断 `rgbd_6101_view_rollback_probe_01.log` 与 `rgbd_6102_view_rollback_probe_01.log` 均找到当前版本认证的回退候选；执行被记录替身代替，不是实际恢复。新增 12 项专项测试后，全量 `pytest_full_mission_26.xml` 为 350 项通过。随后两个 08 闭环复测均已结束，结果如下。

### 08 复测：6102 完整通过；6101 放置被宽图表面否决

`rgbd_pilot_repair_08_6102_set` 完成实际唯一物体的释放与撤离：57 段、4,407 个 wrench 样本、73.304 s 运动时间。实际水平误差 1.555 mm、支撑间隙 1.112 mm、撤离 80 mm；44 个真实物体凸包顶点均被测量包络包含；几何及反作用复核通过，最大分量误差 `7.932e-7` SI，源码未变化。力 RMS/峰值 0.09123/0.22296 N，力矩 0.04587/0.11179 N·m；流式 P99 4.455 ms，但含启动墙钟 272.625 s。该次没有触发相机回退，因此不能据此宣称回退策略有效。

`_08_6101_set` 在首个物体放置时停止，49 段、3,918 个样本、65.181 s；已执行几何与反作用复核通过，最大分量误差 `7.254e-7` SI，整场失败。该次同样未触发回退；不同异步观测引起的轨迹差异不归因于未执行的新功能。

保存地图显示当前静止状态安全，但 3 mm 下放候选在宽图右臂 a4 附近被单元 `(87,34,64)` 拒绝。该格 TSDF `+0.023962829 m`、权重 32、10 个独立自由视角，同时保留 possible-surface veto。原宽图使用永久表面否决，而细图已经采用显式静态区间条件查询。新版本将同一条件查询扩展至宽图；必须仍有静态假设、至少两个完整自由观测且没有非正 TSDF 实占据，不修改地图或投票。配置记录 `static_interval_scope=broad_and_fine`，旧版缺此字段只表示 fine-only，不能混淆。

`rgbd_6101_08_place_static_probe_01.log` 在同一保存地图上取得完整 IK/机器人/持物/视觉候选，未执行。独立初始 commissioning 地图审计中，18,333 个真实相交的 10 mm 球探针误放行 0，50,000 个真实物体表面样本落入认证空闲为 0；这是有限采样检验而非普遍安全证明。持物途中必须区分机器人拥有的载荷和环境，否则会把单独几何检查的载荷误当成环境漏洞。

### 解码缓存与第二轮冻结配对

认证解码缓存最多保留 2 GiB 深度/内参/变换，只缓存验证后的原始测量，不缓存可直接授权运动的自由图。每次重放仍逐文件验证 SHA256，地图得到独立工作副本；超预算回退认证流式实现。实际 732 帧、9,203,205 个细图单元微基准中，冷缓存 23.213 s、暖缓存 14.793 s、原认证流式 31.416 s；所有地图字段 SHA256 一致。暖缓存相对流式为 2.124 倍，但只是一组有序重放，不是完整任务实时性结果。

全量 `pytest_full_mission_29.xml` 360 项通过。`rgbd_paired_mission_pilot_02` 冻结当前代码、复用原 6101/6102/6103 场景与原始扫描，按轮换顺序运行 set/pose/hold 共 9 次。运行期间不修改运行源码或并行启动数值性能试验；最终统计仅在 9 次审计全部结束且哈希未变后生成。未执行的新保留集是 82001–82024，不能把当前复用数据称为泛化结果。

## 统计与权限边界

- 失败行不从分母删除，也不按零反作用计入完整任务成本。
- 不能把名义三物体成功推广为随机可靠；也不能从有限开发失败推断所有可达场景不可行。
- 明确区分偏航非执行候选、已实现修订、单元验证与完整闭环效果。新源码快照与独立审计不可省略。
- 不将这些软件与仿真缺项归入实物验证。

## 从论文正文归档的早期里程碑详情

以下为开发历史，不与当前冻结实验合并统计；原始运行文件未删除。

### Verified near-contact milestone

The separately retained `reports/rgbd_registered_capture_01` run used the 516-frame initial recording, an 8 mm broad map, a raw-replayed 2 mm local map, static-interval fusion, and an explicit measured-interface contact permission. Nineteen segments executed in 22.8 simulated seconds; both arms moved during 1,136 of 1,368 ticks. Attachment was confirmed by binary simulator actuator feedback after a currently certified C2 rest endpoint. Independent truth audit measured a 7.002 mm gap and 1.481 mm target/standoff error; all nineteen geometric segments passed. Every actual logged midpoint wrench was independently checked, with maximum component discrepancy 6.389e-7 in its corresponding SI unit. Force RMS/peak were 0.09355/0.17223 N; torque RMS/peak were 0.04766/0.08644 Nm. These are absolute nominal values, not a matched-policy improvement claim.

The acquisition-to-control observation age P95 was 0.1167 s. Streaming compute P99/max were 6.032/6.984 ms with zero 60 Hz deadline misses in this run. Local raw replay required 17.82 wall-clock seconds at rest, and planning remained a separate endpoint cost. Thus the experiment does not establish whole-stack real-time execution.

Terminal visibility was improved by a registered-RGB task-interface contract: known robot/tool/C materials must be distinguishable from docking colors; RGB-depth registration and circle fitting must pass. Only guidance can recover those near-hardware colored returns. Environment integration retains the original conservative self mask. Lighting, color changes, reflections, and registration errors can invalidate the contract; this is not a general semantic segmentation result.

An earlier retained capture (`reports/rgbd_interval_contact_capture_03`) required a visual-loss brake followed by a zero-motion binary latch attempt. Its last target age was 0.4167 s and was not relabelled fresh. In that run's local map, surface-distance P95 was 0.924 mm and 92.706% of 15,931 dense true part-surface samples lay within 5 mm of the reconstruction; none lay in certified-free cells. These precision, coverage, and unsafe-free denominators are different, and this local result must not be substituted for whole-workspace accuracy or assigned to a different run.

### Independently audited single-object release

The later development run `reports/rgbd_measured_transfer_09` completed a measured single-object transfer and release in 79.576 simulated seconds. All 53 segments passed independent continuous geometry checks, including 34 carrying segments. The measured envelope contained all 44 vertices of the actual payload collision convex hull. Horizontal placement error was 0.967 mm; the physical collision-solid bottom was 1.081 mm above the placement support. A binary release event and terminal C2 rest were both recorded. All 4,780 logged wrench samples were checked against the Cartesian reference, with maximum component discrepancy 8.809e-7 SI units. These checks use simulation collision solids and do not establish accuracy relative to physical metrology.

Both arms moved during 3,783 ticks. Two certified left-only coverage motions recovered otherwise insufficient measured free space. Force RMS/peak were 0.07747/0.23189 N; torque RMS/peak were 0.03697/0.10086 N·m. Maximum 0.25 s force/torque window impulses were 0.05411 N·s and 0.02347 N·m·s. These are unpaired nominal values, not percentage improvements. Streaming P99/max were 4.328/5.660 ms with no observed streaming deadline miss; image age P95 was 0.0667 s. Local raw replay took 22.872 wall seconds and the largest endpoint planning pause was 5.756 s. Reporting only streaming latency would therefore materially misrepresent system timing.

The run does not contain a post-release withdrawal. Earlier transfer runs are retained as failures: insufficient initial free-view support, conservative source-region revocation, and placement-marker occlusion caused certified stops. In particular, run 08 completed transport but braked during placement when its tracked marker became stale; it was not relabelled a success. The revised marker tracker obtains new center measurements only when current depth-supported white edges span both dimensions of the known marking, and estimates support from visible neighboring measured plane samples. Historical tracking regions aid association but do not refresh timestamps on empty images.

An additive decomposition of this same executed trajectory gives left/right force RMS of 0.03335/0.08711 N and combined RMS of 0.07747 N. Corresponding torque RMS values are 0.01385/0.04089/0.03697 N·m. Left and right contributions oppose in direction during 79.48% of jointly non-negligible force time and 88.37% of jointly non-negligible torque time. The decomposition sums to the logged wrench within 5.115e-8 SI units. It demonstrates partial compensation along the recorded motion, not improvement over a separately executable baseline: suppressing one arm's contribution while retaining its time-varying poses is not a feasible alternative trajectory. Peak combined force (0.23189 N) slightly exceeds peak right contribution (0.23148 N), further precluding an all-metrics reduction claim. Data and denominator definitions are retained in `reaction_sharing.json` and `reaction_sharing.npz`.

![Nominal measured single-object transfer: RGB-D, executed joint geometry, combined reaction, waypoint distance, and target freshness. Gray geometry is the initial measured surface; cyan is current robot-filtered depth, not a fabricated complete map.](../../reports/rgbd_measured_transfer_09/rgbd_guidance_summary.png)

### Retained-surface comparison at one completed-object state

The stopped three-object development run `reports/rgbd_nominal_multi_mission_02` completed the first object's release and withdrawal. An offline comparison at that exact final scene state uses the same 8 mm workspace map, the same 50,000 area-weighted true part-surface samples, and the same free-space grid. Adding the retained measured first-object surface increased aggregate three-object coverage within 5 mm from 62.424% to 73.982%, an 11.558 percentage-point gain; coverage-distance P95 changed from 19.515 to 14.885 mm. The measured-vertex precision P95 was 7.189 mm without and 6.920 mm with the retained layer. These are whole-map results, not the earlier 2 mm local-map precision. Neither variant classified a sampled true part surface as free or admitted any of 18,355 known-colliding sphere probes. Finite probes do not establish universal safety. This is one paired representation audit, not a multi-scene mapping comparison, a watertightness result, or permission to replace unknown space with free space.

### Complete-cycle matched nominal comparison

`reports/rgbd_measured_transfer_15_pose` completed release and withdrawal with the same code snapshot, scene, 732-frame initial recording, initial joints, task request, and common duration constraints as run 15. Online views, IK solutions, and achieved durations differ as policy outcomes; this is not identical-image replay. Both runs passed independent full-path geometry and wrench checks. The baseline imposes differential camera-pose preservation, \(J_{cam}\Delta q_L=0\), rather than promising exact nonlinear pose equality after a finite increment.

| Metric | Visual set | Differential pose preservation | Set relative change |
|---|---:|---:|---:|
| Release and withdrawal audit | Pass | Pass | — |
| Executed segments | 56 | 55 | — |
| Force RMS / N | 0.08467 | 0.08355 | +1.34% |
| Force peak / N | 0.20530 | 0.18487 | +11.05% |
| Force accumulated magnitude / N·s | 5.07862 | 4.88086 | +4.05% |
| Maximum 0.25 s force impulse / N·s | 0.04801 | 0.04320 | +11.13% |
| Torque RMS / N·m | 0.03874 | 0.04218 | −8.16% |
| Torque peak / N·m | 0.10013 | 0.11383 | −12.04% |
| Torque accumulated magnitude / N·m·s | 2.28435 | 2.47295 | −7.63% |
| Maximum 0.25 s torque impulse / N·m·s | 0.02342 | 0.02664 | −12.08% |
| Simulated motion time / s | 68.77895 | 67.07884 | +2.53% |
| Horizontal placement error / mm | 0.384 | 1.355 | — |
| Streaming compute P99 / ms | 4.363 | 4.408 | −1.02% |

Using the objective's already implemented scales, the derived time-mean normalized reaction energy is

\[
E=F_{RMS}^2/(0.35\ \mathrm N)^2+\tau_{RMS}^2/(0.15\ \mathrm{N\,m})^2.
\]

It is 0.12523 versus 0.13607 for this pair (−7.97%); the corresponding motion-time integrals are 8.61289 and 9.12761 s. This is a post-hoc derived scalar summary with the existing objective scales, not a preregistered population endpoint or a new experiment. It explains how the optimized weighted tradeoff can improve while individual force metrics worsen. The adverse component results above remain necessary.

This one pair supports a torque-versus-force tradeoff, not uniform six-axis improvement or a statistically significant population effect. The pose run's 4,028 wrench samples agree with the independent reference to 6.467e-7 SI units; all 55 geometry checks pass. Machine-readable matching checks and the signed comparison are in `reports/rgbd_matched_cycle_15.json`. Higher force peaks must not be omitted when presenting the lower torque result.

### Two completed objects within an unfinished three-object mission

The later nominal multi-object run `reports/rgbd_nominal_multi_mission_07` released and withdrew from two objects consecutively without resetting the robot, sensor or broad map. The controller learned their interfaces from RGB-D and received no scene count. Independent audit validated both cycles: horizontal errors were 0.772 mm for the bracket and 1.054 mm for the washer, with support gaps 1.088 and 1.121 mm. Their measured envelopes contained all 44 and 96 actual collision-hull vertices. All 136 segment geometry checks and 10,586 logged reaction samples passed; the maximum reference discrepancy was 6.850e-7 SI units. The third approach stopped at a measured-space IK failure, so this remains an unsuccessful full-scene trial.

The run lasted 176.122 simulated motion seconds, with both arms moving on 8,529 ticks. Force RMS/peak were 0.09224/0.42232 N; torque RMS/peak were 0.04699/0.28823 N m. These unpaired values must not be compared with a shorter single-object run as an algorithmic percentage improvement. Streaming P99 was 4.422 ms, but recorded task wall time including 44.452 s of setup was 643.111 s, before final artifact export. Full observations, map commits, storage, and execution have nested inclusive timing intervals and cannot simply be added. This first whole-task wall measurement makes the current lack of full-stack real time explicit.

A saved-map diagnostic at the failed third approach rejected a 20 mm descent proposal but certified a 10 mm proposal with the same geometry and unknown-space rules. The subsequent implementation therefore uses the existing transport-style 20/10/5 mm backtracking rule in approach too. A saved-map proposal is not evidence that the third object has been delivered; a new closed-loop run and audit are required.

### Remaining scope and interpretation

The random development scene 6100 subsequently completed an observed-interface mission (`reports/rgbd_random_6100_mission_03`). Its actual object count, read only by the independent auditor, was one. The controller received no count, automatically selected the observed colored interface, and completed a 50-segment release-and-withdrawal cycle. Actual horizontal placement error was 1.060 mm and support gap 1.110 mm. All 96 payload convex-hull vertices were contained in the measured envelope, all 50 geometric segments passed, and all 3,853 logged wrench samples passed the separate reference calculation (maximum component discrepancy 6.613e-7 SI units). One certified camera-only handover reacquired the placement marking before transport. This seed had already been used to diagnose handover failure, so its success is a development result, not holdout generalization.

This run used its own 516-frame initial recording. Motion time was 64.178 s, force RMS/peak 0.08875/0.21330 N, torque RMS/peak 0.04114/0.10121 N·m, and streaming P99 4.397 ms. Raw local-map replay took 21.918 wall seconds; maximum endpoint planning took 8.816 s. The prior failed run remains in the development record. The diagnosed defect was unnecessary subdivision of stationary payload checks based on motion of an arm whose geometry was excluded from that fine-map checker. Restricting remainder calculation to included geometry removed redundant queries without increasing the one-million-cell budget or changing measured-space authority. Independent broad-map and inter-arm checks still cover the moving camera arm.

The subsequent `reports/rgbd_measured_transfer_15` establishes the first independently audited nominal release-and-withdrawal cycle. It used 732 raw initial frames and 224 independent view bins, rather than the earlier 516-frame acquisition. All 56 continuous geometry checks passed, including 32 carrying segments and five post-release withdrawal segments. All 44 actual payload convex-hull vertices lay inside the measured envelope. Horizontal placement error was 0.384 mm, support gap 1.081 mm, and terminal velocity and acceleration were zero within the audit tolerance. The 4,130 logged midpoint wrenches agreed with the independent reference to a maximum component discrepancy of 6.663e-7 SI units. This is a single development trial, not a success-rate estimate.

Execution lasted 68.779 simulated seconds; both arms moved on 3,860 ticks. Force RMS/peak were 0.08467/0.20530 N and torque RMS/peak were 0.03874/0.10013 N·m. Maximum 0.25 s window impulses were 0.04801 N·s and 0.02342 N·m·s. Streaming P99 was 4.363 ms, maximum 5.836 ms, and image age P95 0.0667 s. Initial local-map replay took 31.681 wall seconds and the largest endpoint planning cost was 5.832 s. Source hashes were unchanged during this run. Its predecessor, run 14, released accurately but could not withdraw because distance-band invalidation discarded usable air evidence. That failure is retained. Run 15's result supports the revised update in one case, not an isolated causal ablation or an arbitrary dynamic-scene guarantee.


## Additional source-specific development history (archived from main manuscript)

The following chronology is retained for auditability, not as current default-method claims. The current default does not use the destination-dispersion heuristic. No record below is a held-out result.

### 7.5 First frozen randomized development pilot

The prespecified development seeds 6101, 6102 and 6103 were each evaluated with set, differential-pose and hold policies, using one independently recorded 732-frame initial scan per scene. All nine scheduled trials and their independent audits completed with unchanged source hashes. Generation and initialization succeeded for all three scenes; none was filtered by downstream visibility, IK feasibility or task success. Policy order was rotated across scenes. These are development scenes, not unseen holdout data.

| Policy | Independently audited whole-scene successes | 95% Wilson interval |
|---|---:|---:|
| Visual set | 0/3 | [0.000, 0.561] |
| Differential pose | 1/3 | [0.061, 0.792] |
| Hold | 1/3 | [0.061, 0.792] |

Both baseline successes occurred in scene 6103. There are no common successful scenes involving set, so no complete-task reaction-effect estimate is reported. Failed partial trajectories are not assigned zero task cost. The small sample and broad intervals preclude a population ranking, but the result directly rules out claiming that the current implementation has established reliable randomized missions.

The retained records reveal actionable failure modes. Scene 6101 set stopped because local-map ROI selection unnecessarily required a fresh destination marking, even though the markings had previously been observed; pose and hold later stopped at measured-space IK rejection during transport. In the saved hold map, three rejected cells near the final right-arm link each had only one independent free-space observation, below the required two. Scene 6102 set stopped during lift, while pose and hold performed visual-loss braking during transport. Scene 6103 set stopped during camera-only view restoration: its guard used a nominal 100 mm tool standoff although measured FK placed the tool approximately 7 mm from the target, inadvertently revoking the existing bounded contact permission.

Subsequent repairs separate historical ROI proposals from fresh execution authority, use the actual tool standoff during sensing-only restoration, and allow a new full-frame observation after a verified C2 visual stop. A further source-view restoration was required at the capture-to-destination transition. With these repairs, the repeated development scene 6103 (`rgbd_pilot_repair_03_6103_set`) completed both objects: all 123 continuous geometry checks and 9,196 logged wrench checks passed, with maximum component discrepancy 8.693e-7 SI. Horizontal placement errors were 1.564 and 1.946 mm, and both actual withdrawals were approximately 80 mm. Motion lasted 153.169 s and task wall time including setup was 530.302 s. This is closed-loop repair evidence on a reused scene, not holdout success or a matched comparison against older-source baselines. Revised runs are kept under new source snapshots and cannot be pooled into the frozen pilot. Newly rendered stress experiments, successful matched multi-scene comparisons and independent holdout validation remain outstanding.

### 7.6 Subsequent development and exact replay optimization

A later reused single-object scene, `rgbd_pilot_repair_08_6102_set`, completed all 57 segments and 4,407 logged reaction checks. Actual horizontal placement error was 1.555 mm, support gap 1.112 mm and withdrawal 80 mm; all 44 actual payload hull vertices were contained by the measured envelope. Maximum wrench component discrepancy was 7.932e-7 SI. Motion lasted 73.304 s versus 272.625 wall seconds including setup, with streaming P99 4.455 ms. Force RMS/peak were 0.09123/0.22296 N and torque RMS/peak 0.04587/0.11179 N m. No camera rollback was triggered. The companion 6101 trial still stopped during first-object placement after 49 segments, despite passing executed-geometry and reaction checks. Neither result is held-out or paired with older-source baselines.

The latter failure identified an inconsistency between broad persistent-veto queries and fine static-interval queries: a rejected broad cell had ten independent whole-cell free observations and positive TSDF but retained its possible-surface flag. The new revision applies the declared static-interval query to both maps, retaining all evidence and rejection of insufficiently observed or nonpositive-TSDF cells. Run configuration explicitly distinguishes this broad-and-fine scope from earlier fine-only experiments. A saved-state proposal passed the revised full guard. The subsequently frozen comparison reused the original scenes and recordings; its completed results follow.

An authenticated decoded-measurement cache addresses repeated local-map construction without changing the map algorithm. It retains at most 2 GiB of immutable sanitized depth, intrinsics and integration transforms; every replay still validates the original raw-file hashes before map mutation, supplies working copies and falls back to authenticated streaming when over budget. For the same 732 raw frames and 9,203,205-cell fine ROI, cold-cache, warm-cache and streaming replays took 23.213, 14.793 and 31.416 s. All distance, weight, free-view, surface-veto, view-bit and change-epoch arrays were byte-identical. The 2.124-fold warm-versus-streaming ratio is a single ordered microbenchmark subject to cache/warmup effects, not a full-mission speedup or real-time guarantee.


### 7.8 Post-pilot clock validation and remaining failures

The separate `rgbd_clock_support_development_01` batch completed both scheduled set-policy development trials. Both whole-scene tasks failed: 6101 completed one object and passed the second lift but stopped in transport; 6102 stopped in placement. Their 127/52 executed segments and 9,005/4,824 reaction samples passed independent geometry and wrench checks. A separate recorded-clock audit checked all pre-execution refreshes and 3,580 online raw-file hashes and request clocks. Both runs passed; maximum logged normal-motion target ages were 0.249110 and 0.236771 s. Braking ticks are explicitly separated from normal visual permission. These are actual watchdog executions, not newly rendered controlled sensor-fault experiments or full-stack scheduling proofs. The disclosed concurrent diagnostic workload also prevents treating their timings as uncontended benchmarks.

Subsequent bounded-corridor and complete-component refinement changes passed saved-state full proposals for the 6102 placement and 6101 transport failures. The latter diagnostic reconstructs 1,268,064 fine cells from the same 732 raw frames, replays three change events, and proposes motion toward the actual remaining destination, not a zero-motion check at the last height-adjustment endpoint. It is archived in `rgbd_refined_robot_probe_01` and is not an executed trajectory.

The subsequent `rgbd_refinement_development_01` batch completed scene 6102: all 57 geometric segments, 4,415 reaction samples, and the recorded-clock audit passed. Horizontal placement error was 1.312 mm, support gap 1.126 mm, and actual withdrawal 80 mm. Scene 6101 still failed at its second-object approach after completing one object. Neither run constructed a robot refinement block, so the successful task cannot validate that new block. The first run exposed a separate scheduling defect: some pre-execution visual certificates took about 140 ms over 749 intervals, leaving too little image lifetime before the next request, previously submitted only on the fourth motion tick. A later revision limits runtime visual certificates to 127 intervals and primes the next sensor request before motion; complex candidates remain subject to rejection and bounded backtracking. This revision has 404 passing tests and is under separate closed-loop evaluation. No failed task is removed from the development record.

The sensing-pipeline development rerun passed its recorded-clock audit over 84 segments and 6,553 normal ticks, with no visual braking and maximum logged target age 0.170206 s. It nonetheless failed during the second lift after repeated payload-query budget exhaustion and subsequent map expiry. This is not a completed mission. A saved-state ordered microbenchmark of the exact contact-query reordering took 3.846 s before and 0.623 s after reordering; both produced identical accepted endpoints. The enumeration totals over five guard calls were 2,186,805 and 62,478 cells, respectively. The original count includes wholly permitted cells later filtered out. These results are not a full-task speedup or a new contact allowance.

The contact-query rerun also failed in the second lift after completing one object (100 executed segments, 8,071 logged ticks). Its recorded-clock audit passed all 2,217 raw records, with maximum normal-motion target age 0.237864 s; 65 braking ticks were separately classified. A 996,170-cell block for components a6/b6 was actually used by six sensing-motion certificates. This validates execution of that mechanism, not successful remaining manipulation. At the final state, fixed-rotation IK did not reach its geometry callback. A paired saved-state diagnostic found a geometrically certified 3 mm proposal when unused world-vertical yaw was released, with position error 1.85e-8 m and vertical-row error 1.84e-7. This diagnosis does not execute motion or validate live visual feedback. The revised complete-task runs remain separate from all earlier comparisons. Full-project regression now passes 441 tests, including the retained drifting-model tests; the earlier 404 count covered only the main test directory.

### 7.9 Vertical-row development validation

The frozen `rgbd_vertical_row_development_01` batch finished both scheduled set-policy tasks. Scene 6102 completed its one-object cycle: 52 geometry-certified segments, 3,818 reference-checked wrench samples, actual horizontal error 2.092 mm, support gap 1.127 mm and withdrawal 80 mm. The recorded-clock audit passed all 1,067 online raw files; no braking occurred and maximum normal-motion target age was 0.135102 s. Motion time was 63.576 s and recorded task wall time including setup was 199.130 s before final artifact export (206.793 s including the subprocess and export). Force RMS/peak were 0.08351/0.18470 N and torque RMS/peak 0.04359/0.10803 N m. These are single-run values, not a matched speed or reaction improvement.

Scene 6101 completed the first object and progressed through the second lift and transport, but failed during second-object placement. Its 108 geometric segments, 8,531 wrench samples and 2,357 raw-clock checks passed; no visual braking occurred and maximum normal target age was 0.237320 s. The remaining payload sweep still encountered insufficiently observed cells. The proposed task-rank correction therefore repaired the saved IK defect but did not establish multi-object reliability. Later occlusion-aware sensing selection ranks the candidate directions using known right-arm and measured-payload ray intersections; it does not inspect environment truth or grant free-space permission. Its closed-loop result is evaluated separately.


## 6. Experimental protocol and early development evidence

The initial measured-only comparison uses one nominal scene, fourteen noncontact segments, the same initial raw scan, and the same minimum-duration rule. It is exploratory, not a random-scene statistical study.

| Metric | Visual-set policy | Pose-preservation policy |
|---|---:|---:|
| Simulated execution time | 16.8 s | 16.8 s |
| Force RMS | 0.10847 N | 0.08150 N |
| Torque RMS | 0.05122 N·m | 0.05073 N·m |
| Peak torque | 0.08068 N·m | 0.09068 N·m |
| Streaming compute P99 | 4.216 ms | 4.197 ms |
| Final independently measured standoff error | 1.489 mm | 1.360 mm |
| Independent whole-segment geometry checks | 14/14 | 14/14 |

Sources are `reports/rgbd_cooperative_async_chord_set_01` and `reports/rgbd_cooperative_async_chord_pose_01`. The adverse force result is retained. Force/torque RMS, peaks, integrated magnitude, net impulse, and maximum 0.25 s window impulse must all be reported; a favorable torque metric cannot stand in for force performance.

The 150-frame initial scan produced 62.01% certified free volume, 3.51 mm surface-distance P95, and 81.41% complete workspace surface coverage within 5 mm. These have different denominators. Fixed-lattice 420-frame and overhead-augmented 516-frame recordings increased certified free volume to 73.09% and 74.50%, respectively. This is increased acquisition effort, not a cost-free algorithm improvement. Near-contact failures from coverage, conservative geometry, and ambiguous surface intervals are preserved in their original report directories. No complete RGB-D-only magnetic transfer is established by this table.


### Historical transport-only yaw fallback

The completed reference and first frozen pilot preserved the tool rotation measured at capture. The current development revision adds a bounded transport-only yaw fallback: after 20, 10 and 5 mm fixed-attitude steps fail measured-space IK, capture-relative world-Z yaw proposals of +15, -15, +30 and -30 degrees are tested through the unchanged full IK, robot, payload, environment and visual certificates. Horizontal magnetic placement does not prescribe this yaw. At a fixed TCP height, e_z^T R_z(psi)=e_z^T preserves every attached point's height at the exact proposed endpoint. This identity does not certify the intervening joint path, which is checked separately. The chosen orientation is committed only after successful execution, retained during placement/withdrawal and reset for a new captured object. It is not changed on a failed proposal or a partial brake. This new fallback is not retroactively attributed to earlier runs; its closed-loop performance remains under evaluation.


### Envelope and destination-selection development records

On the same saved washer/bracket cores and radii 20.545/19.426 mm, volume ratios relative to historical cube padding were 0.7780/0.8568. Both new envelopes contained the actual simulated object in the independent saved-state audit; their face-wise error-ball support checks passed. This is not a reduction of the sensor-error budget or a demonstrated mission improvement. The completed two-scene development batch retained the one-object success in 6102, but 6101 again stopped during the second placement after delivering the first object. Both recorded-clock audits passed: 110/52 segments, 8,623/3,820 ticks and 2,382/1,067 raw records, respectively. Thus tighter valid envelopes alone did not resolve the multi-object availability failure.

The subsequent destination proposal uses only observed placement marks and measured completed destinations. The first assignment maximizes summed distance to the observed alternatives; later assignments maximize distance to the nearest used destination. Scores within a covariance-derived heuristic tie band prefer a shorter transfer. This avoids occupying a central mark first when comparable measured edge marks exist. It never grants collision/contact permission or refreshes a historical timestamp; ordinary fresh guidance and all swept-path certificates remain mandatory. The three-scene development evaluation is in progress, with no result yet assigned to this revision.

