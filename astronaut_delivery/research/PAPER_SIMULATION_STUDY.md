# Task-Sufficient Visual Redundancy Allocation for Low-Reaction Dual-Arm Assistance: A Constraint-Aware Simulation Study

Status: simulation manuscript with completed frozen 50-scene evaluation and explicitly unresolved research gaps. This is not a submission-ready claim that the full intended perception/planning research objective has been achieved. Author, affiliation, funding and competing-interest declarations must reflect the actual researchers and are intentionally not invented.

## Abstract

Implementation scope update: the user now requires RGB-D-only environment collision authority and permits an initial map reconstructed from retained raw RGB-D recordings. The frozen results below used the historical CAD-assisted environment guard. They are not evidence of completed RGB-D-only manipulation; commissioning progress and unresolved gaps are recorded in [RGBD_ONLY_ROUTE.md](RGBD_ONLY_ROUTE.md).

A camera-carrying auxiliary arm can support an execution arm without continuously enforcing a unique camera pose. This observation motivates allocating visual redundancy according to task-sufficient measurement conditions rather than full-pose equality. We study this allocation on an astronaut-associated dual-arm workcell with two eight-joint manipulators, a nominal D435i RGB-D camera and a magnetic delivery interface. The controller combines a small event-triggered reaction objective, measured visual constraints, independently checked geometric paths and a smooth stopping fallback. A bounded instantaneous residual analysis distinguishes reaction cancellation from insufficient control authority. Local map maintenance preserves unchanged measured geometry while invalidating transfer-affected evidence. Across 50 frozen scene proposals, 47 pass the admission screen. Visual-set allocation succeeds in 43/47 executions, compared with 41/47 for local full-pose equality; the paired success difference is not significant (exact McNemar p=0.625). Among 40 common-success scenes, mean torque RMS decreases by 4.89%, whereas force RMS does not show an improvement and accumulated force magnitude increases. Independent mapping audits further distinguish accurate observed surfaces from useful free-space coverage: 4 mm voxels improve observed-surface error but do not establish a map suitable as the sole full-arm planning authority. The results support a conditional torque/vision trade-off, not overall dominance, universal zero reaction, continuous visibility or physical safety.

Keywords: dual-arm coordination; task-sufficient vision; inertial reaction; active perception; conservative mapping; bounded computation.

## 1. Introduction

An execution arm and a camera arm have different task requirements. The former must approach and transfer an object, whereas the latter must supply sufficiently accurate and recent visual information. Enforcing an exact camera pose can consume motion directions that are unnecessary for the immediate visual task. Conversely, moving the camera solely to counterbalance the execution arm may sacrifice visibility or target estimation. The relevant question is therefore not whether camera motion can be combined with reaction reduction, but how much bounded compensation authority remains after satisfying the actual visual task and geometric constraints.

Our application is an auxiliary dual-arm assembly attached to a fixed C-shaped interface associated with an astronaut model. This fixed-interface simulation isolates the reaction load transmitted by arm motion. It is not a free-floating human–robot dynamics model. The execution remains kinematic, while link mass properties are used in a separate inertial-load calculation. Camera, magnetic-tool and payload inertias are excluded as specified by the study design; their collision geometry is retained.

The method under investigation replaces local full-camera-pose equalities with geometric visual inequalities only when a fresh measured target satisfies quality gates. It does not release visual constraints in every phase. A measured TSDF supports environmental reconstruction and exposes the distinction between unknown and certified free space. The complete delivery controller currently retains known CAD as its principal collision authority.

The intended contribution is a reproducible analysis of the coupled visual-accuracy, compensation-authority and computational-budget trade-off. The individual ingredients—task sets, quadratic programming, inertial reaction models, TSDF mapping and separating-axis tests—are established techniques, not independent novelty claims. In particular, a local objective decrease is not treated as evidence of improved complete-mission performance.

## 2. Related work and scope of comparison

