from types import SimpleNamespace
import json
import numpy as np
import pytest
from astronaut_delivery.rgbd_sensor_faults import RenderedSensorFault,validate_fault


def observation():return SimpleNamespace(rgb=np.ones((4,5,3),np.uint8)*99,depth=np.ones((4,5)),
    color_valid=np.ones((4,5),bool),K=np.eye(3),camera_world=np.eye(4))


def test_blackout_expires_on_wall_clock_while_motion_remains_stopped():
    fault=RenderedSensorFault(dict(mode='blackout'))
    obs=observation();info,clean,delay=fault.prepare(obs,4.9,100)
    assert not info['active'] and obs.depth.any()
    obs=observation();info,clean,delay=fault.prepare(obs,5.,101.)
    assert info['active'] and not obs.depth.any() and clean[0].any() and clean[1].any()
    obs=observation();info,_,_=fault.prepare(obs,5.,101.7)
    assert not info['active'] and obs.depth.any()


def test_rgb_only_preserves_depth_and_delay_does_not_retimestamp_pixels():
    obs=observation();fault=RenderedSensorFault(dict(mode='rgb_blackout'))
    info,clean,delay=fault.prepare(obs,5,101)
    assert not obs.rgb.any() and obs.depth.any() and delay==0
    obs=observation();fault=RenderedSensorFault(dict(mode='delay'))
    info,clean,delay=fault.prepare(obs,5,102)
    assert delay==.3 and obs.rgb.any() and info['capture_completed_wall_s']==102


def test_fault_record_keeps_original_render_and_delivered_input(tmp_path):
    fault=RenderedSensorFault(dict(mode='blackout'),tmp_path/'recording');obs=observation()
    info,clean,delay=fault.prepare(obs,5,100)
    request=dict(q=np.zeros((2,8)),stamp=5,requested_wall_s=99.95,id=8)
    fault.record(obs,clean,request,info,delay,100.01)
    row=json.loads((tmp_path/'recording/records.jsonl').read_text())
    with np.load(tmp_path/'recording'/row['file']) as z:
        assert z['clean_rgb'].any() and z['clean_depth'].any() and not z['rgb'].any() and not z['depth'].any()
        assert z['requested_wall_s']==99.95


@pytest.mark.parametrize('c',[dict(mode='other'),dict(mode='delay',duration_wall_s=0),
    dict(mode='blackout',trigger_motion_s=np.nan),dict(mode='delay',delay_wall_s=5),dict(mode='blackout',extra=1)])
def test_invalid_fault_schedule_rejected(c):
    with pytest.raises(ValueError):validate_fault(c)
