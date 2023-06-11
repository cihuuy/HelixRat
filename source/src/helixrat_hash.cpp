#include <common/helixrat_hash.h>
#include <sstream>

#include <iomanip>
// Ultra TOP SECRET crypto algorithm.
// Yes. I rolled my own crypto.
// If this ends up being a ("thing") then I retain full rights to this algorithm.
// With 96 rounds this might end up being decent (not broken right away).
// Please not there is no basis for why this is the way it is. I just made it up.
// It passed Dieharder with flying colors, so it's probably not complete crap?
// I'm not sure if it's secure or not. I'm not a cryptographer. I'm just a guy who likes to play with numbers.
// I'm sure someone will tell me how bad this is.
// This could be faster. I'm not sure how to make it faster other then lowering the number of rounds or invented a ASIC for it.

namespace helixrat::hash
{
#define ROTRIGHT(a, b) (((a) >> (b)) | ((a) << (64 - (b))))
#define ROTLEFT(a, b) (((a) << (b)) | ((a) >> (64 - (b))))
#define NUM_ROUNDS 8
    static inline void permute_box1(uint64_t *a, uint64_t *b, uint64_t *c, uint64_t *d)
    {
        *d ^= (((*a) << 32) ^ ((*d) >> 32)) + (*a) + 1;
        *c ^= (((*b) << 48) ^ ((*c) >> 16)) + (*b) + 1;
        *b ^= (((*c) << 32) ^ ((*b) >> 32)) + (*c) + 1;
        *a ^= (((*d) << 16) ^ ((*a) >> 48)) + (*d) + 1;

        // Additional mixing
        *a += *b & ~*c;
        *b += *c & ~*d;
        *c += *d & ~*a;
        *d += *a & ~*b;
    }

    static inline void permute_box2(uint64_t *a, uint64_t *b, uint64_t *c, uint64_t *d)
    {
        *a ^= ((*a ^ *b) ^ ~*c) << (*d % 7);
        *b ^= ((*b ^ *c) ^ ~*d) << (*a % 7);
        *c ^= ((*c ^ *d) ^ ~*a) << (*b % 7);
        *d ^= ((*d ^ *a) ^ ~*b) << (*c % 7);

        // Additional mixing
        *a = ROTLEFT(*a, 17) ^ ROTRIGHT(*c, 7);
        *b = ROTLEFT(*b, 29) ^ ROTRIGHT(*d, 3);
        *c = ROTLEFT(*c, 47) ^ ROTRIGHT(*b, 13);
        *d = ROTLEFT(*d, 59) ^ ROTRIGHT(*a, 23);
    }

    static inline void permute_box3(uint64_t *a, uint64_t *b, uint64_t *c, uint64_t *d)
    {
        *a ^= ROTRIGHT(((*a ^ *b) ^ ~*c), (*d % 64)) >> (*a % 16);
        *b ^= ROTRIGHT(((*b ^ *c) ^ ~*d), (*c % 64)) >> (*b % 16);
        *c ^= ROTRIGHT(((*c ^ *d) ^ ~*a), (*b % 64)) >> (*c % 16);
        *d ^= ROTRIGHT(((*d ^ *a) ^ ~*b), (*a % 64)) >> (*d % 16);

        // Additional mixing
        *a = (*a ^ *b) + (*c ^ *d);
        *b = (*b ^ *c) + (*d ^ *a);
        *c = (*c ^ *d) + (*a ^ *b);
        *d = (*d ^ *a) + (*b ^ *c);
    }

    std::string helixrat_hash(uint8_t * buffer, uint64_t length)
    {
        uint64_t state_1 = NUM_ROUNDS;
        uint64_t state_2 = 0;
        uint64_t state_3 = 0;
        uint64_t state_4 = 0;

        // Iterate over the buffer in 32 byte chunks
        for (uint32_t i = 0; i < length; i += 32)
        {
            // Load the next 32 bytes into the state
            state_1 = *((uint64_t *)(buffer + i));
            state_2 = *((uint64_t *)(buffer + i + 8));
            state_3 = *((uint64_t *)(buffer + i + 16));
            state_4 = *((uint64_t *)(buffer + i + 24));

            // Perform the rounds
            for (uint16_t j = 0; j < NUM_ROUNDS; j++)
            {
                permute_box1(&state_1, &state_2, &state_3, &state_4);
                permute_box2(&state_1, &state_2, &state_3, &state_4);
                permute_box3(&state_1, &state_2, &state_3, &state_4);
            }
        }
        uint8_t hash[32];
        *((uint64_t *)(hash + 0)) = state_1;
        *((uint64_t *)(hash + 8)) = state_2;
        *((uint64_t *)(hash + 16)) = state_3;
        *((uint64_t *)(hash + 24)) = state_4;
        return std::string((char *)hash, 32);
    }
    std::string helixrat_hashx(uint8_t *data, uint64_t length)
    {
        std::string hash = helixrat_hash(data, length);
        std::stringstream ss;
        ss << std::hex << std::setfill('0'); // Set the stream to output hexadecimal values with leading zeros

        for (int i = 0; i < 32; i++)
        {
            ss << std::setw(2) << (unsigned int)(uint8_t)hash[i];
        }
        return ss.str();
    }
    uint64_t helixrat_check(uint8_t *data, uint64_t length)
    {
        return *(uint64_t *)helixrat_hash(data, length).data();
    }
}