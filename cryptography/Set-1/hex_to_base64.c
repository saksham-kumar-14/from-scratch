#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#define INPUT_LEN 1000

static const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int hex_to_int(char c){
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

void hex_to_base64(char input[], int len, char output[]){
    unsigned char* bytes = malloc(len / 2);
    int bytes_len = 0;

    for(int i = 0; i < len; i += 2){
        bytes[bytes_len++] = (hex_to_int(input[i]) << 4) | (hex_to_int(input[i + 1]));
    }

    int j = 0;
    for (int i = 0; i < bytes_len; i += 3){
        unsigned int val = (bytes[i] << 16);
        if (i + 1 < bytes_len) val |= (bytes[i+1] << 8);
        if (i + 2 < bytes_len) val |= bytes[i+2];

        output[j++] = b64_table[(val >> 18) & 0x3F];
        output[j++] = b64_table[(val >> 12) & 0x3F];
        output[j++] = (i + 1 < bytes_len) ? b64_table[(val >> 6) & 0x3F] : '=';
        output[j++] = (i + 2 < bytes_len) ? b64_table[val & 0x3F] : '=';
    }

    output[j] = '\n';
    free(bytes);
}

int main(){
    char input[INPUT_LEN];
    int len = 0;
    for(int i = 0; i < INPUT_LEN; ++i){
        scanf("%c", &input[i]);
        if(input[i] == '\n'){
            len = i;
            break;
        }
    }
    char output[INPUT_LEN];

    hex_to_base64(input, len, output);
    printf("%s\n", output);

    return 0;
}
