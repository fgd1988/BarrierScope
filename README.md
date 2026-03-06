# BarrierScope

This repository contains the experiments of evaluation and case studies discussed in the paper  
* "BarrierScope: Exploiting Indirect Branch Prediction Barrier for Mounting Interrupt Side-Channel Attacks"
  
BarrierScope is an interrupt side channel attack that exploits IBPB mitigation as a detectable footprint for detecting precise interrupts on AMD processors.

## Tested Setup

### Software dependencies
In order to run the experiments and proof-of-concepts, the following prerequisites need to be fulfilled:

- Linux installation
  - Build tools (gcc, make)
  - Python 3
  - Linux versions above 6.1 for AMD
- Browsers (for website fingerprinting)
  - [Chrome Browser](https://example.com).
  - You also need to install the drivers for the browsers you would like to use.
  - Chrome: Download [here](https://example.com) and add `chromedriver` to your path
  
### Hardware dependencies
Throughout our experiments, we successfully evaluated our implementations on the following environments. We recommend to test SegScope on bare-metal machines.


| Machine         | CPU                  | Kernel        |
|-----------------|----------------------|---------------|
| Honor Magicbook 16 Pro | AMD Ryzen 7 5800H    | Linux 6.14.0    |



## Materials
This repository contains the following materials:

- **Detection of BTB invalidation**: contains the code that we apply BarrierScope to detect BTB invalidation.
- **Keystroke**: contains the code that we use BarrierScope to detect keystroke.
- **Website Fingerprinting**: contains the code that we rely on BarrierScope to fingerprint websites.

## Contact

If there are questions regarding these experiments, please send an email to `yfgao25@stu.pku.edu.cn`.
