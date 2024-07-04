# 获取当前时间并覆写文件
(Get-Date -Format 'yyyy-MM-dd HH:mm:ss') > compile_time.txt

# 获取当前提交的 hash 并追加到文件
(git rev-parse HEAD) >> compile_time.txt
