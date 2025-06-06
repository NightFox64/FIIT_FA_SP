#include <string>
#include <sstream>
#include <algorithm>
#include <utility>
#include "../include/client_logger.h"
#include <not_implemented.h>

std::unordered_map<std::string, std::pair<size_t, std::ofstream>> client_logger::refcounted_stream::_global_streams;


// logger& client_logger::log(
//     const std::string &text,
//     logger::severity severity) &
//     {
//         auto it = _output_streams.find(severity);
//         if (it != _output_streams.end())
//         {
//             const auto formatted_message = make_format(text, severity);
            
//             for (auto &stream : it->second.first)
//             {
//                 if (stream._stream.second && stream._stream.second->is_open())
//                 {
//                     *stream._stream.second << formatted_message << std::endl;
//                 }
//             }
            
//             if (it->second.second)
//             {
//                 std::cout << formatted_message << std::endl;
//             }
//         }
//         return *this;
//     }

logger& client_logger::log(const std::string &text, logger::severity severity) &
{
    auto it = _output_streams.find(severity);
    if (it != _output_streams.end())
    {
        const auto formatted_message = make_format(text, severity);
        
        for (auto &stream : it->second.first)
        {
            if (stream._stream.second && stream._stream.second->is_open())
            {
                try {
                    *stream._stream.second << formatted_message << std::endl;
                } catch (const std::exception &ex) {
                    std::cerr << "Failed to write to log: " << ex.what() << std::endl;
                }
            }
        }
        
        if (it->second.second)
        {
            std::cout << formatted_message << std::endl;
        }
    }
    return *this;
}

std::string client_logger::make_format(const std::string &message, severity sev) const
{
    std::string result;
    for (size_t i = 0; i < _format.size(); ++i)
    {
        if (_format[i] == '%' && i + 1 < _format.size())
        {
            switch (char_to_flag(_format[++i]))
            {
                case flag::DATE:
                    result += logger::current_date_to_string();
                    break;
                case flag::TIME:
                    result += logger::current_time_to_string();
                    break;
                case flag::SEVERITY:
                    result += logger::severity_to_string(sev);
                    break;
                case flag::MESSAGE:
                    result += message;
                    break;
                case flag::NO_FLAG:
                    result += '%';
                    result += _format[i];
                    break;
            }
        }
        else
        {
            result += _format[i];
        }
    }
    return result;
}

client_logger::client_logger(
        const std::unordered_map<logger::severity, std::pair<std::forward_list<refcounted_stream>, bool>> &streams,
        std::string format) :
        _output_streams(streams),
        _format(std::move(format))
        {
            for (auto &[severity, streams_pair] : _output_streams)
            {
                for (auto &stream : streams_pair.first)
                {
                    stream.open();
                }
            }
        }

client_logger::flag client_logger::char_to_flag(char c) noexcept
{
    switch (c)
    {
        case 'd': return flag::DATE;
        case 't': return flag::TIME;
        case 's': return flag::SEVERITY;
        case 'm': return flag::MESSAGE;
        default: return flag::NO_FLAG;
    }
}

client_logger::client_logger(const client_logger &other) :
    _output_streams(other._output_streams),
    _format(other._format)
{
for (auto &[severity, streams_pair] : _output_streams)
{
    for (auto &stream : streams_pair.first)
    {
        stream.open();
    }
}
}

client_logger &client_logger::operator=(const client_logger &other)
{
    if (this != &other)
    {
        this->~client_logger();
        _output_streams = other._output_streams;
        _format = other._format;
        
        for (auto &[severity, streams_pair] : _output_streams)
        {
            for (auto &stream : streams_pair.first)
            {
                stream.open();
            }
        }
    }
    return *this;
}

// client_logger::client_logger(client_logger &&other) noexcept
// {}

client_logger::client_logger(client_logger &&other) noexcept :
    _output_streams(std::move(other._output_streams)),
    _format(std::move(other._format))
{}

client_logger &client_logger::operator=(client_logger &&other) noexcept
{
    if (this != &other)
    {
        this->~client_logger();
        _output_streams = std::move(other._output_streams);
        _format = std::move(other._format);
    }
    return *this;
}

client_logger::~client_logger() noexcept = default;

// client_logger::refcounted_stream::refcounted_stream(const std::string &path) :
//     _stream(std::make_pair(path, nullptr))
// {
//     auto it = _global_streams.find(path);
//     if (it == _global_streams.end())
//     {
//         _global_streams[path] = std::make_pair(1, std::ofstream(path));
//         _stream.second = &_global_streams[path].second;
//     }
//     else
//     {
//         ++it->second.first;
//         _stream.second = &it->second.second;
//     }
// }

client_logger::refcounted_stream::refcounted_stream(const std::string &path) :
    _stream(std::make_pair(path, nullptr))
{
    if (path.empty()) return;

    auto it = _global_streams.find(path);
    if (it == _global_streams.end())
    {
        std::ofstream new_stream(path, std::ios::app); // Открываем с append mode
        if (!new_stream.is_open())
        {
            throw std::runtime_error("Cannot open log file: " + path);
        }
        _global_streams[path] = std::make_pair(1, std::move(new_stream));
        _stream.second = &_global_streams[path].second;
    }
    else
    {
        ++it->second.first;
        _stream.second = &it->second.second;
    }
}

client_logger::refcounted_stream::refcounted_stream(const client_logger::refcounted_stream &oth) :
    _stream(oth._stream)
{
    if (!_stream.first.empty())
    {
        ++_global_streams[_stream.first].first;
    }
}

client_logger::refcounted_stream &
client_logger::refcounted_stream::operator=(const client_logger::refcounted_stream &oth)
{
    if (this != &oth)
    {
        this->~refcounted_stream();
        _stream = oth._stream;
        if (!_stream.first.empty())
        {
            ++_global_streams[_stream.first].first;
        }
    }
    return *this;
}

client_logger::refcounted_stream::refcounted_stream(client_logger::refcounted_stream &&oth) noexcept :
    _stream(std::move(oth._stream))
{
    oth._stream = std::make_pair("", nullptr);
}

client_logger::refcounted_stream &client_logger::refcounted_stream::operator=(client_logger::refcounted_stream &&oth) noexcept
{
    if (this != &oth)
    {
        this->~refcounted_stream();
        _stream = std::move(oth._stream);
        oth._stream = std::make_pair("", nullptr);
    }
    return *this;
}

client_logger::refcounted_stream::~refcounted_stream()
{
    if (!_stream.first.empty())
    {
        auto it = _global_streams.find(_stream.first);
        if (it != _global_streams.end() && --it->second.first == 0)
        {
            it->second.second.close();
            _global_streams.erase(it);
        }
    }
}

void client_logger::refcounted_stream::open()
{
    if (_stream.second == nullptr && !_stream.first.empty())
    {
        auto it = _global_streams.find(_stream.first);
        if (it != _global_streams.end())
        {
            _stream.second = &it->second.second;
        }
    }
}
