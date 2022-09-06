import base64
import binascii

# "3040ZHGW"
class my_rc4(object):
    def __init__(self, key):
        self.key = bytearray(key, 'utf-8')
        self.sbox = bytearray(32)
        self.kbox = bytearray(32)
    
    def init_kbox(self):
        for i in range(32):
            self.kbox[i] = self.key[i % len(self.key)]
        # print(self.kbox)

    def init_sbox(self):
        j = 0
        self.sbox = bytearray(list(range(32)))

        for i in range(32):
            j = (j + self.sbox[i] + self.kbox[i]) % 32
            self.sbox[i], self.sbox[j]  = self.sbox[j], self.sbox[i]
        # print(self.sbox)

    def decrypt(self, en_bytes, de_bytes):
        self.init_kbox()
        self.init_sbox()
        j = 0
        k = 0
        for i in range(len(en_bytes)):
            j = (j + 1) % 32
            k = (k + self.sbox[j]) % 32
            self.sbox[j], self.sbox[k]  = self.sbox[k], self.sbox[j]
            t = (self.sbox[j] + self.sbox[k]) % 32
            de_bytes[i] = en_bytes[i] ^ self.sbox[t]

    def encrypt(self, de_bytes, en_bytes):
        self.init_kbox()
        self.init_sbox()
        j = 0
        k = 0
        for i in range(len(de_bytes)):
            j = (j + 1) % 32
            k = (k + self.sbox[j]) % 32
            self.sbox[j], self.sbox[k]  = self.sbox[k], self.sbox[j]

            t = (self.sbox[j] + self.sbox[k]) % 32
            en_bytes[i] = de_bytes[i] ^ self.sbox[t]


# De-crypt test
print("RC4 decrypt test...........")
en_str = "03001156A80A0519045ED111106D0B345B0A000B41B8011F111F53DA15166B033E64"
en_bytes = binascii.a2b_hex(en_str)
de_bytes = bytearray(len(en_bytes))

rc4 = my_rc4("3040ZHGW")
rc4.decrypt(en_bytes, de_bytes)
de_str = binascii.b2a_hex(de_bytes)
print(de_bytes)
print(de_str)

# En-crypt test
print("RC4 encrypt test...........")
de_str = "02000041a00000010041c80000630f204002000041a00000010041c80000630f207c"
de_bytes = binascii.a2b_hex(de_str)
en_bytes = bytearray(len(de_bytes))
rc4.decrypt(de_bytes, en_bytes)
en_str = binascii.b2a_hex(en_bytes)
print(en_bytes)
print(en_str)

