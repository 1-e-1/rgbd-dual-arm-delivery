# 论文主线与验收计划：面向宇航员辅助双臂的视觉充分性和低基座反作用

2026-09-08；研究工作计划，不是已达到 JCR Q1 或可投稿的声明。期刊分区须在选刊时按当年 JCR 类别核验，不用中科院分区或 SJR 分区代替。

## 不变的任务边界

固定 C 形件上安装两条 8 DOF 机械臂；左臂 D435i 提供环境信息和右臂视觉引导；右臂执行磁吸递送；二者在线协作。按用户要求忽略摄像头、磁吸器和负载惯量，但保留它们的碰撞几何；无重力，执行方式仍为运动学。用实际模块质量和 USD 惯量估计 C 参考原点上的合成六维惯性反作用。

固定 C 只代表接口负载评估模型，不等价于自由漂浮宇航员的完整动力学。无惯性反作用不代表不存在磁吸接触冲击或柔性振动。未测的 COM、惯量、基座柔性和接触不应在论文里被当成已验证条件。

## 核心科学问题

在右臂保持任务精度、视觉满足所需而非最大化视觉质量时，可向反作用补偿释放多少左臂运动能力？这种能力如何受到遮挡、碰撞、关节限幅、测量不确定性和计算预算影响？

候选方法名：Task-Sufficient Visual Redundancy Allocation（TSVRA）。名字是项目内工作名，并非新颖性证据。

候选贡献（均需完成相应验证后才能写成论文贡献）：

1. 从抓取误差预算构造阶段相关视觉充分集合，将不必要的相机位姿等式改为集合约束，量化其释放的反作用补偿能力。
2. 以 C 接口合成六维反作用和短窗冲量为目标，明确局部零反作用的可行条件、受约束时的残余，而不是仅报告净冲量接近零。
3. 将事件触发的小规模优化与可复用安全轨迹分离，给出预算超时/观测不足时的可执行后备行为，并验证精度—反作用—计算开销的权衡。

单独的“宇航员佩戴”“多臂视觉伺服”“动量补偿”“set-based control”“QP”“避碰”均不能作为首次提出的创新点。

## 当前实现到哪一步

追加实现：局部地图事件维护替代全图清除；51 状态批量解析惯性反作用；段端一次有界失视恢复；实测开放曲面的 OBJ/梯度/Hessian 导出。详细公式见 `../COMPLETE_TECHNICAL_ROUTE.md`。
初始新随机 1000–1007 测试为 7/8 完整任务通过，1003 在第三物体失视安全停止。后续恢复修复的同场景回归必须单独列出，不把原 7/8 改写为 8/8，也不把失败场景当作独立测试集。

- 新增 `visual_sufficiency.py`：8 变量 QP；只有新鲜、支持点数/协方差达标且初始几何视角处于充分集合内时才释放相机位姿等式。
- 视觉集合使用视域、距离、表面观察倾角和像素支持预测；边界包含目标半径和 3σ 工程裕量。协方差未标定，因此 3σ **不代表已证明的概率保证**。
- `momentum` 为起点动量代理消融；`wrench` 为实际六维反作用的路径采样 Gauss–Newton 局部模型。
- 完整任务通过 `sufficiency_delivery.py` opt-in 接入。只在未吸附的视觉引导阶段释放；携物阶段暂时保留原控制器。当前阶段区分还不完整。
- 现有连续 CAD 避碰门槛保留；视觉路径检查是 9 个几何采样点，**不是连续可见性认证**。遇到不合格观测回退原视觉策略，不能称为始终满足视觉充分性。
- `sufficiency_benchmark.py` 固定同一初态、右臂曲线、运动时间比较 frozen / pose / set。所有候选、质量不合格和碰撞失败记录保留。
- 未将新方法替换为默认控制入口；原有完整版本和数据保留。

## 最关键的数学边界

令 `a` 为 16 维关节加速度，`b=Adot(q,v)v`。在当前状态，零反作用要求：

\[
A(q)a+b=0.
\]

若任务等式满足 `a=a_p+N_T z`，先忽略不等式和加速度边界，零反作用等价于

\[
-(Aa_p+b)\in\operatorname{Range}(AN_T).
\]

无界线性最小残余为

\[
r_{lb}=\|(I-BB^+)(Aa_p+b)\|,\qquad B=AN_T.
\]

这是标准线性代数关系，不是本项目原创定理。加入视觉/碰撞/限幅后还需满足 `a_p+N_T z ∈ K`。即使 `r_lb=0`、秩足够，也不能保证存在满足全部约束的解。

视觉活跃不等式构成可行方向锥，不能全部当成双边等式去计数自由度。当前记录的 active rank 只是边界切空间诊断。有限路径段的常量关节位移参数化比逐时加速度可行集更受限；即便每个时刻存在反作用抵消方向，也不代表一条关节直线能全程零反作用。

从静止出发，严格 `W_C(t)=0` 对应合成动量沿全过程保持零。整段正负反作用互相抵消而净冲量接近零，不属于“零基座冲击”。主要结果必须分别给出 F/τ 的峰值、RMS、0.25 s 线/角冲量和任务时间。

## 已核对的邻近工作

