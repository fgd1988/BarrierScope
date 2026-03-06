# Detection of BTB invalidation

Uses BarrierScope to detect BTB invalidation. The BTB manipulations are performed by executing the `jmp` or `ret` instruction.
In our attack code, we placed `jmp` instructions for mistraining at the same virtual address in the attacker as in the victim process. 
We chose `jmp` instruction for the reason that, unlike a return instruction, there were no adjacent operations that might un-evict the return address (e.g., by accessing the stack) and limit speculative execution. 
In order to get the victim to speculatively execute the gadget, we caused the memory location containing the jump destination to be uncached. In addition, we mistrained the branch predictor to send speculative execution to the gadget.


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
...output...
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

```


Evaluate the leakage rate for different umwait timeouts from 1000 to 200000. The result is stored in log.csv.
```
./test.sh
```

Expected results are as follows:
```

```

In our previous experiments, we found that 

```
gcc arch-write.c -lpthread -o arch -static -lm -O0

./arch
```
