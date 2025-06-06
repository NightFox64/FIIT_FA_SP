#include "../include/server_logger_builder.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

logger_builder& server_logger_builder::add_file_stream(
    std::string const &stream_file_path,
    logger::severity severity) &
{
    _output_streams[severity] = {stream_file_path, false};
    return *this;
}

logger_builder& server_logger_builder::add_console_stream(
    logger::severity severity) &
{
    _output_streams[severity] = {"console", true};
    return *this;
}

logger_builder& server_logger_builder::transform_with_configuration(
    std::string const &configuration_file_path,
    std::string const &configuration_path) &
{
    std::ifstream config_file(configuration_file_path);
    if (!config_file.is_open())
    {
        throw std::runtime_error("Cannot open configuration file");
    }

    json config;
    try
    {
        config_file >> config;
    }
    catch (const json::parse_error& e)
    {
        throw std::runtime_error("Invalid JSON configuration");
    }

    if (!config.contains(configuration_path))
    {
        throw std::runtime_error("Configuration path not found");
    }

    json logger_config = config[configuration_path];

    if (logger_config.contains("destination"))
    {
        _destination = logger_config["destination"];
    }

    if (logger_config.contains("format"))
    {
        _format = logger_config["format"];
    }

    if (logger_config.contains("streams"))
    {
        for (auto& stream : logger_config["streams"])
        {
            std::string severity_str = stream["severity"];
            logger::severity sev = logger_builder::string_to_severity(severity_str);
            
            if (stream["type"] == "file")
            {
                _output_streams[sev] = {stream["path"], false};
            }
            else if (stream["type"] == "console")
            {
                _output_streams[sev] = {"console", true};
            }
        }
    }

    return *this;
}

logger_builder& server_logger_builder::set_destination(const std::string& dest) &
{
    _destination = dest;
    return *this;
}

logger_builder& server_logger_builder::clear() &
{
    _output_streams.clear();
    _format = "%d %t %s %m";
    _destination = "127.0.0.1:9200";
    return *this;
}

logger_builder& server_logger_builder::set_format(const std::string& format) &
{
    _format = format;
    return *this;
}

logger *server_logger_builder::build() const
{
    auto* logger = new server_logger(_destination, _output_streams, _format);
    
    std::ostringstream config_msg;
    config_msg << logger->inner_getpid() << "|CONFIG|";
    
    for (const auto& stream : _output_streams)
    {
        config_msg << logger::severity_to_string(stream.first) << ":";
        if (stream.second.second)
        {
            config_msg << "console";
        }
        else
        {
            config_msg << "file:" << stream.second.first;
        }
        config_msg << ";";
    }
    config_msg << "\n";
    
    int temp_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (temp_sock >= 0)
    {
        struct sockaddr_in server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        
        size_t colon_pos = _destination.find(':');
        std::string host = _destination.substr(0, colon_pos);
        int port = std::stoi(_destination.substr(colon_pos + 1));
        
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(port);
        inet_pton(AF_INET, host.c_str(), &server_addr.sin_addr);
        
        if (connect(temp_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) >= 0)
        {
            send(temp_sock, config_msg.str().c_str(), config_msg.str().size(), 0);
        }
        close(temp_sock);
    }
    
    return logger;
}