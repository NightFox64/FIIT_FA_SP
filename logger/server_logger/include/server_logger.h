#ifndef MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H
#define MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H

#include <logger.h>
#include <unordered_map>
#include <string>
#include <memory>
#include <mutex>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

class server_logger_builder;
class server_logger final:
    public logger
{
private:
    std::string _destination;
    std::unordered_map<logger::severity, std::pair<std::string, bool>> _streams;
    std::string _format;
    int _socket_fd;
    struct sockaddr_in _server_addr;
    std::mutex _log_mutex;

    server_logger(const std::string& dest, 
                 const std::unordered_map<logger::severity, std::pair<std::string, bool>>& streams,
                 const std::string& format);

    bool connect_to_server();
    void send_to_server(const std::string& message);

    friend server_logger_builder;

    static int inner_getpid();

public:
    server_logger(server_logger const &other) = delete;
    server_logger &operator=(server_logger const &other) = delete;
    server_logger(server_logger &&other) noexcept;
    server_logger &operator=(server_logger &&other) noexcept;
    ~server_logger() noexcept final;

public:
    [[nodiscard]] logger& log(
        const std::string &message,
        logger::severity severity) & override;
};

#endif //MATH_PRACTICE_AND_OPERATING_SYSTEMS_SERVER_LOGGER_H