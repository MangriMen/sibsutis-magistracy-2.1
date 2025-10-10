#pragma once

#include <gmp.h>
#include <string>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET SocketType;
#define INVALID_SOCKET_VALUE INVALID_SOCKET
#define CLOSE_SOCKET closesocket
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
typedef int SocketType;
#define INVALID_SOCKET_VALUE (-1)
#define CLOSE_SOCKET close
#endif

const int BUFFER_SIZE = 4096;

std::string mpz_to_string(const mpz_t num);
void string_to_mpz(mpz_t num, const std::string& str);

bool send_mpz(SocketType sock, const mpz_t num);
bool receive_mpz(SocketType sock, mpz_t num);