Reactionless visual servoing and simultaneous additional manipulation have already been studied for multi-arm space robots. Hafez et al. formulate visual and base-attitude tasks in a redundancy framework and demonstrate several robot configurations. Their formulation is a direct conceptual reference, not a result that can be ignored when positioning this study. [Author manuscript](https://surilshah.weebly.com/uploads/1/1/4/6/11462120/2016_robot_ras.pdf), [published article](https://doi.org/10.1016/j.robot.2016.12.010).

Reaction-null-space camera inspection also illustrates the dependence of attainable reactionless motion on manipulator architecture, attachment configuration and task selection. Our bounded residual diagnostic addresses a related feasibility issue, but does not reproduce the complete free-flying dynamics of that work. [Reactionless camera inspection, 2016](https://doi.org/10.1016/j.actaastro.2016.08.027).

Set-based tasks within task-priority inverse kinematics provide an established basis for replacing unnecessary equalities by admissible task ranges. This study instead asks whether a particular measured visual set produces useful complete-task gains under the fixed-interface reaction metric. [Moe et al., 2016](https://www.frontiersin.org/journals/robotics-and-ai/articles/10.3389/frobt.2016.00016/full).

Probabilistic barrier certificates have been proposed for occlusion-free image-based visual servoing under feature uncertainty. Our interval check concerns geometric projection and range only; it is not an alternative proof of the same probabilistic occlusion property. [Zhang, Yang and Luo](https://arxiv.org/abs/2309.03476).

Recent base-reaction-aware planning embeds a reaction proxy in bidirectional tree growth and follows the selected reference using spline generation and QP-CBF execution. The publisher preview describes a pre-contact free-floating dual-arm setting and separate all-seed and common-success analyses. We therefore do not claim that combining reaction-aware planning with safety checks is new. [Zhou et al., available online 4 September 2026](https://doi.org/10.1016/j.asr.2026.08.112). This comparison is based on the publisher's accessible preview, not an obtained full-text implementation.

## 3. System and information boundaries

The joint vector is \(q=[q_L^T,q_R^T]^T\in\mathbb R^{16}\). The fixed interface frame defines the common origin and axes for reaction summation. Forward kinematics and collision covers are extracted from the USD assembly. The simulated RGB-D sensor ray-casts the actual scene mesh and supplies aligned colour, axial depth, intrinsics, camera extrinsics and timestamps. Scene object poses are available to the simulator and independent auditor but are not passed to the target estimator as observations.

The operational fixture contains three identified, coloured, horizontal magnetic docking interfaces with a known 20 mm disk radius. Object body dimensions, four body-shape choices, planar placement, yaw and table height vary in the randomized complete-task experiments. These experiments do not establish arbitrary-count, unmarked-object or arbitrary-interface recognition. The nominal depth and RGB resolutions are 848×480 and 1280×720; native-grid crops reduce streaming cost without changing focal length.

The image estimator uses colour separation and a known-radius circular boundary fit. This interface prior should not be confused with a learned general object detector. Target estimates include position, covariance, pixel support and exposure time. Right-arm corrections use measured target position and are limited to 25 mm per servo iteration. The magnetic latch independently checks an 8 mm simulated proximity condition; it does not snap a visually estimated object into place before checking distance.

No learning-based policy is used in this evaluated branch. Training loss, reward and training episodes are therefore not applicable. Historical flow-matching and drifting architectures remain separate from these experiments.

## 4. Method

### 4.1 Combined inertial reaction

For rigid link \(i\), let \(m_i,c_i,R_i,I_i\) denote mass, world COM, orientation and local COM inertia. Let \(J_{v,i},J_{\omega,i}\) be its COM velocity and angular-velocity Jacobians. With fixed reference origin \(o_C\), the momentum map is

\[
A(q)=\sum_i\begin{bmatrix}
m_iJ_{v,i}\\
R_iI_iR_i^TJ_{\omega,i}+[c_i-o_C]_\times m_iJ_{v,i}
\end{bmatrix}.
\]

All blocks are transformed to the common C-frame before summation. The corresponding interface reaction is

\[
W_C(t)=-\frac{d}{dt}(A(q)\dot q)=-A(q)\ddot q-\dot A(q,\dot q)\dot q.
\]

The implementation uses batched analytic rigid-body recursion in the local optimizer. An independently implemented Cartesian finite-difference Newton–Euler calculation audits selected logged states. Tool, camera and payload inertia exclusions apply to both calculations. Force, torque and their short-window integrals are reported separately; a near-zero net rest-to-rest impulse is not interpreted as zero intermediate reaction.

### 4.2 Task-sufficient visual set

For a measured target \(\hat p\), its camera coordinates are \(y=R_c^T(\hat p-p_c)\), with axial depth \(z=-y_z\), range \(d=\|y\|\), normalized image coordinates \(u=y_x/z\), \(v=-y_y/z\), and viewing elevation \(e=(p_{c,z}-\hat p_z)/d\). Let \(\sigma\) be the square root of the largest eigenvalue of the estimated covariance.

The current set enforces image boundaries after a disk-support margin \(s=(r+3\sigma)/z\), a minimum 0.45 m standoff, a bounded maximum range, minimum elevation, and minimum predicted pixel support. Measurement age must not exceed 0.25 s; at least 12 pixels and \(\sigma\le5\) mm are required before allocation. These thresholds are engineering settings rather than physically calibrated confidence bounds.

The range/elevation pixel model is

\[
\hat N(q)=N_0\left(\frac{d_0}{d(q)}\right)^2\frac{e(q)}{e_0}.
\]

This is a local visibility-support proxy. It does not model an intervening occluder. The set is activated for fresh, unheld pick targets; held-object transport retains the prior camera controller.

### 4.3 Small event-triggered allocation

For a candidate right-arm segment, an eight-dimensional left-arm increment \(x\) parameterizes a common-clock joint segment. A Gauss–Newton model of weighted six-axis reaction is formed at three fixed normalized times. The small quadratic program minimizes

\[
\frac12\|r_0+J_rx\|_2^2+\frac\lambda2\|x\|_2^2
\]

subject to a trust region and linearized visual inequalities. The local full-pose baseline instead preserves the six-dimensional camera twist to first order, \(J_{camera}x=0\); this is not exact nonlinear pose invariance over a finite segment.

The solver has a fixed iteration limit. A nonconverged candidate is considered only if independently verified feasible after contraction towards the feasible zero increment. The retained fraction is bounded by every linear inequality along that ray. Linear feasibility, non-increasing local merit and nonlinear geometric tests remain mandatory. None of these gates asserts global nonlinear optimality or a complete-mission improvement.

### 4.4 Whole-segment geometric verification

The final geometric visual gate partitions the joint segment into interval boxes. At a box centre, global serial-chain lever bounds give

\[
\epsilon_p=\sum_jL_j\delta_j,\qquad
\theta_R\le\min(\pi,\sum_j\delta_j),\qquad
\epsilon_y=\epsilon_p+2d_c\sin(\theta_R/2).
\]

Camera-coordinate, depth, distance and elevation intervals yield lower bounds for all visual margins. A box is accepted only if every lower bound is nonnegative. Unresolved boxes are subdivided until accepted or the box/depth budget is exhausted; exhaustion rejects the proposal. The proof covers the defined fixed-target geometric proxy, not continuous image availability.

Collision verification uses conservative full-link and payload geometry, including self/inter-arm and fixed-structure checks. An optional post-freeze repair proposes extra separating directions after conservative rejection; direct projection of all current convex-hull vertices must exceed the unchanged margin. Solver success alone never grants clearance. This repair is reported separately from the frozen primary comparison.

### 4.5 Bounded residual diagnosis

Fixing the right-arm acceleration at an observed state yields a normalized residual \(Bx+b\) for the left-arm acceleration \(x\). Define \(K=\{x:\ell\le x\le u,Ex=f,Cx\le h\}\). For any \(\|d\|\le1\), \(\lambda\ge0\) and free \(\nu\),

\[
\min_{x\in K}\|Bx+b\|\ge
d^Tb-\lambda^Th-\nu^Tf+
\sum_j\min(s_j\ell_j,s_ju_j),\quad
s=B^Td+C^T\lambda+E^T\nu.
\]

This weak-duality bound includes stationarity residuals through exact minimization of the remaining linear term over the box. A positive numerical lower bound rules out zero within the stated instantaneous feasible set. Zero feasibility in that set does not establish a collision-free, visually valid, integrable full trajectory. In particular, the diagnostic fixes the right-arm command and does not rule out all possible alternative right-arm trajectories.

### 4.6 One measured map with local maintenance

Depth is back-projected through calibrated intrinsics and camera pose into a robust confidence-weighted TSDF. Object transfer invalidates only conservatively bounded source/destination regions. Both occupied and free evidence become unknown in those regions, while unchanged evidence is preserved. Full reset remains a conservative fallback when the change region or event history is unavailable.

The displayed surface is the observed zero set of the TSDF. Gradient and Hessian stencils produce normals and curvature where all required neighbours are valid. Missing observations are not converted into watertight completion. Planning export instead measures distance to all uncertified cells and subtracts discretization and uncertainty margins.

Independent auditing motivated two experimental evidence refinements: selecting the smallest pixel patch sufficient to cover projection uncertainty, and maintaining per-voxel view provenance so repeated exact-view images can fill previously missing pixels without receiving independent votes. These refinements preserve invalid-pixel rejection and do not themselves imply useful full-arm free-space coverage.

### 4.7 Timing and fallback

Both arms execute the same smooth joint-time reference. Quintic limits constrain velocity, acceleration and jerk. Slow perception and planning are separated from the cheap executor in the asynchronous implementation. A geometric certificate can be reused for a monotone smooth stop on the original segment only while its scene and payload assumptions remain valid. New unannounced moving obstacles invalidate this argument.

At a rest endpoint, stale visual guidance permits one bounded camera reacquisition attempt. Failure terminates the mission. This is a bounded recovery mechanism, not a proof of unconditional recursive feasibility or eventual task completion. Reported stream timings exclude any unsupported claim that the entire planning stack meets a hard real-time deadline.

## 5. Experimental protocol

The primary comparison freezes 50 scene proposals (seeds 2000–2049), method parameters and source hashes before execution. Each admitted scene is run with local full-pose equality and visual-set allocation; method order is counterbalanced by seed parity. A known-CAD pregrasp IK screen defines admission but does not guarantee complete-task feasibility. Rejected proposals, stopped runs, timeouts, audit failures and runner errors are retained rather than replaced.

The primary endpoint is independently audited full-mission success. Results must report both all-proposal and admitted-execution denominators. Clopper–Pearson intervals quantify binomial uncertainty; discordant success pairs use the exact McNemar test. Reaction and task-time differences are secondary, conditional on both methods succeeding in the same scene. Their uncertainty is obtained by resampling scenes, not trajectory ticks or segments. The secondary intervals are exploratory and not adjusted for multiple comparisons.

Concurrent wall-clock durations are not used as a real-time performance claim. The current protocol does not externally preregister the study or pin the Python hash seed; repeatability and infrastructure effects must therefore be considered separately from source-code identity. Post-freeze mapping and geometry repairs are not mixed into the primary results.

Additional experiments include equal-clock segment ablations, nominal complete-task audits, COM/inertia perturbations on fixed logged kinematics, independent visual-error observations on 20 new scenes, synthetic depth loss and complete-frame failures, independent surface-distance/coverage audits and known-colliding sphere probes for map export.

## 6. Results currently established

### 6.1 Nominal integration and reaction feasibility

The continuous-geometric-gate nominal run completes three deliveries in 109.985 s simulated time. Independent auditing rechecks 32 motion segments, with maximum selected-state reaction disagreement below \(4\times10^{-7}\). Capture gaps are 2.039, 2.249 and 1.739 mm. These are simulated interface gaps, not whole-environment map errors.

Of 100 time-index samples on that trajectory, 88 have a positive instantaneous residual lower bound even in the left-acceleration box superset. Preserving the logged camera acceleration produces 98 positive bounds, one numerical zero-feasible case and one inconclusive case. This establishes limits of the sampled fixed-right-command allocation problem, not a universal impossibility theorem for the robot.

### 6.2 Inertia sensitivity

For 100 fixed-trajectory perturbations, original module masses remain unchanged while COM components vary by ±5 mm, inertia scale varies from 0.8 to 1.2 and inertia-frame orientation varies by up to 5°. The nominal force peak is 0.33705 N; perturbed peaks range from 0.33552 to 0.33995 N. Nominal torque peak is 0.14450 N·m; perturbed peaks range from 0.14388 to 0.14602 N·m. These assumed distributions quantify an engineering sensitivity, not measured mass-property uncertainty or a robust worst-case certificate.

### 6.3 Observed surface accuracy and missing coverage

Under the same nominal mission, 12 mm voxels yield observed-vertex distance P95 of 4.234 mm. The 4 mm map yields 0.792 mm. However, the fraction of sampled complete object surfaces within 5 mm of the measured mesh rises only from 27.1% to 40.6%. Unobserved surfaces, including occluded faces, remain absent. Thus accurate observed geometry cannot justify a claim of precise complete object reconstruction.

The initial adaptive-patch map obtains 18,134 certified-free cells, but its maximum sampled conservative clearance is only 4.16 mm. None of the queried free-cell centres admits even a 5 mm-radius sphere. Zero false-free responses among 71,332 known-colliding sphere probes must therefore be interpreted alongside this lack of utility, not as successful map-only motion planning.

A separate exact-view temporal-evidence experiment retains the minimum valid ray depth within an unchanged scene epoch, without interpolating absent pixels or awarding repeated independent-view votes. It completes three deliveries and two additional scan views, passing a 34-segment audit. The scan adds 19.928 s to the 109.985 s delivery. Observed surface error P95 is 0.967 mm and complete-part coverage within 5 mm is 47.08%; nevertheless no queried free centre admits a 10 mm-radius sphere. This variant therefore does not resolve measured-map-only planning and is not promoted as a superior controller. Earlier postscan coverage of 47.69% came from a different trajectory/observation sequence and is not a matched-input causal ablation.

The post-freeze asynchronous integration independently completes three deliveries in 110.897 s, with a 32-segment audit. Its stream-cycle P99 is 6.779 ms and maximum is 7.650 ms, with no observed 16.667 ms deadline miss; observation age P95 is 100 ms. These timings exclude between-segment planning. Observed surface error P95 is 0.775 mm, but complete-part coverage within 5 mm remains 40.85%. Its high-resolution mapping and geometry repair were not evaluated in the primary frozen paired batch.

### 6.4 Visual errors and fault outcomes

Across 20 independent scene draws and four recorded pregrasp camera poses, nominal target detection is 240/240 with conditional error P95 3.77 mm. Doubling the synthetic noise yields 203/240 and 6.38 mm; quadrupling it yields 49/240 and 11.28 mm. With 20% missing depth, the corresponding result is 240/240 and 4.02 mm. These repeated target observations are not independent mission trials.

A 20% depth-dropout complete task passes a 32-segment audit. Four seconds of complete image/depth loss triggers reacquisition and completes all deliveries, passing a 34-segment audit. Permanent loss stops without further delivery; the partial path audit reports missing task completion but no geometry, derivative or reaction disagreement. A two-times-noise run initially stops after two deliveries because no continuous recovery-path certificate is available. The additional bounded separating-axis proposal repair completes the same case and passes a 32-segment audit. This is a regression correction, not an independent holdout success.

### 6.5 Frozen paired missions

All 50 proposed scenes are accounted for, with three admission rejections (seeds 2002, 2007, 2043). Both methods execute in each of the 47 admitted scenes. The full-pose method has 41 audited successes, four stopped runs and two timeouts; the visual-set method has 43 successes, three stopped runs and one timeout. Conditional success proportions are 87.23% (exact 95% CI 74.26–95.17%) and 91.49% (79.62–97.63%), respectively. Using all proposals, including admission failures, they are 82% and 86%. Three scenes succeed only with the visual-set method and one only with full-pose equality; exact McNemar p=0.625 does not establish a success-rate improvement.

The following secondary analysis is conditional on 40 common-success scenes. Differences are visual set minus full pose, with 10,000 scene-bootstrap replicates and exploratory, unadjusted 95% intervals.

| Endpoint | Full pose | Visual set | Paired difference [95% interval] |
|---|---:|---:|---:|
| Mission time, s | 107.900 | 110.121 | +2.221 [-1.030, +6.378] |
| Force RMS, N | 0.115249 | 0.116226 | +0.000977 [-0.001366, +0.003391] |
| Torque RMS, N m | 0.066252 | 0.063014 | -0.003238 [-0.004413, -0.002155] |
| Integrated force magnitude, N s | 9.68974 | 9.90158 | +0.21184 [+0.00587, +0.45267] |
| Maximum 0.25-s force impulse, N s | 0.081432 | 0.081888 | +0.000456 [-0.000371, +0.001371] |

The torque RMS mean decreases by approximately 4.89%, but the force RMS mean increases by 0.85% with an interval spanning zero, and integrated force magnitude increases by approximately 2.19%. The post-hoc normalized wrench RMS endpoint decreases from 0.551066 to 0.535596 (difference -0.015471, interval [-0.025081, -0.006356]); its analysis-stage addition precludes presenting it as a preregistered primary result. The tiny torque-peak difference, about 1.1e-5 N m, is not promoted as practically important. Full endpoints and retained failure reasons are in [paired results](../../reports/formal_paired_50_v1/RESULTS.md).

All frozen source hashes remain unchanged. Primary timeouts are retained even though a separate repeat of seed 2016 completed and passed audit; that diagnostic process also failed to exit after producing valid outputs and was explicitly terminated. The cause of the primary timeout outcomes remains unresolved. Neither that repeat nor post-freeze geometry repairs replace original outcomes.

## 7. Discussion

Visual-set relaxation can expose motion directions excluded by a local full-pose equality, but exposed directions need not be useful after acceleration limits, collision constraints and nonlinear task execution are applied. Global force peaks may also be dominated by phases in which the new allocator is not active. Complete-task time and reaction therefore need to be assessed jointly; comparing a slowed task with a fixed-clock baseline would confound compensation with retiming.

Mapping accuracy and planning feasibility are separate bottlenecks. Finer voxels can improve measured-surface localization while creating no sufficiently connected certified free space. Declaring unknown cells free or completing hidden geometry from simulator truth would conceal this problem. The measured-map interface must demonstrate actual tool/full-link paths, not only attractive surfaces and low point-to-surface error.

The noise experiments likewise show why accepted-estimate accuracy must be accompanied by rejection counts. A method can appear accurate after discarding most difficult observations. The full-mission failure records and fixed proposal set are intended to make these trade-offs visible.

## 8. Limitations and conclusion

The present evidence supports a functioning low-reaction kinematic dual-arm simulation prototype, bounded local geometric checks, conditional stopping/reacquisition and explicit residual diagnostics. It does not yet establish continuous occlusion-free guidance, complete unknown-environment reconstruction suitable as the sole full-arm collision authority, universal zero reaction, arbitrary object/interface generalization or whole-stack hard real-time control. Physical validation is outside this task, but these simulation limitations cannot be dismissed merely because hardware experiments are excluded.

The completed paired study supports lower conditional torque RMS, but not a significant success-rate gain or uniform reduction of all reaction metrics. Increased integrated force magnitude and unresolved map-only feasibility limit a broader superiority claim. The result is a mixed experimental outcome, not evidence that the complete intended research objective has been achieved. The latest post-freeze integrated controller also needs its own independent multi-scene evaluation. Journal acceptance and annual JCR quartile classification are external publication outcomes, not algorithm verification criteria.

## Reproducibility and supporting material

Implementation and mathematical details: `../COMPLETE_TECHNICAL_ROUTE.md` and `THEORY_AND_VALIDATION.md`. Primary experiment protocol and hashes: `../../reports/formal_paired_50_v1/protocol.json`. Raw failed trials are retained. Map and sensor variants have separate report directories. The complete release test run contains 197 passing tests, zero failures, zero errors and zero skips (`../../reports/nonhardware_all_tests_04.xml`). The evidence manifest (`../../reports/nonhardware_acceptance_01.json`) distinguishes completed software/experiment checks from unestablished research claims.

No physical experiment, human-subject test, real sensor calibration, external dataset release or journal submission is claimed. Source code and local artifacts are available in the supplied project workspace; no public repository URL or license grant is invented.
