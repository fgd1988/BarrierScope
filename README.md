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
  - [Chrome Browser](https://example.com) or [Tor Browser](https://example.com).
  - You also need to install the drivers for the browsers you would like to use.
  - Chrome: Download [here](https://example.com) and add `chromedriver` to your path
  - [Tor Browser]: Install [tor-browser-selenium](https://example.com)
### Hardware dependencies
Throughout our experiments, we successfully evaluated our implementations on the following environments. We recommend to test SegScope on bare-metal machines.

| Machine                     | CPU                     | Kernel         |
|-----------------------------|-------------------------|----------------|
| Xiaomi Air 13.3             | Intel Core i5-8250U     | Linux 5.15.0   |
| Lenovo Yangtian 4900v       | Intel Core i7-4790      | Linux 5.8.0    |
| Lenovo Savior Y9000P        | Intel Core i9-12900H    | Linux 5.15.0   |
| Honor Magicbook 16 Pro      | AMD Ryzen 7 5800H        | Linux 5.15.0   |
| Amazon t2.large (Xen)       | Intel Xeon E5-2686      | Linux 5.15.0   |
| Amazon c5.large (KVM)       | Intel Xeon 8275CL       | Linux 5.15.0   |

*Note: The enhanced Spectral attack relies on the UMONITOR/UMWAIT instructions that are only available on Intel latest core processors*

## Materials

## Contact

If there are questions regarding these experiments, please send an email to `yfgao25@stu.pku.edu.cn`.


## How should I cite this work?
