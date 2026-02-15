#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <linux/input.h>

// 函数声明
void log_key_event(int type, int code, int value);

int main() {
    struct input_event ev;
    int fd, rd;
    const char *dev = "/dev/input/event3"; // 替换为你的键盘设备路径

    // 打开键盘设备
    fd = open(dev, O_RDONLY);
    if (fd == -1) {
        perror("Cannot open device");
        exit(EXIT_FAILURE);
    }

    // 创建日志文件
    FILE *logfile;
    char filename[100];
    snprintf(filename, sizeof(filename), "keylog_rdtscp.txt"); // 文件名可以根据需要修改
    logfile = fopen(filename, "w");
    if (logfile == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    fprintf(logfile, "Timestamp,Event,Key\n");

    // 读取键盘事件并记录
    while (1) {
        rd = read(fd, &ev, sizeof(struct input_event));
        if (rd == -1) {
            if (errno == EINTR)
                continue;
            else
                break;
        } else if (rd == sizeof(struct input_event)) {
            if (ev.type == EV_KEY) {
                log_key_event(ev.type, ev.code, ev.value);
            }
        }
    }

    // 关闭文件和设备
    fclose(logfile);
    close(fd);

    return 0;
}

// 记录按键事件到日志文件
void log_key_event(int type, int code, int value) {
    FILE *logfile = fopen("keylog_rdtscp.txt", "a");
    unsigned int eax, edx;
    unsigned long long tsc;

    // 使用rdtscp指令获取CPU周期计数器值
    asm volatile ("rdtscp\n\t"
                  "mov %%eax, %0\n\t"
                  "mov %%edx, %1\n\t"
                  : "=r" (eax), "=r" (edx)
                  :
                  : "%rax", "%rcx", "%rdx");

    tsc = ((unsigned long long)edx << 32) | eax;

    // 将事件记录到文件
    fprintf(logfile, "%llu,%s,%d\n", tsc, (value ? "Pressed" : "Released"), code);
    fclose(logfile);
}

