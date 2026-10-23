"""Pinned pre-PPO measured-only route; experimental entries remain available."""
import argparse

from threadpoolctl import threadpool_limits

from .rgbd_mission import RGBDMission


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('output', 'scene', 'initial-scan', 'scene-token', 'calibration-id'):
        parser.add_argument('--' + name, required=True)
    parser.add_argument('--assume-unchanged-scene', action='store_true')
    parser.add_argument('--mode', choices=('set', 'pose', 'hold'), default='set')
    parser.add_argument('--maximum-cycles', type=int, default=8)
    parser.add_argument('--local-map-cell-budget', type=int, default=16000000,
                        help='Fine-map capacity only; unchanged 2 mm resolution and safety margins. '
                             'Use 12000000 to reproduce the original capacity setting.')
    args = parser.parse_args(argv)
    with threadpool_limits(1):
        task = RGBDMission(
            args.output, args.scene, args.initial_scan, args.scene_token,
            args.calibration_id,
            assume_unchanged_scene=args.assume_unchanged_scene,
            mode=args.mode, asynchronous=True, contact_permission=True,
            capture=True, local_contact_map=True, static_interval_fusion=True,
            maximum_cycles=args.maximum_cycles,
            local_map_cell_budget=args.local_map_cell_budget,
            adaptive_visual_proof=False, quality_aware_handover=False,
            avoid_rejected_proposals=False, sensor_fault=None,
        )
        task.task_metadata['active_route'] = 'classical_pre_ppo_capacity_repair'
        task.run(48, .007, 'auto')


if __name__ == '__main__':
    main()
