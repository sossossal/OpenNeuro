@echo off
REM 提交所有 workflow 文件的 v4 更新

echo =========================================
echo 提交 GitHub Actions v4 更新
echo =========================================
echo.

cd /d "D:\新建文件夹\OpenNeuro"

echo [1/4] 检查当前状态...
git status
echo.

echo [2/4] 添加修改的文件...
git add .github/workflows/build.yml .github/workflows/ci-ptp.yml
echo [完成]
echo.

echo [3/4] 提交更改...
git commit -m "fix: update all workflow files to use actions v4

- Update build.yml: all artifact actions to v4
- Update ci-ptp.yml: checkout and artifact actions to v4
- Fixes deprecated actions warnings in CI/CD

Changes:
- actions/upload-artifact@v3 -> v4
- actions/download-artifact@v3 -> v4
- actions/checkout@v3 -> v4

This resolves all deprecation warnings in GitHub Actions."

if %ERRORLEVEL% NEQ 0 (
    echo [错误] 提交失败
    pause
    exit /b 1
)
echo [完成]
echo.

echo [4/4] 推送到 GitHub...
git push origin main

if %ERRORLEVEL% NEQ 0 (
    echo [错误] 推送失败
    pause
    exit /b 1
)
echo [完成]
echo.

echo =========================================
echo ✅ 所有 workflow 文件已成功更新！
echo =========================================
echo.
echo 访问以下链接查看 CI/CD 状态:
echo https://github.com/sossossal/OpenNeuro/actions
echo.
pause
