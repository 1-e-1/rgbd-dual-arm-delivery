# 完整技术路线与数学运算：视觉充分性驱动的低反作用双臂协作

后续约束更新：环境避碰必须完全来自 RGB-D，允许有原始扫描记录的初始地图。本文中的完整递送与环境 CAD 检查属于历史集成路线；当前独立技术路线及全部计算见 [RGB-D 方法与数学](research/RGBD_METHOD_AND_PROTOCOL.md)，证明见 [条件性数学性质](research/RGBD_CONDITIONAL_PROPERTIES.md)，结果与尚缺实验见 [验收矩阵](research/NONHARDWARE_ACCEPTANCE.md)。纯 RGB-D 已有名义三物体完整任务和开发随机双物体成功，但随机可靠性与全栈时序尚未验收，不能把历史 CAD 结果补入新路线。

本文对应工程当前实现，不把设想写成已完成的功能。数值验收另见 `LOCAL_UPDATE_DELIVERY.md` 与各运行目录的 JSON；本文件单独说明输入输出、计算、伪代码和边界。

## 1. 研究对象、坐标与适用范围

宇航员和 C 形件固定，左右各 8 DOF。左臂携带 D435i，右臂磁吸递送。重力为零，执行位置参考而不是动力学力矩控制；同时根据刚体质量/惯量估算固定 C 接口的惯性反作用。固定基座模型不等价于自由漂浮人体的动力学。

用 W 表示世界坐标，C 表示 C 形件参考系，L/R 表示臂，K 表示 OpenGL 相机系。齐次变换采用列向量约定，`motion.py` 显式转换 USD 的行向量矩阵。

\[
q=[q_L^T,q_R^T]^T\in\mathbb R^{16},\quad v=\dot q,\quad a=\ddot q,
\qquad {}^WT_C=\begin{bmatrix}R_C&o_C\\0&1\end{bmatrix}.
\]

摄像头、安装适配器、磁吸器、负载的惯量按研究约定排除，但它们的碰撞几何不排除。每个模块质量 0.25042 kg，每臂 16 个质量刚体合计 4.00672 kg，包含固定安装模块。COM、主惯量和惯量主轴来自 USD，尚未独立实测。

当前任务先验：三个已知颜色的标准水平圆形磁吸接口，半径 20 mm；物体初始位置由图像估计，目标放置位置由任务给定。随机实验改变本体形状、尺寸、yaw、位置和桌面高度，但不等于任意数量、无标记、任意朝向接口的识别。

## 2. 系统结构及输入输出

```text
D435i RGB-D + 曝光时刻 + 标定外参
                 │
       自身/携带物测量点屏蔽
                 ├── 单一 TSDF ── 局部变更维护 ── 实测曲面/偏导数
                 └── 对接面中心、协方差、点数、年龄
                                      │
任务阶段/右臂下一目标 ── 视觉充分集合 + 左臂 8 变量反作用 QP
                                      │
                         两臂联合几何认证 + 五次时间律
                                      │
                          60 Hz 运动学执行 / 同路径停车
                                      │
             FK、吸附/释放事件、下一帧观测反馈到上游
```

感知进程只接收 RGB、轴向深度、内参、相机外参、曝光时间、机器人自遮罩与动作状态。为了渲染而使用的场景真值位姿不输入目标估计器。**碰撞检查仍使用已知 USD/CAD 环境；不是仅凭地图完成未知世界避障。**

本支路没有神经网络训练：Observation 如上；State 包含 q/v/a、动作阶段、地图事件版本、测量及其时间戳；Action 是左右臂同步参考轨迹；Reward、训练 loss、PPO、world model 均不适用。旧 CFM/Drifting 研究代码保留，但不在本协作控制闭环中。

## 3. 运动学与 Jacobian

USD 关节树每条边包含父/子安装变换 \(F_{0j},F_{1j}\)、局部转轴 \(e_j\)：

\[
T_j(q)=T_{p(j)}F_{0j}\operatorname{Rot}(e_j,q_j)F_{1j}.
\]

固定连接省略旋转项。世界中的关节轴与原点为 \(z_j=R_{pF_0}e_j,o_j=p_{pF_0}\)。某刚体上的点 \(p\) 对上游转动关节满足

