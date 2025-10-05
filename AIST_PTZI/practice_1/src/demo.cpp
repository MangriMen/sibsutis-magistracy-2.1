#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <chrono>
#include <rdtsc.h>
#include "dh_params.h"
#include "dh.h"
#include "mqv.h"
#include "utils.h"
#include "prime.h"
#include "generators.h"

void demo_dh(const std::string &params_path)
{
  DHParams params;
  init_dh_params(params);
  load_params_from_file(params, params_path);

  const int name_width = 24;
  const int cycles_width = 20;

  std::cout << center("Diffie-Hellman protocol", name_width + cycles_width + 1) << "\n";
  std::cout << std::string(name_width, '-') << "|" << std::string(cycles_width, '-') << "\n";
  std::cout << center("Name", name_width) << "|" << center("Cycles", cycles_width) << "\n";
  std::cout << std::string(name_width, '-') << "|" << std::string(cycles_width, '-') << "\n";

  // Generate keys

  mpz_t alice_private, alice_public;
  mpz_init(alice_private);
  mpz_init(alice_public);

  unsigned int start = CC();
  generate_private_key(alice_private, params.q);
  generate_public_key(alice_public, params.g, alice_private, params.p);
  unsigned int end = CC();

  std::cout << left("Alice key", name_width) << "|" << prd(end - start, cycles_width) << "\n";

  mpz_t bob_private, bob_public;
  mpz_init(bob_private);
  mpz_init(bob_public);

  start = CC();
  generate_private_key(bob_private, params.q);
  generate_public_key(bob_public, params.g, bob_private, params.p);
  end = CC();

  std::cout << left("Bob key", name_width) << "|" << prd(end - start, cycles_width) << "\n";

  // Compute shared secrets

  mpz_t alice_secret;
  mpz_init(alice_secret);

  start = CC();
  compute_shared_secret(alice_secret, bob_public, alice_private, params.p);
  end = CC();

  std::cout << left("Alice shared secret", name_width) << "|" << prd(end - start, cycles_width) << "\n";

  mpz_t bob_secret;
  mpz_init(bob_secret);

  start = CC();
  compute_shared_secret(bob_secret, alice_public, bob_private, params.p);
  end = CC();

  std::cout << left("Bob shared secret", name_width) << "|" << prd(end - start, cycles_width) << "\n";
  std::cout << std::endl;

  print_number("Alice's shared secret", alice_secret);
  print_number("  Bob's shared secret", bob_secret);
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

void demo_subgroup_dh(const std::string &params_path)
{
  DHParams params;
  init_dh_params(params);
  load_params_from_file(params, params_path);

  const int name_width = 24;
  const int cycles_width = 20;
  std::cout << center("subgroup Diffie-Hellman protocol", name_width + cycles_width + 1) << "\n";
  std::cout << std::string(name_width, '-') << "|" << std::string(cycles_width, '-') << "\n";
  std::cout << center("Name", name_width) << "|" << center("Cycles", cycles_width) << "\n";
  std::cout << std::string(name_width, '-') << "|" << std::string(cycles_width, '-') << "\n";

  // Generate keys

  mpz_t alice_private, alice_public;
  mpz_init(alice_private);
  mpz_init(alice_public);

  unsigned int start = CC();
  generate_private_key(alice_private, params.q);
  generate_public_key(alice_public, params.g, alice_private, params.p);
  unsigned int end = CC();

  std::cout << left("Alice key", name_width) << "|" << prd(end - start, cycles_width) << "\n";

  mpz_t bob_private, bob_public;
  mpz_init(bob_private);
  mpz_init(bob_public);

  start = CC();
  generate_private_key(bob_private, params.q);
  generate_public_key(bob_public, params.g, bob_private, params.p);
  end = CC();

  std::cout << left("Bob key", name_width) << "|" << prd(end - start, cycles_width) << "\n";

  // Compute shared secrets

  mpz_t alice_secret;
  mpz_init(alice_secret);

  start = CC();
  compute_shared_secret(alice_secret, bob_public, alice_private, params.p);
  end = CC();

  std::cout << left("Alice shared secret", name_width) << "|" << prd(end - start, cycles_width) << "\n";

  mpz_t bob_secret;
  mpz_init(bob_secret);

  start = CC();
  compute_shared_secret(bob_secret, alice_public, bob_private, params.p);
  end = CC();

  std::cout << left("Bob shared secret", name_width) << "|" << prd(end - start, cycles_width) << "\n";
  std::cout << std::endl;

  print_number("Alice's shared secret", alice_secret);
  print_number("  Bob's shared secret", bob_secret);
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

  const int name_width = 24;
  const int cycles_width = 20;
  std::cout << center("MQV protocol", name_width + cycles_width + 1) << "\n";
  std::cout << std::string(name_width, '-') << "|" << std::string(cycles_width, '-') << "\n";
  std::cout << center("Name", name_width) << "|" << center("Cycles", cycles_width) << "\n";
  std::cout << std::string(name_width, '-') << "|" << std::string(cycles_width, '-') << "\n";

  // Generate static key pairs

  MQVKeyPair alice_static;
  init_mqv_keypair(alice_static);

  unsigned int start = CC();
  generate_mqv_keypair(alice_static, params);
  unsigned int end = CC();
  std::cout << left("Alice static keypair", name_width) << "|" << prd(end - start, cycles_width) << "\n";

  MQVKeyPair bob_static;
  init_mqv_keypair(bob_static);

  start = CC();
  generate_mqv_keypair(bob_static, params);
  end = CC();

  std::cout << left("Bob static keypair", name_width) << "|" << prd(end - start, cycles_width) << "\n";

  // Generate ephemeral keys
  mpz_t alice_ephemeral_private, alice_ephemeral_public;
  mpz_init(alice_ephemeral_private);
  mpz_init(alice_ephemeral_public);

  start = CC();
  generate_private_key(alice_ephemeral_private, params.q);
  generate_public_key(alice_ephemeral_public, params.g, alice_ephemeral_private, params.p);
  end = CC();

  std::cout << left("Alice ephemeral key", name_width) << "|" << prd(end - start, cycles_width) << "\n";

  mpz_t bob_ephemeral_private, bob_ephemeral_public;
  mpz_init(bob_ephemeral_private);
  mpz_init(bob_ephemeral_public);

  start = CC();
  generate_private_key(bob_ephemeral_private, params.q);
  generate_public_key(bob_ephemeral_public, params.g, bob_ephemeral_private, params.p);
  end = CC();

  std::cout << left("Bob ephemeral key", name_width) << "|" << prd(end - start, cycles_width) << "\n";

  // Compute shared secrets
  mpz_t alice_secret;
  mpz_init(alice_secret);

  start = CC();
  compute_mqv_shared_secret(alice_secret, alice_static, alice_ephemeral_private, alice_ephemeral_public,
                            bob_ephemeral_public, bob_static.public_key, params);
  end = CC();

  std::cout << left("Alice MQV shared secret", name_width) << "|" << prd(end - start, cycles_width) << "\n";

  mpz_t bob_secret;
  mpz_init(bob_secret);

  start = CC();
  compute_mqv_shared_secret(bob_secret, bob_static, bob_ephemeral_private, bob_ephemeral_public,
                            alice_ephemeral_public, alice_static.public_key, params);
  end = CC();

  std::cout << left("Bob MQV shared secret", name_width) << "|" << prd(end - start, cycles_width) << "\n";
  std::cout << std::endl;

  print_number("Alice's shared secret", alice_secret);
  print_number("  Bob's shared secret", bob_secret);
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
  const std::string params_dir_path = "";

  const std::string multiplicative_params_path = params_dir_path + "multiplicative_params.txt";
  const std::string cyclic_params_path = params_dir_path + "cyclic_params.txt";

  demo_dh(multiplicative_params_path);
  std::cout << std::endl;
  demo_subgroup_dh(cyclic_params_path);
  std::cout << std::endl;
  demo_mqv(multiplicative_params_path);

  return EXIT_SUCCESS;
}