# Detection of BTB invalidation

Uses BarrierScope to detect BTB invalidation. The BTB manipulations are performed by executing the `jmp` or `ret` instruction.
In our attack code, we placed `jmp` instructions for mistraining at the same virtual address in the attacker as in the victim process. 
We chose `jmp` instruction for the reason that, unlike a return instruction, there were no adjacent operations that might un-evict the return address (e.g., by accessing the stack) and limit speculative execution. 
In order to get the victim to speculatively execute the gadget, we caused the memory location containing the jump destination to be uncached. In addition, we mistrained the branch predictor to send speculative execution to the gadget.

We utilize `syscall` to examine the effectiveness of our BTB invalidation detection code. In file `detetion`, code without the insertion of `syscall` results in a high success rate, while the code with a `syscall` insertion in file `detetion_invalidation` results in a low success rate because of the invalidation of BTB.

## How to Run
Build our code
```
make
```
Quick test for the attack. 
```
sudo ./detection
```
Expected results are as follows. 
```
Progress: 100/1000 attempts completed. Current success rate: 100.00%
Progress: 200/1000 attempts completed. Current success rate: 99.00%
Progress: 300/1000 attempts completed. Current success rate: 99.33%
Progress: 400/1000 attempts completed. Current success rate: 98.25%
Progress: 500/1000 attempts completed. Current success rate: 98.40%
Progress: 600/1000 attempts completed. Current success rate: 98.67%
Progress: 700/1000 attempts completed. Current success rate: 98.86%
Progress: 800/1000 attempts completed. Current success rate: 98.88%
Progress: 900/1000 attempts completed. Current success rate: 99.00%
Progress: 1000/1000 attempts completed. Current success rate: 99.10%
----------------------------------------
Attack completed!
Total attempts: 1000
Successful attempts: 991
Success rate: 99.10%
----------------------------------------
Note: The target character is 'C' (ASCII: 67).
In each successful attack, the predicted character should be 'C' (ASCII: 67).



In `detetion_invalidation.c`, we insert `syscall` to invalidate the BTB.
```
pid_t pid = syscall(SYS_getpid);
```
Build code and test.
```
make
sudo ./detection
```

```
Expected results are as follows. 
```
Target character: 'C' (ASCII: 67)
Total attempts: 1000
----------------------------------------
Progress: 100/1000 attempts completed. Current success rate: 2.00%
Progress: 200/1000 attempts completed. Current success rate: 1.00%
Progress: 300/1000 attempts completed. Current success rate: 0.67%
Progress: 400/1000 attempts completed. Current success rate: 0.50%
Progress: 500/1000 attempts completed. Current success rate: 0.40%
Progress: 600/1000 attempts completed. Current success rate: 0.33%
Progress: 700/1000 attempts completed. Current success rate: 0.29%
Progress: 800/1000 attempts completed. Current success rate: 0.25%
Progress: 900/1000 attempts completed. Current success rate: 0.22%
Progress: 1000/1000 attempts completed. Current success rate: 0.20%
----------------------------------------
Attack completed!
Total attempts: 1000
Successful attempts: 2
Success rate: 0.20%
----------------------------------------
Note: The target character is 'C' (ASCII: 67).
In each successful attack, the predicted character should be 'C' (ASCII: 67).
```

In our previous experiments, we found that 

