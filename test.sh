LOG_FILE=/tmp/process_memory_status.log
echo "Memory status of the process using most memory" >> "${LOG_FILE}"
while true; do
    date >> "${LOG_FILE}"
    ps aux | sort -r -k 4 | head -6 >> "${LOG_FILE}"
    PID=$(ps aux | sort -nr -k 4 | head -1 | awk '{print $2}')
    pmap $PID >> "${LOG_FILE}"
    sleep 1
done
