import binascii


def english_score(decoded):
    most_common = "etaoinshrdlu"
    score = 0
    for char in decoded.lower():
        if char in most_common:
            score += 1
        elif char.isalpha():
            score += 0.5
        elif char not in " \n\r\t!,.?'\"":
            score -= 10
    return score


def decrypt(s):
    txt = binascii.unhexlify(s)
    best_score = -float("inf")
    best_key = None
    decrypted_msg = ""

    for i in range(256):
        possibility = bytes([j ^ i for j in txt])

        try:
            decoded = possibility.decode("ascii")
            score = english_score(decoded)

            if score > best_score:
                best_score = score
                best_key = i
                decrypted_msg = decoded
        except Exception:
            continue

    return best_key, decrypted_msg


with open("detect_single_input.txt", "r") as file:
    msgs = file.read().splitlines()

with open("detect_single_output.txt", "w") as file:
    pass
for i in msgs:
    key, msg = decrypt(i)
    with open("detect_single_output.txt", "a") as file:
        file.write("Message is : " + msg + "\n")
