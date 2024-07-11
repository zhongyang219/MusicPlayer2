@echo off
setlocal

:: Get the local date and time
for /f "tokens=2 delims==." %%i in ('wmic os get localdatetime /value') do set datetime=%%i
:: Extract the parts (cmd is not case sensitive)
set year=%datetime:~0,4%
set month=%datetime:~4,2%
set day=%datetime:~6,2%
set hh=%datetime:~8,2%
set mm=%datetime:~10,2%
set ss=%datetime:~12,2%
:: Formatted as yyyy-MM-dd HH:mm:ss
set formatted_datetime=%year%-%month%-%day% %hh%:%mm%:%ss%
:: Output to compile_time.txt file
echo %formatted_datetime% > compile_time.txt

:: Get the current Git commit hash (without showing error messages)
for /f %%i in ('git rev-parse HEAD 2^>nul') do set commit_hash=%%i
:: If the git command is executed successfully, the commit hash is appended to the compile_time.txt file.
if not "%commit_hash%"=="" (echo %commit_hash% >> compile_time.txt)

endlocal
