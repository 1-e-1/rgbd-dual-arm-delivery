# RGB-D Evidence-Constrained Visual Redundancy Allocation for Low-Reaction Dual-Arm Assistance

Working simulation manuscript, 10 September 2026. This replaces neither the archived CAD-assisted study nor its results. The requested complete measured-environment manipulation study is still in progress. No physical experiments, authorship, funding, journal acceptance, or unperformed comparisons are asserted here.

## Abstract

An astronaut-mounted camera arm and manipulation arm share a mechanical interface, coupling visual information to combined inertial reaction. We investigate an evidence-constrained architecture that releases camera motion within a minimum visual-quality set. Environment information is restricted to retained RGB-D measurements; known robot geometry and inertia remain available. An eight-variable optimization allocates visual redundancy, while whole-segment checks restrict motion to measured admissible space. Local evidence revision preserves unaffected free-space evidence, and a separate rigidly transported measured-surface layer retains observed geometry without granting new free space. Nominal development achieved a three-object task over 169 independently audited segments, with horizontal placement errors of 0.607-0.857 mm and 12,933 reaction samples checked against a separate Newton-Euler implementation. Retained measured surfaces increased final 5 mm object-surface coverage from 46.89% to 77.24%. However, the second frozen three-scene development comparison yielded whole-scene successes of only 1/3 for visual set, 2/3 for differential pose, and 1/3 for hold. In the only common successful scene, set reduced torque RMS by 12.19% relative to hold but increased torque peak by 40.86%. Streaming P99 near 4.44 ms did not imply full-stack real time: nominal task wall and motion times were 766.08 and 215.31 s. These results demonstrate conditional feasibility, not reliable randomized missions or uniform reaction superiority. A later single-scene newly rendered sensor-fault study completed nominal, RGB-D blackout, RGB-only blackout and delayed-delivery runs; every fault case executed a certified brake and recovered. Multi-object repairs remain under evaluation; physical experiments are excluded.

Keywords: RGB-D mapping; active vision; dual-arm coordination; reaction-aware motion; kinematic manipulation; evidence-constrained planning.

## 1. Introduction

A shared-base dual-arm assistant must allocate limited motion authority among manipulation, sensing, and disturbance reduction. Holding the camera at one pose can unnecessarily restrict compensation. Conversely, minimizing a reaction surrogate without preserving useful measurements can prevent manipulation. Safety is also limited by what has actually been observed: a collision checker supplied with an exact simulation scene solves a different problem from one supplied only with RGB-D.

The research question is whether a minimum visual requirement can release useful camera-arm motion while maintaining measured-environment feasibility and reducing the combined reaction of both arms under a bounded computational workload. We do not equate a large number of joints with feasible zero reaction, or a near-zero net impulse with low instantaneous loading.

The intended contribution is the interaction of measurement authority, visual-set allocation, and executable segment certificates. It is not the first combination of visual servoing and reaction compensation. Hafez et al. previously studied reactionless multi-arm visual servoing together with other manipulation tasks. [1] Set-based task-priority control, including visual-field constraints, also predates this work. [2] GPU TSDF/ESDF mapping is an established building block rather than a novelty claim. [3] Distinct benefits of the present architecture therefore require matched experiments and failure-accounted evaluation.

### 1.1 Related methodological foundations

The author-hosted preprint of [1] explicitly derives a quadratic secondary objective within a visual-task null space (Section 5.3, equations 31–34). Its conclusion identifies collision and singularity avoidance as future work. This establishes a relevant gap for that particular formulation, not for the entire subsequent literature. Our pose policy is a controlled in-system restriction, not a faithful reproduction of that paper's floating-base controller; it must not be labelled “Hafez baseline.”

Reaction-null-space methods already address mechanical coupling and have applications beyond satellite manipulators. [4] Receding-horizon next-best-view planning already links sensing actions to unknown-space exploration. [5] Incremental TSDF-to-ESDF construction is likewise established in voxblox. [6] Our small convex subproblem uses the existing OSQP solver, rather than a new optimization algorithm. [7] These components must be credited; assembling them does not by itself establish a new control principle.

Recent active-vision research also improves manipulation through viewpoint and focal-length adjustment, as exemplified by AVR. [8] Adaptive on-orbit visual servoing has incorporated state estimation and fault recovery under visual loss. [9] The present conservative response differs in scope: it brakes within an already certified static segment rather than claiming arbitrary occlusion prediction. None of these brief comparisons establishes that the entire combination is unprecedented. The testable question remains whether measured-space authority and minimum visual constraints provide useful, computationally bounded reaction compensation during a complete task.

Object-level RGB-D persistence is also established: Co-Fusion jointly segments, tracks, and reconstructs independently moving object models, and MaskFusion adds instance-level semantic object representation. [10,11] Our retained-surface layer is deliberately narrower. It uses the commanded rigid attachment transform and a measured connected component, does not infer arbitrary object motion, and does not claim novelty for object-centric mapping itself. The study concerns its interaction with measured-free-space authority and visual/reaction allocation, not replacement of general dynamic SLAM.

Acceleration-level adaptive reactionless manipulation after closed-loop capture has also been studied by Dal and Shah, including estimation of unknown target inertia. [12] Li and Wei's 2026 conference study explicitly combines visual servoing with reaction control. [13] Thus neither acceleration-level reaction accounting nor adding vision to multi-arm compensation is a defensible first-of-its-kind claim here. Our fixed-mount, excluded-payload-inertia experiment cannot substitute for their floating-base or adaptive-load analyses. The narrower proposed distinction concerns the measured-only environmental permission to execute visual redundancy and its local revision after object transport; a comparative benefit still requires the experiments below.

### 1.2 Additional 2026 precedents

Recent prior work further narrows the novelty claim. Zhou et al., available online on 4 September 2026, incorporate predicted base reaction into bidirectional tree growth and combine reference generation with constrained execution for a free-floating dual-arm robot. [15] Su et al. use hierarchical QP with a learned differentiable collision-distance surrogate for cooperative free-floating manipulation. [16] Govoni et al. integrate Bernstein-polynomial robot distance fields and their gradients into reactive task-priority avoidance using sensed environmental obstacles. [17] Their published task models and timing denominators differ from the fixed-mount measured-permission experiment here; neither nominal millisecond streaming nor the use of a small QP establishes superiority over them. Action-aware safe exploration and information-versus-navigation-cost selection also predate this work. [18] Consequently, reaction-aware planning, task-priority collision avoidance and action-informed sensing are not new standalone claims. The present hypothesis must be evaluated specifically through measurement-authority-preserving map changes and the executable visual/reaction tradeoff.

## 2. System and information boundary

The simulation contains a fixed astronaut, a fixed C-shaped mounting interface, and two eight-joint arms. The left arm carries a D435i-model RGB-D sensor. The right arm has a magnetic tool. Joints are actuated kinematically in a gravity-free scene; reaction is evaluated separately from imported inertial properties with user-confirmed module masses. Camera, adapter, magnetic-tool, and payload inertia are excluded as specified by the application. Their geometry is not thereby exempt from collision checks. Base flexibility, astronaut motion, actuator rotor dynamics, and physical contact impulses are outside the current model.

