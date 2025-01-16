#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h>
#include <netinet/ether.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define PORT 12345

int main()
{
    int sockfd, recv_len;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[1024];
    char *src_ip = malloc(32 * sizeof(char)), *dest_ip = malloc(32 * sizeof(char)); 
    time_t now_time;

    struct ethhdr *eth_header = (struct ethhdr *)buffer;
    struct iphdr *ip_header = (struct iphdr *)(buffer + sizeof(struct ethhdr));
    struct udphdr *udp_head = (struct udphdr *) (buffer + sizeof(struct ethhdr) + sizeof(struct iphdr));
    struct in_addr src_addr, dest_addr;

    // 创建UDP 套接字
        sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
        if (sockfd < 0) {
            perror("Socket creation failed");
            return 1;
        }

    // 获取网卡接口MAC 地址
    struct ifreq ifr_mac;
    memset(&ifr_mac, 0, sizeof(ifr_mac));
    snprintf(ifr_mac.ifr_name, sizeof(ifr_mac.ifr_name), "ens33");
    if (ioctl(sockfd, SIOCGIFHWADDR, &ifr_mac) < 0) {
        perror("ioctl");
        return 1;
    }

    unsigned char my_mac[ETH_ALEN] = {0x00, 0x0c, 0x29, 0x7c, 0x73, 0x65}; // MAC地址

    do {
        // 清空buffer
        memset(buffer, 0, sizeof(buffer));

        // 接收数据包
        recv_len = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&client_addr, &addr_len);
        if (recv_len < 0) {
            perror("Recvfrom failed");
            return 1;
        }

        src_addr.s_addr = ip_header->saddr;
        dest_addr.s_addr = ip_header->daddr;
        strcpy(src_ip, inet_ntoa(src_addr));
        strcpy(dest_ip, inet_ntoa(dest_addr));
        // if (eth_header->h_dest[5] != 0x65)
        //     continue; // 跳过不是发给自己的包
    } while(udp_head->dest != htons(PORT) || eth_header->h_dest[5] != 0x65);
    time(&now_time);

    // 获取 UDP 数据
    char *message = (char *)(buffer + sizeof(struct ethhdr) + (ip_header->ihl * 4) + sizeof(struct udphdr));
    size_t message_len = recv_len - (sizeof(struct ethhdr) + (ip_header->ihl * 4) + sizeof(struct udphdr));

    // 输出信息
    if (message_len > 0) {
        message[message_len] = '\0';  // Null-terminate the string
        printf("Received message: %s\n", message);
    }
    printf("接收时间：%s", ctime(&now_time));
    printf("源MAC地址：%02X:%02X:%02X:%02X:%02X:%02X，", eth_header->h_source[0], eth_header->h_source[1], eth_header->h_source[2], eth_header->h_source[3], eth_header->h_source[4], eth_header->h_source[5]);
    printf("目的MAC地址：%02X:%02X:%02X:%02X:%02X:%02X\n", eth_header->h_dest[0], eth_header->h_dest[1], eth_header->h_dest[2], eth_header->h_dest[3], eth_header->h_dest[4], eth_header->h_dest[5]);
    printf("源IP地址：%s，目的IP地址：%s\n", src_ip, dest_ip);
    printf("TTL：%d\n", ip_header->ttl);

    close(sockfd);
    return 0;
}