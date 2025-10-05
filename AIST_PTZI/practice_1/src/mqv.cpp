#include "mqv.h"

#include "dh.h"

void generate_mqv_keypair(MQVKeyPair& keypair, const DHParams& params)
{
    generate_private_key(keypair.private_key, params.q);
    generate_public_key(keypair.public_key, params.g, keypair.private_key,
        params.p);
}

void compute_mqv_shared_secret(mpz_t shared_secret,
    const MQVKeyPair& static_keypair,
    const mpz_t ephemeral_private,
    const mpz_t ephemeral_public_mine,
    const mpz_t ephemeral_public_theirs,
    const mpz_t static_public_theirs,
    const DHParams& params)
{
    mpz_t d, e, pow2_l, tmp, exponent, base;
    mpz_inits(d, e, pow2_l, tmp, exponent, base, NULL);

    unsigned int l = mpz_sizeinbase(params.q, 2) / 2;
    mpz_ui_pow_ui(pow2_l, 2, l);

    // X = ephemeral_public_mine
    // d = 2^l + (X mod 2^l)
    mpz_mod(tmp, ephemeral_public_mine, pow2_l);
    mpz_add(d, pow2_l, tmp);

    // Y = ephemeral_public_theirs
    // e = 2^l + (Y mod 2^l)
    mpz_mod(tmp, ephemeral_public_theirs, pow2_l);
    mpz_add(e, pow2_l, tmp);

    // x = ephemeral_private
    // a = static_keypair.private_key
    // exponent = (x + d*a) mod q
    mpz_mul(tmp, d, static_keypair.private_key);
    mpz_add(exponent, ephemeral_private, tmp);
    mpz_mod(exponent, exponent, params.q);

    // B = static_public_theirs
    // base = (Y * B^e) mod p
    mpz_powm(tmp, static_public_theirs, e, params.p);
    mpz_mul(base, ephemeral_public_theirs, tmp);
    mpz_mod(base, base, params.p);

    // shared_secret = base^exponent mod p
    mpz_powm(shared_secret, base, exponent, params.p);

    mpz_clears(d, e, pow2_l, tmp, exponent, base, NULL);
}