The planner receives RGB, axial depth, calibration, exposure timestamps, and robot joint measurements. It does not receive object poses, CAD shapes, or renderer instance labels. Environment truth is isolated to sensor simulation, actuator simulation, scene generation, and post-execution audit. Colored circular magnetic interfaces are a declared task prior; their positions must be estimated from images. This is not arbitrary semantic grasping.

An initial map may be loaded only with its raw RGB-D recording. The loader checks frame hashes, field whitelist, calibration identity, scene identity, proper camera rotations, and monotonically increasing timestamps before integration. File integrity does not establish acquisition authenticity or scene stationarity. Current initial scans are explicitly simulated external predeployment acquisitions with removable robot hardware absent; they are not autonomous wrist-scanning trajectories. The environment must remain unchanged before robot installation and startup.

The imported model contains sixteen mass-bearing modules per arm, each 0.25042 kg, for 4.00672 kg per arm including the stationary installation module. Stationary bodies contribute no motion-induced reaction. The masses follow the user-confirmed model convention; centers of mass, principal inertias and principal axes come from USD and have not been independently measured. Fixed bodies sharing a motion chain are aggregated using the parallel-axis theorem, while the original bodies remain available to the independent reaction calculation.

Reported measured-only experiments use headless USD joint kinematics and NVIDIA Warp ray-based RGB-D rendering, with the separate inertial-reaction model above. They are not measurements of a PhysX contact-force sensor or results of an IsaacLab reinforcement-learning policy. Joint feedback and hand–eye transforms are nominal in the simulation; depth noise, stereo correspondence and RGB registration are modeled, but the declared calibration-error budgets are not equivalent to injecting and validating all possible calibration errors. The composed robot and workcell USD assets remain available for scene inspection.

The nominal optical fields are 87 by 58 degrees for depth and 69 by 42 degrees for RGB, consistent with the manufacturer's published D435i specifications. [14] The implementation selects 848 by 480 depth and 1280 by 720 RGB at a nominal 30 Hz; online guidance requests arrive at 15 Hz and use crops with unchanged focal lengths, not fictitiously wider optics. Because RGB has a narrower field, depth-aligned colour has invalid outer regions; these black regions are not observations of black environmental surfaces. The real RGB sensor uses a rolling shutter, whereas the current renderer uses a single pose per exposure. Rolling-shutter motion, motion blur, material-dependent stereo failure, projector photometry and flight-environment lighting are not validated by this simulation. No millimeter-level hardware accuracy is inferred from the following reconstruction errors.

The reported completed pilots use a simulation motion clock, which pauses during stationary planning and map rebuilding. Their age bounds therefore do not establish wall-clock freshness. A subsequent implementation separates motion and monotonic request clocks, takes the maximum of their ages, and requires a new accepted exposure before execution, with unchanged geometric map revisions and a renewed geometric visual check. A retained target does not receive a new timestamp merely because another image arrived. This repair has software tests; closed-loop validation is separate from the historical results below. A fresh-exposure gate is not a proof of full-stack real-time schedulability.

## 3. Evidence map and geometry

### 3.1 Measurement integration

For a pixel \((u,v)\), axial depth \(z\), and OpenGL optical frame,

\[
p_c=[(u-c_x)z/f_x,-(v-c_y)z/f_y,-z]^T,\qquad p_w=R_{wc}p_c+t_{wc}.
\]

Self returns are removed using known robot geometry, not instance labels. Missing samples remain missing. A temporal pixel cache combines only identical optical poses and retains the minimum valid depth rather than averaging foreground and background into apparently free space. Near-identical poses do not borrow one another's evidence.

The map retains signed distance, integration weight, independent free-view counts, possible-surface flags, and local change epochs. A free-view vote is produced only when a valid depth patch covers the projected voxel and calibration-error footprint and its minimum depth is beyond the entire candidate cell. The declared axial-depth, translation, rotation, and intrinsic-pixel bounds are 8 mm, 3 mm, 0.2 degrees, and one pixel, respectively. They are engineering assumptions, not hardware-certified bounds; simulated Gaussian errors are not absolutely bounded either.

At least two independent free views are required. The original conservative query additionally rejects every cell ever intersecting a possible-surface interval. An experimental static-interval query distinguishes a possible surface location from definite occupancy: under the explicit unchanged-epoch assumption, independent whole-cell free certificates may disambiguate the former. It still rejects cells without two free certificates and cells with nonpositive measured TSDF. The two policies must be reported separately and independently audited. Neither policy treats unseen space as free.

### 3.2 Why local invalidation, not deletion of the old map

Unchanged measurements remain useful after an object moves. Deleting the entire map discards safe-space evidence needed to move either arm. Simply retaining the old map is also wrong: the object's old occupied volume is no longer current, and its new location may invalidate previous free evidence. The intended update invalidates both occupied and free evidence inside conservatively bounded action regions, clears affected independent-view bookkeeping, increments the map revision, and reconstructs those regions from new RGB-D. No region becomes free merely because a grasp or placement succeeded. Unannounced external motion requires a separate change detector and is not covered by a static-segment certificate.

#### Change-support refinement

The current development revision further represents physical change support by the measured uncertainty-inclusive convex containment, rather than all corners of its enclosing AABB. For a voxel centered at c with edge lengths s and a containment plane n_j^T x+d_j<=0, n_j^T c+d_j-|n_j|^T s/2>1e-6 m certifies whole-voxel separation from that plane. Only voxels not separated by any containment face are retired as potentially intersecting. Omitting edge-axis separation tests can only over-retire evidence. This shape-aware operation applies at both source capture and destination release, with vertices retained in the action log for subsequent raw-map history replay. The distance-band contradiction rule below is unchanged. Earlier nominal reference results used the recorded older AABB update. A later successful single-object development mission used the refinement, but no isolated causal benefit is established by that success.

The release-stage update now distinguishes a physical change support \(\Omega_b\) from its larger TSDF influence band \(\Omega_\phi\). The physical support must conservatively contain the measured object and pose uncertainty. Resetting distance weights across the entire band need not erase already-certified air outside this support. However, simply retaining every old free vote could turn a previously nonpositive TSDF cell into free space when its weight is reset. We therefore additionally revoke evidence in \(\{x\in\Omega_\phi:W(x)>0,D(x)\le0\}\). Distances and weights reset in \(\Omega_\phi\); free votes, view bits, and surface flags reset only in the union of this contradiction set and \(\Omega_b\). Other evidence is retained. Before any new observation, the operation cannot enlarge the certified-free set: revoked cells have zero votes, untouched cells are unchanged, and retained-band cells already satisfied the former distance-sign condition. This is a conditional evidence-preservation property, not proof of unseen object containment or arbitrary dynamic-scene safety.

### 3.3 Whole-segment checks

Before describing motion certificates, we distinguish a persistent measured-object surface from the free-space evidence volume. In random development scene 6100, the final local TSDF had a measured-vertex distance P95 of 0.921 mm, but only 65.442% of 50,000 true object-surface samples were within 5 mm of the reconstruction. The truth object was transformed to its actual release pose for this audit. High precision of retained vertices therefore did not imply complete post-release geometry.

