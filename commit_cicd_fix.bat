@echo off
REM OpenNeuro CI/CD 修复提交脚本

echo =========================================
echo OpenNeuro CI/CD 修复提交
echo =========================================
echo.

REM 进入 OpenNeuro 目录
cd /d "D:\VScode专用\OpenNeuro"
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 无法进入目录
    pause
    exit /b 1
)

echo [1/5] 当前目录: %CD%
echo.

REM 检查状态
echo [2/5] 检查 Git 状态...
git status
echo.

REM 添加文件
echo [3/5] 添加修改的文件...
git add .github/workflows/ci-cd.yml CI_CD_FIX.md
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 添加文件失败
    pause
    exit /b 1
)
echo [完成] 文件已添加
echo.

REM 提交
echo [4/5] 提交修改...
git commit -m "fix: update GitHub Actions to v4 and improve error handling

- Update all artifact actions from v3 to v4
- Update checkout action to v4
- Add Python setup action v5
- Add fail-fast: false to prevent cascade failures
- Add continue-on-error to optional test steps
- Improve error handling in shell scripts

Fixes:
- Deprecated actions/upload-artifact@v3
- Deprecated actions/download-artifact@v3
- Build cascade failures
- Missing error handling"

if %ERRORLEVEL% NEQ 0 (
    echo [错误] 提交失败
    pause
    exit /b 1
)
echo [完成] 提交成功
echo.

REM 推送
echo [5/5] 推送到 GitHub...
git push origin main
if %ERRORLEVEL% NEQ 0 (
    echo [错误] 推送失败
    echo.
    echo 可能的原因:
    echo 1. 需要 GitHub 认证
    echo 2. 网络连接问题
    echo 3. 远程仓库冲突
    pause
    exit /b 1
)
echo [完成] 推送成功
echo.

echo =========================================
echo ✅ CI/CD 修复已成功推送到 GitHub!
echo =========================================
echo.
echo 下一步:
echo 1. 访问 https://github.com/sossossal/OpenNeuro/actions
echo 2. 查看新的 workflow run
echo 3. 确认所有任务使用 v4 actions
echo 4. 验证构建成功
echo.
pause
