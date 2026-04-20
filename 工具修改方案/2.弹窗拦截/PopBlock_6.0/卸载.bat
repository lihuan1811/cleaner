@ECHO OFF & CD /D %~DP0
>NUL 2>&1 reg.exe query "HKU\S-1-5-19" || (
    ECHO SET UAC = CreateObject^("Shell.Application"^) > "%TEMP%\Getadmin.vbs"
    ECHO UAC.ShellExecute "%~f0", "%1", "", "runas", 1 >> "%TEMP%\Getadmin.vbs"
    "%TEMP%\Getadmin.vbs"
    DEL /f /q "%TEMP%\Getadmin.vbs" 2>NUL
    Exit /b
)
askkill /f /im PopBlock.exe 2>NUL
reg delete HKLM\SOFTWARE\Huorong /f 2>NUL
schtasks /delete /TN "Huorong PopBlock" /f 2>NUL
rd /s /q C:\ProgramData\Huorong 2>NUL