\[
J_{v,j}=z_j\times(p-o_j),\qquad J_{\omega,j}=z_j.
\]

不在祖先链中的列为零。左右臂分别从各自 USD 基座读取安装坐标，不通过把反射矩阵直接当作旋转矩阵来“镜像 FK”。

## 4. D435i 与点云反投影

标称深度分辨率 848×480、HFOV/VFOV=87°/58°；RGB 1280×720、69°/42°。这些是本工程近似模型，不是某台实物的逐机标定。深度有效范围 0.3–2 m，目标工作距离 0.45–1.2 m。运动时 ROI 保留原始焦距，仅改变主点；不是把低分辨率图像放大伪装成原生细节。

\[
f_x=\frac{w/2}{\tan(\theta_x/2)},\quad
f_y=\frac{h/2}{\tan(\theta_y/2)},\quad
p_K=D(u,v)\begin{bmatrix}(u-c_x)/f_x\\-(v-c_y)/f_y\\-1\end{bmatrix},
\quad p_W=R_Kp_K+t_K.
\]

深度是轴向 z，不是射线欧氏距离。RGB 与深度分别投影，颜色的视场较小；缺失颜色不等于缺失深度。射线命中用真实 USD 三角网格 + Warp 模拟，不是 RTX 渲染器或物理双目匹配器。

双目几何 \(z=f_xb/d_{disp}\) 给出距离相关噪声尺度

\[
\sigma_z\approx\frac{z^2}{f_xb}\sigma_{disp},\quad b=0.05\ \mathrm m.
\]

工程配置使用 0.04 pixel 视差噪声、0.5 mm 底噪及 0.5% dropout 等压力模型，不可转述成厂家精度承诺。

机器人自身用 CAD/FK 自遮罩排除。携物时，除已知颜色外，再排除以右 TCP 为中心、半径 0.11 m 的已测点：

\[
\|p_W-p_{TCP,R}(q_R)\|\le0.11\ \Rightarrow\ D(u,v)\text{ 本次不参与静态建图}.
\]

该半径依赖已知小零件尺寸上界，解决灰色本体未被颜色遮罩覆盖的问题；不会把被遮挡的背景当作自由空间。

## 5. 图像目标中心与不确定性

按颜色归一化匹配连通域，反投影得到点集，保留最高带内的接口点。可见点的中位数在单侧遮挡下有偏，因此使用标准圆弧先验：

\[
\hat c_{xy}=\arg\min_c\sum_{i\in\mathcal I}
\rho\big(\|p_{i,xy}-c\|-r\big),\qquad r=0.02\ \mathrm m.
\]

先在凸边界上用点对产生固定半径圆心候选，筛除遮挡直边、支持不足与大量点落在圆外的候选，再用 `soft_l1` 最小二乘细化。z 使用顶带中位数；不读取物体真值中心。

协方差工程估计：

\[
\Sigma=\operatorname{Cov}(P_{top})/N+(0.002)^2I+\operatorname{median}(\sigma_z^2)I.
\]

小幅连续测量使用 \(\hat p_k=0.25p_k+0.75\hat p_{k-1}\)。新候选点数低于旧值 40% 或跳变超过 6 mm 时拒绝；旧目标过期后需要三帧相互接近（3 mm）的候选才能重捕获。误差相关性和遮挡偏差未完全进入协方差，不能把这里的 3σ 当作概率安全证书。

## 6. 单一 TSDF 融合

协作支路地图体素为 12 mm、截断距离 \(\mu=36\) mm；独立非神经网络建图支路有 4 mm 配置，不能混报。

体素中心 x 投影到相机后，测量残差为

\[
\eta_k(x)=D_k(\pi(x))-z_k(x),\qquad
\phi_k^{obs}(x)=\operatorname{clip}(\eta_k(x),-\mu,\mu).
\]

只更新有效观测且 \(\eta\ge-\mu\) 的位置；深度边缘避免跨物体双线性混合。已有两次以上权重时，鲁棒系数近似为

