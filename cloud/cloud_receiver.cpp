#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

int main()
{
    const int port = 8080;

    int serverFd = socket(AF_INET, SOCK_STREAM, 0);

    if (serverFd < 0) {
        cerr << "ERROR: Failed to create socket\n";
        return 1;
    }

    int reuse = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(port);

    if (bind(serverFd,
             reinterpret_cast<sockaddr*>(&serverAddress),
             sizeof(serverAddress)) < 0) {
        cerr << "ERROR: Failed to bind port " << port << "\n";
        close(serverFd);
        return 1;
    }

    if (listen(serverFd, 5) < 0) {
        cerr << "ERROR: Failed to listen\n";
        close(serverFd);
        return 1;
    }

    cout << "C++ Cloud Receiver\n";
    cout << "Listening on http://127.0.0.1:" << port << "\n";
    cout << "Waiting for meter data...\n";

    while (true) {
        sockaddr_in clientAddress{};
        socklen_t clientLength = sizeof(clientAddress);

        int clientFd = accept(
            serverFd,
            reinterpret_cast<sockaddr*>(&clientAddress),
            &clientLength);

        if (clientFd < 0) {
            cerr << "ERROR: Failed to accept connection\n";
            continue;
        }

        char buffer[8192] = {0};

        ssize_t bytesRead = read(
            clientFd,
            buffer,
            sizeof(buffer) - 1);

        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';

            string request(buffer);

            size_t bodyStart = request.find("\r\n\r\n");

            cout << "\n========== CLOUD RECEIVER ==========\n";

            if (bodyStart != string::npos) {
                string body = request.substr(bodyStart + 4);
                cout << "Received JSON:\n" << body << "\n";
            } else {
                cout << "Received request:\n" << request << "\n";
            }

            cout << "====================================\n";

            const string responseBody =
                "{\"status\":\"received\",\"message\":\"Meter data saved\"}";

            const string response =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: application/json\r\n"
                "Content-Length: " + to_string(responseBody.size()) + "\r\n"
                "Connection: close\r\n"
                "\r\n" +
                responseBody;

            write(clientFd, response.c_str(), response.size());
        }

        close(clientFd);
    }

    close(serverFd);
    return 0;
}
