def repeated_xor(plain_text, key):
    ans = ""
    for i in range(len(plain_text)):
        xor_result = ord(plain_text[i]) ^ ord(key[i % len(key)])
        ans += format(xor_result, "02x")
    return ans


stanza = "Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal"

key = "ICE"
encrypted = repeated_xor(stanza, key)

print(f"Hex Result:\n{encrypted}")
