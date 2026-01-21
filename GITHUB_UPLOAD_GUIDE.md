# OpenNeuro GitHub 上传指南

本文档说明如何将 OpenNeuro 项目上传到 GitHub。

---

## 📋 准备工作

### 1. 检查文件完整性

确保以下文件存在：

- [x] `LICENSE` - MIT 许可证
- [x] `README.md` - 项目说明
- [x] `CONTRIBUTING.md` - 贡献指南
- [x] `CHANGELOG.md` - 更新日志
- [x] `SECURITY.md` - 安全政策
- [x] `.gitignore` - Git 忽略文件
- [x] `.github/ISSUE_TEMPLATE/` - Issue 模板
- [x] `.github/PULL_REQUEST_TEMPLATE.md` - PR 模板
- [x] `.github/workflows/ci-cd.yml` - CI/CD 配置

---

## 🚀 上传步骤

### 方法 1: 使用脚本 (推荐)

```bash
cd D:\VScode专用\OpenNeuro

# 运行 Git 初始化脚本
bash scripts/git_init.sh

# 添加远程仓库
git remote add origin https://github.com/YOUR_USERNAME/OpenNeuro.git

# 推送到 GitHub
git branch -M main
git push -u origin main
```

---

### 方法 2: 手动操作

#### 步骤 1: 初始化 Git 仓库

```bash
cd D:\VScode专用\OpenNeuro
git init
```

#### 步骤 2: 配置 Git

```bash
git config user.name "Your Name"
git config user.email "your.email@example.com"
```

#### 步骤 3: 添加文件

```bash
git add .
```

#### 步骤 4: 创建首次提交

```bash
git commit -m "feat: initial commit - OpenNeuro v0.2.0-alpha.1"
```

#### 步骤 5: 创建 GitHub 仓库

1. 访问 https://github.com/new
2. 仓库名称: `OpenNeuro`
3. 描述: `Next-generation communication architecture for embodied AI robots`
4. 选择 Public
5. **不要**初始化 README, .gitignore, License (我们已经有了)
6. 点击 "Create repository"

#### 步骤 6: 添加远程仓库

```bash
git remote add origin https://github.com/YOUR_USERNAME/OpenNeuro.git
```

#### 步骤 7: 推送到 GitHub

```bash
git branch -M main
git push -u origin main
```

---

## 🏷️ 创建 Release

### 创建 v0.2.0-alpha.1 Release

1. 访问 GitHub 仓库页面
2. 点击 "Releases" → "Create a new release"
3. 填写信息：
   - **Tag**: `v0.2.0-alpha.1`
   - **Title**: `OpenNeuro v0.2.0-alpha.1 (M2.1 Alpha)`
   - **Description**: 复制 `RELEASE_NOTES_v0.2.0-alpha.1.md` 内容
4. 勾选 "This is a pre-release"
5. 点击 "Publish release"

---

## 📝 GitHub 仓库设置

### 1. 仓库描述

```
Next-generation communication architecture for embodied AI robots. 
Features PTP time sync (<1µs), TSN scheduling, and Zenoh integration.
```

### 2. Topics (标签)

添加以下 topics：

- `robotics`
- `ptp`
- `tsn`
- `zenoh`
- `real-time`
- `time-synchronization`
- `embodied-ai`
- `ieee-1588`
- `ieee-802-1q`

### 3. 启用功能

- [x] Issues
- [x] Projects
- [x] Wiki
- [x] Discussions
- [x] Actions

### 4. 分支保护

为 `main` 分支设置保护规则：

- [x] Require pull request reviews before merging
- [x] Require status checks to pass before merging
- [x] Require branches to be up to date before merging

---

## 📊 GitHub Actions

CI/CD 管道会自动运行：

- ✅ 多平台构建 (Ubuntu/Windows)
- ✅ 自动化测试
- ✅ 性能基准测试
- ✅ 发布打包

检查状态：https://github.com/YOUR_USERNAME/OpenNeuro/actions

---

## 🎨 添加徽章

在 `README.md` 顶部添加徽章：

```markdown
![Build Status](https://github.com/YOUR_USERNAME/OpenNeuro/workflows/CI%2FCD/badge.svg)
![License](https://img.shields.io/github/license/YOUR_USERNAME/OpenNeuro)
![Version](https://img.shields.io/github/v/release/YOUR_USERNAME/OpenNeuro)
![Stars](https://img.shields.io/github/stars/YOUR_USERNAME/OpenNeuro)
```

---

## 🔗 相关链接

- **GitHub**: https://github.com/YOUR_USERNAME/OpenNeuro
- **Issues**: https://github.com/YOUR_USERNAME/OpenNeuro/issues
- **Discussions**: https://github.com/YOUR_USERNAME/OpenNeuro/discussions
- **Actions**: https://github.com/YOUR_USERNAME/OpenNeuro/actions

---

## ✅ 上传检查清单

- [ ] Git 仓库初始化
- [ ] 首次提交完成
- [ ] GitHub 仓库创建
- [ ] 代码推送成功
- [ ] Release 创建
- [ ] 仓库设置完成
- [ ] Topics 添加
- [ ] 分支保护设置
- [ ] CI/CD 运行成功
- [ ] README 徽章添加

---

**祝上传顺利！** 🚀
