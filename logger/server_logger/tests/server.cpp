#include "server.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iomanip>

server::server(uint16_t port) : _running(true)
{
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd < 0)
    {
        throw std::runtime_error("Socket creation failed");
    }

    int opt = 1;
    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        throw std::runtime_error("Setsockopt failed");
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(_server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        throw std::runtime_error("Bind failed");
    }

    if (listen(_server_fd, 10) < 0)
    {
        throw std::runtime_error("Listen failed");
    }

    std::cout << "Server started on port " << port << std::endl;

    _worker_threads.emplace_back([this]() {
        while (_running)
        {
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            int client_socket = accept(_server_fd, (struct sockaddr*)&client_addr, &client_len);
            
            if (client_socket < 0)
            {
                if (_running)
                {
                    std::cerr << "Accept failed" << std::endl;
                }
                continue;
            }

            _worker_threads.emplace_back(&server::handle_client, this, client_socket);
        }
    });
}

server::~server() noexcept
{
    stop();
    if (_server_fd >= 0)
    {
        close(_server_fd);
    }
}

void server::stop()
{
    _running = false;
    shutdown(_server_fd, SHUT_RDWR);
    
    for (auto& thread : _worker_threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
    _worker_threads.clear();
}

void server::handle_client(int client_socket)
{
    char buffer[1024] = {0};
    while (_running)
    {
        ssize_t bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_read <= 0)
        {
            break;
        }

        buffer[bytes_read] = '\0';
        std::string message(buffer);
        
        size_t start = 0;
        size_t end = message.find('\n');
        while (end != std::string::npos)
        {
            std::string line = message.substr(start, end - start);
            process_message(line);
            start = end + 1;
            end = message.find('\n', start);
        }
    }
    close(client_socket);
}

void server::process_message(const std::string& message)
{
    size_t pipe_pos = message.find('|');
    if (pipe_pos == std::string::npos)
    {
        return;
    }

    int pid = std::stoi(message.substr(0, pipe_pos));
    std::string log_message = message.substr(pipe_pos + 1);

    std::shared_lock<std::shared_mutex> lock(_mut);
    auto it = _streams.find(pid);
    if (it != _streams.end())
    {
        for (auto& stream : it->second)
        {
            if (stream.second.second) // console
            {
                std::cout << log_message << std::endl;
            }
            else // file
            {
                std::ofstream file(stream.second.first, std::ios::app);
                if (file)
                {
                    file << log_message << std::endl;
                }
            }
        }
    }
    else
    {
        // Default behavior if no specific streams for this PID
        std::cout << log_message << std::endl;
    }
}