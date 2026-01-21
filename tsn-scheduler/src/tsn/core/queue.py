"""Priority Queue Implementation for TSN"""

from typing import Any, Optional
import heapq


class PriorityQueue:
    """IEEE 802.1Q Priority Queue (8 levels: 0-7)"""
    
    def __init__(self, max_size: int = 1000):
        """Initialize priority queue
        
        Args:
            max_size: Maximum queue size
        """
        self.max_size = max_size
        self.queues = [[] for _ in range(8)]  # 8 priority levels
        self.count = 0
    
    def enqueue(self, data: Any, priority: int) -> bool:
        """Enqueue data with priority
        
        Args:
            data: Data to enqueue
            priority: Priority level (0-7, 7 is highest)
            
        Returns:
            True if successful, False if queue full
        """
        if priority < 0 or priority > 7:
            raise ValueError(f"Priority must be 0-7, got {priority}")
        
        if self.count >= self.max_size:
            return False
        
        self.queues[priority].append(data)
        self.count += 1
        return True
    
    def dequeue(self) -> Optional[Any]:
        """Dequeue highest priority item
        
        Returns:
            Dequeued data or None if empty
        """
        # Check from highest to lowest priority
        for priority in range(7, -1, -1):
            if self.queues[priority]:
                data = self.queues[priority].pop(0)
                self.count -= 1
                return data
        return None
    
    def is_empty(self) -> bool:
        """Check if queue is empty"""
        return self.count == 0
    
    def get_stats(self) -> dict:
        """Get queue statistics"""
        level_sizes = [len(q) for q in self.queues]
        return {
            'total': self.count,
            'by_priority': level_sizes,
            'utilization': self.count / self.max_size
        }
