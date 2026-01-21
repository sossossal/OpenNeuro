@echo off
REM OpenNeuro GitHub 自动上传脚本
REM 此脚本将自动完成所有 Git 操作

echo =========================================
echo OpenNeuro GitHub 自动上传脚本
echo =========================================
echo.

REM 设置颜色（可选）
color 0A

REM 1. 进入项目目录
echo [1/10] 进入项目目录...
cd /d "D:\VScode专用\OpenNeuro"
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 无法进入目录 D:\VScode专用\OpenNeuro
    echo 请确认目录是否存在
    pause
    exit /b 1
)
echo [完成] 当前目录: %CD%
echo.

REM 2. 初始化 Git 仓库
echo [2/10] 初始化 Git 仓库...
git init
if %ERRORLEVEL% NEQ 0 (
    echo [警告] Git 初始化失败，可能已经初始化过
)
echo [完成]
echo.

REM 3. 配置 Git 用户信息
echo [3/10] 配置 Git 用户信息...
echo 请输入您的 GitHub 用户名:
set /p GIT_USERNAME=
git config user.name "%GIT_USERNAME%"

echo 请输入您的 GitHub 邮箱:
set /p GIT_EMAIL=
git config user.email "%GIT_EMAIL%"
echo [完成] 用户名: %GIT_USERNAME%
echo [完成] 邮箱: %GIT_EMAIL%
echo.

REM 4. 添加所有文件
echo [4/10] 添加所有文件到 Git...
echo 这可能需要几分钟，请稍候...
git add .
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 添加文件失败
    pause
    exit /b 1
)
echo [完成] 所有文件已添加
echo.

REM 5. 创建首次提交
echo [5/10] 创建首次提交...
git commit -m "feat: initial commit - OpenNeuro v0.2.0-alpha.1

Major Features:
- PTP Stack (Master/Slave) with ~10ns latency
- TSN Scheduler with O(1) gate control
- Zenoh Bridge with HAL abstraction
- Hardware timestamp support (M2.2)
- Complete documentation (20+ docs)
- CI/CD pipeline
- 88%% test coverage

Components:
- ptp-stack/: PTP protocol implementation
- tsn-scheduler/: TSN scheduling with Python bindings
- zenoh-bridge/: Zenoh HAL and QoS mapping
- hardware/: Zone Controller design docs
- docs/: Technical documentation
- examples/: Integration examples

Performance:
- PTP TX latency: ~10ns (2000x better than target)
- TSN scheduling: O(1) complexity
- Test coverage: 88%%

Project Status: Alpha to Beta transition"

if %ERRORLEVEL% NEQ 0 (
    echo [错误] 提交失败
    pause
    exit /b 1
)
echo [完成] 首次提交已创建
echo.

REM 6. 提示创建 GitHub 仓库
echo [6/10] 创建 GitHub 仓库
echo =========================================
echo 请按照以下步骤在 GitHub 创建仓库:
echo.
echo 1. 打开浏览器访问: https://github.com/new
echo 2. 仓库名称: OpenNeuro
echo 3. 描述: Next-generation communication architecture for embodied AI robots
echo 4. 选择 Public (公开)
echo 5. 不要勾选任何初始化选项 (README, .gitignore, License)
echo 6. 点击 "Create repository"
echo.
echo 完成后，复制仓库 URL (例如: https://github.com/username/OpenNeuro.git)
echo =========================================
echo.
echo 请输入您的 GitHub 仓库 URL:
set /p REPO_URL=
echo.

REM 7. 添加远程仓库
echo [7/10] 添加远程仓库...
git remote add origin %REPO_URL%
if %ERRORLEVEL% NEQ 0 (
    echo [警告] 添加远程仓库失败，可能已经存在
    echo 尝试更新远程仓库 URL...
    git remote set-url origin %REPO_URL%
)
echo [完成] 远程仓库: %REPO_URL%
echo.

REM 8. 重命名分支为 main
echo [8/10] 重命名分支为 main...
git branch -M main
echo [完成]
echo.

REM 9. 推送到 GitHub
echo [9/10] 推送到 GitHub...
echo 这可能需要几分钟，请稍候...
echo 如果弹出登录窗口，请输入您的 GitHub 凭据
echo.
git push -u origin main
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 推送失败
    echo.
    echo 可能的原因:
    echo 1. 需要 GitHub 登录认证
    echo 2. 网络连接问题
    echo 3. 仓库 URL 不正确
    echo.
    echo 请尝试手动推送: git push -u origin main
    pause
    exit /b 1
)
echo [完成] 代码已成功推送到 GitHub!
echo.

REM 10. 完成
echo [10/10] 上传完成!
echo =========================================
echo.
echo ✅ OpenNeuro 已成功上传到 GitHub!
echo.
echo 您的仓库地址: %REPO_URL%
echo.
echo 下一步建议:
echo 1. 访问仓库页面查看代码
echo 2. 添加 Topics 标签 (robotics, ptp, tsn, zenoh, etc.)
echo 3. 创建 Release v0.2.0-alpha.1
echo 4. 在社交媒体分享项目
echo.
echo =========================================
echo.
pause
