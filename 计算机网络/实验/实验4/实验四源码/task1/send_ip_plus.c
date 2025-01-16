#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>


int main()
{
    int sockfd;
    struct sockaddr_in dest_addr;
    char message[1024];
    int port = 12345; // 目标端口号

    // 创建UDP 套接字
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("socket");
        return 1;
    }

    // 目标地址
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    dest_addr.sin_addr.s_addr = inet_addr("192.168.164.138");


    while(1) {
        fgets(message, sizeof(message), stdin);
        // 发送数据报
        if (sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
            perror("sendto");
            return 1;
        }
        printf("Datagram sent.\n");
        printf("目的端口：%d\n\n", port);
    }
    
    close(sockfd);
    return 0;
}