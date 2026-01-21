# Contributing to OpenNeuro

感谢您对 OpenNeuro 项目的关注！我们欢迎所有形式的贡献。

## 🤝 贡献方式

### 报告 Bug

如果您发现了 Bug，请：

1. 检查 [Issues](https://github.com/OpenNeuro/OpenNeuro/issues) 确认问题未被报告
2. 创建新 Issue，使用 Bug Report 模板
3. 提供详细的复现步骤和环境信息

### 提出新功能

如果您有新功能建议：

1. 创建 Feature Request Issue
2. 描述功能的用途和价值
3. 讨论实现方案

### 提交代码

1. **Fork 仓库**
   ```bash
   git clone https://github.com/YOUR_USERNAME/OpenNeuro.git
   cd OpenNeuro
   ```

2. **创建分支**
   ```bash
   git checkout -b feature/your-feature-name
   # 或
   git checkout -b fix/your-bug-fix
   ```

3. **编写代码**
   - 遵循项目代码风格
   - 添加必要的测试
   - 更新相关文档

4. **提交更改**
   ```bash
   git add .
   git commit -m "feat: add your feature description"
   ```

5. **推送并创建 PR**
   ```bash
   git push origin feature/your-feature-name
   ```

## 📝 代码规范

### C/C++ 代码

- 使用 4 空格缩进
- 函数命名：`snake_case`
- 类型命名：`snake_case_t`
- 宏命名：`UPPER_CASE`

### Python 代码

- 遵循 PEP 8
- 使用 4 空格缩进
- 函数命名：`snake_case`
- 类命名：`PascalCase`

### Commit 消息

遵循 [Conventional Commits](https://www.conventionalcommits.org/)：

```
<type>(<scope>): <subject>

<body>

<footer>
```

**类型**:
- `feat`: 新功能
- `fix`: Bug 修复
- `docs`: 文档更新
- `style`: 代码格式
- `refactor`: 重构
- `test`: 测试
- `chore`: 构建/工具

**示例**:
```
feat(ptp): add hardware timestamp support

Implement SO_TIMESTAMPING for Linux to achieve <1µs precision.

Closes #123
```

## 🧪 测试

提交代码前请确保：

```bash
# 编译所有组件
./scripts/build_all.sh

# 运行所有测试
./scripts/verify_all.sh

# 检查测试覆盖
# (确保新代码有测试覆盖)
```

## 📚 文档

如果您的更改影响用户使用：

- 更新 `README.md`
- 更新 `API_REFERENCE.md`
- 添加示例代码
- 更新 `CHANGELOG.md`

## 🔍 代码审查

所有 PR 都需要：

- ✅ 通过 CI/CD 测试
- ✅ 至少一位维护者审查
- ✅ 解决所有审查意见
- ✅ 更新相关文档

## 💬 社区

- **Discord**: https://discord.gg/openneuro
- **GitHub Discussions**: https://github.com/OpenNeuro/OpenNeuro/discussions

## 📄 许可证

提交代码即表示您同意将代码以 MIT License 发布。

---

**感谢您的贡献！** 🎉
