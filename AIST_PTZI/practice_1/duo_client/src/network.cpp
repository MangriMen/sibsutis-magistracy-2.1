#include "network.h"

// Function to convert mpz_t to string for network transmission
std::string mpz_to_string(const mpz_t num)
{
    char* str = mpz_get_str(nullptr, 16, num);
    std::string result(str);
    free(str);
    return result;
}

// Function to convert string back to mpz_t
void string_to_mpz(mpz_t num, const std::string& str)
{
    mpz_set_str(num, str.c_str(), 16);
}

// Function to send mpz_t over socket
bool send_mpz(SOCKET sock, const mpz_t num)
{
    std::string str = mpz_to_string(num);
    str += "\n";
    return send(sock, str.c_str(), str.length(), 0) != SOCKET_ERROR;
}

// Function to receive mpz_t from socket
bool receive_mpz(SOCKET sock, mpz_t num)
{
    char buffer[BUFFER_SIZE];
    std::string str;
    int bytes;

    while ((bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes] = '\0';
        str += buffer;
        if (str.find('\n') != std::string::npos) {
            break;
        }
    }

    if (bytes <= 0)
        return false;

    str = str.substr(0, str.find('\n'));
    string_to_mpz(num, str);
    return true;
}