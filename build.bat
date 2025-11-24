@echo off
setlocal

python "%~dp0\tools\build.py" --build_dir "%~dp0\build\Windows" %*
