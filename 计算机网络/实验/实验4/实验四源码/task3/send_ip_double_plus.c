#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/ether.h>
#include <unistd.h>

#define DEST_IP "192.168.2.2"
#define SRC_PORT 54321
#define DEST_PORT 12345
#define MESSAGE "Hello, this is a test message."

int main()
{
    int sockfd, sockfd_reply;
    struct sockaddr_in server_addr, client_addr, dest_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[1024];

    // 创建UDP 套接字用于发送消息
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // 创建UDP 套接字用于接收信息
    sockfd_reply = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd_reply < 0) {
        perror("Sockfd_reply creation failed");
        return 1;
    }

    // 设置源地址（绑定源端口）
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SRC_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sockfd_reply, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    // 设置目的地址
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(DEST_PORT);
    inet_pton(AF_INET, DEST_IP, &dest_addr.sin_addr);

    // 发送数据包
    if (sendto(sockfd, MESSAGE, strlen(MESSAGE), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        perror("Sendto failed");
        return 1;
    }
    printf("Message sent to %s:%d\n", DEST_IP, DEST_PORT);
    printf("目的端口：%d\n", DEST_PORT);

    // 接收数据包
    while(1) {
        int recv_len = recvfrom(sockfd_reply, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&client_addr, &addr_len);
        if (recv_len < 0) {
            perror("Recvfrom failed");
            return 1;
        }
        
        buffer[recv_len] = '\0';
        printf("Received message: %s\n", buffer);
        break;
    }
    
    close(sockfd);
    close(sockfd_reply);
    return 0;
}