The subsequent implementation preserves the observed connected object's vertices and original triangles at capture, excluding the hypothesized support-plane extension and convex-envelope padding. For known capture and subsequent tool transforms, the retained world surface is \(p_t=T_tT_c^{-1}p_c\). Gradients and Hessians transform as \(g_t=Rg_c\) and \(H_t=RH_cR^T\) in the corresponding local/world frames; invalid stencils remain invalid and curvature is invariant under the rigid transform. This layer assumes rigid attachment, correct component ownership, and known kinematics. It is not a direct new exposure at the release pose, a completed underside, or a watertight solid. It adds **zero free-space votes** and does not replace the evidence volume used to certify motion. Its coverage benefit must be evaluated using new paired surface audits, not assumed from the implementation.

Let \(U\) be the union of unknown and blocked cells and \(H_i(q)\) a known robot convex component. A sufficient joint-box condition is

\[
\bigl(H_i(q_c)\oplus B(m+b_i(\delta))\bigr)\cap U=\varnothing.
\]

For a correlated joint line \(q(u)=q_a+u\Delta q\), define \(\beta=\sum_j|\Delta q_j|\). Serial rotation-product differentiation gives a global pointwise bound \(\|p''(u)\|\le R_i\beta^2\), where \(R_i\) is constructed from known chain lengths and component extents. Linear-interpolation error therefore satisfies

\[
\|p(u)-(1-u)p(0)-up(1)\|\le R_i\beta^2/8.
\]

The endpoint-component convex hull, enlarged by this remainder and the 5 mm planning margin, covers the swept component. Endpoint face normals define a conservative outer approximation. For cell center \(c\), cell edge vector \(s\), unit normal \(n\), and endpoint support \(h\), potential intersection requires

\[
n^Tc\le h+\tfrac12|n|^Ts+m+R_i\beta^2/8.
\]

If any separating direction excludes a cell, it cannot intersect that envelope. Remaining candidate intersections trigger subdivision or rejection; exhaustion of the node or cell budget does not authorize motion. Self, inter-arm, and mounting-interface checks are independent additional requirements.

The magnetic tool can be represented by a conservative axial convex cover of its known closed triangle mesh instead of one large convex hull. Every triangle intersecting a slab contributes all its vertices, preserving a cover of each solid cross-section. This is robot-model refinement, not environment completion.

### 3.4 Near-contact refinement

The current optional near-contact design retains the broad 8 mm map for other links and reconstructs a target-centered 2 mm volume from the same authenticated raw measurements. The complete tool sweep must pass the fine volume; it cannot inherit permission from a coarse free cell. Both revisions are recorded.

The current development revision refines a voxel that straddles the existing contact-footprint boundary without enlarging that footprint. For each forbidden footprint halfspace, its intersection with the voxel is obtained from original corners and voxel-edge/plane intersections. A directly verified unit-axis projection gap must exceed the original planning margin plus sweep remainder between the robot convex envelope and every such forbidden solid. The union of these solids contains the voxel portion outside the permitted footprint; their separation therefore certifies that any remaining intersection is inside the original contact set. The complete voxel must still meet the original vertical-band and local-radius bounds. Refinement is restricted to the magnetic tool and at most 32 residual cells; insufficient separation or exhausted budget remains rejection. It neither modifies map evidence nor grants whole-voxel free space. This conditional contact-query refinement was not used in the historical nominal and first-pilot results.

Intentional magnetic contact is a separate permission, not a free-space certificate. It is restricted to the magnetic tool, an observed target footprint, a bounded top band, and a near-vertical approach corridor. Measured object envelopes assume a connected object supported by an observed plane; they do not prove containment of an unseen shape. The envelope padding includes axial-depth ray scale, translation and rotation error, intrinsic uncertainty, and voxel quantization. An isolated, static, horizontal interface is an explicit hypothesis. Independent truth checks remain necessary to validate the simulation cases, and do not feed geometry back to the planner.

The same bounded intersection logic is subsequently applied to the existing source/support contact column for the tool and held payload, with its original vertical band and footprint. In addition, a contact-corridor check subdivides a rejected joint interval before concluding that its curvature remainder is too large. Every accepted leaf still uses the original rotation-product remainder and must keep the TCP inside the 8 mm lateral band and the payload above the original bottom minus 2 mm. At most 127 intervals and depth eight are allowed; an actual violation or budget exhaustion rejects the proposal. This reduces conservative rejection without expanding either contact permission.

An exact query reordering addresses repeated enumeration of the permitted source volume. For unsafe-cell set U, wholly permitted contact cells A, and a swept hull H, testing the intersection of H with U minus A is equivalent to filtering A after intersecting U with H. A private, read-only query grid caches this existing permission only for the named tool/payload; original TSDF, free votes and snapshot fields are unchanged. The bounded mask construction examines at most three million cells in chunks of 65,536. Forbidden-cell geometry queries retain their original one-million-cell limit. Other links and transport without contact receive no such mask. A private contact permission must never be exported or reused as observed free space.

#### Euclidean error-envelope representation

The measured surface padding is a Euclidean distance bound, but the historical implementation added an axis-aligned cube at every core vertex. Cube corners can introduce unnecessary occupied volume when that envelope is transported and rotated. A later bounded supporting-plane outer approximation preserves the same radius and every original measurement/calibration allowance. For unit directions n_j and measured core P, it uses

\[
\widehat E=\bigcap_j\{x:n_j^Tx\le\max_{p\in P}n_j^Tp+r+10^{-8}\}.
\]

Every point in conv(P) plus a radius-r Euclidean ball satisfies every inequality; finite direction selection therefore remains an outer approximation. Cartesian directions guarantee boundedness. At most 96 planes are selected from 26 fixed axial/diagonal directions and angularly separated measured-core face normals; the bounded three-dimensional polytope has at most 188 vertices. Numerical output faces are checked against the exact core-plus-ball support. Unseen-object containment remains a separate hypothesis and independent audit requirement. The open measured surface, free-space evidence and original 5 mm robot clearance are not replaced by this envelope.

### 3.5 On-demand complete-component evidence refinement

A failed broad query can arise because an 8 mm cell is not wholly observed even when the small region needed by the robot is measurable at finer resolution. The development revision retains one optional 2 mm robot-component block, distinct from the tool/payload block. It replays the same authenticated initial RGB-D, applies every recorded object-change event, and then integrates a new full frame. Its ROI is determined from the known rejected robot component pair and a 40 mm border, not from environment CAD. Memory is bounded at two million cells and three block constructions per task, with only one active robot block. Subsequent measurement integration and evidence invalidation include this block.

Before querying occupancy, complete components are assigned by geometric containment. If the endpoint-component convex envelope enlarged by the original motion remainder and planning margin lies strictly within the block, that entire component is checked against the fine evidence. Otherwise the original broad query owns that component. A fine-assigned unknown cell cannot fall back to a favorable coarse answer. Other components and the payload retain their separate complete checks. The certificate records both the block generation and its revision; replacement or integration revokes previous permissions. This is a local resolution refinement of the same measurement model, not free-space completion, a second learned estimator, or selection between conflicting favorable answers. It provides a bounded conditional certificate, not a guarantee that enough RGB-D evidence or an admissible route exists.

## 4. Visual sufficiency and combined reaction

