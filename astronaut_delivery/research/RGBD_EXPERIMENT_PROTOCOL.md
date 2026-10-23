# Pure RGB-D simulation experiment protocol

Status: protocol, not completed experimental results. Hardware experiments are excluded. Development seeds must never be reclassified as held-out data.

## Common information boundary

Every scene owns an external predeployment RGB-D scan with retained raw images, calibration and exposure transforms. Set, differential-pose and no-reaction-optimization policies share that recording, initial joints, task-interface priors, geometric margins, motion limits and bounded search budgets. Online views may differ because of executed actions. Scene truth is confined to generation, rendering, binary actuator simulation and independent audits. No admission by visibility, reachability or successful planning is permitted after generation.

The controller receives no object count. A trial succeeds only when the independent audit establishes that every scene object was released in an unused measured destination and the tool withdrew, all geometry checks passed, every logged wrench was cross-checked, terminal velocity/acceleration are zero within tolerance, and source hashes remained unchanged. The current dual-clock protocol additionally requires a passed independent audit of original RGB-D request/exposure records against normal-motion commands. A missing or failed recorded-clock audit is a failure even when geometric/task audits pass. Observed-queue exhaustion without true scene completion is a failure. Historical pilots with an earlier clock protocol remain separately labeled and are not retroactively credited with this check.

## Comparisons

| Comparison | Controlled difference | Required outputs |
|---|---|---|
| Set versus pose | Camera visual inequalities versus additional differential pose equality | All-proposal mission success; common-success reaction/time; every adverse component metric |
| Set versus hold | Reaction optimization enabled versus no left-arm reaction increment | Same metrics; necessary certified sensing recovery allowed to both |
| Surface memory | Current TSDF versus same TSDF plus rigidly transported observed component | Same scene state, exact same samples/free grid; per-object coverage and aggregate precision |
| Broad/local resolution | Coarse queries versus explicit fine contact map | Actual payload containment/contact error, false-free probes, query/rebuild cost; no label as safe merely because coarser succeeds |
| Static evidence policy | Persistent possible-surface veto versus declared static-epoch interval policy | Free-space utility and independent unsafe-free tests, retained failures |
| Acquisition budget | 516 versus 732 actual frames under fixed scan rule | Total frames, independent bins, acquisition cost and task completion |
| Sensor stress | Missing, delayed or poor-quality measurements at predefined times | Brake delay, C2 rest, certified-line containment, recovery/completion; failures retained |
| Evidence revision | Local two-support update versus whole-volume retirement | Retained useful evidence, no newly free cells without observation, rebuilding/wait cost |

The table specifies planned comparisons. Unit tests or old CAD-assisted trials do not count as completed closed-loop comparisons.

## Sampling and policy ordering

Current generator varies 1–3 rigid objects, box/cylinder/hexagonal-prism/wedge shape, horizontal size 42–66 mm, height 32–58 mm, horizontal placement and yaw, separation 14–35 mm, and support height 0.72–0.80 m. Exact implemented sampler and each generated metadata file remain authoritative; the paper must not describe arbitrary SO(3), unmarked grasping or multiple same-color identities as evaluated capabilities.

Development next-seed list: 6101, 6102, 6103. Freeze runtime source and configuration before the batch. Within each seed, cyclically rotate the policy ordering by seed modulo number of policies; do not run all favorable cases first. Any subsequent algorithm change invalidates pooling those trials as one frozen comparison. Preserve changed-version trials as development evidence.

Holdout allocation was reserved before generation in `configs/rgbd_holdout_protocol.json`: seeds 82001–82024, set/pose/hold, full observed-queue mission and 732 raw initial frames per scene. The frozen batch is now running in `reports/rgbd_holdout_01`; final results are not yet assigned. The launcher verifies exact seed, policy and acquisition agreement and retains the protocol hash. The pre-generation status in the archived allocation describes its creation, not current execution progress. The 24-scene allocation estimates feasibility and paired effects; it is not a powered superiority claim (even 24/24 successes gives a Wilson lower bound of about 0.862). At least report confidence intervals broad enough to reveal uncertainty. Tuning after inspecting this allocation converts it to development data and requires a new holdout allocation.

