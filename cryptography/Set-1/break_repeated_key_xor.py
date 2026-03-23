import base64


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


def hamming_distance(b1, b2):
    return sum(bin(byte1 ^ byte2).count("1") for byte1, byte2 in zip(b1, b2))


def break_repeating_key_xor(ciphertext):
    normalized_distances = []

    for keysize in range(2, 41):
        chunks = [ciphertext[i : i + keysize] for i in range(0, keysize * 4, keysize)]
        if len(chunks) < 4:
            break

        dist = 0
        pairs = 0
        for i in range(len(chunks) - 1):
            dist += hamming_distance(chunks[i], chunks[i + 1])
            pairs += 1

        normalized = (dist / pairs) / keysize
        normalized_distances.append((normalized, keysize))

    best_keysize = sorted(normalized_distances)[0][1]

    transposed_blocks = [bytearray() for _ in range(best_keysize)]
    for i, byte in enumerate(ciphertext):
        transposed_blocks[i % best_keysize].append(byte)

    recovered_key = bytearray()

    for block in transposed_blocks:
        best_score = -float("inf")
        best_byte = 0

        for candidate_byte in range(256):
            try:
                decrypted_bytes = bytes([b ^ candidate_byte for b in block])
                decrypted_str = decrypted_bytes.decode("ascii")
                current_score = english_score(decrypted_str)

                if current_score > best_score:
                    best_score = current_score
                    best_byte = candidate_byte
            except UnicodeDecodeError:
                continue

        recovered_key.append(best_byte)

    return bytes(recovered_key)


def repeated_key_xor(plaintext, key):
    output = bytearray()
    for i, byte in enumerate(plaintext):
        output.append(byte ^ key[i % len(key)])
    return bytes(output)


try:
    with open("break_repeated_input.txt", "r") as f:
        data = base64.b64decode(f.read())

    key = break_repeating_key_xor(data)
    print(f"Key: {key.decode()}")

    decrypted_message = repeated_key_xor(data, key)
    print(decrypted_message.decode())

except FileNotFoundError:
    print("File not found.")
