
def gf_mul(a, b) -> int:
    overflow_replace = 27  # = 00011011
    res = 0
    for i in range(8):
        if a & (1 << i) != 0:
            res ^= b
        b <<= 1
        if b > 255:
            mask = b & 255
            b = overflow_replace ^ mask
            
    return res

def gf_add(a, b):
    res = a ^ b
    return res

def gf_mul_table(i, j):
    table = []
    for i in range(256):
        row = []
        for j in range(256):
            product = gf_mul(i, j)
            row.append(product)
        table.append(row)
    
    return table

def write_mul_table(filename):
    try:
        with open(filename, 'w') as f:
            for row in gf_mul_table(256, 256):
                row_out = []
                for n in row:
                    row_out.append(str(n))
                
                # Join the row with spaces and add newline
                f.write(' '.join(row_out) + '\n')
    except IOError as e:
        raise IOError(f"Error writing to file {filename}: {str(e)}")


def gf_mul_table_inv():
    invs = []
    table = gf_mul_table(256, 256)
    for row in table[1:]:
        for i in range(256):
           mul = row[i]
           if mul == 1:
                invs.append(i)

    return invs


def verify_properties():
    properties_hold = True
    for a in range(1, 256):
        for b in range(1, 256):
            if gf_mul(a, b) != gf_mul(b, a):
                print(f"{a} * {b} is not commutative")
                properties_hold = False
                break
            for c in range(1, 256):
                if gf_mul(gf_mul(a, b), c) != gf_mul(a, gf_mul(b, c)):
                    print(f"{a}, {b}, and {c} are not associative")
                    properties_hold = False
                    break
                if gf_mul(a, gf_add(b, c)) != gf_add(gf_mul(a, b), gf_mul(a, c)):
                    print(f"{a}, {b}, and {c} are not distributive")
                    properties_hold = False
                    break

    if properties_hold:
        print("The commutative, associative, and distributive properties hold")
    else:
        print("The commutative, associative, and distributive properties do not hold")


def key_stream():
    k = 0b01001010
    n = 0b01100000
    invs = gf_mul_table_inv()
    
    for i in range(4):
        x = n | (i % 16)
        inv = invs[x - 1]
        nk = inv ^ k
        print(f"{nk}")
        print(f"{nk:08b}")

def h(x):
    x2 = (x**2) % 256
    m = (x2 >> 2) & 15
    return m

def hmac(x):
    K = 0b1001
    ipad = 0b0011
    opad = 0b0101

    H = h(((K ^ opad) << 4) | h(((K ^ ipad) << 4) | x))
    print(f"{H:04b}")


def main():
    #P3 a)
    write_mul_table("./my_mul.txt")

    #P3 b)
    print(gf_mul_table_inv())
    
    #P3 c)
    #Takes some time, maybe up to 1 minute
    verify_properties()
    
    #P4 a)
    key_stream()
    
    #P5 a)
    hmac(0b0110)

    #P5 b)
    hmac(0b0111)

main()