The frozen default assigns at most six rescan proposals to each newly observed object, with no same-task reset, no reset before the preceding object's audited controller completion event, and an eight-cycle task cap. This finite rule is identical across policies and does not use the true object count. Other bounded recovery limits are unchanged. The automatic entry `scripts/run_rgbd_paper_validation.py` runs trials serially, then independently audits clocks and broad/local current/persistent maps, and produces paired statistics. It never retunes or resamples a failed scene. Monitor `evaluation.json`, then `postprocess.json` and `postprocess_summary.json`; finishing a batch does not mean that all tasks succeeded.

Development batch `rgbd_paired_mission_pilot_02` reuses the exact original 6101–6103 scenes and 732-frame recordings from pilot 01 through `--reuse-inputs-from`; it is a new frozen version comparison, not new data or holdout. Do not pool it with pilot 01 or isolated repairs. The static-interval assumption is applied to both broad and fine queries in this revision; the run records `static_interval_scope=broad_and_fine`, whereas older runs without this field used the fine-only scope.

## Endpoints and mathematics

For scene-level audited success s_i in {0,1}, report n, sum(s_i), p_hat=sum(s_i)/n and a 95% Wilson interval. With z=1.959964, center and half-width are

\[
c=\frac{\hat p+z^2/(2n)}{1+z^2/n},\qquad
w=\frac{z}{1+z^2/n}\sqrt{\frac{\hat p(1-\hat p)}n+\frac{z^2}{4n^2}}.
\]

For paired binary comparisons, report both discordant scene counts, not just marginal success percentages. Continuous comparisons are paired by scene and restricted to common audited successes when a complete-task cost is required. Report the size of that subset; do not assign zero energy or zero error to failed trials. Separately summarize energy/time spent before failure for all attempts without calling it complete-task performance. Bootstrap scene pairs, not individual frames, if confidence intervals on mean or median differences are computed.

For piecewise-constant recorded force samples F_k and durations dt_k,

\[
F_{RMS}=\sqrt{\frac{\sum_k\|F_k\|^2dt_k}{\sum_kdt_k}},\quad
F_{peak}=\max_k\|F_k\|,\quad L_F=\sum_k\|F_k\|dt_k.
\]

Torque is analogous. The maximum 0.25 s impulse is the maximum norm of the vector integral over trailing windows, not the integral of force magnitude. Also report all six component curves and the combined norm. Exact rest-to-rest net inertial impulse is zero under the model and is not an improvement endpoint.

The already implemented normalized reaction objective uses 0.35 N and 0.15 N m scales. Derived energy E=(F_RMS/0.35)^2+(T_RMS/0.15)^2 may summarize the tradeoff, but never replaces the separate force and torque RMS, peaks and window impulses. If first computed after seeing a development pair, label that development summary post-hoc.

## Map and timing denominators

Precision samples are measured vertices. Coverage samples are area-weighted true surfaces, including surfaces not seen by the sensor; report the ROI fraction and equal per-object sample counts as well as whole-scene aggregates. Missing geometry does not become an occupied box or a completed underside. A retained object model contains original measured triangles only, transformed by known rigid attachment; it provides no additional free-space votes.

Audit each map at its export time. A first-object snapshot is not a final-three-object map. The scene-state record stores event history and joint state; only the independent auditor uses it to place actual simulation geometry.

Timing must distinguish control streaming, sensing, planning/geometry certification, map rebuild/commit and raw-storage work. Record total wall time and time spent at rest waiting for computation. Do not dilute motion reaction RMS by inserting computational waiting samples, and do not promote streaming P99 below 16.67 ms to a whole-stack hard-real-time claim. Formal timing trials must not overlap unrelated GPU/CPU benchmarks.