An image-derived target is \((\hat p,\Sigma,N,t_o)\). Camera features are normalized image coordinates, range \(r\), and elevation sine \(e\). The visual set limits image extent, range, elevation, projected support, covariance, and observation age. Current thresholds are twelve supporting pixels, maximum standard deviation 5 mm, age 0.25 s, range 0.45–1.2 m, and elevation sine at least 0.30. A three-sigma support term is a local design surrogate, not a calibrated probability guarantee.

An interval certificate can cover these geometric proxies throughout a joint segment, but cannot certify arbitrary occlusion. Fresh image measurements are therefore checked during execution. A stale target or sensor exception triggers a smooth stop along the already certified static path, rather than an instantaneous zero-velocity command.

In the common C frame, combined momentum and reaction wrench are

\[
h_C=A_L(q_L)\dot q_L+A_R(q_R)\dot q_R,\qquad
W_C=-A(q)\ddot q-\dot A(q,\dot q)\dot q.
\]

A body's momentum contribution contains \(mJ_v\) and \(I_wJ_\omega+[r]_\times mJ_v\). All terms are transformed into the same reference frame before summation. An independent Cartesian Newton–Euler calculation checks this implementation.

For a fixed inertial reference and the stated no-external-load model,

\[
\int_{t_0}^{t_1}W_C(t)\,dt=-[h_C(t_1)-h_C(t_0)].
\]

Consequently, **every rest-to-rest trajectory has zero exact net model impulse**, regardless of whether its instantaneous loading is large. Small numerical net impulse is therefore primarily an integration/consistency check, not evidence of reactionless cooperation. We instead evaluate six-axis RMS and peaks, integrated magnitude, and the maximum norm of a sliding 0.25 s impulse. Physical attachment impacts are excluded and cannot be inferred from this identity.

With the execution-arm increment fixed, the camera-arm increment \(x\in\mathbb R^8\) solves a bounded local problem

\[
\min_x\|Mx+y\|^2+10^{-3}\|x\|^2,
\quad \ell\le x\le u,\quad g(q_L)+Gx\ge0.
\]

The objective linearizes normalized six-axis wrench at three unit-time quintic nodes, using force and torque scales 0.35 N and 0.15 N·m. Joint increments are bounded by 0.12 rad. OSQP uses at most 400 iterations; every accepted feasible iterate is rechecked. The pose-preservation baseline adds \(J_{cam}x=0\). The visual-set feasible region contains that equality-constrained region under identical linearization and other constraints, so its globally optimal local objective cannot be worse. This elementary inclusion result does not imply that approximate solutions or complete nonlinear tasks improve every measured reaction metric.

The set policy also includes the pose proposal and finite scalings in its candidate portfolio. Candidates are ranked by their actual sampled wrench cost; the first satisfying environment, robot, and visual certificates is selected. Both policies use the same minimum 1.2 s segment duration. Zero reaction is possible only if the bounded task/safety feasible set intersects the reaction-cancellation equations; joint count or an unbounded rank test is insufficient.

## 5. Execution algorithm

![Algorithm and data-authority schematic, not an experimental image. Both arms contribute to the modeled C-frame reaction. Environment geometry comes from retained RGB-D only; known robot geometry also enters the independent collision checks. Dashed feedback denotes new measurements after motion, not predicted depth.](../../output/figures/rgbd_architecture_02.png)

```text
Authenticate initial RGB-D; reconstruct evidence map
Select and certify a robot-ready RESET condition, not an unobserved startup path
Acquire current RGB-D and estimate a magnetic interface
while the current task phase is incomplete:
    use an image-derived target hypothesis for bounded proposals, not execution authority
    propose a small execution-arm IK increment
    generate bounded visual-set / pose-preserving camera-arm candidates
    rank candidates by evaluated combined reaction cost
    require whole-segment environment, robot, and geometric-visual certificates
    if no candidate passes: attempt bounded evidence refinement or certified rescan, else stop
    reacquire RGB-D at rest; require motion-clock and wall-clock freshness and quality
    renew geometric-visual checks and verify unchanged geometric-map certificate revisions
    if this execution gate fails: reject the unexecuted proposal and reobserve, else stop
    execute both arms on a smooth common clock
    receive new RGB-D asynchronously; brake on stale/failed feedback
    at a C2 rest endpoint: commit map updates and flush raw recordings
    before contact: require the separately logged measured-contact permission
    after scene-changing actuation: revoke affected evidence before reuse
```

The quintic time law is \(s(u)=10u^3-15u^4+6u^5\). Duration enforces velocity, acceleration, and jerk limits of 0.3 rad/s, 0.5 rad/s², and 5 rad/s³. Raw RGB-D acquisition and estimation are in a separate process. The current architecture retains endpoint pauses for planning, spatial commits, and storage; continuous streaming between endpoints is not whole-stack hard real time.

### 5.1 Bounded branch selection and task transition

The execution-arm IK search is restricted to ±0.30 rad per joint around the current measured joint state. A candidate must certify the entire current-to-candidate joint line with the camera arm stationary, not merely its endpoint. This is a conservative branch-selection filter; coordinated camera motion is checked subsequently. If the finite IK search fails, at most four offending right-arm link components and twenty measured blocked/unknown cell centers per component define an additional separation residual at the joint-line midpoint and endpoint. This residual only proposes IK solutions. It does not change the map, expand free space, or replace the complete certificate.

The current right-arm endpoint task preserves the captured world-vertical rotation row and leaves world-Z yaw free (Section 5.6). All robot, payload, environment, contact-corridor and visual checks remain mandatory. Earlier frozen trials instead used full-attitude or discrete-yaw tasks; their version-specific results are not reassigned to this revision.

Height is proposed from the measured surface maximum, payload extent relative to the captured tool, and the locally retired source-volume boundary. A bounded downward height proposal is permitted only within the measured clearance band and after the same whole-robot and payload checks. Historical placement estimates may propose an ROI or gaze during source-to-destination handover, but only a newly acquired depth-supported placement estimate authorizes the new visual task. A source that is fresh but outside the operational visual set first requires sensing-only restoration; a quality recovery may monitor another sufficiently measured visual witness. The execution arm remains stationary throughout these recoveries. These mechanisms are finite and can legitimately fail.

After release, the measured object envelope is transformed by the known rigid capture-to-release tool transform. The map update in Section 3.2 is applied before withdrawal. A separate measured-contact permission permits initial tool separation from the released interface; the released object is no longer masked as an attached payload. The full tool cover must again pass the fine map. This is not an exemption for the entire arm or an assumption that the placed object has disappeared.

### 5.2 Sensing-only feasibility restoration between objects

Seeing an interface does not necessarily satisfy the operational visual set. For example, the next interface in a retained nominal multi-object attempt was freshly detected but had elevation sine 0.247 against the 0.30 requirement. Rotating the optical axis alone cannot increase this positional elevation. With the execution arm at C2 rest, a bounded eight-variable restoration subproblem proposes

\[
\min_x\|x\|^2,\qquad \ell\le x\le u,\qquad g(q_L)+Gx\ge0.005\mathbf 1.
\]

The camera increment bound remains 0.12 rad. Only proposals meeting the original nonlinear endpoint visual set and complete environment/self/inter-arm checks may execute. This is explicitly a sensing-only phase, not a claim that the operational visual set held throughout the acquisition path. The execution arm cannot move, and fresh feedback and smooth braking remain active. Operational manipulation resumes only after a new sufficiently supported measurement passes the unchanged set. Finite IK-based gaze proposals remain a fallback. Neither the restoration step nor queue exhaustion proves that all objects in the scene have been discovered.

