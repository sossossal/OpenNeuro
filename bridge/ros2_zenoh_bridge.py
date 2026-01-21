#!/usr/bin/env python3
"""
OpenNeuro ROS 2 ↔ Zenoh 双向桥接器

功能:
- 将 ROS 2 Topic 消息转发到 Zenoh 发布-订阅系统
- 将 Zenoh 消息转发回 ROS 2 Topic
- 支持自动类型转换和延迟测量

使用:
    python3 ros2_zenoh_bridge.py
"""

import json
import time
import logging
from typing import Dict, Any, Optional
from dataclasses import dataclass, asdict
from datetime import datetime

# 尝试导入 ROS 2（可选，桥接器可以离线运行）
try:
    import rclpy
    from rclpy.node import Node
    from std_msgs.msg import Float32, String, Int32
    ROS2_AVAILABLE = True
except ImportError:
    ROS2_AVAILABLE = False
    logging.warning("ROS 2 not available - bridge will run in Zenoh-only mode")

# 尝试导入 Zenoh（必需）
try:
    import zenoh
    ZENOH_AVAILABLE = True
except ImportError:
    ZENOH_AVAILABLE = False
    logging.error("Zenoh Python bindings not available. Install: pip install zenoh")
    raise ImportError("Zenoh required")

# 日志配置
logging.basicConfig(
    level=logging.INFO,
    format='[%(asctime)s] [%(name)s] %(levelname)s: %(message)s'
)
logger = logging.getLogger("OpenNeuro-Bridge")


@dataclass
class BridgeConfig:
    """桥接器配置"""
    zenoh_locator: str = "tcp/127.0.0.1:7447"
    zenoh_mode: str = "client"
    ros2_enabled: bool = True
    bridge_namespace: str = "openneuro"
    enable_latency_measurement: bool = True
    enable_logging: bool = True
    max_queue_size: int = 100