\[
w_{rob}=\min\left(1,\frac{\delta}{|\phi^{obs}-\phi|}\right),\quad
\Delta w=\min(w_{max}-w,w_{measurement}w_{rob}),
\]
\[
\phi^+=\frac{w\phi+\Delta w\phi^{obs}}{w+\Delta w},\qquad w^+=w+\Delta w,\quad w_{max}=32.
\]

\(\delta=6\) mm。表面只从两端均有观测支持的异号边插值得到：

\[
\alpha=\frac{\phi_a}{\phi_a-\phi_b},\qquad p=(1-\alpha)x_a+\alpha x_b.
\]

正 TSDF 不自动等价于可规划自由空间；权重为零始终表示未知。

## 7. 为什么原来清图，现在如何更新

原实现假设静态环境，有两项保守机制：融合权重到 32 后不再接受增量；某体素曾收到近表面证据后，`surface_veto` 持续有效。这能防止噪声把障碍“平均掉”，却不能直接处理物体搬移。

如果仅继续平均，物体原位置可能留下占据；新放置位置可能保留以前的空闲票。因此原版在吸附/释放后全图 reset。这是保守但浪费信息的静态地图变更处理，并不是“地图必须删除”或“更新算法做不到”。

新实现采用动作事件局部维护：吸附时围绕最后视觉测到的源位置，释放时围绕任务给定的目的位置，构造半边长 0.14 m 的区域 \(\Omega_e\)。这是 80 mm 物体包络 + 36 mm 截断带 + 24 mm 姿态/误差余量的任务级上界，不是每个新物体都天然满足的通用常数。

对体积与区域相交的体素执行

\[
(\phi,w,n_{free},b_{surface})^+(x)=
\begin{cases}
(+\mu,0,0,0),&V_x\cap\Omega_e\ne\varnothing,\\
(\phi,w,n_{free},b_{surface})(x),&\text{其余位置}.
\end{cases}
\]

重点是“变成未知”，不是“标为空闲”。下一帧仍融合到同一张地图中，未变区域逐元素保留。释放事件在撤离动作前生效，避免新放置区域继续沿用过时空闲证据。

独立视角需平移差 ≥4 cm 或转角差 ≥5°；A→B→A 不得重复加票。局部清除后新增体素变更 epoch 与视角已消费 epoch，仅允许该视角为后来失效的体素重新投票，不为未变体素增加票数。ROI 重访保守地可能少计票，但不故意多计独立证据。

异步输入携带累计事件序列；即使丢掉中间图像，也按连续 epoch 重放所有地图变更。事件历史缺失才安全回退全图 reset。迟到的旧 epoch 视觉结果不进入当前控制状态。

这不是一般动态 SLAM：未由动作事件解释的外来物体运动、落物、基座/相机标定变化仍需额外变更检测。尺寸超出包络时必须扩大区域或重新初始化，不能静默沿用当前保证。

## 8. 复杂曲面、偏导数与曲率

`ActiveTSDF.mesh()` 使用同一 TSDF 的 Surface Nets：只有八个角均有观测的异号体素单元产生顶点，相邻有效单元连接成三角网格。保留凹陷和非凸轮廓，不使用一个大包围盒代替物体，不人为闭合未知背面。

沿轴 i 的一、二阶偏导和混合项：

\[
g_i=\frac{\phi(x+h_ie_i)-\phi(x-h_ie_i)}{2h_i},\qquad
H_{ii}=\frac{\phi(x+h_ie_i)-2\phi(x)+\phi(x-h_ie_i)}{h_i^2},
\]
\[
H_{ij}=\frac{\phi(x+h_ie_i+h_je_j)-\phi(x+h_ie_i-h_je_j)-\phi(x-h_ie_i+h_je_j)+\phi(x-h_ie_i-h_je_j)}{4h_ih_j}.
\]

任一差分模板涉及未观测体素，导数无效，导出为 NaN 而不是猜测。有效处

\[
n=g/\|g\|,\qquad
\mathcal H=\frac{\operatorname{tr}(H)-n^THn}{2\|g\|}.
\]

这是等值面的有符号平均曲率；单位分别为梯度无量纲、Hessian/曲率 1/m。工程不把 Hessian 直接当作物体边界：边界由 \(\phi=0\) 决定，偏导描述其法向和弯曲。

