#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <unordered_map>
#include <ctime>
#include <arpa/inet.h>

// --------------------
// Rate Limiter Config
// --------------------
struct RateData {
    int count = 0;
    time_t window_start = 0;
    time_t last_seen = 0;
};

std::unordered_map<std::string, RateData> rate_map;

const int LIMIT = 5;
const int WINDOW = 60;
const int CLEANUP_TTL = 300;
const int CLEANUP_INTERVAL = 30;

int request_counter = 0;

// --------------------
// Cleanup Logic
// --------------------
void cleanup_old_ips() {
    time_t now = time(NULL);

    for (auto it = rate_map.begin(); it != rate_map.end(); ) {
        if (now - it->second.last_seen > CLEANUP_TTL) {
            std::cout << "CLEANUP IP: " << it->first << "\n";
            it = rate_map.erase(it);
        } else {
            ++it;
        }
    }
}

// --------------------
// Main Server
// --------------------
int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed\n";
        return 1;
    }

    if (listen(server_fd, 10) < 0) {
        std::cerr << "Listen failed\n";
        return 1;
    }

    std::cout << "Rate Limiter running on port 8080\n";

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);

        int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            std::cerr << "Accept failed\n";
            continue;
        }

        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, ip_str, INET_ADDRSTRLEN);
        std::string client_ip(ip_str);

        time_t now = time(NULL);
        auto &data = rate_map[client_ip];

        data.last_seen = now;
        request_counter++;

        if (data.window_start == 0 || now - data.window_start > WINDOW) {
            data.count = 0;
            data.window_start = now;
        }

        data.count++;

        if (request_counter % CLEANUP_INTERVAL == 0) {
            cleanup_old_ips();
        }

        if (data.count > LIMIT) {
            std::cout << "BLOCKED IP: " << client_ip << "\n";

            const char* blocked =
                "HTTP/1.1 429 Too Many Requests\n"
                "Content-Type: text/plain\n\n"
                "Too many requests. Try later.\n";

            send(client_socket, blocked, strlen(blocked), 0);
            usleep(50000);
            close(client_socket);
            continue;
        }

        char buffer[1024] = {0};
        read(client_socket, buffer, 1024);

        std::cout << "ALLOWED IP: " << client_ip << "\n";

        const char* response =
            "HTTP/1.1 200 OK\n"
            "Content-Type: text/plain\n\n"
            "Request allowed\n";

        send(client_socket, response, strlen(response), 0);
        usleep(50000);
        close(client_socket);
    }

    close(server_fd);
    return 0;
}