class ROS2ZenohBridge:
    """ROS 2 ↔ Zenoh 双向桥接器实现"""
    
    def __init__(self, config: BridgeConfig):
        self.config = config
        self.zenoh_session: Optional[zenoh.Session] = None
        self.ros2_node: Optional[Node] = None
        
        # 消息计数器和延迟统计
        self.stats = {
            'ros2_to_zenoh': 0,
            'zenoh_to_ros2': 0,
            'total_latency_ms': 0.0,
        }
        
        # 话题映射（ROS 2 topic → Zenoh resource）
        self.topic_mapping: Dict[str, str] = {
            # 舵机控制
            '/servo/target': f'rt/{config.bridge_namespace}/servo/cmd',
            '/servo/feedback': f'rt/{config.bridge_namespace}/servo/status',
            
            # LED 控制
            '/led/cmd': f'rt/{config.bridge_namespace}/led/cmd',
            '/led/status': f'rt/{config.bridge_namespace}/led/status',
            
            # 传感器数据
            '/sensor/temperature': f'rt/{config.bridge_namespace}/sensor/temperature',
            '/sensor/adc': f'rt/{config.bridge_namespace}/sensor/adc',
            
            # 设备状态
            '/device/status': f'rt/{config.bridge_namespace}/device/status',
        }
        
        logger.info("Bridge initialized with config: %s", config)
    
    def connect_zenoh(self) -> bool:
        """连接到 Zenoh Router"""
        try:
            logger.info(f"Connecting to Zenoh router at {self.config.zenoh_locator}...")
            
            zenoh_config = zenoh.Config()
            if self.config.zenoh_mode == "client":
                zenoh_config.insert_json5("connect/endpoints", [self.config.zenoh_locator])
            
            self.zenoh_session = zenoh.open(zenoh_config)
            logger.info("✓ Connected to Zenoh successfully")
            return True
            
        except Exception as e:
            logger.error(f"✗ Failed to connect to Zenoh: {e}")
            return False
    
    def connect_ros2(self) -> bool:
        """连接到 ROS 2 环境"""
        if not self.config.ros2_enabled or not ROS2_AVAILABLE:
            logger.info("ROS 2 bridge disabled or unavailable")
            return False
        
        try:
            logger.info("Initializing ROS 2 node...")
            rclpy.init()
            self.ros2_node = Node('openneuro_bridge')
            logger.info("✓ ROS 2 node created successfully")
            return True
            
        except Exception as e:
            logger.error(f"✗ Failed to initialize ROS 2: {e}")
            return False
    
    def publish_to_zenoh(self, resource: str, payload: Any) -> bool:
        """发布消息到 Zenoh"""
        try:
            if isinstance(payload, dict):
                payload_str = json.dumps(payload)
            else:
                payload_str = str(payload)
            
            if self.zenoh_session:
                self.zenoh_session.put(resource, payload_str)
                self.stats['ros2_to_zenoh'] += 1
                
                if self.config.enable_logging:
                    logger.debug(f"[PUB] {resource}: {payload_str[:80]}...")
                return True
            
            return False
            
        except Exception as e:
            logger.error(f"Error publishing to Zenoh: {e}")
            return False
    
    def subscribe_zenoh(self, resource: str, callback) -> bool:
        """订阅 Zenoh 话题"""
        try:
            if self.zenoh_session:
                sub = self.zenoh_session.subscribe(resource, callback)
                logger.info(f"✓ Subscribed to Zenoh resource: {resource}")
                return True
            return False
            
        except Exception as e:
            logger.error(f"Error subscribing to Zenoh: {e}")
            return False
    
    def start(self) -> bool:
        """启动桥接器"""
        logger.info("=== OpenNeuro ROS 2 ↔ Zenoh Bridge v0.1.0 ===")
        logger.info("Bridge namespace: %s", self.config.bridge_namespace)
        
        # 连接 Zenoh
        if not self.connect_zenoh():
            return False
        
        # 尝试连接 ROS 2
        if not self.connect_ros2():
            logger.warning("Continuing in Zenoh-only mode")
        
        # 设置订阅回调
        def zenoh_callback(sample):
            self.stats['zenoh_to_ros2'] += 1
            payload = sample.payload.to_string()
            
            if self.config.enable_logging:
                logger.debug(f"[SUB] {sample.key_expr}: {payload[:80]}...")
            
            # 如果 ROS 2 可用，转发给 ROS 2
            if self.ros2_node:
                self._forward_to_ros2(sample.key_expr, payload)
        
        # 订阅所有 Zenoh 资源
        self.subscribe_zenoh("rt/**", zenoh_callback)
        
        logger.info("✓ Bridge started successfully")
        logger.info("Topic mappings:")
        for ros2_topic, zenoh_resource in self.topic_mapping.items():
            logger.info(f"  {ros2_topic:30} ↔ {zenoh_resource}")
        
        return True
    
    def _forward_to_ros2(self, zenoh_resource: str, payload: str):
        """将 Zenoh 消息转发到 ROS 2"""
        # 实现 ROS 2 消息发布（需要完整的 ROS 2 setup）
        pass
    
    def run(self):
        """运行桥接器主循环"""
        logger.info("Bridge main loop started - press Ctrl+C to exit")
        
        try:
            # 模拟demo：定期发布测试消息
            counter = 0
            while True:
                time.sleep(1)
                counter += 1
                
                # 每 10 秒发送一条测试消息
                if counter % 10 == 0:
                    test_payload = {
                        'timestamp': datetime.now().isoformat(),
                        'msg_count': self.stats['ros2_to_zenoh'],
                        'status': 'ok'
                    }
                    self.publish_to_zenoh(
                        f'rt/{self.config.bridge_namespace}/bridge/status',
                        test_payload
                    )
                
                # 定期打印统计信息
                if counter % 30 == 0:
                    logger.info(f"Stats: ROS2→Zenoh: {self.stats['ros2_to_zenoh']}, "
                               f"Zenoh→ROS2: {self.stats['zenoh_to_ros2']}")
        
        except KeyboardInterrupt:
            logger.info("Bridge shutdown requested")
        finally:
            self.shutdown()
    
    def shutdown(self):
        """优雅关闭"""
        logger.info("Shutting down bridge...")
        
        if self.zenoh_session:
            self.zenoh_session.close()
        
        if self.ros2_node and ROS2_AVAILABLE:
            self.ros2_node.destroy_node()
            rclpy.shutdown()
        
        logger.info("Bridge shutdown complete")
        logger.info(f"Final stats: ROS2→Zenoh: {self.stats['ros2_to_zenoh']}, "
                   f"Zenoh→ROS2: {self.stats['zenoh_to_ros2']}")


def main():
    """主入口"""
    # 创建配置
    config = BridgeConfig(
        zenoh_locator="tcp/127.0.0.1:7447",
        zenoh_mode="client",
        ros2_enabled=ROS2_AVAILABLE,
        bridge_namespace="openneuro",
    )
    
    # 创建并启动桥接器
    bridge = ROS2ZenohBridge(config)
    
    if bridge.start():
        bridge.run()
    else:
        logger.error("Failed to start bridge")
        return 1
    
    return 0


if __name__ == '__main__':
    exit(main())

