#ifndef MP_OS_SERVER_H
#define MP_OS_SERVER_H

#include <unordered_map>
#include <logger.h>
#include <shared_mutex>
#include <string>
#include <thread>
#include <vector>
#include <atomic>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

class server
{
private:
    std::unordered_map<int, std::unordered_map<logger::severity, std::pair<std::string, bool>>> _streams;
    std::shared_mutex _mut;
    int _server_fd;
    std::atomic<bool> _running;
    std::vector<std::thread> _worker_threads;

    void handle_client(int client_socket);
    void process_message(const std::string& message);

public:
    explicit server(uint16_t port = 9200);
    ~server() noexcept;

    server(const server&) = delete;
    server& operator=(const server&) = delete;
    server(server&&) noexcept = delete;
    server& operator=(server&&) noexcept = delete;

    void stop();
};

#endif //MP_OS_SERVER_H