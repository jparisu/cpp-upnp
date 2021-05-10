// Copyright 2021 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file main.cpp
 *
 */

#include <upnp.h>
#include <iostream>
#include <boost/asio/ip/udp.hpp>
#include <upnp/third_party/optionparser.h>
#include <upnp/third_party/result.h>

namespace net = upnp::net;

/*
 * Struct to parse the executable arguments
 */
struct Arg: public option::Arg
{
    static void print_error(const char* msg1, const option::Option& opt, const char* msg2)
    {
        fprintf(stderr, "%s", msg1);
        fwrite(opt.name, opt.namelen, 1, stderr);
        fprintf(stderr, "%s", msg2);
    }

    static option::ArgStatus Unknown(const option::Option& option, bool msg)
    {
        if (msg) print_error("Unknown option '", option, "'\n");
        return option::ARG_ILLEGAL;
    }

    static option::ArgStatus Required(const option::Option& option, bool msg)
    {
        if (option.arg != 0 && option.arg[0] != 0)
        return option::ARG_OK;

        if (msg) print_error("Option '", option, "' requires an argument\n");
        return option::ARG_ILLEGAL;
    }

    static option::ArgStatus Numeric(const option::Option& option, bool msg)
    {
        char* endptr = 0;
        if (option.arg != 0 && std::strtol(option.arg, &endptr, 10))
        {
        }
        if (endptr != option.arg && *endptr == 0)
        {
            return option::ARG_OK;
        }

        if (msg)
        {
            print_error("Option '", option, "' requires a numeric argument\n");
        }
        return option::ARG_ILLEGAL;
    }

    static option::ArgStatus Float(const option::Option& option, bool msg)
    {
        // char* endptr = 0;
        // if (option.arg != 0 && std::stof(option.arg, &endptr, 10))
        // {
        // }
        // if (endptr != option.arg && *endptr == 0)
        // {
        //     return option::ARG_OK;
        // }

        // if (msg)
        // {
        //     print_error("Option '", option, "' requires a float argument\n");
        // }
        // return option::ARG_ILLEGAL;
        return option::ARG_OK;
    }

    static option::ArgStatus String(const option::Option& option, bool msg)
    {
        if (option.arg != 0)
        {
            return option::ARG_OK;
        }
        if (msg)
        {
            print_error("Option '", option, "' requires a numeric argument\n");
        }
        return option::ARG_ILLEGAL;
    }
};

// TODO is possible to open 2 transports in same port, so tcp and udp could be set as same port for future versions
/*
 * Option arguments available
 */
enum  optionIndex {
    UNKNOWN_OPT,
    HELP,
    LOGICAL_PORT,
    PHYSICAL_PORT,
    TIME,
    DESCRIPTION,
    CLIENT
};

/*
 * Usage description
 */
const option::Descriptor usage[] = {
    { UNKNOWN_OPT, 0,"", "",                    Arg::None,
        "Usage: Forward a IGD port to this device"},
    { HELP,    0,"h", "help",                   Arg::None,
        "-h \t--help  \tProduce help message." },
    { LOGICAL_PORT, 0, "l", "logical-port",     Arg::Numeric,
        "-l <num>\t  --logical-port=<num> \tPublic port to open in the router."},
    { PHYSICAL_PORT, 0, "p", "physical-port",   Arg::Numeric,
        "-p <num>\t  --physical-port=<num> \tHost port to listen packets from the router."},
    { TIME, 0, "t", "time",                     Arg::Numeric,
        "-t <num> \t--time=<num> \tTime in seconds until the port is closed again (Default 60)."},
    { DESCRIPTION, 0, "d", "description",       Arg::String,
        "-d <str> \t--description=<str> \tDescription for the port mapping (Default 'test')."},
    { CLIENT, 0, "c", "client",       Arg::String,
        "-c <str> \t--client=<str> \tIp of the client to open port (Default <own ip>)."},
    { 0, 0, 0, 0, 0, 0 }
};

