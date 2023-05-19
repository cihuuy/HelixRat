// Ultra TOP SECRET crypto algorithm.
// Yes. I rolled my own crypto.
// With 96 rounds this might end up being decent (not broken right away).
// Please not there is no basis for why this is the way it is. I just made it up.
// It passed Dieharder with flying colors, so it's probably not complete crap?
// I'm not sure if it's secure or not. I'm not a cryptographer. I'm just a guy who likes to play with numbers.
// I'm sure someone will tell me how bad this is.
// This could be faster. I'm not sure how to make it faster other then lowering the number of rounds or invented a ASIC for it.

#include <string>
#include <iostream>
#include <sstream>
#include <string.h>
#include <iomanip>

#define ROTLEFT(a, b) (((a) << (b)) | ((a) >> (32 - (b))))
#define ROTRIGHT(a, b) (((a) >> (b)) | ((a) << (32 - (b))))

// Pseudo-random permutation functions
// These took me awhile to invent
#define pbox1(a, b, c)                                                             \
    ({                                                                             \
        uint32_t tmp = (a ^ b) ^ ~c;                                               \
        uint32_t s0 = ROTRIGHT(a, 2) ^ ROTLEFT(a, 12) ^ ROTRIGHT(a, c % 32);       \
        uint32_t s1 = ROTLEFT(tmp, 6) ^ ROTRIGHT(tmp, 9) ^ ROTLEFT(tmp, tmp % 32); \
        s0 *tmp + s1;                                                              \
    })

// Pseudo-random permutation functions
// These took me awhile to invent
#define pbox2(a, b, c)                                                                    \
    ({                                                                                    \
        uint32_t tmp = (a & b) ^ (~a & c);                                                \
        uint32_t s0 = ROTLEFT(a, (a + b + c) % 32) ^ ROTRIGHT(a, 11) ^ ROTLEFT(a, 7);     \
        uint32_t s1 = ROTRIGHT(tmp, s0 % 32) ^ ROTLEFT(tmp, 2) ^ ROTRIGHT(tmp, tmp % 32); \
        s0 + tmp *s1;                                                                     \
    })

    std::string helixrat_hash(uint8_t *data, uint64_t length)
    {
        uint32_t digest[8] = {0x7b7ffcd1, 0x0d5c75a4, 0x57892cb9, 0xdf7dda71, 0x449355d1, 0xeff23264, 0xf6b54056, 0xf921ec5b};

        for (uint32_t r = 0; r < 96; r++)
        {
            for (uint64_t i = 0; i < length; i++)
            {
                // This is the core of the algorithm.
                uint32_t a = digest[0];
                uint32_t b = digest[1];
                uint32_t c = digest[2];
                uint32_t d = digest[3];
                uint32_t e = digest[4];
                uint32_t f = digest[5];
                uint32_t g = digest[6];
                uint32_t h = digest[7];

                uint32_t x = data[i];
                uint32_t y = data[(i + 1) % length];

                uint32_t t = pbox2(a, b, c) ^ pbox2(d, e, f) ^ pbox2(g, h, x) ^ pbox2(y, a, b);
                digest[0] = pbox1(a, b, c) ^ t;
                digest[1] = pbox1(d, e, f) ^ t;
                digest[2] = pbox1(g, h, x) ^ t;
                digest[3] = pbox1(y, a, b) ^ t;
                digest[4] = pbox1(a, b, c) ^ t;
                digest[5] = pbox1(d, e, f) ^ t;
                digest[6] = pbox1(g, h, x) ^ t;
                digest[7] = pbox1(y, a, b) ^ t;
            }
            // remix the digest/state
            uint32_t a = digest[0];
            uint32_t b = digest[1];
            uint32_t c = digest[2];
            uint32_t d = digest[3];
            uint32_t e = digest[4];
            uint32_t f = digest[5];
            uint32_t g = digest[6];
            uint32_t h = digest[7];

            // Got to love Permutation Boxes
            uint32_t t = pbox2(a, b, c) ^ pbox2(d, e, f) ^ pbox2(g, h, a) ^ pbox2(b, c, d);
            uint32_t u = pbox2(e, f, g) ^ pbox2(h, a, b) ^ pbox2(c, d, e) ^ pbox2(f, g, h);
            uint32_t v = pbox2(f, g, a) * pbox2(h, a, t) * pbox2(c, d, u) * pbox2(g, h, length);
            for (uint8_t i = 0; i < 8; i++)
            {
                digest[i] ^= pbox1(digest[i], digest[(i + 1) % 8], digest[(i + 2) % 8]) ^ v;
                digest[u % 8] ^= pbox1(digest[u % 8], digest[(u + 1) % 8], digest[i]) ^ v;
                digest[t % 8] ^= pbox1(digest[t % 8], digest[(t + 1) % 8], digest[i]) ^ v;
            }
        }

        return std::string((char *)digest, 32);
    }

int main(int argc, char **argv)
{

    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <string>" << std::endl;
        return 1;
    }

    std::cout << helixrat_hash((uint8_t *)argv[1], strlen(argv[1]));


    return 0;
}