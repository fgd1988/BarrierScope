## Detection of BTB invalidation

Uses BarrierScope to detect BTB invalidation.

### Install dependencies.
```bash
pip install -r requirements.txt


## How to Run
Take it one step at a time:
```
$ make
$ ./spectrev2
...output...
```
If you see the secret phrase, that means the demo worked! If not, the code might
not be set up correctly for your system. Feel free to leave a GitHub issue, or
submit a pull request if you were able to fix it.

## Acknowledgements
This code was written while I was working on the project
[Ward](https://github.com/mit-pdos/ward).

The Makefile was taken from Eugnis'
[repository](https://github.com/Eugnis/spectre-attack) for a PoC of the Spectre
v1 attack.
