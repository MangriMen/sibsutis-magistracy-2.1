#include "mqv.h"
#include "dh.h"

// MQV protocol functions
void init_mqv_keypair(MQVKeyPair &keypair)
{
  mpz_init(keypair.private_key);
  mpz_init(keypair.public_key);
}

void clear_mqv_keypair(MQVKeyPair &keypair)
{
  mpz_clear(keypair.private_key);
  mpz_clear(keypair.public_key);
}

void generate_mqv_keypair(MQVKeyPair &keypair, const DHParams &params)
{
  generate_private_key(keypair.private_key, params.q);
  generate_public_key(keypair.public_key, params.g, keypair.private_key, params.p);
}

void compute_mqv_shared_secret(mpz_t shared_secret,
                               const MQVKeyPair &static_keypair,
                               const mpz_t ephemeral_private,
                               const mpz_t ephemeral_public_mine,
                               const mpz_t ephemeral_public_theirs,
                               const mpz_t static_public_theirs,
                               const DHParams &params)
{
  mpz_t d, e, temp1, temp2, exponent;
  mpz_init(d);
  mpz_init(e);
  mpz_init(temp1);
  mpz_init(temp2);
  mpz_init(exponent);

  // Compute d = 2^l + (X mod 2^l)
  unsigned int l = mpz_sizeinbase(params.q, 2) / 2;
  mpz_ui_pow_ui(d, 2, l);
  mpz_mod(temp1, ephemeral_public_mine, d);
  mpz_add(d, d, temp1);

  // Compute e = 2^l + (Y mod 2^l)
  mpz_ui_pow_ui(e, 2, l);
  mpz_mod(temp1, ephemeral_public_theirs, e);
  mpz_add(e, e, temp1);

  // Compute (x + da) mod q
  mpz_mul(temp1, d, static_keypair.private_key);
  mpz_add(temp1, ephemeral_private, temp1);
  mpz_mod(exponent, temp1, params.q);

  // Compute Y * B^e
  mpz_powm(temp1, static_public_theirs, e, params.p);
  mpz_mul(temp2, ephemeral_public_theirs, temp1);
  mpz_mod(temp2, temp2, params.p);

  // Final computation
  mpz_powm(shared_secret, temp2, exponent, params.p);

  mpz_clear(d);
  mpz_clear(e);
  mpz_clear(temp1);
  mpz_clear(temp2);
  mpz_clear(exponent);
}