| 工作 | 已覆盖内容 | 与我们的关系 / 待核查 |
| --- | --- | --- |
| [Reactionless visual servoing of a multi-arm space robot combined with other manipulation tasks, 2017](https://www.sciencedirect.com/science/article/pii/S0921889016301208) | 多臂视觉任务与低基座扰动的任务优先级、二次优化 | 必须直接对照，不能声称首次融合视觉和反作用 |
| [Reactionless camera inspection…, 2016](https://www.sciencedirect.com/science/article/pii/S0094576516301084) | 相机检查、reaction null space、构型和冗余的作用 | 不能把利用相机冗余抵消反作用单独作为创新 |
| [Set-Based Tasks within the Singularity-Robust Multiple Task-Priority IK Framework, 2016](https://www.frontiersin.org/journals/robotics-and-ai/articles/10.3389/frobt.2016.00016/full) | 集合任务、严格任务优先级和理论/实物验证 | 集合代替等式已有基础；新贡献要来自任务视觉误差预算与反作用能力关系 |
| [Occlusion-Free IBVS using Probabilistic Control Barrier Certificates, 2023](https://arxiv.org/abs/2309.03476) | 噪声下视域和遮挡的概率约束，结合 MPC | 不能把图像边界检查写成同等级的遮挡概率保证 |
| [Trajectory planning…minimal base disturbance in obstacle environments, 2024](https://www.sciencedirect.com/science/article/pii/S0273117724004125?dgcid=rss_sd_all) | 任务臂与平衡臂、障碍约束、基座扰动最小化 | 需要对照“纯平衡臂”与“必须维持视觉职责的补偿臂” |
| [Supernumerary Robotic Limbs for Next Generation Space Suit Technology, ICRA 2023](https://ieeexplore.ieee.org/document/10161579/) | 宇航服双辅助臂、扶手支撑、姿态稳定和实尺寸样机 | 宇航员佩戴场景本身不是空白；当前不是同等硬件证据 |

检索还发现 [Base-Reaction-Aware Trajectory Planning and Safe Execution for a Free-Floating Dual-Arm Space Robot](https://www.sciencedirect.com/science/article/pii/S0273117726012639) 的出版方索引结果。正文访问返回 403，作者/日期/完整方法尚未复核；将其列为**必须取得全文的高相关条目**，不据索引推断其未解决的问题。另有 2026 年宇航员 partial-gravity SuperLimbs 设计研究；partial gravity 与本项目轨道微重力不能混称。

这是邻近工作筛查，不是完整 systematic review。所有“现有方法尚未解决”的陈述在全文对比前都保留为研究假设。

## 论文推进顺序与验收门槛

| 阶段 | 工作 | 完成门槛 |
| --- | --- | --- |
| M1 方法可行性（当前） | 集合释放、真实反作用目标、等时独立段消融 | 反力/力矩/冲量分开报告；不隐去负结果；完整任务至少跑通并审计 |
| M2 视觉—反作用理论 | 抓取误差预算、受约束可行域/残余、阶段切换 | 明确假设；数值反例与推导一致；禁止把 rank 当全局可行证明 |
| M3 安全和预算 | 遮挡后备、有限时间求解、复用证书、完整双臂任务 | 感知/规划/执行分别计时；超时也计失败；安全停车连续可执行 |
| M4 冻结方法后的评估 | 独立随机场景、消融、敏感性和泛化 | 至少 50 个新场景种子；配对比较、置信区间、失败分类；不能把同场景多段当独立场景 |
| M5 外部有效性 | D435i 标定、COM/惯量误差敏感性、六维力传感器台架（条件允许） | 实际传感/质量误差验证；若只仿真，明确降低结论范围，不能伪称在轨验证 |
| M6 成稿与选刊 | 冻结数据、完整论文与补充材料、JCR 类别核验 | 结论逐项对应证据；作者/基金/伦理/硬件信息由实际事实填写 |

M4 预留新种子 1000–1049；本轮是探索性 pilot，不称预注册结果。现有随机生成器仅三个已知标准接口物体，数量/未知身份/任意姿态泛化还没有实现。新增压力测试建议在实现后固定：观测丢失、视线遮挡、台面高度、接近狭窄通道、惯量偏差和计算预算。范围应由可达性/传感器测量支持，不通过挑成功样本调整。

公平基线：原固定视觉任务优先级；相机完整位姿零空间；集合 + 动量代理；集合 + 路径反作用；去掉视觉门控（仅离线诊断）；无约束零反作用下界（不可执行上限）。同时间段消融和自然时间完整任务结果分开，不能交叉比较不同可行子集的均值。

## 命令

```powershell
$PY = 'D:\conda_envs\env_isaaclab\python.exe'
& $PY -B -m astronaut_delivery.sufficiency_benchmark --output reports/new_matched --objective wrench
& $PY -B -m astronaut_delivery.sufficiency_delivery --output reports/new_full --allocation set --objective wrench --telemetry
& $PY -B -m astronaut_delivery.cooperative_audit --run reports/new_full
& $PY -B -m astronaut_delivery.dashboard --run reports/new_full --frames 120
```

可选限预算分支：在完整任务命令中增加 `--anytime-feasible`。当前实现对 set 模式的候选做线性可行方向收缩，并独立检查代价和非线性采样约束，不因优化器未收敛就自动放宽限制。`sufficiency_wrench_ray_01` 为该选项本轮最新实验；严格收敛版本仍可通过不传此选项复现。已生成的动图来自 `sufficiency_wrench_01` 的严格收敛版本，不与后续限预算轨迹混称。

本轮实际结果及论文章节草案见同目录 `PAPER_DRAFT.md`。当前不代表论文研究已完成，更不代表已被 Q1 期刊接收。
