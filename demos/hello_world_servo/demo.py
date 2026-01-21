#!/usr/bin/env python3
"""
OpenNeuro Hello World Demo: 用 Zenoh CLI 控制 LED 和读取传感器

演示场景:
1. 启动 Zenoh Router
2. 烧写 ESP32 固件
3. 运行本脚本

交互命令:
- 发送 LED 控制: echo '{"on": 1}' | zenoh pub "rt/openneuro/led/cmd" -
- 发送舵机控制: echo '{"angle": 90}' | zenoh pub "rt/openneuro/servo/cmd" -
- 订阅传感器:  zenoh sub "rt/openneuro/sensor/temperature"
"""

import json
import time
import sys
import logging
from typing import Optional
from datetime import datetime

# 配置日志
logging.basicConfig(
    level=logging.INFO,
    format='[%(asctime)s.%(msecs)03d] [%(name)s] %(levelname)s: %(message)s',
    datefmt='%H:%M:%S'
)
logger = logging.getLogger("HelloWorld-Demo")


class HelloWorldDemo:
    """Hello World Demo 控制器"""
    
    def __init__(self):
        self.start_time = time.time()
        self.command_count = 0
        self.sensor_sample_count = 0
        self.led_state = False
        self.servo_angle = 0.0
    
    def get_elapsed_time(self) -> float:
        """获取运行时间（秒）"""
        return time.time() - self.start_time
    
    def print_banner(self):
        """打印欢迎信息"""
        print("\n")
        print("╔" + "=" * 78 + "╗")
        print("║" + " " * 78 + "║")
        print("║" + "  OpenNeuro Hello World Demo - Stage 1 (Soft Bridge)".center(78) + "║")
        print("║" + "  通过 Zenoh 控制 ESP32 的 LED 和舵机".center(78) + "║")
        print("║" + " " * 78 + "║")
        print("╠" + "=" * 78 + "╣")
        print("║ 前置要求:                                                                 ║")
        print("║   1. Zenoh Router 运行中:        zenohd                                   ║")
        print("║   2. ESP32 已连接 WiFi 并启动 Zenoh-Pico                                 ║")
        print("║   3. 舵机连接到 GPIO 18,  LED 连接到 GPIO 2                              ║")
        print("║ 交互命令:                                                                 ║")
        print("║   $ echo '{\"on\": 1}' | zenoh pub \"rt/openneuro/led/cmd\" -            ║")
        print("║   $ echo '{\"on\": 0}' | zenoh pub \"rt/openneuro/led/cmd\" -            ║")
        print("║   $ echo '{\"angle\": 90}' | zenoh pub \"rt/openneuro/servo/cmd\" -      ║")
        print("║   $ zenoh sub \"rt/openneuro/sensor/temperature\"                        ║")
        print("╠" + "=" * 78 + "╣")
        print("║ 实验步骤:                                                                 ║")
        print("║   步骤 1: LED 闪烁 (On/Off 切换)                     预期延迟: <2ms       ║")
        print("║   步骤 2: 舵机扫动 (0° → 180° → 0°)                预期延迟: <2ms       ║")
        print("║   步骤 3: 传感器数据读取 (10Hz)                    预期频率: 10±1 Hz     ║")
        print("║   步骤 4: 端到端延迟测量                           预期延迟: <2ms       ║")
        print("║ " + " " * 76 + " ║")
        print("╚" + "=" * 78 + "╝")
        print("\n")
    
    def test_led_toggle(self):
        """测试 1: LED 闪烁"""
        print("\n" + "━" * 80)
        logger.info("TEST 1: LED 闪烁控制")
        logger.info("命令: echo '{\"on\": 1}' | zenoh pub \"rt/openneuro/led/cmd\" -")
        print("━" * 80)
        
        logger.info("发送 LED ON 命令...")
        print("\n  请在新终端执行下面的命令：")
        print("  $ echo '{\"on\": 1, \"brightness\": 255}' | zenoh pub \"rt/openneuro/led/cmd\" -\n")
        
        logger.info("预期结果: ESP32 上的 LED 应该点亮")
        logger.info("观察的延迟应该 <2ms\n")
        
        time.sleep(2)
        
        logger.info("发送 LED OFF 命令...")
        print("  $ echo '{\"on\": 0}' | zenoh pub \"rt/openneuro/led/cmd\" -\n")
        logger.info("预期结果: ESP32 上的 LED 应该熄灭\n")
    
    def test_servo_sweep(self):
        """测试 2: 舵机扫动"""
        print("\n" + "━" * 80)
        logger.info("TEST 2: 舵机扫动控制")
        print("━" * 80)
        
        angles = [0, 45, 90, 135, 180, 135, 90, 45, 0]
        
        logger.info("舵机将从 0° 扫动到 180° 再回到 0°")
        print("\n  执行以下命令序列：\n")
        
        for angle in angles:
            cmd = f'echo \'{{"angle": {angle}}}\' | zenoh pub "rt/openneuro/servo/cmd" -'
            print(f"  $ {cmd}")
            self.servo_angle = angle
            time.sleep(0.5)
        
        print("\n")
        logger.info("预期结果: 舵机平稳扫动，每步延迟 <2ms")
    
    def test_sensor_reading(self):
        """测试 3: 传感器数据读取"""
        print("\n" + "━" * 80)
        logger.info("TEST 3: 传感器数据读取 (10Hz)")
        print("━" * 80)
        
        logger.info("在新终端订阅传感器数据：")
        print("\n  $ zenoh sub \"rt/openneuro/sensor/temperature\"\n")
        logger.info("应该看到每 100ms 一条数据（10Hz）")
        logger.info("样本数据格式:")
        
        sample = {
            "temperature": 28.5,
            "humidity": 45.2,
            "timestamp_ms": 12345
        }
        print(f"  {json.dumps(sample, indent=2)}\n")
        
        logger.info("观察 10 秒内接收到的消息数量（应该 ≈ 100 条）")
    
    def test_latency_measurement(self):
        """测试 4: 端到端延迟测量"""
        print("\n" + "━" * 80)
        logger.info("TEST 4: 端到端延迟测量")
        print("━" * 80)
        
        logger.info("本测试通过时间戳对比来测量往返延迟 (RTT)")
        logger.info("步骤:")
        logger.info("  1. 记录 PC 发送时间: T1")
        logger.info("  2. ESP32 接收命令, 立即发送反馈")
        logger.info("  3. PC 接收反馈, 记录接收时间: T2")
        logger.info("  4. RTT = T2 - T1, 单向延迟 ≈ RTT/2")
        
        print("\n  执行命令：")
        print("  $ python3 latency_measurement.py\n")
        
        logger.info("预期结果:")
        logger.info("  - 平均延迟: ~1.8ms (WiFi) 或 ~0.3ms (有线)")
        logger.info("  - 99th 百分位: ~2.4ms (WiFi)")
        logger.info("  - 最大延迟: <5ms (正常情况)\n")
    
    def run_interactive_mode(self):
        """交互模式"""
        print("\n" + "━" * 80)
        logger.info("交互模式 - 你可以手动尝试各种命令")
        print("━" * 80)
        
        commands = [
            ("LED 开启", 'echo \'{"on": 1}\' | zenoh pub "rt/openneuro/led/cmd" -'),
            ("LED 关闭", 'echo \'{"on": 0}\' | zenoh pub "rt/openneuro/led/cmd" -'),
            ("舵机 0°", 'echo \'{"angle": 0}\' | zenoh pub "rt/openneuro/servo/cmd" -'),
            ("舵机 90°", 'echo \'{"angle": 90}\' | zenoh pub "rt/openneuro/servo/cmd" -'),
            ("舵机 180°", 'echo \'{"angle": 180}\' | zenoh pub "rt/openneuro/servo/cmd" -'),
            ("查看传感器", 'zenoh sub "rt/openneuro/sensor/**"'),
        ]
        
        print("\n快速命令参考:\n")
        for i, (desc, cmd) in enumerate(commands, 1):
            print(f"  {i}. {desc}")
            print(f"     $ {cmd}")
            print()
    
    def run(self):
        """运行所有测试"""
        self.print_banner()
        
        input("按 Enter 开始测试...")
        
        # 测试 1: LED 闪烁
        self.test_led_toggle()
        input("\n按 Enter 继续到测试 2...")
        
        # 测试 2: 舵机扫动
        self.test_servo_sweep()
        input("\n按 Enter 继续到测试 3...")
        
        # 测试 3: 传感器读取
        self.test_sensor_reading()
        input("\n按 Enter 继续到测试 4...")
        
        # 测试 4: 延迟测量
        self.test_latency_measurement()
        input("\n按 Enter 进入交互模式...")
        
        # 交互模式
        self.run_interactive_mode()
        
        # 总结
        elapsed = self.get_elapsed_time()
        print("\n" + "=" * 80)
        logger.info("Demo 完成！")
        logger.info(f"总耗时: {elapsed:.1f} 秒")
        print("=" * 80)
        print("\n下一步:")
        print("  1. 尝试修改 ESP32 固件中的舵机角度范围")
        print("  2. 添加更多传感器（温湿度、IMU 等）")
        print("  3. 集成 ROS 2，使用 rqt_plot 可视化数据")
        print("  4. 实现多个 ESP32 的协同控制")
        print("\n祝你玩得开心！ 🚀\n")


def main():
    """主入口"""
    demo = HelloWorldDemo()
    try:
        demo.run()
    except KeyboardInterrupt:
        logger.info("\nDemo 被中断")
        return 1
    except Exception as e:
        logger.error(f"Error: {e}", exc_info=True)
        return 1
    
    return 0


if __name__ == '__main__':
    sys.exit(main())

