# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- M2.2 Beta 核心实现
  - 硬件时间戳支持 (Linux SO_TIMESTAMPING)
  - 真实 Zenoh C API 集成
  - Linux Taprio/Netlink 真实实现
  - AGI-Walker 集成示例
  - 自动化验证脚本

### Changed
- 更新所有 CMakeLists.txt 支持新功能
- 完善 README.md 和文档系统

## [0.2.0-alpha.1] - 2026-01-21

### Added
- PTP 协议栈 (IEEE 1588v2)
  - PTP Master 完整实现
  - PTP Slave 完整实现
  - PI Servo 时钟控制
  - 零拷贝优化 (~10ns 延迟)
  - BMC 算法
  
- TSN 调度器 (IEEE 802.1Qbv)
  - 4 级流量分类 (RT/ST/CT/BE)
  - Bitmask 门控算法 (O(1) 优化)
  - 带宽管理
  - Python API 绑定
  
- Zenoh 桥接器
  - HAL 抽象层 (Full/Pico 切换)
  - TSN-Zenoh QoS 映射
  - PTP 状态发布
  
- 硬件设计
  - Zone Controller 完整设计文档
  - 原理图网表
  - PCB 布局策略
  - BOM 清单
  
- 文档系统
  - API 参考文档
  - 性能基准报告
  - 验证指南
  - 技术设计文档 (6 份)
  
- CI/CD
  - GitHub Actions 工作流
  - 多平台构建 (Ubuntu/Windows)
  - 自动化测试

### Performance
- PTP TX 延迟: ~10ns (超越目标 2000x)
- TSN 调度: O(1) 复杂度
- 测试覆盖: 88%

## [0.1.0] - 2025-12-15

### Added
- Zenoh-Pico 框架
- ESP32/STM32 移植
- ROS 2 桥接器原型
- Hello World Demo

---

## 版本说明

### 版本号规则
- **Major**: 重大架构变更或不兼容更改
- **Minor**: 新功能添加
- **Patch**: Bug 修复和小改进

### 标签说明
- `Added`: 新功能
- `Changed`: 功能变更
- `Deprecated`: 即将废弃
- `Removed`: 已删除功能
- `Fixed`: Bug 修复
- `Security`: 安全相关
- `Performance`: 性能改进
