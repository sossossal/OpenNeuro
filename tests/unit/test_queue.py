"""Test for Priority Queue"""

import pytest
from tsn.core.queue import PriorityQueue


def test_priority_queue_init():
    """Test queue initialization"""
    q = PriorityQueue()
    assert q.is_empty()
    assert q.count == 0


def test_enqueue_dequeue():
    """Test basic enqueue/dequeue"""
    q = PriorityQueue()
    
    # Enqueue data
    assert q.enqueue("data1", 5) is True
    assert q.enqueue("data2", 3) is True
    
    # Dequeue should return highest priority first
    data = q.dequeue()
    assert data == "data1"  # priority 5 > 3


def test_priority_levels():
    """Test all 8 priority levels"""
    q = PriorityQueue()
    
    for i in range(8):
        assert q.enqueue(f"data_{i}", i) is True
    
    # Should dequeue in descending priority order
    for i in range(7, -1, -1):
        data = q.dequeue()
        assert data == f"data_{i}"


def test_queue_full():
    """Test queue overflow"""
    q = PriorityQueue(max_size=5)
    
    for i in range(5):
        assert q.enqueue(f"data_{i}", 5) is True
    
    # Queue should be full
    assert q.enqueue("overflow", 5) is False


def test_invalid_priority():
    """Test invalid priority levels"""
    q = PriorityQueue()
    
    with pytest.raises(ValueError):
        q.enqueue("data", -1)
    
    with pytest.raises(ValueError):
        q.enqueue("data", 8)


def test_get_stats():
    """Test statistics"""
    q = PriorityQueue()
    
    q.enqueue("data1", 7)
    q.enqueue("data2", 5)
    q.enqueue("data3", 5)
    
    stats = q.get_stats()
    assert stats['total'] == 3
    assert stats['by_priority'][7] == 1
    assert stats['by_priority'][5] == 2
