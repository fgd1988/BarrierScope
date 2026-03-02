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
- Browsers (for website fingerprinting)
  - [Chrome Browser](https://example.com).
  - You also need to install the drivers for the browsers you would like to use.
  - Chrome: Download [here](https://example.com) and add `chromedriver` to your path
  
### Hardware dependencies
Throughout our experiments, we successfully evaluated our implementations on the following environments. We recommend to test SegScope on bare-metal machines.

| Machine                     | CPU                     | Kernel         |
| Honor Magicbook 16 Pro      | AMD Ryzen 7 5800H        | Linux 5.15.0   |


*Note: The enhanced Spectral attack relies on the UMONITOR/UMWAIT instructions that are only available on Intel latest core processors*

## Materials
This repository contains the following materials:

- **E1-Website Fingerprinting**: contains the code that we apply SegScope to detect interrupts while opening a website.
- **E2-Enhancing Spectral attack**: contains the code that we use SegScope to enhance a non-interrupt side channel attack (i.e., [spectral](https://example.com)).
- **E3-Breaking KASLR**: contains the code that we rely on SegScope-based timer to derandomize KASLR.

## Contact

If there are questions regarding these experiments, please send an email to `yfgao25@stu.pku.edu.cn`.