### 5.3 Measured placement-center estimation

The placement prior is an axis-aligned square marking with 90 mm bar-center separation, not known object dimensions or a supplied destination pose. After RGB-D grouping, each observed border point is assigned to its closest horizontal or vertical bar. The center coordinates are robust medians of \(p_{ij}-s_i(0.045\mathrm m)\), where \(s_i\in\{-1,+1\}\) denotes the observed bar side. At least three supported bar identities, both coordinate directions, eight samples per direction, and 80% residual support within 8 mm are required. Four bounded reassignment steps are used. A center shift exceeding 15 mm from the observed extent initialization is rejected. Neighboring non-marking depth measures the support height; an unobserved underside is not assumed.

The earlier estimator used point-set covariance divided by sample count although its center came from extent quantiles. That quantity included the physical 90 mm border spread and did not describe this center estimator. The revised local variance uses \((\pi/2)\hat\sigma_j^2/n_j\) for each median coordinate, with robust residual scale at least the known 4 mm bar-width scale \(0.004/\sqrt{12}\). A 2 mm common-mode floor and the simulated depth-noise term are retained without dividing them by the number of points. Pixel independence and these floors are not hardware calibrated; the result remains a design uncertainty model, not a certified probability statement. The 5 mm gate is unchanged.

A development replay of twenty retained full frames from nominal multi-object run 05 yielded 51 estimates with horizontal nearest-marking error P95 1.915 mm and maximum 2.917 mm. Truth marker centers were consulted only after estimation. The old recordings did not retain every sensor-side variance/mask array, so this replay explicitly reconstructs depth sigma from noisy saved depth and self masks from known robot geometry. It does not establish exact old-pipeline replay or closed-loop superiority. New recordings retain these arrays. The unchanged freshness, pixel-count and covariance conditions are now also monitored during streaming, with a smooth certified-line brake if they fail; full-frame remeasurement at rest precedes any resumed task increment.

### 5.4 Recovery when the task marker itself is insufficient

A retained multi-object development trial stopped after completing one object and attaching the next: its destination was still observed, but the largest covariance eigenvalue exceeded the 5 mm standard-deviation gate. Requiring that same marker to satisfy the ordinary recovery constraints prevents recovery from starting. We do not enlarge the covariance threshold or relabel an old observation as current. At a C2 rest, a separate sufficient, fresh measured interface can instead serve as a temporary visual witness for a camera-only rescan aimed at the original measured destination. The execution arm remains stationary, including its held object's geometry, and the unchanged robot/environment and witness-view certificates apply. After the rescan, the controller restores the original task identity and requires a new sufficient original-target measurement before proposing an execution-arm increment. If no such witness or certified scan is available, it stops. This bounded fallback is not a claim that a suitable witness always exists.

### 5.5 Sensing-only return after complete task-marker loss

The current development revision handles an additional deadlock: a certified camera-only rescan can lose every task marker because geometric visual proxies do not certify occlusion. At C2 rest, a bounded return to that scan's actual starting camera configuration may be proposed only if the execution arm was and remains stationary and the stopped state lies on the original sensing joint line. The reversed subsegment must pass newly evaluated current-map and robot/payload certificates. It is a distinct acquisition mode: task-marker sufficiency is not asserted, while raw RGB-D frame liveness and sensor-exception braking remain active. A new sufficient task measurement is mandatory before any execution-arm increment resumes. At most two returns are allowed, and a return cannot itself be reversed as another recovery. Historical target stamps remain unchanged. Unit tests and saved-map certificates do not establish closed-loop recovery success; new trials are reported separately below.

### 5.6 Minimal right-arm task orientation

The horizontal magnetic transfer task does not prescribe world-vertical yaw. The revised right-arm endpoint objective retains TCP position and the captured third row of its rotation, rather than all three orientation degrees of freedom. Let the captured rotation be R0 and a measured payload point in TCP coordinates be p. A world-Z rotation satisfies

\[
e_z^T R_z(\psi)R_0=e_z^T R_0,
\qquad z(p)=e_z^T R_0p+t_z.
\]

Thus the task can impose the two independent constraints of a unit vertical row while releasing yaw. The bounded IK residual is

\[
r(q_R)=\left[12(t(q_R)-t_d)^T,\ 2(e_z^TR(q_R)-e_z^TR_0),\ 0.0005(q_R-q_{init})^T\right]^T.
\]

The accepted endpoint row error is at most 0.001, so the rotational contribution to each point-height discrepancy is at most 0.001 times its TCP-relative radius. This is an endpoint property; nonlinear joint interpolation need not keep the row exactly fixed. All original swept robot/payload, source-contact corridor, measured-space, fresh-vision and joint-increment checks therefore remain mandatory. The captured row is not replaced by successive estimated endpoints, preventing accumulated reference drift. The ordinary unattached approach retains its original downward-axis task. Here q_init is the initialization of the current bounded IK restart. This change removes four discrete transport-yaw fallback searches rather than adding another planner. Its separate development evaluation is reported below.

### 5.7 Occlusion-aware bounded rescan ordering

When rejected measured-space cells supply a sensing frontier, the current proposal rule ranks six small camera offsets instead of always trying upward motion first. It uses only known right-arm convex components and the currently held measured envelope. At most 20 rejected cells are expanded into a finite center/corner ray bundle (at most 180 rays); this bundle is a heuristic, not the uncertainty certificate used by the collision guard.

For a ray x(s)=c+s(p-c), 0<=s<=1, and convex component n_j^T x+d_j<=0, substitution gives intervals from the scalar inequalities

\[
\alpha_j+s\beta_j\le0,\qquad
\alpha_j=n_j^Tc+d_j,\quad\beta_j=n_j^T(p-c).
\]

Intersecting these intervals yields the component's occluded ray segment. Candidates are ordered first by visible-ray fraction and then by mean summed occluded length. The latter supplies a ranking direction even if all nearby candidates remain behind the payload silhouette. Component overlaps can double-count length; no exact optical-probability interpretation is claimed. Two additional offsets provide downward and backward/downward proposals. Every original whole-segment environment, robot and visual check remains mandatory; the current task-scaled proposal budget is specified in Section 5.8. Actual new RGB-D, not this visibility score, decides whether the formerly blocked space gains evidence. Environmental occluders not represented by robot/payload geometry can still defeat the proposal.

### 5.8 Task-scaled finite sensing allowance

The default mission selects the nearest available RGB-D placement marking; historical estimates propose a gaze/ROI only. Each observed object task receives at most six accepted rescan proposals, including its incoming visual handover. A new allowance is created only after the previous object completed release and withdrawal and the next interface is already present in measured task memory. Repeated attempts on the same interface cannot renew it. With a configured cap of eight object cycles, there are at most 48 such proposals, without supplying the true object count. The proposal counter is distinct from actual execution because a later freshness check may still reject a proposed segment. Original per-attempt IK, swept-geometry, visual and braking conditions remain unchanged.

## 6. Experimental protocol

