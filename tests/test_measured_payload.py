import numpy as np
import pytest
from itertools import product
from types import SimpleNamespace
from astronaut_delivery.measured_payload import payload_from_measurement,mask_payload_returns
from classical_fusion.payload_contact_guard import SupportContactColumn


def test_measured_payload_round_trip():
    points=np.array(list(product([.1,.2],[.3,.4],[.5,.6])))
    T=np.eye(4);T[:3,3]=[.15,.35,.7];T[:3,:3]=np.diag([1,-1,-1])
    payload=payload_from_measurement('measured',SimpleNamespace(vertices=points),T)
    recovered=payload.local_points@T[:3,:3].T+T[:3,3]
    assert np.allclose(np.sort(recovered,axis=0),np.sort(points,axis=0))


def test_payload_return_mask_never_creates_free_depth():
    p=np.array(list(product([-.02,.02],[-.02,.02],[-.02,.02])))
    d=np.array([[1.,0.,2.]]);K=np.array([[100.,0,0],[0,100.,0],[0,0,1]])
    tcp=np.eye(4);tcp[2,3]=-1
    out=mask_payload_returns(d,K,np.eye(4),p,tcp,padding=.001)
    assert np.array_equal(out,[[0.,0.,2.]])
    assert np.array_equal(d,[[1.,0.,2.]])


def test_contact_column_whole_cell_and_link_scope():
    p=np.array(list(product([-.05,.05],[-.05,.05])))
    column=SupportContactColumn(p,.1,.2,['payload','magnet'])
    centers=np.array([[0,0,.15],[.049,0,.15],[0,0,.101]])
    assert np.array_equal(column.cells('payload',centers,[.004]*3),[True,False,False])
    assert not column.cells('b8',centers,[.004]*3).any()
    with pytest.raises(ValueError):SupportContactColumn(p,.1,.5,['payload'])
    with pytest.raises(ValueError):SupportContactColumn(p,.1,.2,['b8'])
