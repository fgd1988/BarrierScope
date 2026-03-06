#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <linux/input.h>


void log_key_event(int type, int code, int value);

int main() {
    struct input_event ev;
    int fd, rd;
    const char *dev = "/dev/input/event3"; // Replace with the path of your keyboard device

    // Turn on the keyboard device
    fd = open(dev, O_RDONLY);
    if (fd == -1) {
        perror("Cannot open device");
        exit(EXIT_FAILURE);
    }


    FILE *logfile;
    char filename[100];
    snprintf(filename, sizeof(filename), "keylog_rdtscp.txt"); // 文件名可以根据需要修改
    logfile = fopen(filename, "w");
    if (logfile == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    fprintf(logfile, "Timestamp,Event,Key\n");

    // Read keyboard events and record them
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

    // Close the files and the devices.
    fclose(logfile);
    close(fd);

    return 0;
}

// Record key events in the log file
void log_key_event(int type, int code, int value) {
    FILE *logfile = fopen("keylog_rdtscp.txt", "a");
    unsigned int eax, edx;
    unsigned long long tsc;

    // Use the rdtscp instruction to obtain the CPU cycle counter value
    asm volatile ("rdtscp\n\t"
                  "mov %%eax, %0\n\t"
                  "mov %%edx, %1\n\t"
                  : "=r" (eax), "=r" (edx)
                  :
                  : "%rax", "%rcx", "%rdx");

    tsc = ((unsigned long long)edx << 32) | eax;

    // Record the event in a file
    fprintf(logfile, "%llu,%s,%d\n", tsc, (value ? "Pressed" : "Released"), code);
    fclose(logfile);
}