新运行结束导出 `measured_surface.obj`、`measured_surface.npz`（顶点/三角形/梯度/Hessian/法向/曲率/有效标记）和 `planning_map_state.npz`。这是实测开放曲面，不是经过认证的闭合碰撞实体，12 mm 栅格也不是毫米级全环境精度证明。

## 9. 保守空闲空间

至少两个独立视角、完整邻域深度在体素及误差上界之后、无表面 veto，才形成空闲证据。对体素半对角 r、外参平移误差 \(e_t\)、旋转误差 \(e_R\)、轴向深度 z 和射线系数 l：

\[
d=r+e_t+2zl\sin(e_R/2),\qquad guard=d+e_z.
\]

完整像素邻域的最小深度需超过 \(z+guard\)，且投影不确定范围落在该邻域内。未知、地图外、过期或版本不一致查询不提供正间隙。

在已认证空闲体素集合上对“不安全体素”做 EDT：

\[
c(p)=\max(0,EDT(x_{cell})-r_{cell}-\|p-x_{cell}\|-m).
\]

它与 TSDF 的截断值不同。地图模块有扫掠球接口，但当前递送任务的主碰撞认证仍来自下一节的已知 CAD。

## 10. 视觉充分集合而非固定相机位姿

令目标在相机系内为 \(p=(x,y,-z)\)，距离 \(d=\|p_W-t_K\|\)，观察仰角代理 \(e=(t_{K,z}-p_{W,z})/d\)，特征为

\[
f=[x/z,-y/z,d,e]^T.
\]

新鲜度 ≤0.25 s、支持点数 ≥12、\(\sigma=\sqrt{\lambda_{max}(\Sigma)}\le5\) mm 且当前视角可行，才启用集合自由度分配。

\[
r_{img}=\frac{0.02+3\sigma}{z},\quad
d_{max}=\min(1.2,d_0\sqrt{0.005/\max(\sigma,10^{-8})}),
\]
\[
\hat N=N_0(d_0/d)^2e/e_0.
\]

不等式 \(g(q_L)\ge0\) 包含

\[
0.60-r_{img}\pm f_1,\quad0.33-r_{img}\pm f_2,\quad
d-0.45,\quad d_{max}-d,\quad e-0.30,\quad(\hat N-12)/\max(N_0,12).
\]

这些是局部几何/测量质量代理，不是连续遮挡证书。携物阶段接口可能被磁吸器遮住，当前使用旧相机策略，不虚构持续可见测量。

相机系目标坐标的 Jacobian：

\[
J_p=-R_K^TJ_v+R_K^T[p_W-t_K]_\times J_\omega,
\quad J_{uv}=\begin{bmatrix}1/z&0&x/z^2\\0&-1/z&-y/z^2\end{bmatrix}J_p.
\]

距离、仰角导数由链式法则计算，已对照中心差分测试。集合约束 G 当前用 8 个方向的中心差分构建，\(h=10^{-5}\) rad。

## 11. C 接口动量与合成反作用

刚体 i 世界 COM 为 \(p_i\)，世界惯量 \(I_i=R_iI_i^{body}R_i^T\)。

\[
A_W(q)=\sum_i\begin{bmatrix}m_iJ_{v,i}\\I_iJ_{\omega,i}+[p_i-o_C]_\times m_iJ_{v,i}\end{bmatrix},\quad
A_C=\operatorname{diag}(R_C^T,R_C^T)A_W.
\]
\[
H_C=A_Cv,\qquad W_C=\begin{bmatrix}F_C\\\tau_C\end{bmatrix}=-\frac{dH_C}{dt}=-(A_Ca+\dot A_Cv).
\]

负号表示机械臂向固定安装基座施加的惯性反作用；不是关节驱动力矩。固定连接的刚体用平行轴定理精确聚合：

\[
m=\sum m_i,\quad c=\sum m_ic_i/m,\quad
I=\sum_i[I_i+m_i(\|c_i-c\|^2I_3-(c_i-c)(c_i-c)^T)].
\]

