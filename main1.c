#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    const char *server_name = "localhost";
    const int server_port = 4445;
    const char* data_to_send = argv[1];
    struct sockaddr_in server_address;
    int sock;
    int len;
    char buffer[256];

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;

    inet_pton(AF_INET, server_name, &server_address.sin_addr);
    server_address.sin_port = htons(server_port);

    sock = socket(PF_INET, SOCK_DGRAM, 0);

    if(sock < 0)
    {
        perror("socket");

        return EXIT_FAILURE;
    }

    len = sendto(sock, data_to_send, strlen(data_to_send), 0,
                   (struct sockaddr*)&server_address, sizeof(server_address));

    recvfrom(sock, buffer, len, 0, NULL, NULL);

    buffer[len] = '\0';
    printf("'%s'\n", buffer);

    close(sock);

    return EXIT_SUCCESS;
}