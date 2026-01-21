"""TSN Scheduler Implementation"""

from typing import Dict, List, Optional
from dataclasses import dataclass


@dataclass
class Flow:
    """TSN Flow Definition"""
    flow_id: int
    priority: int
    deadline: float  # microseconds
    period: float    # microseconds
    payload_size: int  # bytes


class TSNScheduler:
    """IEEE 802.1Qbv Time-Aware Scheduler"""
    
    def __init__(self, cycle_time: float = 1000.0):
        """Initialize scheduler
        
        Args:
            cycle_time: Cycle time in microseconds (default 1ms)
        """
        self.cycle_time = cycle_time
        self.flows: Dict[int, Flow] = {}
        self.gate_states = [[False] * 8 for _ in range(100)]  # 8 queues, 100 time slots
        self.scheduled_flows: List[int] = []
    
    def schedule(self, flow: Flow) -> bool:
        """Schedule a new flow
        
        Args:
            flow: Flow to schedule
            
        Returns:
            True if scheduling successful
        """
        if flow.flow_id in self.flows:
            return False
        
        if flow.priority < 0 or flow.priority > 7:
            return False
        
        self.flows[flow.flow_id] = flow
        self.scheduled_flows.append(flow.flow_id)
        
        return True
    
    def get_gate_states(self, time_slot: int) -> List[bool]:
        """Get gate states for a time slot
        
        Args:
            time_slot: Time slot index
            
        Returns:
            List of gate states for each priority queue
        """
        if time_slot < 0 or time_slot >= len(self.gate_states):
            return [False] * 8
        
        return self.gate_states[time_slot]
    
    def update(self, time_slot: int) -> None:
        """Update scheduler for next time slot
        
        Args:
            time_slot: Current time slot
        """
        # Simple implementation: open gates based on flow priority
        next_slot = (time_slot + 1) % len(self.gate_states)
        
        for priority in range(8):
            self.gate_states[next_slot][priority] = True
    
    def get_flow_schedule(self) -> Dict[int, dict]:
        """Get current flow schedule
        
        Returns:
            Dictionary of flow schedules
        """
        return {
            fid: {
                'priority': flow.priority,
                'deadline': flow.deadline,
                'period': flow.period,
                'payload_size': flow.payload_size
            }
            for fid, flow in self.flows.items()
        }
