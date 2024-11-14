## 1a)
Strømmechiffer: Fra nøkkel genereres nøkkestrøm. Forskjellige typer: Synkrone og asynkrone. Klatekst kombineres med nøkkelstrøm ved en enkel operasjon, typisk xor.
Sikkerhet er avhengig av at nøkkelstrøm er tilstrekkelig tilfeldig (pseudo-tilfeldig).
En renger vanligvis ikke å padde meldinger. 
En utfordring er å lage tilstrekkelig tilfeldige nøkkelstrømmer. 
For strømmechiffer er det lettere å modifisere deler av meldingen på en forutsigbar måte.
Blokkchiffer krypterer en blokk om gangen, derfor må meldinger vanligvis paddes opp til blokklengde. 
Et strømmechiffer er vanligvis mer effektiv å beregne enn blokkchiffer. Et blokkchiffer er mer anvendelig, og fungerer som en
pseudo-tilfeldig permutasjon av blokkene. Kan brukes til f.eks. også å lage
nøkkelstrømmer, og MAC'er.

## 1b)
$$z_5 = z_1 + z_2 = 0 + 1 = 1 \mod 2 \\
z_6 = z_2 + z_3 = 1 + 0 = 1$$

Nøkkelstrøm: 
0101 1110 0010 011 
0101 1110 0010 011\ 
(fortsetter likt som starten: 0 1011). Husk at starten av nøkkelstrømmen er $K$
Perioden er 15



## 1c)
$e_K(x)=x\oplus K = 101001 \oplus 010111 = 111110$

## 1d)
Første bits i melding: $x=x_1x_2x_3=011$, chiffertekst: $y=y_1y_2y_3=x_1x_2x_3\oplus z_1z_2z_3=110$, ukjent nøkkel: $z_1z_2z_3$

Vi finner nøkkel ved å ta xor av chiffertekst og klartekst: $y\oplus x=(x\oplus z)\oplus x = z\oplus(x\oplus x)=z$

Så $z_1z_2z_3=110\oplus 011 = 101$. Vi krypterer ønsket start med nøkkel: $101\oplus101=000$, og erstatter første tre bits med dette.



## 1e)
Vi kan bruke et blokkchiffer forskjellige "modes of operation", f.eks. CTR-mode. Da brukes en "nounce" sammen med teller som nøkkel til blokk-chifferet, og output fungerer da som nøkkelstrøm (synkron).

## 2a)
Fordi 14 ikke er relativt prmisk til 32

## 2b) 
$e(1)=5\cdot1+11=16$

$e(10) =  5\cdot10+11=61=29$


## 2c)
Equal plaintexts encrypt to equal ciphertext. One can identify same message, and one can easily use statistical methods, like frequency analysis of ciphertexts, and compare it to natural language frequencies.

## 2d) 
$\operatorname{Enc}_k(x)=ax+b \mod 32$. Substitute the two known pairs of plain and cipher texts, to get two equations in the two unknowns a,b:
$$\begin{align*}
a\cdot2+b &= 10 \\
a\cdot5+b &= 3 
\end{align*}$$
This system of equations can easily be solved, working modulo 32. For example, from first equation we have $b=10-a\cdot2$.
Subsitute into second equation:
$$\begin{align*}
a\cdot5 + (10 - a\cdot2) &= 3 \\
a(5-2) &= 3 - 10 \\
a\cdot 3 &= -7 \\
a &= -7\cdot 3^{-1}, \quad \text{use now that } 3^{-1}=11 \\
a &= -77 = -77 + 3\cdot 32 = 19 
\end{align*}$$

We substitute back in eqn for $b$:
$$b=10-2\cdot 19=10-38= -28 = 4$$

We now have the encryption key. The decryption key is found by inverting the function mod 32:
$$y = e(x) = 19x-28 \\
d(y) = x =19^{-1}(y+28)=27(y+28) = 27y+20 \\
d(23)= 27\cdot 23 + 20 = 13+20 = 33 = 1
$$  

## 2e)
Generelt er ikke affint chiffer sikkert.
Det er en type subsitusjonschiffer, men med relativt få nøkler, så det er sårbart for "brute force" angrep, og også  frekvensanalyse for kun kjent chiffertekst-angrep. I tillegg kan det, som vi ser fra forrige punkt, knekkes fullstendig ved kjent klartekst-angrep.

For å gjøre frekvensanalyse vanskeligere kan en bruke en passende "mode of operation" f.eks. CBC.
For å gjøre brute force vanskeligere, kan bruke store $n$ som også er primtall, slik at det er fler nøkler ($a$ må være relativt primisk til $n$).