在线记录仍保留原动量矩阵中心差分计算作为一条参考路径；优化器新增批量解析 Newton–Euler 递推，不改变力学目标。

## 12. 批量解析 Newton–Euler

父体状态为旋转 R、位置 p、角速度 \(\omega\)、角加速度 \(\alpha\)、原点加速度 \(a_o\)。对父安装偏移 \(r_0=RF_{0,t}\)：

\[
a_j=a_o+\alpha\times r_0+\omega\times(\omega\times r_0).
\]

世界转轴为 z，关节旋转后

\[
\omega_j=\omega+z\dot q_j,\qquad
\alpha_j=\alpha+z\ddot q_j+(\omega\times z)\dot q_j.
\]

旋转后的子体安装偏移 \(r_1\) 给出

\[
a_{child}=a_j+\alpha_j\times r_1+\omega_j\times(\omega_j\times r_1).
\]

COM 相对原点的世界偏移 \(r_c=R_{child}c\)：

\[
a_{COM}=a_{child}+\alpha_j\times r_c+\omega_j\times(\omega_j\times r_c),
\]
\[
f_i=m_ia_{COM},\quad
\tau_i=I_i\alpha_j+\omega_j\times(I_i\omega_j)+(p_i-o_C)\times f_i.
\]

取各体和的负值并转入 C 系即得到 W。批量维度同时处理 17 个关节扰动 ×3 个路径位置 =51 个状态，避免每个状态多次重建动量矩阵。与原差分公式及逐原始刚体的独立二阶位置差分进行交叉验证。

## 13. 左臂 8 变量 QP

右臂下一关节目标暂定，决策 \(x=\Delta q_L\in\mathbb R^8\)。在单位时长五次曲线的 \(u\in\{0.2113248654,0.5,0.7886751346\}\) 处计算

\[
r(x)=\operatorname{stack}_u\left(D_WW_C(q(u;x),v(u;x),a(u;x))\right),
\quad D_W=\operatorname{diag}(1/0.35,1/0.35,1/0.35,1/0.15,1/0.15,1/0.15).
\]

使用 \(r(x)\approx y+Mx\)，其中 \(y=r(0)\)，M 的列由 ±\(10^{-4}\) rad 中心差分求得。优化为

\[
\min_x\|Mx+y\|^2+10^{-3}\|x\|^2,
\]
\[
-0.12\le x_j\le0.12,\quad-\pi\le q_{L,j}+x_j\le\pi,\quad g_0+Gx\ge0.
\]

`pose` 消融额外施加完整位姿 \(J_{pose}x=0\)；`set` 不施加这一等式。`momentum` 消融改用起点动量代理，不是推荐力学目标；此前它出现了反力变差和任务中止，失败记录保留。

OSQP 上限 400 次迭代、绝对/相对阈值 \(10^{-7}\)。达到迭代上限不等于可以直接执行。若启用 anytime feasible，且零增量可行，对候选沿零增量射线收缩：

\[
\eta=\min\left(1,\min_{(Cx)_i>0}\frac{u_i}{(Cx)_i},\min_{(Cx)_i<0}\frac{l_i}{(Cx)_i}\right),\quad \tilde x=\eta x.
\]

独立检查线性违反量 ≤\(10^{-6}\)、二次模型代价不高于零增量，再以 1、1/2、1/4、1/8 尺度检查 9 个路径点的非线性视觉裕量。失败保持或回退既有视觉策略。该机制保证所检查的局部约束，不保证真实非线性代价每次下降、任意时刻不遮挡、硬墙钟 deadline 或递归可行。

## 14. “零反作用”何时可能

任务等式给出 \(a=a_p+N_Tz\)，零反作用要求

\[
AN_Tz=-(Aa_p+\dot Av).
\]

忽略不等式时，可解等价于右端属于 \(\operatorname{Range}(AN_T)\)。无界线性最小残余

\[
r_{lb}=\|(I-BB^+)(Aa_p+\dot Av)\|,\quad B=AN_T.
\]

这是标准线性代数条件，不是新的原创定理；满秩不代表限幅、避碰和视觉约束下仍有解。满足集合而非唯一位姿通常扩大可行域，但不能据此保证每个闭环场景的反作用更低。当前目标是可行条件下尽量低反作用，不是已经实现严格零反作用。

