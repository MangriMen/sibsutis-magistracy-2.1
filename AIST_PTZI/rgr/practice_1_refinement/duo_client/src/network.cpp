#include "network.h"
#include <cstring>
#include <iostream>

std::string mpz_to_string(const mpz_t num)
{
    char* str = mpz_get_str(nullptr, 16, num);
    std::string result(str);
    free(str);
    return result;
}

void string_to_mpz(mpz_t num, const std::string& str)
{
    mpz_set_str(num, str.c_str(), 16);
}

bool send_mpz(SocketType sock, const mpz_t num)
{
    std::string str = mpz_to_string(num);
    str += "\n";

#ifdef _WIN32
    int result = send(sock, str.c_str(), static_cast<int>(str.length()), 0);
    return result != SOCKET_ERROR && static_cast<size_t>(result) == str.length();
#else
    ssize_t bytes_sent = send(sock, str.c_str(), str.length(), 0);
    return bytes_sent != -1 && static_cast<size_t>(bytes_sent) == str.length();
#endif
}

bool receive_mpz(SocketType sock, mpz_t num)
{
    char buffer[BUFFER_SIZE];
    std::string str;

#ifdef _WIN32
    int bytes;
    while ((bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0)) > 0) {
#else
    ssize_t bytes;
    while ((bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0)) > 0) {
#endif
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