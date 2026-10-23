from types import SimpleNamespace
import numpy as np
from astronaut_delivery.rgbd_mission import RGBDMission
from astronaut_delivery.rgbd_transfer import RGBDTransfer
from astronaut_delivery.measured_task_memory import MeasuredTaskMemory
from astronaut_delivery.rgbd_mission_audit import completed_scene


def test_empty_observed_queue_is_not_scene_completion():
    assert not completed_scene(['a','b'],['a'],True)
    assert not completed_scene(['a'],['a'],False)
    assert not completed_scene([],[],True)
    assert completed_scene(['a','b'],['b','a'],True)


def test_mission_completes_only_observed_objects_without_reset(monkeypatch):
    run=RGBDMission.__new__(RGBDMission);run.memory=MeasuredTaskMemory()
    run.memory.observe({'x':SimpleNamespace(name='x',kind='magnetic_interface',stamp=0)})
    run.active='x';run.maximum_cycles=8;run.place_xy=np.array([.3,0]);run.place_z=.7
    run.time=10.;run.transfer_events=[];run.task_metadata={};run.observe=lambda **kw:None;run.local_mapper=None
    monkeypatch.setattr(RGBDTransfer,'after_capture',lambda self,status:'cycle_done')
    status=run.after_capture('captured')
    assert status=='OBSERVED_RGBD_TASK_QUEUE_COMPLETED_PENDING_SCENE_AUDIT'
    assert run.memory.completed==['x'] and run.task_metadata['observed_queue_exhausted']
    assert run.transfer_events[0]['scene_count_known'] is False


def test_previous_object_flag_cannot_complete_later_retreat():
    from astronaut_delivery.rgbd_mission_audit import retreat_evidence
    result=dict(mission_cycle_budget=8,post_release_retreat_completed=True)
    release=np.eye(4);withdrawn=np.eye(4);withdrawn[2,3]=.08
    evidence=retreat_evidence(result,None,True,release,withdrawn)
    assert not evidence['retreat_logged']
    assert not evidence['retreat_geometry_completed']
    assert retreat_evidence(result,dict(time_s=1),True,release,withdrawn)['retreat_geometry_completed']


def test_logged_completion_still_requires_actual_withdrawal_distance():
    from astronaut_delivery.rgbd_mission_audit import retreat_evidence
    result=dict(post_release_retreat_completed=True)
    release=np.eye(4);withdrawn=np.eye(4);withdrawn[2,3]=.02
    evidence=retreat_evidence(result,None,True,release,withdrawn)
    assert evidence['retreat_logged'] and not evidence['retreat_geometry_completed']
    withdrawn[2,3]=.08
    assert retreat_evidence(result,None,True,release,withdrawn)['retreat_geometry_completed']
    assert not retreat_evidence(result,None,False,release,withdrawn)['retreat_geometry_completed']
