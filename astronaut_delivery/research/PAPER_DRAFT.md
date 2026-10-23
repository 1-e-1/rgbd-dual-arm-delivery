# Task-Sufficient Visual Redundancy Allocation for Low-Reaction Astronaut-Worn Dual-Arm Assistance

Working manuscript, 2026-09-08. **Exploratory simulation results; not submission-ready.** Author, affiliation, funding, and hardware-validation statements are intentionally absent until verified.

Implementation update: action-local map maintenance, batched analytic reaction evaluation,
and one bounded active-view recovery are now available. The initial eight new random
scenes yielded seven complete successes and one stale-view stop; its subsequent recovery
regression is not an independent holdout. Two further scenes completed, one requiring an
extended independent separating-axis audit (the original inconclusive audit is retained).
See `../LOCAL_UPDATE_DELIVERY.md` for exact cohorts, timing scopes and retained negative
results, and `../COMPLETE_TECHNICAL_ROUTE.md` for complete implementation mathematics.
These updates do not resolve the manuscript's open scientific validation gates.

## Abstract — provisional

This study investigates how a camera-carrying auxiliary arm can support manipulation while contributing to the reduction of the combined inertial reaction at a shared astronaut-mounted interface. Instead of assigning the camera a unique desired pose, an experimental controller permits motion within a geometric visual-quality set, subject to fresh image measurements. An eight-variable allocation problem modifies the camera-arm endpoint while retaining the execution-arm path. A preliminary momentum-based objective reveals an important failure mode: reducing an initial momentum surrogate can increase force peaks and short-window linear impulse. A revised objective linearizes sampled six-axis reaction along the motion segment. Matched-segment experiments separate allocation benefits from time scaling. The present results establish a limited proof of concept, not zero-reaction control, calibrated task-sufficient perception, continuous occlusion avoidance, or hard-real-time execution. Complete randomized evaluation and external validation remain necessary.

## 1. Introduction and research question

The intended application comprises two astronaut-mounted auxiliary arms in an orbital microgravity environment. One arm provides RGB-D environment information and visual guidance, while the other performs magnetically assisted part delivery. Both arms transmit inertial loads through a common C-shaped interface. The question is not whether visual servoing and reaction compensation can be combined—prior work has already addressed such combinations—but whether task-sufficient visual requirements can release useful motion authority without sacrificing manipulation or safety.

The proposed research focuses on the relationship among visual margins, constrained reaction-compensation authority, and computational cost. The application does not justify a claim of novelty by itself. The related-work matrix and outstanding full-text checks are maintained in [RESEARCH_PLAN.md](RESEARCH_PLAN.md).

## 2. Problem formulation

Let q∈R¹⁶ denote the two eight-joint configurations. The C interface and astronaut geometry are fixed in this first simulation model. The reference controller remains kinematic. User-confirmed module masses and imported USD mass properties are used only to estimate the inertial wrench. Camera, magnetic-tool and payload inertia are excluded by the stated task scope; their collision geometry remains included.

In a common C frame,

\[
H_C=A(q)\dot q,\qquad W_C=-\left(A(q)\ddot q+\dot A(q,\dot q)\dot q\right).
\]

Strict zero reaction requires W_C(t)=0 throughout motion. A nearly zero integral over a rest-to-rest trajectory is insufficient evidence because large positive and negative transients may cancel. Force and torque peaks, time-weighted RMS, and maximum sliding-window linear/angular impulses must therefore be reported separately.

The local acceleration condition Aa+b=0 must intersect the manipulation, visual, collision, and actuation feasible set. The unbounded rank/projection calculation provides only a necessary diagnostic once inequalities are present. Zero reaction is not guaranteed by counting degrees of freedom.

## 3. Experimental method

### 3.1 Measurement-gated visual set

A fresh RGB-D estimate supplies a docking-center position, engineering covariance, visible support, and exposure time. Current experiments use known-colour horizontal circular magnetic interfaces of radius 20 mm. Geometric constraints include image-plane margins, target range, viewing elevation, and predicted support. A projected radius enlarged by an engineering 3σ margin shrinks the field-of-view set. This covariance is not probabilistically calibrated.

The reference equality baseline imposes J_camera δq_L=0 to first order. The set-based variant replaces this equality with inequalities g(q_L+δq_L)≥0. Linearized constraints are used in the small QP, followed by sampled nonlinear checks and the existing independent continuous CAD path check. An active inequality admits one-sided directions and must not generally be replaced by a bilateral equality when analysing authority.