The final protocol must freeze implementation and acquisition rules before holdout evaluation. Every generated scene remains in the denominator, including unobservable, unreachable, timeout, contact, or collision-audit failures. Object shape, size, pose, spacing, count, and support height must be randomized within declared reachable ranges without using truth-based admission to favor one method. Each scene requires its own raw initial scan. Paired comparisons must use matched task conditions and report both all-proposal success statistics and common-success conditional metrics. Frames from one trajectory are not independent scene samples.

Required ablations include pose preservation, no reaction optimization, static-veto versus interval fusion, scan budget, local refinement, sensor loss/occlusion, and local versus whole-map invalidation. Unimplemented methods must not appear as completed baselines. Historical CAD-assisted delivery trials are separate evidence and cannot fill missing RGB-D-only results.

## 7. Verified development results

### 7.1 Complete measured-only three-object mission

The nominal run `reports/rgbd_nominal_multi_mission_08` completed the entire observed-interface queue without resetting either arm or the broad evidence map. Only the independent auditor subsequently read the scene count and verified that all three actual objects had been delivered. Every object's release, its own completion event, and actual executed withdrawal state were checked separately. An earlier object's completion flag cannot validate a later object's interrupted retreat. The controller had no object-count input.

| Actual object, audit only | Horizontal placement error / mm | Bottom-to-support gap / mm | Actual vertical withdrawal / mm | Actual hull vertices inside measured envelope |
|---|---:|---:|---:|---:|
| Bracket | 0.607 | 1.084 | 80.000 | 44 / 44 |
| Washer | 0.857 | 1.081 | 80.000 | 96 / 96 |
| Bolt | 0.690 | 1.161 | 80.000 | 160 / 160 |

All 169 full-segment geometry checks passed against the independent simulation collision solids, with released objects fixed at their recorded release transforms during later checks. All 12,933 midpoint reaction samples passed the independent Newton–Euler calculation; maximum component discrepancy was 9.000e-7 SI units. Terminal velocity and acceleration were zero within tolerance. Both arms moved on 11,613 ticks, while required sensing-only phases held the right arm stationary. Motion time was 215.308 s. Force RMS/peak were 0.09305/0.39866 N, and torque RMS/peak were 0.04434/0.28072 N m. Maximum 0.25 s force/torque window impulses were 0.09327 N s and 0.06568 N m s. These are absolute values from one development scene, not a percentage improvement over a shorter task or evidence of zero reaction.

Streaming P99/max were 4.437/6.481 ms; observation age P95 was 0.0667 s. However, task wall time including initialization was 766.081 s, before final artifact export. The asynchronous sensing/control loop does not eliminate full-frame acquisition, local raw replay, endpoint planning, and storage waits. Named timing intervals are inclusive and overlap. We therefore report simultaneous online feedback during motion, not uninterrupted whole-stack hard-real-time planning.

![Measured RGB-D colour, executed robot kinematics, combined six-axis inertial reaction and visual guidance for the complete three-object run. The gray cloud is the initial measured surface, not a final fused-map claim.](../../reports/rgbd_nominal_multi_mission_08/rgbd_guidance_summary.png)

### 7.2 Surface persistence at the exact final scene state

The same final 8 mm workspace TSDF was audited without and with its separately retained measured object triangles. Both variants use the exact same certified-free grid and the same 50,000 area-weighted true object-surface samples. Aggregate coverage within 5 mm increased from 46.892% to 77.242% (+30.350 percentage points), and coverage-distance P95 decreased from 27.181 to 13.666 mm. Measured-vertex precision P95 was 7.428 mm without memory and 6.585 mm with memory. These are broad-map values, not millimeter-level local-map accuracy.

| Object, audit only | Current TSDF coverage within 5 mm / % | With measured-surface memory / % |
|---|---:|---:|
| Bolt | 31.930 | 78.470 |
| Washer | 45.690 | 73.190 |
| Bracket | 58.825 | 79.380 |

Each per-object entry uses 20,000 area-weighted surface samples and a 100% ROI fraction; the samples are not independent manipulation trials. Neither variant placed a sampled true part surface in certified-free space or admitted a sampled known-colliding sphere. Finite probes cannot establish universal collision safety. The last 2 mm local-map snapshot had measured-vertex precision P95 0.917 mm without memory and 0.935 mm with memory; its 5 mm coverage increased from 48.377% to 77.364% on that ROI's shared sample set. Adding measured memory can improve completeness while slightly worsening local vertex precision. Earlier cycle snapshots retain their own export-time event history, avoiding comparison with later object poses.

The final broad map reached its declared 252 independent-view capacity, compared with 224 used by commissioning. Subsequent new poses cannot add free votes beyond that capacity; this is conservative but can reduce long-mission coverage and feasibility. The result therefore does not establish unbounded-duration mapping. Current 1–3-object random trials retain the same capacity for every policy.

![Actual measured triangles and final-state coverage, with no CAD surface or invented closure in the visualization. Top panels crop the operation area; metrics use the full audited map.](../../reports/rgbd_nominal_multi_mission_08/rgbd_surface_comparison.png)

### 7.3 Recorded-feedback fault injection and inertial sensitivity

The actual streaming controller was replayed on the first ordinary approach/lift/transport/place/retreat segment for each of the three object cycles: 15 recorded segments. Four faults were injected at 20%, 50%, and 80% of each segment: missing measurements, pixel support reduced to three, standard deviation raised to 8 mm, and sensor-worker exception. All 180 planned cases triggered the corresponding stop, remained monotone on the original certified joint line, and ended at C2 rest. No replayed packet was accepted before its original receive/exposure time. Maximum detection delay was 0.2154 s for stream absence and one 60 Hz period (16.67 ms) for the other faults. Maximum time from fault onset to rest was 1.0022 s for missing feedback and 0.9375 s for the other faults. Braking is not instantaneous; its geometry inherits safety only under the original unchanged-scene/ownership certificate.

These are controller and recorded-feedback fault-injection experiments, not newly rendered closed-loop occlusion-and-recovery trials. Wall pacing was disabled only in the offline replay process; those runs provide no computational timing benchmark. The new robot/payload masks and measurement noise fields are retained with raw RGB-D so future replay does not silently substitute different filtering.

An additional 32-draw engineering sensitivity analysis reused all 12,933 recorded midpoint states. Original module masses were unchanged; each module's center of mass was independently shifted up to 5 mm per coordinate, its positive inertia scale varied in [0.8,1.2], and its principal frame rotated by at most 5 degrees before parallel-axis aggregation. Force RMS ranged from 0.09239 to 0.09354 N and torque RMS from 0.04412 to 0.04475 N m. Force and torque peaks ranged from 0.39556 to 0.40109 N and 0.27866 to 0.28277 N m. This fixed-trajectory sensitivity is neither a calibrated physical uncertainty distribution nor proof that a perturbed closed-loop controller would execute the same task safely. Excluded tool/camera/payload inertias remain excluded.

### 7.4 Complete-cycle matched nominal comparison

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

### 7.5 Frozen development comparison

All nine trials in `reports/rgbd_paired_mission_pilot_02` finished with independent audits and unchanged source hashes. The scene-level success counts were set 1/3 (95% Wilson interval [0.061, 0.792]), differential pose 2/3 ([0.208, 0.939]), and hold 1/3 ([0.061, 0.792]). These reused development inputs are not held-out observations and must not be pooled with the earlier source version.

