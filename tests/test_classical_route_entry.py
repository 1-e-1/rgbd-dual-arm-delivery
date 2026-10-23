import pytest

from astronaut_delivery import rgbd_classical_mission as entry


ARGS = ['--output', 'unused-output', '--scene', 'scene.usda',
        '--initial-scan', 'raw-scan', '--scene-token', 'scene-id',
        '--calibration-id', 'camera-id']


@pytest.mark.parametrize('extra,mode,budget,unchanged', [
    ([], 'set', 16000000, False),
    (['--mode', 'hold', '--local-map-cell-budget', '12000000',
      '--assume-unchanged-scene'], 'hold', 12000000, True),
])
def test_pinned_classical_options(monkeypatch, extra, mode, budget, unchanged):
    captured = {}

    class FakeMission:
        def __init__(self, *args, **kwargs):
            captured.update(args=args, options=kwargs)
            self.task_metadata = {}
            captured['metadata'] = self.task_metadata

        def run(self, *args):
            captured['run'] = args

    monkeypatch.setattr(entry, 'RGBDMission', FakeMission)
    entry.main(ARGS + extra)
    options = captured['options']
    assert options['mode'] == mode
    assert options['local_map_cell_budget'] == budget
    assert options['assume_unchanged_scene'] is unchanged
    for name in ('adaptive_visual_proof', 'quality_aware_handover',
                 'avoid_rejected_proposals'):
        assert options[name] is False
    for name in ('asynchronous', 'contact_permission', 'capture',
                 'local_contact_map', 'static_interval_fusion'):
        assert options[name] is True
    assert captured['run'] == (48, .007, 'auto')
    assert captured['metadata']['active_route'] == 'classical_pre_ppo_capacity_repair'


def test_uses_original_classical_class():
    from astronaut_delivery.rgbd_mission import RGBDMission
    assert entry.RGBDMission is RGBDMission
    assert not hasattr(RGBDMission, 'residual_proposal')


@pytest.mark.parametrize('flag', ['--adaptive-visual-proof', '--quality-aware-handover',
                                '--avoid-rejected-proposals', '--checkpoint'])
def test_experiments_cannot_be_enabled_by_this_entry(flag):
    with pytest.raises(SystemExit) as error:
        entry.main(ARGS + [flag])
    assert error.value.code == 2