The current full-mission prototype releases constraints only for fresh, unheld pick targets. Transport and ineligible visual states use the existing controller. This restricted scope is not a completed phase-dependent perceptual policy.

### 3.2 Why initial momentum is insufficient

The first tested allocation minimizes

\[
\|D(A_L\delta q_L+A_R\Delta q_R)\|^2+\lambda\|\delta q_L\|^2.
\]

This objective omits changes in A along the finite path and does not directly minimize force or torque peaks. The experiment below demonstrates that a better surrogate value need not imply lower force or impulse.

### 3.3 Sampled reaction allocation

For a common quintic scalar clock s(t), define

\[
q(t;x)=q_0+\begin{bmatrix}x\\\Delta q_R\end{bmatrix}s(t),
\quad w_j(x)=S W_C(q(t_j;x),\dot q(t_j;x),\ddot q(t_j;x)).
\]

Here S normalizes force by 0.35 N and torque by 0.15 N·m; these are engineering objective scales, not certified safety thresholds. The implementation samples normalized times 0.2113248654, 0.5, and 0.7886751346. Central finite differences of the actual wrench function produce a local Gauss–Newton matrix M at x=0. The eight-variable QP is

\[
\min_x\|w(0)+Mx\|^2+\lambda\|x\|^2
\]

subject to linearized visual inequalities, ±0.12 rad trust bounds, and joint limits. OSQP is limited to 400 iterations and residual-checked before use. A finite set of step reductions and nine nonlinear geometric samples screens the candidate. These tests do not establish continuous visibility or peak-wrench bounds.

The actual complete curve is certified for self/inter-arm/known-environment collision using the retained controller. The expensive reaction linearization occurs at planning events, not at each 60 Hz execution tick. Derivative caching/analytic differentiation and a strict planning deadline are not yet implemented.

## 4. Evaluation protocol

Two evaluations must remain distinct:

1. Independent matched segments fix the initial state, right-arm curve and duration for frozen-camera, pose-equality and visual-set variants. No reaction-driven retiming is used. Thus any within-cohort differences cannot be attributed to slowing the execution arm.
2. Complete closed-loop missions allow image observations and paths to evolve. They retain the original reaction-aware time scaling. These runs test integration, not an isolated equal-time causal effect.

All 19 pick segments from the existing nominal trajectory enter the matched experiment. Two fail initial visual-set eligibility; all failures remain logged. Each eligible candidate is independently checked for collision and inspected through actual simulated RGB-D at the start, middle, and end. The three probes do not prove continuous occlusion avoidance. Segments from one mission are correlated and cannot be treated as independent randomized trials for population inference.

## 5. Exploratory matched results

### 5.1 Negative momentum-surrogate result

Source: `reports/sufficiency_matched_02/`. Of 19 candidate segments, 17 were visually eligible; only seven were jointly collision-feasible for all three tested variants. Within this common subset:

| Metric: mean of segment values | Frozen camera | Pose equality | Visual set + momentum |
| --- | ---: | ---: | ---: |
| Force peak, N | 0.12356 | 0.14583 | 0.14726 |
| Torque peak, N·m | 0.07113 | 0.06968 | 0.06252 |
| Maximum 0.25 s linear impulse, N·s | 0.02960 | 0.03467 | 0.03518 |

The set/momentum variant does not meet a simultaneous low-force/low-torque claim. Its complete mission `reports/sufficiency_set_02/` delivered two parts and stopped on a stale bracket visual target. This failure is retained; the baseline controller was not replaced.

### 5.2 Revised reaction objective

Source: `reports/sufficiency_matched_wrench_01/`. The same 19 candidate segments produce 17 eligible segments; all three variants are collision-feasible on all 17:

| Metric: mean of segment values | Frozen camera | Pose equality + wrench | Visual set + wrench |
| --- | ---: | ---: | ---: |
| Force peak, N | 0.16052 | 0.15682 | 0.15196 |
| Torque peak, N·m | 0.08291 | 0.08318 | 0.07851 |
| Maximum 0.25 s linear impulse, N·s | 0.03769 | 0.03676 | 0.03543 |
| All three measured visual probes sufficient | 14/17 | 14/17 | 14/17 |

Relative to the frozen variant in this matched cohort, mean segment force/torque peaks decrease by about 5.3%, and mean maximum short-window linear impulse decreases by about 6.0%. The set variant reduces force in 14 of 17 segments and torque in 16 of 17; it is not uniformly Pareto-superior. The cohorts of Sections 5.1 and 5.2 differ, so their raw aggregate means must not be directly compared.