void add_port(int logic_port, int physic_port, int time, std::string desc, std::string ip)
{
    net::io_context ctx;

    net::spawn(ctx, [&] (net::yield_context yield)
    {
        std::cout << "Discovering IGDs" << std::endl;

        auto r_igds = upnp::igd::discover(ctx.get_executor(), yield);

        if (r_igds)
        {
            std::cout << " " << "Success discovery. Found " << r_igds.value().size() << " IGDs" << std::endl;
        }
        else
        {
            std::cerr << " " << "Error in discovery: " << r_igds.error().message() << std::endl;
            return;
        }

        auto igds = move(r_igds.value());

        net::ip::udp::socket
            socket(ctx, net::ip::udp::endpoint(net::ip::address_v4::any(), 0));

        for (auto& igd : igds)
        {
            std::cout << "IGD: " << igd.friendly_name() << std::endl;

            auto address_response = igd.get_external_address(yield);

            if (address_response)
            {
                std::cout << "IGD " << igd.friendly_name() << " External address: "
                    << address_response.value() << std::endl;
            }
            else
            {
                std::cerr << "Error response for getting external address: " << address_response.error() << std::endl;
                continue;
            }

            bool success = false;

            if (ip == "")
            {
                auto port_response = igd.add_internal_port_mapping(
                    upnp::igd::tcp,
                    physic_port,
                    logic_port,
                    desc,
                    std::chrono::seconds(time),
                    yield);

                if (port_response)
                {
                    std::cout << "The External address: " << address_response.value() << ":" << logic_port
                        << " will be forwarded to this host port: " << physic_port
                        << " during " << time << " seconds" << std::endl;
                }
                else
                {
                    std::cerr << "Error response for adding port mapping: " << port_response.error() << std::endl;
                    continue;
                }
            }
            else
            {
                auto port_response = igd.add_port_mapping(
                    upnp::igd::tcp,
                    physic_port,
                    logic_port,
                    net::ip::address::from_string(ip),
                    desc,
                    std::chrono::seconds(time),
                    yield);

                if (port_response)
                {
                    std::cout << "The External address: " << address_response.value() << ":" << logic_port
                        << " will be forwarded to address: " << ip << ":" << physic_port
                        << " during " << time << " seconds" << std::endl;
                }
                else
                {
                    std::cerr << "Error response for adding port mapping: " << port_response.error() << std::endl;
                    continue;
                }
            }
        }
    });

    ctx.run();
}

int main(int argc, char** argv)
{
    // Variable to pretty print usage help
    int columns;
#if defined(_WIN32)
    char* buf = nullptr;
    size_t sz = 0;
    if (_dupenv_s(&buf, &sz, "COLUMNS") == 0 && buf != nullptr)
    {
        columns = strtol(buf, nullptr, 10);
        free(buf);
    }
    else
    {
        columns = 80;
    }
#else
    columns = getenv("COLUMNS") ? atoi(getenv("COLUMNS")) : 80;
#endif

    // Get executable arguments
    int logic_port = -1;
    int physic_port = -1;
    int time = 60;
    std::string desc("test");
    std::string ip("");

    // 2 required arguments
    if (argc > 2)
    {

        argc -= (argc > 0); // reduce arg count of program name if present
        argv += (argc > 0); // skip program name argv[0] if present

        option::Stats stats(usage, argc, argv);
        std::vector<option::Option> options(stats.options_max);
        std::vector<option::Option> buffer(stats.buffer_max);
        option::Parser parse(usage, argc, argv, &options[0], &buffer[0]);

        if (parse.error())
        {
            return 1;
        }

        if (options[HELP])
        {
            option::printUsage(fwrite, stdout, usage, columns);
            return 0;
        }

        for (int i = 0; i < parse.optionsCount(); ++i)
        {
            option::Option& opt = buffer[i];
            switch (opt.index())
            {
                case HELP:
                    option::printUsage(fwrite, stdout, usage, columns);
                    return 0;
                    break;

                case LOGICAL_PORT:
                    logic_port = std::strtol(opt.arg, nullptr, 10);
                    break;

                case PHYSICAL_PORT:
                    physic_port = std::strtol(opt.arg, nullptr, 10);
                    break;

                case TIME:
                    time = std::strtol(opt.arg, nullptr, 10);
                    break;

                case DESCRIPTION:
                    desc = opt.arg;
                    break;

                case CLIENT:
                    ip = opt.arg;
                    break;

                case UNKNOWN_OPT:
                    option::printUsage(fwrite, stdout, usage, columns);
                    return 1;
                    break;
            }
        }
    }
    else
    {
        option::printUsage(fwrite, stdout, usage, columns);
        return 1;
    }

    // Ports must be specified
    if (logic_port == -1 || physic_port == -1)
    {
        std::cout << "CLI error: Logical and Physical ports must be specified" << std::endl;
        option::printUsage(fwrite, stdout, usage, columns);
        return 1;
    }

    add_port(logic_port, physic_port, time, desc, ip);

    return 0;
}
