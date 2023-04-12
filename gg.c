#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint8_t toNibble(char c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }

    if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }

    if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }

    return 0xff;
}

void toCodeStep1(uint8_t * dst, uint8_t * src) {
    dst[0] = src[2];
    dst[1] = src[4];
    dst[2] = ((src[3] & 3) << 2) | ((src[0] >> 2) & 3);
    dst[3] = ((src[0] & 3) << 2) | ((src[5] >> 2) & 3);
    dst[4] = ((src[5] & 3) << 2) | ((src[1] >> 2) & 3);
    dst[5] = ((src[1] & 3) << 2) | ((src[3] >> 2) & 3);
}

const uint8_t toCodeMap[16] = {
    0xd, 0xf, 0x4, 0x7, 0x0, 0x9, 0x1, 0x5,
    0x6, 0xb, 0xc, 0x8, 0xa, 0x2, 0x3, 0xe,
};

void toCodeStep2(uint8_t * x) {
    for (int i = 0; i < 8; ++i) {
        if (x[i] <= 0xf) {
            x[i] = toCodeMap[x[i]];
        } else {
            x[i] = 0xff;
        }
    }
}

const uint8_t toAddrMap[16] = {
    0x4, 0x6, 0xd, 0xe, 0x2, 0x7, 0x8, 0x3,
    0xb, 0x5, 0xc, 0x9, 0xa, 0x0, 0xf, 0x1,
};

void toAddrStep1(uint8_t * x) {
    for (int i = 0; i < 8; ++i) {
        if (x[i] <= 0xf) {
            x[i] = toAddrMap[x[i]];
        } else {
            x[i] = 0xff;
        }
    }
}

void toAddrStep2(uint8_t * dst, uint8_t * src) {
    dst[0] = ((src[2] & 3) << 2) | ((src[3] >> 2) & 3);
    dst[1] = ((src[4] & 3) << 2) | ((src[5] >> 2) & 3);
    dst[2] = src[0];
    dst[3] = ((src[5] & 3) << 2) | ((src[2] >> 2) & 3);
    dst[4] = src[1];
    dst[5] = ((src[3] & 3) << 2) | ((src[4] >> 2) & 3);
}

int main(int argc, char ** argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <XXYY-YYYY>\n", argv[0]);
        fprintf(stderr, "       %s <YYYYYY:XX>\n", argv[0]);
        return 1;
    }

    if (strlen(argv[1]) != 9) {
        fprintf(stderr, "invalid code format: %s\n", argv[1]);
        fprintf(stderr, "must be XXYY-YYYY or YYYYYY:XX\n");
        return 1;
    }

    int toCode = argv[1][6] == ':';
    int toAddr = argv[1][4] == '-';

    if ((toCode ^ toAddr) == 0) {
        fprintf(stderr, "invalid code format: %s\n", argv[1]);
        fprintf(stderr, "must be XXYY-YYYY or YYYYYY:XX\n");
        return 1;
    }

    uint8_t work[8];
    uint8_t result[8];

    char separator = toCode ? ':' : '-';
    for (int i = 0, j = 0; i < 9; ++i) {
        if (argv[1][i] == separator) {
            continue;
        }

        work[j++] = toNibble(argv[1][i]);
    }

    if (toCode) {
        result[0] = work[6];
        result[1] = work[7];
        toCodeStep1(result + 2, work);
        toCodeStep2(result);
    } else {
        toAddrStep1(work);
        result[6] = work[0];
        result[7] = work[1];
        toAddrStep2(result, work + 2);
    }

    printf(toCode ? "%X%X%X%X-%X%X%X%X\n" : "%X%X%X%X%X%X:%X%X\n",
            result[0], result[1], result[2], result[3],
            result[4], result[5], result[6], result[7]);

    return 0;
}