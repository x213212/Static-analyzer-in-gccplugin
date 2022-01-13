int parseLine(char *line) {
    // This assumes that a digit will be found and the line ends in " Kb".
    int i = strlen(line);
    const char *p = line;
    while (*p < '0' || *p > '9') p++;
    line[i - 3] = '\0';
    i = atoi(p);
    return i;
}

// 虚拟内存和物理内存，单位为kb
typedef struct {
    uint32_t virtualMem;
    uint32_t physicalMem;
} processMem_t;

processMem_t GetProcessMemory() {
    FILE *file = fopen("/proc/self/status", "r");
    char line[128];
    processMem_t processMem;

    while (fgets(line, 128, file) != NULL) {
        if (strncmp(line, "VmSize:", 7) == 0) {
            processMem.virtualMem = parseLine(line);
            break;
        }

        if (strncmp(line, "VmRSS:", 6) == 0) {
            processMem.physicalMem = parseLine(line);
            break;
        }
    }
    fclose(file);
    return processMem;
}

struct timespec diff(struct timespec start, struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec - start.tv_nsec) < 0)
	{
		temp.tv_sec = end.tv_sec - start.tv_sec - 1;
		temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
	}
	else
	{
		temp.tv_sec = end.tv_sec - start.tv_sec;
		temp.tv_nsec = end.tv_nsec - start.tv_nsec;
	}
	return temp;
}