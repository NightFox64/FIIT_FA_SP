#include "server.h"
#include <iostream>
#include <csignal>
#include <chrono>

std::atomic<bool> running(true);

void signal_handler(int signal)
{
    running = false;
}

int main(int argc, char* argv[])
{
    std::signal(SIGINT, signal_handler);
    std::signal(SIGTERM, signal_handler);

    try
    {
        server s;
        std::cout << "Server started. Press Ctrl+C to stop..." << std::endl;
        
        while (running)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
        std::cout << "Shutting down server..." << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}