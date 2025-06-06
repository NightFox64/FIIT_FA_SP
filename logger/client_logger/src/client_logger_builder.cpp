#include <filesystem>
#include <utility>
#include <not_implemented.h>
#include "../include/client_logger_builder.h"
#include <not_implemented.h>

using namespace nlohmann;

logger_builder& client_logger_builder::add_file_stream(
    std::string const &stream_file_path,
    logger::severity severity) &
{
    auto &streams_pair = _output_streams[severity];
    streams_pair.first.emplace_front(stream_file_path);
    return *this;
}

logger_builder& client_logger_builder::add_console_stream(
    logger::severity severity) &
{
    _output_streams[severity].second = true;
    return *this;
}

logger_builder& client_logger_builder::transform_with_configuration(
    std::string const &configuration_file_path,
    std::string const &configuration_path) &
{
    if (!std::filesystem::exists(configuration_file_path))
    {
        std::cerr << "Configuration file doesn't exist: " << configuration_file_path << std::endl;
        return *this;
    }

    if (std::filesystem::file_size(configuration_file_path) == 0)
    {
        std::cerr << "Configuration file is empty: " << configuration_file_path << std::endl;
        return *this;
    }

    try
    {
        std::ifstream config_file(configuration_file_path);
        if (!config_file.is_open())
        {
            std::cerr << "Failed to open configuration file: " << configuration_file_path << std::endl;
            return *this;
        }

        nlohmann::json config;
        config_file >> config;

        if (config.is_null())
        {
            std::cerr << "Empty JSON configuration in file: " << configuration_file_path << std::endl;
            return *this;
        }

        nlohmann::json *current = &config;
        if (!configuration_path.empty())
        {
            std::istringstream path_stream(configuration_path);
            std::string path_part;
            while (std::getline(path_stream, path_part, '.'))
            {
                if (current->contains(path_part))
                {
                    current = &(*current)[path_part];
                }
                else
                {
                    std::cerr << "Configuration path not found: " << configuration_path 
                              << " in file: " << configuration_file_path << std::endl;
                    return *this;
                }
            }
        }

        if (current->contains("format"))
        {
            try {
                _format = current->at("format").template get<std::string>();
            } catch (const nlohmann::json::exception& e) {
                std::cerr << "Failed to parse format: " << e.what() << std::endl;
            }
        }

        if (current->contains("streams"))
        {
            try {
                for (auto &[key, value] : current->at("streams").items())
                {
                    try {
                        parse_severity(string_to_severity(key), value);
                    } catch (const std::exception& e) {
                        std::cerr << "Failed to parse severity " << key << ": " << e.what() << std::endl;
                    }
                }
            } catch (const nlohmann::json::exception& e) {
                std::cerr << "Failed to parse streams: " << e.what() << std::endl;
            }
        }
    }
    catch (const nlohmann::json::exception& e)
    {
        std::cerr << "JSON parsing error in file " << configuration_file_path 
                  << ": " << e.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error processing configuration file " << configuration_file_path 
                  << ": " << e.what() << std::endl;
    }

    return *this;
}

logger_builder& client_logger_builder::clear() &
{
    _output_streams.clear();
    _format = "%m";
    return *this;
}

logger *client_logger_builder::build() const
{
    return new client_logger(_output_streams, _format);
}

logger_builder& client_logger_builder::set_format(const std::string &format) &
{
    _format = format;
    return *this;
}

void client_logger_builder::parse_severity(logger::severity sev, nlohmann::json& j)
{
    if (j.is_object())
    {
        if (j.contains("file") && j["file"].is_string())
        {
            add_file_stream(j["file"].template get<std::string>(), sev);
        }
        if (j.contains("console") && j["console"].is_boolean() && j["console"].get<bool>())
        {
            add_console_stream(sev);
        }
    }
    else if (j.is_string())
    {
        add_file_stream(j.template get<std::string>(), sev);
    }
    else if (j.is_boolean() && j.get<bool>())
    {
        add_console_stream(sev);
    }
    else if (j.is_array())
    {
        for (auto &item : j)
        {
            if (item.is_string())
            {
                add_file_stream(item.template get<std::string>(), sev);
            }
            else if (item.is_boolean() && item.get<bool>())
            {
                add_console_stream(sev);
            }
        }
    }
}

logger_builder& client_logger_builder::set_destination(const std::string &format) &
{
    throw std::logic_error("Invalid operation: set_destination is not supported");
}
