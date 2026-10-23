# Classical-first improvement and bounded residual PPO

This is a development experiment, not a replacement of the frozen paper results.
The original holdout remains set 16/24, pose 16/24, hold 18/24. Previously reused
holdout scenes are now development data, not a new independent test set.

## Why try learning now

Three opt-in classical changes were tested: next-phase visual quality checking,
adaptive continuous visual proof subdivision, and recent rejected-proposal memory.
82015 completed with the quality switch both ON and OFF; no causal gain established.
Adaptive proof moved 82017 beyond approach but stopped at payload placement (0/1).
On 82022 adaptive proof alone completed 0/3; adding rejection memory completed 1/3,
with 3 candidates skipped and pre-execution rejections decreasing from 33 to 9.
This remains worse task progress than the historical 2/3. Its executed-path and
clock audits are separate from scene task completion; audit success cannot turn
an incomplete mission into success. None of these switches is promoted to default.

## Policy contract

Entry: `python -m astronaut_delivery.rgbd_residual_mission`.
Without `--checkpoint` it trains from scratch; with a checkpoint it evaluates the
deterministic mean without optimizer updates. The baseline entry is unchanged.
Training and evaluation must use different output directories. A policy query
occurs only after the classical controller has found a measured-safe right-arm
IK candidate. Thus this prototype cannot directly rescue `no_measured_safe_IK` at
the current state, nor missing observations, unknown free space, or contact failure.
It can only learn to choose less problematic preceding joint configurations.

Observation (107 scalar features, clipped to [-10,10]):

| Feature | Size | Deployable source |
|---|---:|---|
| sin/cos of 16 joint positions | 32 | encoders |
| last joint velocity | 16 | encoder/control history |
| classical joint proposal minus current state / 0.3 | 16 | controller |
| measured goal minus current TCP / 0.1 m | 3 | RGB-D + FK |
| sigma/5 mm, pixels/100, motion age/0.25 s | 3 | RGB-D estimator |
| current visual-set margins | 8 | measured target + known camera/FK |
| approach/lift/transport/place/retreat indicators | 5 | task state machine |
| held-object flag | 1 | task state |
| last modeled wrench / [0.35 N,0.15 Nm] | 6 | known arm inertial model + motion history |
| previous residual / 0.02 rad | 16 | controller history |
| most recent decision was a pre-execution rejection | 1 | controller history |

The observation is a partial state, not a claim of Markov sufficiency. In
particular motion-age input is not a substitute for monotonic wall-age safety
checks; those remain outside the policy. Map geometry is currently represented
indirectly by the baseline proposal and visual margins, not a learned map encoder.

The actor and critic each have two 64-unit tanh hidden layers. The actor produces
16 Normal means with learned diagonal log standard deviation, initialized -0.7.
The last actor layer is zero initialized. The action is

`z ~ Normal(mu(o), diag(exp(log_std)^2)); delta_q = 0.02 * tanh(z)`.

Left joints receive the residual relative to the classical candidate. The right
residual seeds a one-start IK solve for the SAME classical TCP goal and task
orientation (including the captured vertical row). Projection is rejected if it
exceeds 0.02 rad relative to the original candidate. Joint limits, 0.12 rad left
and 0.30 rad right incremental bounds remain. Every remaining candidate must pass
the original continuous RGB-D robot/payload, self/inter-arm, visual and endpoint
reserve certificates, plus fresh-image pre-execution checks. A rejected proposal
falls back to classical candidates; the network never grants motion permission.
Recovery/contact/release logic is not replaced by PPO.

## Online learning protocol

A transition spans consecutive policy queries, including the real execution or
rejection and any intervening classical recovery. Terminal transitions are flushed
after the actual mission stops. No offline PPO update on historical baseline logs.

Reward:

`r = clip((d_previous-d_current)/0.02,-2,2) + 5*new_observed_completions`

`    -0.1*mean_dt(sum((w/[.35,.35,.35,.15,.15,.15])^2))`

`    -0.25*(residual_not_certified) -0.25*pre_execution_rejections`

`    -0.05*(no_executed_ticks)`.

