#!/bin/bash
# OpenNeuro CI/CD Fix - Git Commit Script

cd "D:\VScode专用\OpenNeuro" || exit 1

echo "Adding files..."
git add .github/workflows/ci-cd.yml CI_CD_FIX.md

echo "Committing..."
git commit -m "fix: update GitHub Actions to v4 and improve error handling"

echo "Pushing to GitHub..."
git push origin main

echo "Done!"
