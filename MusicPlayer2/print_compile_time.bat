@echo off
setlocal

:: Get formated date time by PowerShell.
for /f "tokens=*" %%i in ('powershell -Command "Get-Date -Format 'yyyy-MM-dd HH:mm:ss'"') do set "formatted_datetime=%%i"

:: output to ompile_time.txt file.
echo %formatted_datetime% > compile_time.txt

:: Get the current Git commit hash (without showing error messages)
for /f %%i in ('git rev-parse HEAD 2^>nul') do set commit_hash=%%i

:: If the git command is executed successfully, the commit hash is appended to the compile_time.txt file.
if not "%commit_hash%"=="" (
    echo %commit_hash% >> compile_time.txt
)

endlocal