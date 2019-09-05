@echo off

cd %~dp0

FOR %%F IN (.\*.sth3) DO (
  set filename=%%F
  goto x
)
:x
echo "找到并选择 Seth 数据文件 %filename%"

set /p password=< password-easy-win.txt

if [%password%] == [] (
  echo "未设置密码，将使用 123123。要设置密码。"
  set filename=123123
) 

sethcli4win -NoConfigFileCreation -LoadPlatformDefaultConfig -OverrideConfig seth.filename "%filename%" -OverrideConfig pap.password "%password%"
