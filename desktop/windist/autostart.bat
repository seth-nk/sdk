@echo off

reg add "HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run" /v "seth4win" /t REG_SZ /d "\"%~dp0sethdkt-cli.exe\"" /f
if '%errorlevel%' == '0' ( goto gotPrivileges ) else (
if not "%1" == "ranas" (
powershell "Start-Process -FilePath \"%0\" -ArgumentList \"ranas\" -verb runas" >nul 2>&1
)
)
exit /b 
:gotPrivileges
