#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define SRC_IP "192.168.1.2"
#define DEST_PORT 12345
#define SRC_PORT 54321
#define MESSAGE "Hello! Got your message loud and clear."

int main()
{
    int sockfd, sockfd_reply;
    struct sockaddr_in server_addr, client_addr, src_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[1024];

    // 创建UDP 套接字用于接收消息
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // 创建UDP 套接字用于发送消息
    sockfd_reply = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd_reply < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // 绑定套接字到端口
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(DEST_PORT);
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    // 接收数据包
    int recv_len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&client_addr, &addr_len);
    if (recv_len < 0) {
        perror("Recvfrom failed");
        return 1;
    }
    buffer[recv_len] = '\0';
    printf("Received message: %s\n", buffer);

    // 设置目的地址
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.sin_family = AF_INET;
    src_addr.sin_port = htons(SRC_PORT);
    inet_pton(AF_INET, SRC_IP, &src_addr.sin_addr);

    // 发送数据包
    if (sendto(sockfd_reply, MESSAGE, strlen(MESSAGE), 0, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0) {
        perror("Sendto failed");
        return 1;
    }
    printf("Message sent to %s:%d\n", SRC_IP, SRC_PORT);
    printf("目的端口：%d\n", SRC_PORT);

    close(sockfd);
    close(sockfd_reply);
    return 0;
}