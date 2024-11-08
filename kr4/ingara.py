import math

def question_delimeter(q):
    print(f"\n---------- {q} ----------")

def is_prime(n):
    if n < 2:
        return False
    for i in range(2, int(n ** 0.5) + 1):
        if n % i == 0:
            return False
    return True

def get_factors(n):
    factors = []
    for i in range(1, int(n ** 0.5) + 1):
        if n % i == 0:
            # If i is a factor, both i and n/i are factors
            factors.append((i, n // i))
            
    return factors

def print_factors(n):
    factors = get_factors(n)
    print(f"\nFactors of {n}:")
    
    all_factors = sorted(list(set(
        [factor for pair in factors for factor in pair]
    )))
    
    factor_strings = []
    for factor in all_factors:
        prime_status = " (prime)" if is_prime(factor) else ""
        factor_strings.append(f"{factor}{prime_status}")
    print(", ".join(factor_strings))

def extended_gcd(a, b):
    old_r, r = a, b
    old_s, s = 1, 0
    old_t, t = 0, 1

    while r != 0:
        quotient = old_r // r
        old_r, r = r, old_r - quotient * r
        old_s, s = s, old_s - quotient * s
        old_t, t = t, old_t - quotient * t
    
    # old_s and old_t are the Bezout coefficients
    # old_r is the gcd
    # t and s are the quotients by the gcd
    return old_s, old_t, old_r, t, s


def q1a():
    question_delimeter("1a")

    p = 1283
    a = 1307
    b = 1879
    c = 2003
    d = 2027
    print_factors(p-1)
    print_factors(a-1)
    print_factors(b-1)
    print_factors(c-1)
    print_factors(d-1)

"""
P1 a)
One reason for choosing q = d = 2027 from the group is that it is the one furthest away from p. 

Another is that neither p-1 or d-1 have small prime factors, with the smallest prime factor 
for p-1 being 641 and the smallest for d-1 being 1013, as we can see by running q1a. Compare 
this to b and c, which both have small prime factors. While a doesn't have small prime factors,
it is the closest to p, making it unideal for that reason.

For the weak numbers, either Fermat's factorization method or Pollard can be used.
"""


def q1b():
    question_delimeter("1b")

    p = 1283
    q = 2027
    n = (p-1)*(q-1)
    d = 3
    b1, b2, _, _, _ = extended_gcd(d, n)
    b = b1 if b1 != 1 else b2 # Use the Bezout coefficient that is not 1 to find e
    b %= n
    print(f"e = {b}")


def q1c():
    question_delimeter("1c")

    p = 1283
    q = 2027
    e = 1731555 # Found from q1b
    M = 111
    pq = p * q

    exp = M
    C = 1
    while e:
        # If the bit is set, that means that this digit is part of
        # the powers of 2 in the repeated squaring and should be used
        # to encrypt the message
        if e & 1:
            C = (C * exp) % pq
        exp = (exp**2) % pq
        e >>= 1

    print("Encrypted:", C)


"""
P2 a)
ek(x1) = x1^e, ek(x2) = x2^e => ek(x1) * ek(x2) = x1^e * x2^e = (x1 * x2)^e = ek(x1*x2)
QED.

P2 b)
x' = y^d * r^(ed) = y^d * r => x'r^-1 = y^d * r * r^-1 mod n => y^d = x' * r^-1
QED.
"""


def q3():
    question_delimeter("3")

    # Since p-q is relatively small, Fermats factorization method will work well
    n = 275621053
    p = 0
    q = 0
    a = math.ceil(n**0.5)
    while True:
        b = (a**2 - n)**0.5
        if int(b) == b: 
            p = a-b
            q = a+b
            break
        elif a > b:
            a += 1
        else:
            p = 1
            q = n
            break

    print(f"p = {int(p)}, q = {int(q)}")
    

def q4a():
    question_delimeter("4a")

    p = 101
    print("3^i\n")
    for i in range(1, 101):
        if i % 20 == 0:
            print("\n")
        print(3**i % p, end=" ")

    print("\n5^i\n")
    for i in range(1, 101):
        if i % 20 == 0:
            print("\n")
        print(5**i % p, end=" ")

"""
P4 a)
The difference between the sequences is the periodicity of the values.
"""


def q4b():
    question_delimeter("4b")

    p = 101
    n = 3
    a = 33
    b = 65
    k = n**(a*b) % p
    print("Common key:", k)


def shank(p, al, be):
    m = int(math.ceil(math.sqrt(p)))
    jal_pairs = {}
    for j in range(m):
        k = pow(al, m*j, p)
        jal_pairs[k] = j
        # Since we're interested in comparing the values
        # we can use the value as a key in the map to easily
        # retrieve j and i by matching the keys in the two
        # maps

    ibeal_pairs = {}
    for i in range(m):
        k = (be * pow(al, -i, p)) % p
        ibeal_pairs[k] = i
    
    common = jal_pairs.keys() & ibeal_pairs.keys()
    k = common.pop()
    j = jal_pairs[k]
    i = ibeal_pairs[k]
    return (m*j + i) % p


def q5():
    question_delimeter("5")

    al = 5
    be = 144363234
    p = 1000000007
    x = shank(p, al, be)
    print(f"x = {x}")
 

if __name__ == "__main__":
    q1a()
    q1b()
    q1c()
    q3()
    q4a()
    q4b()
    q5()
