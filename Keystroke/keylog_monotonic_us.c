// keylog_monotonic_us.c
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <poll.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

// usage: 
// gcc -O2 -Wall keylog_monotonic_us.c -o keylog_monotonic_us
// sudo ./keylog_monotonic_us
// use Ctrl+C to stop

/*
 * =============================================================     
 * 如何找到你的键盘设备路径 替换argv[1]
 *
 * 推荐方式（最稳妥）：
 *
 *   ls -l /dev/input/by-id/*event-kbd
 *
 * 常见输出示例：
 *
 *   /dev/input/by-id/platform-i8042-serio-0-event-kbd
 *     -> 笔记本内建键盘
 *
 *   /dev/input/by-id/usb-Logitech_USB_Receiver-if01-event-kbd
 *     -> USB / 无线键盘
 *
 * 选择包含 "event-kbd" 的路径即可。
 *
 * =============================================================
 */


static volatile sig_atomic_t g_stop = 0;

static void on_sigint(int sig) {
    (void)sig;
    g_stop = 1;
}

static void die(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    if (errno) fprintf(stderr, ": %s\n", strerror(errno));
    exit(1);
}

static inline uint64_t timeval_to_us(const struct timeval *tv) {
    return (uint64_t)tv->tv_sec * 1000000ull + (uint64_t)tv->tv_usec;
}

int main(int argc, char **argv) {
    const char *dev =
        "/dev/input/by-id/usb-Logitech_USB_Receiver-if01-event-kbd"; // 可用 argv[1] 覆盖
    const char *out_path = "keylog_evdev_ts_us.csv";                // 可用 argv[2] 覆盖

    if (argc >= 2) dev = argv[1];
    if (argc >= 3) out_path = argv[2];

    // Ctrl+C 优雅退出
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = on_sigint;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) == -1) die("sigaction");

    int fd = open(dev, O_RDONLY);
    if (fd == -1) die("open %s", dev);

    // 尝试设定事件时间戳时钟为 CLOCK_MONOTONIC（不支持就继续默认）
    int clk = CLOCK_MONOTONIC;
    if (ioctl(fd, EVIOCSCLOCKID, &clk) == -1) {
        fprintf(stderr, "ioctl(EVIOCSCLOCKID) failed, continue with default clock: %s\n",
                strerror(errno));
        errno = 0;
    }

    FILE *logf = fopen(out_path, "w");
    if (!logf) die("fopen %s", out_path);

    // 行缓冲：每行尽量尽快落到 libc 缓冲区外
    setvbuf(logf, NULL, _IOLBF, 0);
    fprintf(logf, "timestamp_us,event,key_code\n");
    fflush(logf);

    printf("Listening on %s\n", dev);
    printf("Writing CSV to %s\n", out_path);
    printf("Press Ctrl+C to stop gracefully.\n");

    struct pollfd pfd = { .fd = fd, .events = POLLIN };
    struct input_event ev;

    while (!g_stop) {
        int pr = poll(&pfd, 1, 3000);
        if (pr == -1) {
            if (errno == EINTR) continue;
            die("poll");
        }
        if (pr == 0) continue;

        ssize_t rd = read(fd, &ev, sizeof(ev));
        if (rd == -1) {
            if (errno == EINTR) continue;
            die("read");
        }
        if (rd != sizeof(ev)) continue;

        if (ev.type == EV_KEY) {
            const char *state = (ev.value == 1) ? "Pressed"
                               : (ev.value == 0) ? "Released"
                               : "Repeat";
            uint64_t ts_us = timeval_to_us(&ev.time);

            fprintf(logf, "%llu,%s,%u\n",
                    (unsigned long long)ts_us, state, (unsigned)ev.code);
        }
    }

    fflush(logf);
    int fdlog = fileno(logf);
    if (fdlog >= 0) fsync(fdlog);
    fclose(logf);
    close(fd);

    printf("Stopped. Log saved to %s\n", out_path);
    return 0;
}
