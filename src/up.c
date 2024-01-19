#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

const char* domains[] = { "google.com", "rape.im", "serpent.tools", "balls.com", "cia.contact" };

bool getConnectionStatus(const char* domain) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "Failed to initialize Winsock\n");
        return false;
    }

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int res = getaddrinfo(domain, "http", &hints, &result);
    WSACleanup();

    if (res != 0) {
        fprintf(stderr, "getaddrinfo failed: %d\n", res);
        return false;
    }

    freeaddrinfo(result);
    return true;
}

int main() {
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "Failed to initialize Winsock\n");
        return 1;
    }

    bool status;

    for (int i = 0; i < sizeof(domains) / sizeof(domains[0]); ++i) {
        status = getConnectionStatus(domains[i]);
        printf("[Up] %s, %s\n", domains[i], status ? "Connected" : "Not Connected");
    }

    WSACleanup();
    return 0;
}
