#include "sosemanuk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

double duration(struct timeval start, struct timeval end)
{
    return (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0; // in seconds
}

static const unsigned char KEY[] = "0A5DB00356A9FC4FA2F5489BEE4194E7";
static const unsigned char IV[] = "1F86ED54BB2289F057BE258CF35AC128";

#define TEST_SIZES_COUNT 5
const long TEST_SIZES[TEST_SIZES_COUNT] = { 100, 1000, 10000, 100000, 1000000 };

#define ITERATIONS 1000

void test_case(long buffer_size, int iterations)
{
    ECRYPT_ctx ctx;

    struct timeval start, end;

    gettimeofday(&start, NULL);
    for (int i = 0; i < iterations; i++) {
        ECRYPT_init();
        ECRYPT_keysetup(&ctx, KEY, 128, 128);
        ECRYPT_ivsetup(&ctx, IV);
    }
    gettimeofday(&end, NULL);

    double init_duration = duration(start, end) / iterations;

    unsigned char* keystream = (unsigned char*)malloc(buffer_size);
    if (!keystream) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    gettimeofday(&start, NULL);
    for (int i = 0; i < iterations; i++) {
        ECRYPT_keystream_bytes(&ctx, keystream, buffer_size);
    }
    gettimeofday(&end, NULL);

    free(keystream);

    double generation_duration = duration(start, end) / iterations;

    printf("Buffer size: %ld bytes\n", buffer_size);
    printf("Time for init: %.6f sec.\n", init_duration);
    printf("Time for gen: %.6f sec.\n", generation_duration);
}

int main()
{

    for (int i = 0; i < TEST_SIZES_COUNT; i++) {
        test_case(TEST_SIZES[i], ITERATIONS);
        if (i < TEST_SIZES_COUNT - 1) {
            printf("\n");
        }
    }

    return 0;
}