## 15. 联合避碰、时间律和停止

右臂用已知 CAD 的 IK/RRT 寻找下一段路径，再引入左臂候选。整段两臂联合检查，而不是把另一臂视为永远静止。连续关节盒在中点计算几何间隙，对每个覆盖球/凸体加入关节区间引起的位移上界，失败递归二分，最大深度 12。

保守界的思想：第 j 轴对下游体点的影响不超过 \(|\Delta q_j|(r_j+B_{downstream,j})\)，求和包含非线性交叉位移。用两臂运动界之和膨胀互碰撞检验。认证包含右臂自碰撞、宇航员/C/台面/环境、两臂互碰撞及工具/负载；合法接触仅按当前任务阶段特许。

三维凸体只搜索面法向并不完备：可能存在边—边分离。在线检查还复用此前有效的单位轴，但每次都重新投影全部顶点。离线复核增加独立的凸体最近点问题：\(\min_{x\in A,y\in B}\frac12\|x-y\|^2\)，以 \(n=(x-y)/\|x-y\|\) 提议分离轴。无论求解器是否宣称收敛，都必须直接检查

\[
\max\{\min_{a\in A}n^Ta-\max_{b\in B}n^Tb,\ \min_{b\in B}n^Tb-\max_{a\in A}n^Ta\}>m_{required}.
\]

单位投影间隙是欧氏间隙的下界，因此这一补充扩大的是能够找到证据的情形，不是减小安全距离。找不到分离轴仅表示认证不充分，不自动证明真实碰撞。

认证通过后

\[
q(t)=q_0+\Delta q\,s(u),\quad u=t/T,\quad s=10u^3-15u^4+6u^5,
\]
\[
v=\Delta q\,s'/T,\quad a=\Delta q\,s''/T^2,\quad j=\Delta q\,s'''/T^3.
\]

令 \(d_q=\|\Delta q\|_\infty\)，

\[
T\ge\max\left(0.1,\frac{1.875d_q}{0.3},\sqrt{\frac{5.773502692d_q}{0.5}},\sqrt[3]{\frac{60d_q}{5}}\right).
\]

速度/加速度/jerk 上限为 0.3 rad/s、0.5 rad/s²、5 rad/s³，用多项式导数根检验完整区间极值。反作用重定时用 21 点设计采样：

\[
k=1.02\max\left(1,\sqrt{F_{peak,sample}/0.35},\sqrt{\tau_{peak,sample}/0.15}\right).
\]

时间放慢 k 后 \(W_{new}(u)=W_{old}(u)/k^2\)。采样设计不是连续 W 硬上界。若把阶段变慢带来的收益当作自由度分配收益，会混淆因果，故论文消融必须固定共同时间。

检测到停止请求或感知进程异常时，沿已认证标量路径拟合保持当前 s/速度/加速度、终点速度/加速度归零的五次制动曲线，检查单调及全区间导数界。若找不到更短解，执行原认证路径的剩余段停车；不瞬间把速度置零。该保证以场景和几何证书仍有效为条件。

## 16. 时序、闭环与伪代码

关节执行 60 Hz，图像请求 15 Hz，TSDF 最多约 2 Hz；最新帧队列丢旧图像但保留全部动作地图事件。低频规划发生在 C2 静止段端，异步视觉进程持续工作，因此双臂不是“左臂建图全部结束后右臂才开始”，也不是每个 16.7 ms 都重做全局规划。