## 3a) 
Hovedforskjell som må med: RSA er asymmetrisk, også kalt offentlig nøkkel kryptografi. AES er symmetrisk, felles hemmelig nøkkel. Det gir poeng for egenskaper knyttet til denne forskjellen (RSA løser nøkkelutvekslingsproblemet etc,). Andre ting som gir poeng:
- Hvilke algoritmer som brukes
- RSA er mye mer beregningskrevende, 
- typiske nøkkellengde
- 

## 3b) 
Primtallsfaktorisering er antatt beregningsmessig vanskelig, alternativt: Å beregne beregne Eulers totientfunksjon
$\phi(n)$ beregningsmessig vanskelig når $n=pq$ og en ikke kjenner faktorene.

## 3c) 
Vi trenger først å finne $\phi(323)$, og trenger faktoriseringen av $n$ i primtall: $323=17\cdot 19$.

Da blir $\phi(323)=(17-1)(19-1)=16\cdot 18 = 288$
Bruk Euklids utvidete algoritme til å finne multiplikative
invers til 5 modulo 288:
$$288 = 57\cdot 5 + 3 \\
5 = 1\cdot 3 + 2 \\
3 = 1\cdot 2 + 1 \\
\text{så i revers:} \\
1 = 3 - 1\cdot 2 \\
1 = 1\cdot3 - 1(1\cdot 5 - 1\cdot 3) = 2\cdot 3 - 1\cdot 5 \\
1 = 2(1\cdot288 - 57\cdot 5) - 1\cdot 5 = 2\cdot 288 - 115\cdot 5 $$
som betyr at 
$$-115\cdot 5 \equiv 1 \mod 288 \\
5^{-1}\equiv -115 \equiv 288-115 \equiv 173 \mod 323$$ 

## 3d)
All regning modulo 323
$$\begin{align*}
100^2 &= 10000 = 30\cdot323 + 310 = 310 \\
100^4 &= 310^2 = 96100 = 297\cdot 323 + 169 = 169 \\
100^5 &= 100^4 \cdot 100^1 = 169\cdot 100 = 16900 = 52\cdot 323+104
\end{align*} $$
Så  $\operatorname{Enk}_{5}(100) = 104$

## 3e)
Her godtas det angrep både for RSA brukt for kryptering/dekryptering, eller brukt for signering.
For kryptering:
- Som blokkchiffer, like blokker krypteres alltid likt. Bruk av passende operasjonsmodus endrer dette.
- En kan manipulere via multiplikativ egenskap: Krypteringen av et produkt og klartekster er lik produktet av chiffertekstene
Godtar også:
- Hvis differansen mellom $p$ og $q$ er liten, så kan en lett faktorisere $n$. Velg slik at avstand er tilstrekkelig stor.
- $p$ og $q$ har bare (relativt) små faktorer, så kan en faktorisere vha. Pollards (p-1). 
- 

## 3f)
Ja, det vil fungere som normalt, men sikkerheten vil være dårlig.

Grunnen til at det virker, er at vi fortsatt har Eulers teorem / Fermats lille teorem. Hvis $de=1 \mod (p-1)$, så har vi fortsatt at
$$d(e(x))=(x^e)^d = x^{ed} = x^{1+k(p-1)} = x \mod p$$

Sikkerheten vil være dårlig, fordi det er lett å regne ut Eulers totient for et primtall: $\phi(p) = p-1$. Dermed kan en lett
finne hemmelig nøkkel $d$ fra $e$ ved Euklids utvidete algoritme.

## 4a)
Inversbildet: Gitt tagg y, finn x slik at h(x) = y. 
Second pre-image: Gitt x og tagg y, finn x' slik at h(x') = y.



## 4b) 
At en hash-funsksjon er kollisjonsresistent, betyr at det å finne en kollisjon, ikke er vesentlig lettere enn å utføre bursdagsangrepet.
En kollisjon er å finne to forskjellige meldinger med lik hash-verdi/tagg. Bursdagsangrepet er et brute-force-angrep hvor en hasher verdier helt til man finner en kollisjon. I gjennomsnitt kreves det $\sqrt(N)$ hash-verdier før en finner en kollisjon, hvor $N$ er antall mulige hash-verdier. 

## 4c)
Pre-image er vanskelig for de fleste verdier pga. diskrete logaritmeproblemet. Pre-image (inversbilde) her er det samme som å finne logaritmen. 

Second pre-image og kollisjoner er trivielt lette å finne. Hvis $F(x) = y$, så vil 
$F(x+k(p-1))=F(x)$ også, for alle heltall $k$, ved Fermats lille teorem (all regning mod $p$):
$$F(x+k(p-1)) = \alpha^{x+k(p-1)} = \alpha^x \alpha^{(p-1)k} = \alpha^x (\alpha^{p-1})^k = \alpha^x 1^k = \alpha^x = F(x)$$
