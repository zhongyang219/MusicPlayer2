@echo off
setlocal

:: 获取本地日期时间
for /f "tokens=2 delims==." %%i in ('wmic os get localdatetime /value') do set datetime=%%i
:: 提取各部分（cmd不区分大小写）
set year=%datetime:~0,4%
set month=%datetime:~4,2%
set day=%datetime:~6,2%
set hh=%datetime:~8,2%
set mm=%datetime:~10,2%
set ss=%datetime:~12,2%
:: 格式化为 yyyy-MM-dd HH:mm:ss
set formatted_datetime=%year%-%month%-%day% %hh%:%mm%:%ss%
:: 输出到 compile_time.txt 文件
echo %formatted_datetime% > compile_time.txt

:: 获取当前的 Git 提交哈希 (不显示错误信息)
for /f %%i in ('git rev-parse HEAD 2^>nul') do set commit_hash=%%i
:: 如果git命令执行成功那么将提交哈希追加到 compile_time.txt 文件中
if not "%commit_hash%"=="" (echo %commit_hash% >> compile_time.txt)

endlocal
