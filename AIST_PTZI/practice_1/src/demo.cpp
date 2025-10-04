#include <iostream>
#include <chrono>
#include <rdtsc.h>
#include "dh_params.h"
#include "dh.h"
#include "mqv.h"
#include "utils.h"

void demo_parameter_generation(const std::string &params_path)
{
  DHParams params;
  init_dh_params(params);

  std::cout << "Generating parameters..." << std::endl;
  unsigned int start = CC();
  generate_params(params);
  unsigned int end = CC();
  std::cout << "Parameter generation took " << (end - start) << " cycles" << std::endl;
  std::cout << "Generated parameters:" << std::endl;

  print_number("p", params.p);
  print_number("q", params.q);
  print_number("g", params.g);

  save_params_to_file(params, params_path);

  clear_dh_params(params);
}

void demo_original_dh(const std::string &params_path)
{
  DHParams params;
  init_dh_params(params);
  load_params_from_file(params, params_path);

  // Alice's keys
  mpz_t alice_private, alice_public;
  mpz_init(alice_private);
  mpz_init(alice_public);

  // Bob's keys
  mpz_t bob_private, bob_public;
  mpz_init(bob_private);
  mpz_init(bob_public);

  // Shared secrets
  mpz_t alice_secret, bob_secret;
  mpz_init(alice_secret);
  mpz_init(bob_secret);

  std::cout << "\nDemonstrating original DH protocol..." << std::endl;

  // Generate keys
  unsigned int start = CC();
  generate_private_key(alice_private, params.q);
  generate_public_key(alice_public, params.g, alice_private, params.p);
  unsigned int end = CC();
  std::cout << "Alice key generation took " << (end - start) << " cycles" << std::endl;

  start = CC();
  generate_private_key(bob_private, params.q);
  generate_public_key(bob_public, params.g, bob_private, params.p);
  end = CC();
  std::cout << "Bob key generation took " << (end - start) << " cycles" << std::endl;

  // Compute shared secrets
  start = CC();
  compute_shared_secret(alice_secret, bob_public, alice_private, params.p);
  end = CC();
  std::cout << "Alice shared secret computation took " << (end - start) << " cycles" << std::endl;

  start = CC();
  compute_shared_secret(bob_secret, alice_public, bob_private, params.p);
  end = CC();
  std::cout << "Bob shared secret computation took " << (end - start) << " cycles" << std::endl;

  print_number("Alice's shared secret", alice_secret);
  print_number("Bob's shared secret", bob_secret);
  std::cout << "Secrets match: " << (mpz_cmp(alice_secret, bob_secret) == 0 ? "Yes" : "No") << std::endl;

  // Cleanup
  mpz_clear(alice_private);
  mpz_clear(alice_public);
  mpz_clear(bob_private);
  mpz_clear(bob_public);
  mpz_clear(alice_secret);
  mpz_clear(bob_secret);
  clear_dh_params(params);
}

void demo_mqv(const std::string &params_path)
{
  DHParams params;
  init_dh_params(params);
  load_params_from_file(params, params_path);

  // Alice's key pairs
  MQVKeyPair alice_static;
  init_mqv_keypair(alice_static);
  mpz_t alice_ephemeral_private, alice_ephemeral_public;
  mpz_init(alice_ephemeral_private);
  mpz_init(alice_ephemeral_public);

  // Bob's key pairs
  MQVKeyPair bob_static;
  init_mqv_keypair(bob_static);
  mpz_t bob_ephemeral_private, bob_ephemeral_public;
  mpz_init(bob_ephemeral_private);
  mpz_init(bob_ephemeral_public);

  // Generate static key pairs
  unsigned int start = CC();
  generate_mqv_keypair(alice_static, params);
  unsigned int end = CC();
  std::cout << "Alice static keypair generation took " << (end - start) << " cycles" << std::endl;

  start = CC();
  generate_mqv_keypair(bob_static, params);
  end = CC();
  std::cout << "Bob static keypair generation took " << (end - start) << " cycles" << std::endl;

  // Generate ephemeral keys
  start = CC();
  generate_private_key(alice_ephemeral_private, params.q);
  generate_public_key(alice_ephemeral_public, params.g, alice_ephemeral_private, params.p);
  end = CC();
  std::cout << "Alice ephemeral key generation took " << (end - start) << " cycles" << std::endl;

  start = CC();
  generate_private_key(bob_ephemeral_private, params.q);
  generate_public_key(bob_ephemeral_public, params.g, bob_ephemeral_private, params.p);
  end = CC();
  std::cout << "Bob ephemeral key generation took " << (end - start) << " cycles" << std::endl;

  // Compute shared secrets
  mpz_t alice_secret, bob_secret;
  mpz_init(alice_secret);
  mpz_init(bob_secret);

  start = CC();
  compute_mqv_shared_secret(alice_secret, alice_static, alice_ephemeral_private, alice_ephemeral_public,
                            bob_ephemeral_public, bob_static.public_key, params);
  end = CC();
  std::cout << "Alice MQV shared secret computation took " << (end - start) << " cycles" << std::endl;

  start = CC();
  compute_mqv_shared_secret(bob_secret, bob_static, bob_ephemeral_private, bob_ephemeral_public,
                            alice_ephemeral_public, alice_static.public_key, params);
  end = CC();
  std::cout << "Bob MQV shared secret computation took " << (end - start) << " cycles" << std::endl;

  std::cout << "\nDemonstrating MQV protocol..." << std::endl;
  print_number("Alice's shared secret", alice_secret);
  print_number("Bob's shared secret", bob_secret);
  std::cout << "Secrets match: " << (mpz_cmp(alice_secret, bob_secret) == 0 ? "Yes" : "No") << std::endl;

  // Cleanup
  clear_mqv_keypair(alice_static);
  clear_mqv_keypair(bob_static);
  mpz_clear(alice_ephemeral_private);
  mpz_clear(alice_ephemeral_public);
  mpz_clear(bob_ephemeral_private);
  mpz_clear(bob_ephemeral_public);
  mpz_clear(alice_secret);
  mpz_clear(bob_secret);
  clear_dh_params(params);
}

int main()
{
  const std::string params_path = "dh_params.txt";

  demo_parameter_generation(params_path);
  demo_original_dh(params_path);
  demo_mqv(params_path);

  return EXIT_SUCCESS;
}