The set/wrench optimizer P95 was approximately 163 ms in the concurrent pilot workload, including finite-difference reaction linearization. This is a low-frequency planning cost, **not a 60 Hz solver**. It does not establish lower computation than existing methods. Joint execution remains a separate, inexpensive stage. Controlled single-process timing and optimization are required.

### 5.3 Full-mission integration of the revised objective

`reports/sufficiency_wrench_01/` completed all three deliveries in 108.751 s of simulated motion. Independent auditing passed all 32 joint segments and found a maximum wrench discrepancy of 3.87e-7 against the independent Newton–Euler calculation. The capture gaps were 2.337, 2.318 and 1.296 mm. The logged trajectory dashboard is `reports/sufficiency_wrench_01/dashboard.gif`.

Full-mission peak force was 0.33707 N, force RMS 0.11041 N, peak torque 0.14451 N·m, torque RMS 0.05729 N·m, and maximum 0.25 s linear impulse 0.08235 N·s. Compared with the previous complete low-reaction controller, force peak/RMS and linear impulse were worse, although torque RMS and simulated task duration were lower. This integration result does **not** establish complete-system superiority.

Only three of 17 eligible allocation calls returned accepted proposals under the strict 400-iteration solver termination gate; 14 hit the iteration budget and fell back to holding the camera-arm endpoint. This explains why matched-segment success cannot be extrapolated to continued closed-loop improvement. An optional bounded-iteration feasible-iterate gate is under separate evaluation: it must independently verify constraints, non-increasing linearized merit and nonlinear sampled geometry before accepting a suboptimal iterate. It is not an optimality or recursive-feasibility certificate.

### 5.4 Bounded-iteration feasibility recovery

The first optional gate (`reports/sufficiency_wrench_anytime_01/`) completed 3/3 deliveries and passed 32-segment auditing, but admitted only one additional feasible suboptimal iterate (four accepted proposals among 17 eligible calls). Most rejected iterates violated linear constraints; accepting them solely because they improved an objective would have been incorrect.

The subsequent experiment (`reports/sufficiency_wrench_ray_01/`) contracts a candidate toward the feasible zero increment. For linear constraints l≤Cx≤u containing zero, the maximal retained fraction is

\[
\eta=\min\left(1,\min_{(Cx)_i>0}\frac{u_i}{(Cx)_i},\min_{(Cx)_i<0}\frac{l_i}{(Cx)_i}\right),\qquad x_{safe}=\eta x.
\]

A small numerical interior margin is used when contraction is needed. This elementary convex-ray property is not claimed as a novel projection theorem. Independent linear bounds, non-increasing quadratic merit, sampled nonlinear visual constraints and the unchanged collision gate remain mandatory. The property does not prove feasible nonlinear future states or continuous visibility.

This run completed 3/3 deliveries in 109.230 s, with 16 eligible allocation calls and 16 accepted nonzero candidates, including six accepted suboptimal iterates. Peak force was 0.33708 N, force RMS 0.10953 N, peak torque 0.14451 N·m, torque RMS 0.05631 N·m, and maximum 0.25 s linear impulse 0.08190 N·s. The low-frequency optimizer P95 was about 147 ms in this run; timing across these changing workloads is not a controlled computational speedup comparison. Complete-system force/impulse improvement over the previous default controller remains unestablished.

Regression verification: 154 tests passed, including finite-difference Jacobian agreement, invalid-measurement rejection, equality/set feasibility, bounded-iteration acceptance safeguards and radial contraction. Report: `reports/research_milestone_tests.xml`.

## 6. Limitations and required completion work

The evidence is not yet sufficient for a strong journal submission. Remaining gaps include calibrated task-dependent visual thresholds; actual occlusion prediction and recovery; consistent complete-mission improvement rather than one successful integration run; frozen-method evaluation on independent random scenes; reaction/collision/visibility feasibility analysis; bounded planning latency; and mass-property/sensor uncertainty validation. Known CAD geometry, rather than reconstructed point clouds alone, remains the safety authority.

The fixed interface is an evaluation fixture. It does not simulate free-floating astronaut motion, interface flexibility, contact impulse or human response. A physical six-axis force measurement or an independently validated dynamics experiment would materially strengthen external validity, without changing the kinematic nature of the proposed motion references.

No claim is made that the new method is globally optimal, near-zero reaction, novel in all its components, hard real-time, or already suitable for Q1 publication. The manuscript will be revised after the acceptance gates in the research plan are met.
