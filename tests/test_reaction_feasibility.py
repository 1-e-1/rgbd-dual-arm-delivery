import numpy as np
import pytest
from astronaut_delivery.reaction_feasibility import bounded_residual


def test_full_rank_does_not_override_acceleration_limits():
    r=bounded_residual(np.eye(2),[2.,0.],[-.5,-.5],[.5,.5])
    assert r['status']=='POSITIVE_RESIDUAL_CERTIFIED'
    assert r['residual_lower_bound']==pytest.approx(1.5,abs=1e-7)


def test_feasible_zero_and_task_conflict():
    r=bounded_residual(np.eye(2),[.2,-.3],[-.5,-.5],[.5,.5])
    assert r['zero_feasible'] and r['residual']<1e-7
    r=bounded_residual(np.eye(2),[.2,-.3],[-.5,-.5],[.5,.5],E=np.eye(2),f=[0,0])
    assert r['residual_lower_bound']==pytest.approx(np.hypot(.2,.3),abs=1e-7)


def test_inequality_lower_bound_and_infeasible_task():
    r=bounded_residual([[1.]],[1.],[-2.],[2.],C=[[-1.]],h=[0.])
    assert r['residual_lower_bound']==pytest.approx(1.,abs=1e-7)
    r=bounded_residual([[1.]],[1.],[-2.],[2.],E=[[1.]],f=[3.])
    assert not r['zero_feasible'] and r['residual_lower_bound'] is None


def test_rejects_malformed_reaction_matrix():
    for B in ([1.,2.],np.empty((0,2)),np.empty((2,0))):
        with pytest.raises(ValueError):bounded_residual(B,[0.],[-1.],[1.])
