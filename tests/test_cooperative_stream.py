import numpy as np
import pytest
from astronaut_delivery.smooth_path import SmoothPath,candidates

def test_quintic_limits_and_rest_endpoints():
    rng=np.random.default_rng(20260908)
    for _ in range(20):
        path=SmoothPath.make(rng.uniform(-2,2,16),rng.uniform(-2,2,16))
        np.testing.assert_allclose(path.state(0)[0],path.start)
        np.testing.assert_allclose(path.state(path.duration)[0],path.end,atol=1e-12)
        for t in (0,path.duration):
            np.testing.assert_allclose(path.state(t)[1:3],0,atol=1e-12)
        assert np.all(np.array(path.bounds())<=np.array([.3,.5,5.])+1e-9)

@pytest.mark.parametrize('fraction',[0.,.01,.1,.3,.5,.7,.9,.999])
def test_brake_is_c2_bounded_monotone_and_within_original_certificate(fraction):
    p=SmoothPath.make(np.zeros(16),np.linspace(-1,1,16));t=p.duration*fraction;b=p.stop(t)
    for first,second in zip(p.state(t)[:3],b.state(0)[:3]):np.testing.assert_allclose(first,second,atol=1e-10)
    np.testing.assert_allclose(b.state(b.duration)[1:3],0,atol=1e-9)
    assert np.all(np.array(b.bounds())<=np.array([.3,.5,5.])+1e-7)
    samples=b.scalar(candidates(b.coefficients,b.duration))
    assert samples.min()>=p.scalar(t)-1e-8 and samples.max()<=1+1e-8
    assert b.scalar(candidates(b.coefficients,b.duration,1),1).min()>=-1e-8

def test_time_scaling_preserves_geometry_and_reduces_derivatives():
    p=SmoothPath.make(np.zeros(16),np.ones(16));b=p.retime(2.3)
    for t in np.linspace(0,p.duration,10):
        for k in range(4):np.testing.assert_allclose(p.state(t)[k]/2.3**k,b.state(t*2.3)[k],atol=1e-12)
    with pytest.raises(ValueError):p.retime(.5)

def test_cached_separating_axis_rechecks_moving_geometry():
    from astronaut_delivery.motion import CollisionGuard
    g=CollisionGuard.__new__(CollisionGuard);g.separation_axes={}
    a=np.array([[x,y,z] for x in (-1.,1.) for y in (-1.,1.) for z in (-1.,1.)]);n=np.eye(3)
    assert g.separated('pair',a,n,a+[3,0,0],n,.1)
    assert not g.separated('pair',a,n,a+[.5,0,0],n,.1)

def test_partial_circular_cap_does_not_use_visible_centroid():
    from astronaut_delivery.vision import circular_cap_center
    rng=np.random.default_rng(17);angles=rng.uniform(0,2*np.pi,1000);r=.02*np.sqrt(rng.random(1000))
    xy=np.c_[r*np.cos(angles),r*np.sin(angles)];xy=xy[xy[:,1]>-.002]
    points=np.c_[xy+np.array([.58,-.50]),np.full(len(xy),.817)]
    result=circular_cap_center(points)
    assert result is not None
    assert np.linalg.norm(result[:2]-[.58,-.50])<.002
    assert abs(np.median(points[:,1])+.5)>.006

def test_stale_target_requires_three_consistent_new_measurements():
    from astronaut_delivery.vision import VisualGuide,Observation,TargetEstimate
    guide=VisualGuide({'cap':[.8,.1,.1]});guide.reacquire_stale=True
    guide.estimates['cap']=TargetEstimate('cap',np.array([0.,0.,.817]),np.eye(3),100,0.)
    K=np.array([[100.,0,7.5],[0,100.,7.5],[0,0,1.]])
    T=np.eye(4);T[:3,3]=[.02,0,1.]
    for index in range(3):
        obs=Observation(np.tile([.8,.1,.1],(16,16,1)),np.full((16,16),.183),K,T,1.+index*.1)
        guide.update(obs,integrate_map=False)
        if index<2:assert guide.target('cap',obs.stamp) is None
    assert guide.reacquisitions==1
    np.testing.assert_allclose(guide.target('cap',1.2).position,[.02,0,.817],atol=1e-10)

def test_sensor_exception_executes_smooth_brake_not_instant_hold():
    from types import SimpleNamespace
    from astronaut_delivery.cooperative import CooperativeDelivery
    run=CooperativeDelivery.__new__(CooperativeDelivery)
    run.cfg={'control_dt_s':1/60};run.coop={'maximum_velocity_rad_s':.3,'maximum_acceleration_rad_s2':.5,'maximum_jerk_rad_s3':5.}
    run.estop=False;run.fault_at=None;run.fault_fired=False;run.time=0.;run.tick=0;run.camera_ticks=1
    run.steps=[];run.command_times=[];run.cycle_times=[];run.vision_service=None
    run.robot=SimpleNamespace(set_positions=lambda *q:None);run.latch=SimpleNamespace(update=lambda t:None)
    run.right=SimpleNamespace(tcp=lambda q:np.eye(4));run.reaction=SimpleNamespace(wrench=lambda *x:np.zeros(6))
    run.event=lambda *a,**kw:None;run.record=lambda phase:None
    def fail():raise RuntimeError('injected sensor transport failure')
    run.observe=fail
    with pytest.raises(RuntimeError,match='safe_stop'):run.execute(SmoothPath.make(np.zeros(16),np.ones(16)*.05),'test')
    assert run.stopped_smoothly and len(run.steps)>1
    np.testing.assert_allclose(run.steps[-1]['v'],0,atol=1e-9)
    np.testing.assert_allclose(run.steps[-1]['a'],0,atol=1e-9)
    assert np.max(np.abs([s['jerk'] for s in run.steps]))<=5+1e-7

def test_refitted_sensor_mesh_matches_fresh_mesh_after_object_motion():
    from pxr import Usd,UsdGeom
    from astronaut_delivery.vision import MeshCamera
    stage=Usd.Stage.CreateInMemory();cube=UsdGeom.Cube.Define(stage,'/World/Cube');cube.CreateSizeAttr(.2)
    translation=cube.AddTranslateOp();translation.Set((0,0,-.8))
    sensor=MeshCamera(64,48,device='cpu');first=sensor.capture(stage,np.eye(4),0)
    translation.Set((.22,0,-.8));moved=sensor.capture(stage,np.eye(4),1)
    fresh=MeshCamera(64,48,device='cpu').capture(stage,np.eye(4),1)
    assert not np.array_equal(first.depth,moved.depth)
    np.testing.assert_array_equal(moved.depth,fresh.depth)
