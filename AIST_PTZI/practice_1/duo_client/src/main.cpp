#include <iostream>

#include "cli.h"
#include "client.h"
#include "server.h"

int main(int argc, char* argv[])
{
    std::string executable_name = argv[0];

    if (argc < 4) {
        print_usage(executable_name);
        return EXIT_FAILURE;
    }

    Args args;
    try {
        args = parse_args(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        print_usage(executable_name);
        return EXIT_FAILURE;
    }

    try {
        switch (args.mode) {
        case RunMode::Server:
            run_server(args.params_path, args.protocol);
            break;
        case RunMode::Client:
            if (args.server_ip.empty()) {
                return EXIT_FAILURE;
            }
            run_client(args.params_path, args.server_ip, args.protocol);
            break;
        default:
            print_usage(executable_name);
            return EXIT_FAILURE;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}