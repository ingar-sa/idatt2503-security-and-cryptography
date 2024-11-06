import string

def mult_tab_12():
    print("   ", end="")
    for i in range(1, 12):
        print(f"{i:2d}", end=" ")
    print()

    for i in range(1, 12):
        print(f"{i:2d}", end=" ")
        for j in range(1, 12):
            print(f"{(i * j) % 12:2d}", end=" ")
        print()

def mult_tab_11():
    print("   ", end="")
    for i in range(1, 11):
        print(f"{i:2d}", end=" ")
    print()

    for i in range(1, 11):
        print(f"{i:2d}", end=" ")
        for j in range(1, 11):
            print(f"{(i * j) % 11:2d}", end=" ")
        print()

def find_inv_11_mod_29():
    i = 1
    while True:
        res = (i * 11) % 29 
        print(f"{i} * 11 % 29 == {res}")
        if res == 1:
            print(f"{i} is a multiplicative inverse!")
            break
        i+=1

def dk(y, a_inv, b):
        return string.ascii_lowercase[a_inv*(y - b) % 26]

def ek(x, a, b):
        return string.ascii_uppercase[(a*x + b) % 26]


def affine_cipher_permutation():
    for i in range(26):
        lower = string.ascii_lowercase[i]
        upper = ek(i, 3, 11)
        print(f"ek({lower}) = {upper}")

def affine_cipher_permutation_inv():
    inv = 0
    for i in range(26):
        a_inv = 3*i % 26
        if a_inv == 1:
            inv = i
            print(f"{i} is the multiplicative inverse")
            break

    for i in range(26):
        upper = string.ascii_uppercase[i]
        lower = dk(i, inv, 11)
        print(f"ek({upper}) = {lower}")



def affine_coa_brute_force():
    cipher_text = "RGMRQ ERQMZ MZXMD ENNZU QFD"
    cipher_text_pos = []

    for L in cipher_text:
        for i in range(26):
            if L == string.ascii_uppercase[i]:
                cipher_text_pos.append(i)
                break
            elif L == ' ':
                cipher_text_pos.append(-1)
                break

    for a_inv in range(1, 26):
        for b in range(1, 26):
            plain_text = ""
            for y in cipher_text_pos:
                if y == -1:
                    plain_text += " "
                else:
                    plain_text += dk(y, a_inv, b)
            print(plain_text, f" ({a_inv}, {b})")

    i = 1
    while True:
        res = (i * 7) % 26 
        if res == 1:
            print(f"{i} is a in the encryption key!")
            break
        i+=1

cipher_text = "UCBCKOPTKMI QEDVSMV Q S QCBDEKBDQBDAIBIWKODIKVQOYDMIDMWCFIBDSVQBADHWIXSIBEYDKBKOYVQVD TQVDCBOYDRCWFVDRIOODQHD TIDPOKQB IG DQVDOCBADIBCSATDKBJDQVDBC D CCDJQHHIWIB DHWCUDBCWUKOD IG DCBIDVTCSOJDKOVCDFBCRDRTK DOKBASKAID TIDPOKQB IG DQVDRWQ  IBDQBDKVD TIDHWIXSIBEQIVDCHDOI  IWVDLKWYDHWCUDOKBASKAID CDOKBASKAIDVCUID IG VDKWIDTKWJIWD CDMWIKFD TKBDC TIWVDKBDIGKUPOIDQVD TIDVIB IBEID TIDXSQEFDMWCRBDHCGDNSUPVDCLIWD TIDOKZYDJCAD TQVDVIB IBEIDECB KQBVDILIWYDOI  IWDCHD TIDKOPTKMI DBC DTIOPQBAD TIDEWYP KBKOYV DACCJDOSEF"

''' 
Original mapping from https://www.dcode.fr/frequency-analysis
I solved it by switching out letters manually

    'A': 'U',
    'B': 'O',
    'C': 'I',
    'D': 'E',
    'E': 'C',
    'F': 'W',
    'G': 'V',
    'H': 'M',
    'I': 'T',
    'J': 'K',
    'K': 'A',
    'L': 'J',
    'M': 'P',
    'N': 'Q',
    'O': 'S',
    'P': 'F',
    'Q': 'R',
    'R': 'B',
    'S': 'D',
    'T': 'L',
    'U': 'G',
    'V': 'N',
    'W': 'H',
    'X': 'X',
    'Y': 'Y',
    'Z': 'Z',
    ' ': ' '
'''

alphabet_mapping = {
    'A': 'g',
    'B': 'n',
    'C': 'o',
    'D': ' ',
    'E': 'c',
    'F': 'k',
    'G': 'x',
    'H': 'f',
    'I': 'e',
    'J': 'd',
    'K': 'a',
    'L': 'v',
    'M': 'b',
    'N': 'j',
    'O': 'l',
    'P': 'p',
    'Q': 'i',
    'R': 'w',
    'S': 'u',
    'T': 'h',
    'U': 'm',
    'V': 's',
    'W': 'r',
    'X': 'q',
    'Y': 'y',
    'Z': 'z',
    ' ': 't'
}

def frequency_analysis():
    plain_text = ""
    for l in cipher_text:
        plain_text += alphabet_mapping[l]
    print(plain_text)

def main():
    print("Z12 multiplication table")
    mult_tab_12()
    print("\nZ11 multiplication table\n")
    mult_tab_11()
    print("\nMultiplicative inverse of 11 mod 29\n")
    find_inv_11_mod_29()
    print("\nAffine cipher permutations\n")
    affine_cipher_permutation()
    print("\nAffine cipher permutation inverse\n")
    affine_cipher_permutation_inv()
    #This one prints every permutation, so I've commented it out so it's easier to see the other things
    #print("\nAffine cipher brute force attack\n")
    #affine_coa_brute_force()
    print("\nFrequency analysis\n")
    frequency_analysis()

main()
