#define _BSD_SOURCE
#include <iostream>
#include <stdlib.h>
#include <sys/socket.h> // creating a socket
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>

//       arg count   arg vector
int main(int argc, char **argv)
{
    // we need 2 arguments, the filename and the port
    if (argc < 2)
    {
        std::cout << "[Error] Server needs two arguments. \n[Argument 1] Filename.cpp \n[Argument 2] PORT" << std::endl;
    }

    // atoi (included in standard library) converts string to int
    int port = atoi(argv[1]);

    // 1. create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    // 2. bind
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    // htnos host to network short
    address.sin_port = htons(port);
    int bind_value = bind(sock, (struct sockaddr *)&address, sizeof(address));

    // bind will return a negative value if unsuccessful
    if (bind_value < 0)
    {
        perror("Unable to bind");
        return 1;
    }

    // 3. listen
    int listen_value = listen(sock, 1);
    if (listen_value < 0)
    {
        perror("unable to listen");
        return 1;
    }

    // 4. accept
    struct sockaddr_in remote_address;
    memset(&address, 0, sizeof(address));
    socklen_t remote_addrlen = sizeof(address);
    std::cout << "Waiting for a new connection" << std::endl;
    int client_socket = accept(sock, (struct sockaddr *)&remote_address, &remote_addrlen);
    if (client_socket < 0)
    {
        perror("Could not accept.");
        return 1;
    }

    // ntoa change BINARY ADDRESS to character pointer
    std::string client_ip = inet_ntoa(remote_address.sin_addr);
    // ntohs is the INVERSE of ntons
    int remote_port = ntohs(remote_address.sin_port);
    std::cout << "Accepted new client @ " << client_ip << ":" << remote_port << std::endl;

    // buffer for the message being sent from the client
    int BUFFLEN = 1024;
    char buffer[BUFFLEN];

    while (1)
    {
        // always clear out buffer before receiving a new message
        memset(buffer, 0, BUFFLEN);

        // 5. receive
        int bytes_received = recv(client_socket, buffer, BUFFLEN - 1, 0);

        if (bytes_received < 0)
        {
            perror("Could not receive message from clients. 0 bytes detected.");

            // exit
            return 1;
        }
        if (bytes_received == 0)
        {
            std::cout << "client at " << client_ip << ":" << remote_port << " has disconnected." << std::endl;
            perror("Could not receive message from clients. 0 bytes detected.");

            // break out of while loop
            break;
        }

        if (buffer[bytes_received - 1] == '\n')
        {
            buffer[bytes_received - 1] = 0;
        }
        std::cout << "[Client Message] " << buffer << std::endl;
        // send
        std::string response = "[Client IP] " + client_ip +
                               "\n[Client Port] " + std::to_string(remote_port) +
                               "\n[Client Message] " + std::string(buffer) + "\n";

        int bytes_sent = send(client_socket, response.c_str(), response.length(), 0);
        if(bytes_sent < 0)
        {
            perror("Could not send message. Bytes sent are less than 0");
            return 1;
        }
    }

    // after client has disconnected
    std::cout << "Shutting down socket." << std::endl;
    shutdown(client_socket, SHUT_RDWR);

    // return 0;
}