```python
initialize_USD_kinematics_mass_model_and_empty_map()
bootstrap_one_usable_camera_view()
for target in task_interfaces:
    estimate = acquire_fresh_RGBD_estimate(target)
    for right_subgoal in approach_and_incremental_servo(estimate):
        # 右臂近接每次修正不超过 25 mm，独立磁吸近距确认终止。
        right_path = known_CAD_plan(right_subgoal)
        for segment in right_path:
            if unheld_fresh_target_and_current_view_sufficient():
                left_goal = bounded_reaction_QP_with_visual_inequalities()
            else:
                left_goal = legacy_visual_IK_and_redundancy_or_hold()
            joint_path = continuous_joint_box_certificate_or_repair(left_goal, segment)
            curve = quintic_limits_and_sampled_reaction_retiming(joint_path)
            while curve.not_finished:
                execute_both_arms_same_clock(curve)
                request_or_receive_latest_RGBD_without_dropping_map_events()
                if detected_fault: smooth_stop_on_certified_path()
    confirm_magnetic_attachment(distance_at_most_8_mm)
    locally_invalidate_source_evidence_to_UNKNOWN()
    move_lift_transfer_place_with_payload_collision_geometry()
    confirm_release()
    locally_invalidate_destination_evidence_to_UNKNOWN()  # before retract
    retract_and_reobserve()
export_measured_open_surface_and_differentials()
independently_audit_trajectory_geometry_derivatives_and_reaction()
```

观测过期时禁止以旧目标继续下一次视觉伺服修正。新版本在确认段端速度、加速度为零后，允许一次原有有限候选视角搜索与重观测，再继续接近；找不到新测量则停止。`--recovery-attempts 0` 可关闭此恢复，几何/通信/吸附故障不被该逻辑吞掉。已有认证的段内执行不是根据每帧图像任意改写轨迹。未实现连续感知质量 CBF 或所有外界变化下的递归可行性，论文中必须保留这一区别。

## 17. 正确的评价指标

\[
F_{peak}=\max_k\|F_k\|,\quad F_{RMS}=\sqrt{\frac{\sum_k\|F_k\|^2\Delta t_k}{\sum_k\Delta t_k}},
\]
\[
J_{net}=\sum_kF_k\Delta t_k,\quad J_{abs}=\sum_k\|F_k\|\Delta t_k,\quad
J_{window}=\max_t\left\|\int_{t-0.25}^{t}F(s)ds\right\|.
\]

力矩同理。静止到静止时净冲量理论上可能接近零，但中途力/力矩峰值仍大；不能用净冲量近零证明“零冲击”。这里 W 是惯性反作用，不包括磁吸碰撞瞬间的接触冲击。

还记录：递送成功/停止原因、连续认证段数、仿真吸附间隙、任务时间、观测年龄、地图局部保留量、优化器耗时、流式整周期 P95/P99/最大值与超时次数。近距吸附间隙不是完整点云精度，地图点数增加不是误差降低的证明。三次可见性探测也不等于全时段视觉质量保证。

随机测试不得根据任务成功重抽场景；入域 IK 不等于保证任务可完成。失败、超时、拒绝入域均保留。程序和输出 SHA-256、种子、参数应随批次记录；尚未做完的大规模独立验证不写成已完成。

## 18. 运行与交付边界

2026-09-09 补充：接近阶段新增 `continuous_visual.py` 的关节盒区间验证，替代只用 9 点检查作为最后放行依据；9 点检查仍作为优化器内部快速筛选。连续几何检查不等于连续真实可见性。完整推导、有界零反作用下界和正式评估协议见 [理论补充](research/THEORY_AND_VALIDATION.md)。高分辨率/自适应自由空间证据为独立实验分支，不混入已冻结的 50 场景方法版本。

```powershell
$PY = 'D:\conda_envs\env_isaaclab\python.exe'
& $PY -B -m astronaut_delivery.sufficiency_delivery --output reports/my_local_run --objective wrench --anytime-feasible --map-policy local --async-sensor --telemetry
& $PY -B -m astronaut_delivery.cooperative_audit --run reports/my_local_run
& $PY -B -m astronaut_delivery.dashboard --run reports/my_local_run --frames 120
```

输出目录应是新目录。保留旧默认 `cooperative` 入口和 `--map-policy reset` 消融，不删除已有验证数据或神经网络模型。

当前可交付的是已知工作台上的运动学仿真软件和研究原型。仍不能宣布整个研究目标完成：毫米级未知环境地图独立规划、任意数量/接口物体、动态遮挡下连续最低视觉质量、严格零反作用、含所有慢规划的硬实时、自由漂浮人体动力学、实物标定与实验，以及论文新颖性/接收都需要独立证据。JCR Q1 是投稿期刊的年度分类，不是代码通过测试即可获得的算法认证。