Terminal observed-queue exhaustion adds +5, otherwise -2. Distance is evaluated
against the previously measured goal to avoid attributing a task switch as motion.
Wrench cost is time-normalized over executed ticks, not privileged simulated
contact forces. No environment ground-truth collision, true object count, true
poses or true map reconstruction error enters the policy or reward. Observed
queue exhaustion is NOT scene completion; the independent audit determines that.
This prototype does not directly reward global map completeness, and a locally
better reward is not evidence of better task performance or lower wrench peaks.

PPO uses 16-transition on-policy batches, four full-batch epochs, Adam 3e-4,
gamma=0.99, GAE lambda=0.95, clipping=0.2, entropy weight=0.01, value squared-error
weight=0.5, gradient norm cap=0.5. Latent-action likelihood ratios are valid because
the fixed tanh change-of-variable Jacobian cancels. The safety shield/IK projection
is part of the transition kernel, not a substitute action likelihood.

One training episode and a frozen-policy rerun are smoke tests only, not converged
training or held-out evidence. Preserve initial/final weights, transitions, update
statistics, raw RGB-D, source freeze, full failures and independent audits.
Promote no policy until repeated multi-scene comparisons show a task-level gain
without degrading safety, visual quality or the agreed reaction metrics.

## First completed smoke experiment (2026-09-12)

`reports/rgbd_residual_train_82017_01`: 7 on-policy transitions, one four-epoch
update; 3 residual candidates certified, other proposals fell back. 0/1 delivered.
`reports/rgbd_residual_eval_82017_01`: frozen trained weights, 7 queries, zero
updates, 3 residual candidates certified. Also 0/1 delivered. Both stop at
`no_measured_safe_IK`, with 8 executed segments and 587 logged ticks. Independent
continuous geometry, wrench crosscheck and recorded-clock audits pass; complete
scene delivery does not. Source hashes were unchanged during each execution.

Evaluation's final IK log has eight broad-map rejections involving right link a4
intersecting unknown-or-occupied voxels and three fine-map contact-corridor
rejections. Subsequent scan proposals are also limited by continuous visual proof
budget or endpoint IK. This is a measured-authority/path-and-view feasibility
bottleneck, not evidence that the PPO network simply needs more width. The current
post-IK residual hook does not directly act at that failure point. A useful next
experiment would expose the measured rejected-cell witnesses to a pre-IK posture
or safe-sensing proposal mechanism, still subject to all existing guards. Do not
launch large training and assume this prototype already fixes that structural gap.

Final regression: `logs/pytest_residual_final_01.xml`, 510 tests, no failures/errors.
This includes four independent safety-gate fallback tests and PPO bounded action,
actual update, checkpoint roundtrip and no-learning-at-evaluation checks.

## Continued training requested by user

`--checkpoint ... --resume-training` now continues online training. A version-2
checkpoint restores actor, critic, Adam state, completed update history and torch
RNG. It may only be written after the rollout has been flushed. Old checkpoints
lack Adam/RNG state: they are explicitly marked `weights_only` warm starts, not
exact optimizer continuation. Evaluation with only `--checkpoint` still performs
no learning. Actions, reward, input schema and all safety gates are unchanged.

`publication/run_residual_training.py` declares six training episodes (6101,
82007, 82015 repeated twice), then frozen diagnostics on 82017 and 82022. The
starting weights are the previous seven-sample smoke checkpoint. Each episode
starts from its original scene and recorded RGB-D scan; only policy training
state transfers across episodes. No privileged scene information enters training.
Completed or failed tasks are audited and recorded; any geometry, wrench,
recorded-clock or source-integrity violation stops the batch. Failed task
completion alone is training data, not a reason to discard an episode.

Outputs: `reports/rgbd_residual_continuation_01/progress.json`, per-episode logs,
checkpoints, RGB-D records and audits. The checkpoint pointer means latest
safety-audited training checkpoint, NOT best-performing policy. Neither a larger
sample count nor this reused-scene diagnostic constitutes a new holdout result.
This continuation tests the existing post-IK residual interface; it does not
implement the proposed pre-IK recovery extension or solve that structural limit.
