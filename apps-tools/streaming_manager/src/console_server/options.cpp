#include <iostream>
#include <getopt.h>
#include <vector>
#include <cstring>
#include <algorithm>

#include "options.h"

static struct option long_options[] = {
        /* These options set a flag. */
        {"background",       no_argument,       0, 'b'},
        {"file",             required_argument, 0, 'f'},
        {"port",             required_argument, 0, 'p'},
        {"search_port",      required_argument, 0, 's'},
        {"help",             no_argument, 0, 'h'},
        {0, 0, 0, 0}
};

static constexpr char optstring[] = "bf:p:s:h";

std::vector<std::string> ClientOpt::split(const std::string& s, char seperator)
{
    std::vector<std::string> output;

    std::string::size_type prev_pos = 0, pos = 0;

    while((pos = s.find(seperator, pos)) != std::string::npos)
    {
        std::string substring( s.substr(prev_pos, pos-prev_pos) );

        output.push_back(substring);

        prev_pos = ++pos;
    }

    output.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word

    return output;
}

int get_int(int *value, const char *str,const char *message,int min_value, int max_value)
{
    try
    {
        if ( str == NULL || *str == '\0' )
            throw std::invalid_argument("null string");
        auto checkstr = str;
        while(*checkstr)
        {
            if ( *checkstr < '0' || *checkstr > '9' )
                throw std::invalid_argument("invalid input string");
            ++checkstr;
        }
        *value = std::stoi(str);
    }
    catch (...)
    {
        RP_LOG(LOG_ERR,"%s: %s\n",message, str)
        fprintf(stderr, "%s: %s\n",message, str);
        return -1;
    }
    if (*value < min_value){
        RP_LOG(LOG_ERR,"%s: %s\n",message, str)
        fprintf(stderr, "%s: %s\n",message, str);
        return -1;
    }

    if (*value > max_value){
        RP_LOG(LOG_ERR,"%s: %s\n",message, str)
        fprintf(stderr, "%s: %s\n",message, str);
        return -1;
    }
    return 0;
}

/** Print usage information */
auto ClientOpt::usage(char const* progName) -> void{
#ifdef _WIN32
    auto arr = split(std::string(progName),'\\');
#else
    auto arr = split(std::string(progName),'/');
#endif

    std::string name = "";
    if (arr.size() > 0)
        name = arr[arr.size()-1];
    const char *format =
                "Usage: \n"
                "\t%s [-b] [-f PATH] [-p PORT] [-s PORT] [-v]\n"
                "\t%s [--background] [--file=PATH] [--port=PORT] [--search_port=PORT] [--verbose]\n"
                "\n"
                "\t--background          -b        Run service in background.\n"
                "\t--file=PATH           -f FILE   Path to configuration file.\n"
                "\t                                By default uses the config file /root/.streaming_config.\n"
                "\t--port=PORT           -p PORT   Port for configuration server (Default: 8901).\n";
                "\t--search_port=PORT    -s PORT   Port for broadcast (Default: 8902).\n"
                "\n"
                "\t Example:\n"
                "\t\t%s -b -f /root/.streaming_config_new\n";

    auto n = name.c_str();
    RP_LOG(LOG_INFO,format, n ,n);
    fprintf(stdout,format, n ,n);
}

auto ClientOpt::parse(int argc, char* argv[]) -> ClientOpt::Options{
    Options opt;
    if (argc < 2) return opt;
    /* getopt_long stores the option index here. */
    int option_index = 0;
    int ch = -1;
    
    while ((ch = getopt_long(argc, argv, optstring, long_options, &option_index)) != -1) {
        switch (ch) {

            case 'b':
                opt.background = true;
                break;

            case 'p': {
                int config_port = 0;
                if (get_int(&config_port, optarg, "Error get port number for configuration server",1, 65535) != 0) {
                    exit(EXIT_FAILURE);
                }
                opt.config_port = optarg;
                break;
            }

            case 'h': {
                usage(argv[0]);
                exit(EXIT_SUCCESS);
                break;
            }

            case 's': {
                int config_port = 0;
                if (get_int(&config_port, optarg, "Error get port number for broadcast server",1, 65535) != 0) {
                    exit(EXIT_FAILURE);
                }
                opt.broadcast_port = optarg;
                break;
            }

            case 'f': {
                if (strcmp(optarg, "") != 0) {
                    opt.conf_file = optarg;
                } else {
                    RP_LOG(LOG_ERR,"[ERROR] key --file: %s\n", optarg);
                    fprintf(stderr,"[ERROR] key --file: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            }

            default: {
                RP_LOG(LOG_ERR,"[ERROR] Unknown parameter\n");
                fprintf(stderr,"[ERROR] Unknown parameter\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    return opt;
}