| Scene | Set | Differential pose | Hold |
|---|---|---|---|
| 6101 | Failed second-object lift | Failed second-object transport | Failed second-object lift |
| 6102 | Failed placement | Complete one-object scene | Failed placement |
| 6103 | Complete two-object scene | Complete two-object scene | Complete two-object scene |

The only common successful scene is 6103. Its complete-task paired comparison is therefore one scene, not a population estimate:

| Metric | Set | Pose | Hold |
|---|---:|---:|---:|
| Force RMS / N | 0.09205 | 0.08949 | 0.09118 |
| Force peak / N | 0.27259 | 0.25896 | 0.23026 |
| Torque RMS / N m | 0.04220 | 0.04247 | 0.04806 |
| Torque peak / N m | 0.18519 | 0.10871 | 0.13147 |
| Normalized mean-square reaction | 0.14831 | 0.14552 | 0.17052 |
| Motion time / s | 155.889 | 143.046 | 139.089 |
| Task wall time including setup / s | 458.939 | 419.181 | 396.560 |

Relative to hold, set reduced torque RMS by 12.19% and normalized mean-square reaction by 13.02%, but increased torque peak by 40.86%, force peak by 18.39%, and motion time by 12.08%. Relative to pose, its normalized mean-square reaction was 1.92% higher and torque peak 70.36% higher. These adverse outcomes cannot support uniform low-reaction superiority. Failed tasks remain in the denominator and are not assigned zero complete-task cost. Brief CPU-only manuscript layout work overlapped part of the batch; timings describe observed workstation performance, not a dedicated uncontended deadline benchmark.

The repeated failures include boundary-cell contact rejection and exhausted measured-space planning/recovery. A bounded intersection refinement leaves every forbidden portion of a contact-boundary voxel subject to the original hull clearance, rather than requiring the entire voxel to lie in the contact footprint. A saved-map 3 mm lift proposal for scene 6101 passes after this refinement; it is not an executed trajectory. Post-pilot wall-clock and contact changes are under separate closed-loop evaluation and do not alter these frozen statistics.

### 7.6 New-render sensor-fault evaluation

The implemented sensor-side protocol rerenders each online image at the actual robot state and retains both the original render and estimator input for affected packets, including replies subsequently discarded by latest-packet semantics. On the reused development scene 6102, the fixed cases are nominal operation, RGB-D blackout, RGB-only blackout and 0.3 s added delivery delay. A fault starts on the first request at motion time at least 5 s and lasts 0.6 s of monotonic wall time; stopping motion cannot freeze its duration. Original exposure times are never refreshed. This is optical/sensor-stream corruption, not a newly inserted physical occluding body.

All four scheduled cases in `rgbd_rendered_stress_development_01` completed their actual one-object scene with unchanged runtime sources. The three corrupted runs each executed a visual-loss brake and subsequently completed placement and withdrawal. Independent checks covered before/after pixels, recorded exposure clocks, complete geometry, actual release/withdrawal, and every logged wrench sample. The data are newly rendered online, not edited prior trajectories.

| Condition | Executed segments | Wrench samples | Placement XY error / mm | Motion / s | Task wall before final export / s |
|---|---:|---:|---:|---:|---:|
| Nominal | 52 | 3,811 | 2.260 | 63.452 | 199.403 |
| RGB-D blackout | 54 | 3,947 | 2.282 | 65.743 | 209.353 |
| RGB-only blackout | 54 | 3,947 | 1.984 | 65.735 | 207.489 |
| Added 0.3 s delay | 53 | 3,881 | 1.957 | 64.614 | 204.011 |

The complete recorded-clock audit covered 4,339 online raw files. Fault-specific archives additionally retain three, three and two affected before/after renders, plus the first cleared render per condition. The same early approach state produced a 37-tick C2 brake in each corrupted case, from motion time 5.250 to 5.843 s. Reconstruction of position, velocity and acceleration agreed with the original certified line and braking polynomial to 1.47e-14; exact polynomial joint speed, acceleration and jerk maxima were 0.17442 rad/s, 0.49819 rad/s2 and 4.78187 rad/s3. The line parameter stayed between 0.27521 and 0.75438. All three ended at rest before subsequent reobservation and replanning; this is not instantaneous stopping or continued blind task execution.

Maximum normal-motion target ages were 0.12604, 0.23676, 0.23795 and 0.23679 s, respectively; braking may last longer than image freshness. Maximum independent wrench discrepancy across these runs was 7.65e-7 SI. Brief CPU-only diagnostics and document layout overlapped parts of the batch, so observed wall costs are not uncontended scheduling guarantees. One reused scene per condition establishes these three recovery executions, not a population robustness rate, a physical occluder test or a held-out generalization result. The separate two-object occlusion-ranking run still stopped in placement and remains an unsuccessful mission.

![New-render sensor faults on development scene 6102. Gray bands show the 0.6 s sensor fault; dotted lines mark actual braking onset and dashed lines the 0.25 s normal-motion age limit. Only recorded motion ticks are plotted; gaps are not interpolated. Braking can outlast image freshness. All three runs subsequently completed and passed independent audits.](../../reports/rgbd_rendered_stress_development_01/rendered_stress_summary.png)

### 7.7 Subsequent functional revisions and limits

Later frozen two-scene trials of vertical-row IK and Euclidean outer envelopes each completed the one-object scene 6102, but still stopped in the second placement of scene 6101. In the Euclidean-envelope batch, independent recorded-clock checks covered 110/52 segments and 2,382/1,067 raw records. Preserving the same error radius while tightening redundant padding therefore did not alone establish multi-object reliability.

A further measured-mark dispersion heuristic completed scenes 6102 and 6103 but regressed scene 6101 to a first-object placement stop. Its three recorded-clock audits passed 1,174/1,082/2,418 raw records; failure remains failure despite valid clocks and executed-path geometry. This heuristic was removed from the default mission. Its success in scene 6103 is not a matched reaction comparison or evidence that farther destinations are universally better.

The current development revision returns to the nearest available measured mark and allocates the same six rescan proposals separately to each newly observed object task. The earlier first object could exhaust the run-global allowance before the second object began. Same-task retries cannot reset the new allowance; the bounded mission cycle cap remains unchanged. Its new scene-6101 run completed both objects: 114 audited segments, 8,468 independently checked reaction samples (maximum component error 7.05e-7 SI), horizontal errors 1.679/2.026 mm, and 80 mm withdrawal for each object. The recorded-clock audit passed all 2,358 raw records, with maximum normal-motion target age 0.140290 s. Only one rescan proposal was used before the first release, and none in the second task. Therefore this success does not isolate a causal benefit from renewing the second-task budget; asynchronous accepted measurements and resulting planning paths can differ between runs.

The other scheduled scene, 6103, also completed both objects: 117 audited segments, 8,751 reaction samples, horizontal errors 1.945/3.556 mm, and both withdrawals of 80 mm. All 2,437 raw-clock records passed, with maximum normal-motion target age 0.185896 s. Again, only one rescan preceded the first release and none was needed in the second task. These two reused development scenes are not an estimate of held-out success.

On scene 6101's final fine map with retained measured surfaces, surface precision P95 was 1.024 mm. Equal 20,000-point whole-object samples gave 5 mm coverage of 85.325% and 83.360%. The sampled colliding-sphere and 50,000 true-object-surface probes found no falsely free samples. Precision, incomplete coverage and finite collision probing have distinct denominators; zero sampled violations is not universal safety.

