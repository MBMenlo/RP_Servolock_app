#pragma once
#include <string>
#include <vector>
#include <syslog.h>

#define DEBUG

#ifdef DEBUG
#define RP_LOG(...) syslog(__VA_ARGS__);
#else
#define RP_LOG(...)
#endif


namespace ClientOpt {

    struct Options {
        bool background;
        std::string config_port;
        std::string broadcast_port;
        std::string conf_file;
        
        Options(){
            background = false;
            config_port = "8901";
            broadcast_port = "8902";
            conf_file = "/root/.streaming_config";
        };
    };
    auto split(const std::string& s, char seperator) ->  std::vector<std::string>;
    auto usage(char const *progName) -> void;
    auto parse(int argc, char* argv[]) -> Options;
}