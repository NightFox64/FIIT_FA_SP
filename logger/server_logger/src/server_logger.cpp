#include "../include/server_logger.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

server_logger::server_logger(
    const std::string& dest,
    const std::unordered_map<logger::severity, std::pair<std::string, bool>>& streams,
    const std::string& format)
    : _destination(dest), _streams(streams), _format(format), _socket_fd(-1)
{
    if (!connect_to_server())
    {
        throw std::runtime_error("Failed to connect to server");
    }
}

bool server_logger::connect_to_server()
{
    size_t colon_pos = _destination.find(':');
    if (colon_pos == std::string::npos)
    {
        return false;
    }

    std::string host = _destination.substr(0, colon_pos);
    int port = std::stoi(_destination.substr(colon_pos + 1));

    _socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket_fd < 0)
    {
        return false;
    }

    memset(&_server_addr, 0, sizeof(_server_addr));
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &_server_addr.sin_addr) <= 0)
    {
        close(_socket_fd);
        return false;
    }

    if (connect(_socket_fd, (struct sockaddr*)&_server_addr, sizeof(_server_addr)) < 0)
    {
        close(_socket_fd);
        return false;
    }

    return true;
}

void server_logger::send_to_server(const std::string& message)
{
    std::lock_guard<std::mutex> lock(_log_mutex);
    if (_socket_fd < 0)
    {
        if (!connect_to_server())
        {
            return;
        }
    }

    std::string msg = message + "\n";
    send(_socket_fd, msg.c_str(), msg.size(), 0);
}

int server_logger::inner_getpid()
{
#ifdef _WIN32
    return ::_getpid();
#else
    return getpid();
#endif
}

server_logger::~server_logger() noexcept
{
    if (_socket_fd >= 0)
    {
        close(_socket_fd);
    }
}

server_logger::server_logger(server_logger&& other) noexcept
    : _destination(std::move(other._destination)),
      _streams(std::move(other._streams)),
      _format(std::move(other._format)),
      _socket_fd(other._socket_fd),
      _server_addr(other._server_addr)
{
    other._socket_fd = -1;
}

server_logger& server_logger::operator=(server_logger&& other) noexcept
{
    if (this != &other)
    {
        if (_socket_fd >= 0)
        {
            close(_socket_fd);
        }

        _destination = std::move(other._destination);
        _streams = std::move(other._streams);
        _format = std::move(other._format);
        _socket_fd = other._socket_fd;
        _server_addr = other._server_addr;

        other._socket_fd = -1;
    }
    return *this;
}

logger& server_logger::log(
    const std::string &message,
    logger::severity severity) &
{
    auto it = _streams.find(severity);
    if (it == _streams.end())
    {
        return *this;
    }

    std::string formatted_message;
    for (size_t i = 0; i < _format.size(); ++i)
    {
        if (_format[i] == '%' && i + 1 < _format.size())
        {
            switch (_format[i + 1])
            {
                case 'd':
                    formatted_message += logger::current_date_to_string();
                    break;
                case 't':
                    formatted_message += logger::current_time_to_string();
                    break;
                case 's':
                    formatted_message += logger::severity_to_string(severity);
                    break;
                case 'm':
                    formatted_message += message;
                    break;
                default:
                    formatted_message += _format[i + 1];
            }
            ++i;
        }
        else
        {
            formatted_message += _format[i];
        }
    }

    std::ostringstream full_message;
    full_message << inner_getpid() << "|" << logger::severity_to_string(severity) << "|" << formatted_message << "\n";

    std::lock_guard<std::mutex> lock(_log_mutex);
    if (_socket_fd < 0)
    {
        if (!connect_to_server())
        {
            std::cerr << "Failed to send log: " << formatted_message << std::endl;
            return *this;
        }
    }

    ssize_t sent = send(_socket_fd, full_message.str().c_str(), full_message.str().size(), 0);
    if (sent < 0)
    {
        close(_socket_fd);
        _socket_fd = -1;
        
        if (!connect_to_server())
        {
            std::cerr << "Failed to resend log: " << formatted_message << std::endl;
        }
        else
        {
            send(_socket_fd, full_message.str().c_str(), full_message.str().size(), 0);
        }
    }

    return *this;
}