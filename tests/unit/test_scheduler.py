"""Test for TSN Scheduler"""

import pytest
from tsn.core.scheduler import TSNScheduler, Flow


def test_scheduler_init():
    """Test scheduler initialization"""
    sched = TSNScheduler(cycle_time=1000)
    assert sched.cycle_time == 1000
    assert len(sched.flows) == 0


def test_schedule_flow():
    """Test flow scheduling"""
    sched = TSNScheduler()
    
    flow = Flow(
        flow_id=1,
        priority=5,
        deadline=100,
        period=1000,
        payload_size=1500
    )
    
    assert sched.schedule(flow) is True
    assert flow.flow_id in sched.flows


def test_invalid_priority():
    """Test invalid priority"""
    sched = TSNScheduler()
    
    flow = Flow(
        flow_id=1,
        priority=10,  # Invalid
        deadline=100,
        period=1000,
        payload_size=1500
    )
    
    assert sched.schedule(flow) is False


def test_duplicate_flow():
    """Test duplicate flow scheduling"""
    sched = TSNScheduler()
    
    flow = Flow(
        flow_id=1,
        priority=5,
        deadline=100,
        period=1000,
        payload_size=1500
    )
    
    assert sched.schedule(flow) is True
    assert sched.schedule(flow) is False


def test_get_gate_states():
    """Test gate state retrieval"""
    sched = TSNScheduler()
    
    states = sched.get_gate_states(0)
    assert len(states) == 8
    assert all(isinstance(s, bool) for s in states)


def test_get_flow_schedule():
    """Test flow schedule query"""
    sched = TSNScheduler()
    
    flow = Flow(
        flow_id=1,
        priority=5,
        deadline=100,
        period=1000,
        payload_size=1500
    )
    
    sched.schedule(flow)
    schedule = sched.get_flow_schedule()
    
    assert 1 in schedule
    assert schedule[1]['priority'] == 5
    assert schedule[1]['deadline'] == 100