Full-project regression passes 479 tests. A preceding 474-test source snapshot passed after relocation, with all 12 scene USD layers resolving inside the copied workspace; its archived source and packaged asset hashes are retained. A separate 32-frame RGB-D acquisition also ran from that relocated workspace, using its own compiled ray-rendering kernel cache. This is same-interpreter source/asset relocation, not a fresh Python/CUDA installation.

Detailed earlier failures, source-specific clocks, saved-state probes and the unsuccessful destination heuristic remain in the supplementary development record (`RGBD_PILOT_DIAGNOSTICS.md`). They are not pooled with frozen comparative trials.

### 7.8 Prespecified holdout evaluation in progress

The locked implementation is now being evaluated on the 24 previously ungenerated seeds 82001-82024, using all three policies and a distinct 732-frame initial recording per scene. The 72 scheduled trials and their source snapshot are retained in `reports/rgbd_holdout_01`. Scene generation, commissioning and execution failures remain in the denominator. Whole-task success in the final statistics also requires the independent recorded-clock audit; geometry alone cannot promote a stale-view execution to success. Final-state broad/local maps, retained surfaces and unsafe-free probes are audited after execution. No holdout result is assigned while this batch is incomplete, and controller sources are not adjusted in response to its outcomes.

## 8. Discussion and current conclusion

The present implementation establishes a complete nominal measured-environment three-object mission with independent geometry audits and bounded online feedback handling. It does not yet establish randomized mission reliability, a uniformly better reaction policy, or full-stack hard real time. The observation model is simulated and uses known colored interfaces; initial scanning is external; mass centers and inertias have not been independently measured; contact dynamics and hardware execution are excluded. A geometric visual-set certificate does not guarantee persistent image visibility. Unknown externally moving obstacles and arbitrary unresolved objects are outside the current assumptions.

The current experimental contribution is a measured-evidence permission structure: local map revision cannot create free space on its own; retained observed object geometry is separated from that permission; and visual redundancy is released only through a geometrically certified, quality-monitored motion. Existing reaction-null-space, visual-set control and object-centric mapping work remain relevant precedents. The complete nominal task demonstrates feasibility under the stated assumptions, while adverse force results, the unsuccessful randomized set-policy pilot and long computational waits constrain the conclusion. Functional repairs and new frozen comparative experiments are needed before a final method-level performance claim. Physical validation is deliberately excluded rather than replaced with invented evidence.

## References

[1] A. H. Abdul Hafez, P. Mithun, V. V. Anurag, S. V. Shah, and K. Madhava Krishna. Reactionless visual servoing of a multi-arm space robot combined with other manipulation tasks. *Robotics and Autonomous Systems*, 91:1–10, 2017. https://doi.org/10.1016/j.robot.2016.12.010

[2] S. Moe, G. Antonelli, A. R. Teel, K. Y. Pettersen, and J. Schrimpf. Set-Based Tasks within the Singularity-Robust Multiple Task-Priority Inverse Kinematics Framework: General Formulation, Stability Analysis, and Experimental Results. *Frontiers in Robotics and AI*, 3:16, 2016. https://doi.org/10.3389/frobt.2016.00016

[3] A. Millane et al. nvblox: GPU-Accelerated Incremental Signed Distance Field Mapping. arXiv:2311.00626, 2023. https://arxiv.org/abs/2311.00626

[4] D. N. Nenchev. Reaction Null Space of a multibody system with applications in robotics. *Mechanical Sciences*, 4:97–112, 2013. https://doi.org/10.5194/ms-4-97-2013

[5] A. Bircher, M. Kamel, K. Alexis, H. Oleynikova, and R. Siegwart. Receding Horizon “Next-Best-View” Planner for 3D Exploration. *ICRA*, 1462–1468, 2016. https://doi.org/10.1109/ICRA.2016.7487281

[6] H. Oleynikova, Z. Taylor, M. Fehr, J. Nieto, and R. Siegwart. Voxblox: Incremental 3D Euclidean Signed Distance Fields for On-Board MAV Planning. *IROS*, 2017. https://arxiv.org/abs/1611.03631

[7] B. Stellato, G. Banjac, P. Goulart, A. Bemporad, and S. Boyd. OSQP: an operator splitting solver for quadratic programs. *Mathematical Programming Computation*, 12:637–672, 2020. https://doi.org/10.1007/s12532-020-00179-2

[8] Y. Liu et al. AVR: Active Vision-Driven Precise Robot Manipulation with Viewpoint and Focal Length Optimization. arXiv:2503.01439v4, 2025. https://arxiv.org/abs/2503.01439v4

[9] F. Aghili. Adaptive Visual Servoing for On-Orbit Servicing. arXiv:2409.05295, 2024. https://arxiv.org/abs/2409.05295 (Preprint version; the record links an earlier related journal DOI, so the 2024 upload is not treated as proof of first publication.)

[10] M. Rünz and L. Agapito. Co-Fusion: Real-time Segmentation, Tracking and Fusion of Multiple Objects. 2017. https://arxiv.org/abs/1706.06629

[11] M. Rünz, M. Buffier, and L. Agapito. MaskFusion: Real-Time Recognition, Tracking and Reconstruction of Multiple Moving Objects. 2018. https://arxiv.org/abs/1804.09194

[12] P. N. Dal and S. V. Shah. Joint acceleration based adaptive reactionless manipulation of closed-loop multi-arm space robot in post-capture phase. *Acta Astronautica*, 226:439–457, 2025. https://doi.org/10.1016/j.actaastro.2024.10.022

[13] Z. Li and C. Wei. Research on Visual Servo Collaborative Control Method for Multi-arm Space Robot. *Advances in Intelligent Data and Information Processing*, LNNS 1809:146–155, first online 1 April 2026. https://doi.org/10.1007/978-3-032-16705-7_13

[14] RealSense. D435i product specifications: depth and RGB optical fields and shutter types. Manufacturer documentation, accessed 9 September 2026. https://www.realsenseai.com/cn/products/d435i/

[15] S. Zhou, J. Liu, H. Gao, and Z. Xue. Base-Reaction-Aware Trajectory Planning and Safe Execution for a Free-Floating Dual-Arm Space Robot. *Advances in Space Research*, journal pre-proof, available online 4 September 2026. https://doi.org/10.1016/j.asr.2026.08.112

[16] W. Su et al. Cooperative manipulation control with task-prioritized real-time optimization for free-floating dual-arm space robots. *Aerospace Science and Technology*, 171:111584, April 2026. https://doi.org/10.1016/j.ast.2025.111584

[17] A. Govoni, M. Cavuoto, Y. Li, S. Calinon, and G. Palli. Real-time collision avoidance with robot distance fields in a task-priority framework. *Robotics and Autonomous Systems*, 200:105394, June 2026. https://doi.org/10.1016/j.robot.2026.105394

[18] A. İşleyen, R. van de Molengraft, and Ö. Arslan. Action-Aware Pro-Active Safe Exploration for Mobile Robot Mapping. arXiv:2503.09515, 2025. https://arxiv.org/abs/2503.09515 (Preprint; no journal publication is inferred.)
