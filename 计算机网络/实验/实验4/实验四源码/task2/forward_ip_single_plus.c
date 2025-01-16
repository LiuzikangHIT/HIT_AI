#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <netinet/ether.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <time.h>

#define BUFFER_SIZE 65536
#define TABLE_SIZE 256 // 转发表的大小

typedef struct {
    unsigned int src_ip;
    unsigned int dest_ip;
} IpPair;

unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;
    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

// 计算IP地址对的哈希值
unsigned int hash_ip_pair(unsigned int src_ip, unsigned int dest_ip) {
    return (src_ip ^ dest_ip) % TABLE_SIZE;
}

int main() {
    int sockfd;
    struct sockaddr saddr;
    unsigned char *buffer = (unsigned char *)malloc(BUFFER_SIZE);
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    IpPair routing_table[TABLE_SIZE] = {0}; // 初始化转发表

    // 添加路由规则
    routing_table[hash_ip_pair(inet_addr("192.168.164.236"), inet_addr("192.168.164.137"))] = (IpPair){inet_addr("192.168.164.236"), inet_addr("192.168.164.137")};

    while (1) {
        int saddr_len = sizeof(saddr);
        int data_size = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, &saddr, (socklen_t*)&saddr_len);
        if (data_size < 0) {
            perror("Recvfrom error");
            return 1;
        }
        struct ethhdr *eth_header = (struct ethhdr *)buffer;
        struct iphdr *ip_header = (struct iphdr *)(buffer + sizeof(struct ethhdr));
        char src_ip[INET_ADDRSTRLEN];
        char dest_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(ip_header->saddr), src_ip, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(ip_header->daddr), dest_ip, INET_ADDRSTRLEN);

        unsigned int src_ip_num = inet_addr(src_ip);
        unsigned int dest_ip_num = inet_addr(dest_ip);
        unsigned int hash = hash_ip_pair(src_ip_num, dest_ip_num);
        if (routing_table[hash].src_ip == src_ip_num && routing_table[hash].dest_ip == dest_ip_num) {
            // 获取当前系统时间
            time_t rawtime;
            struct tm *timeinfo;
            char time_str[100];
            time(&rawtime);
            timeinfo = localtime(&rawtime);

            // 格式化时间字符串
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);

            // 打印信息
            printf("[%s] Captured packet from %s to %s\n", time_str, src_ip, dest_ip);

            // 修改TTL
            ip_header->ttl -= 1;
            ip_header->check = 0;
            ip_header->check = checksum((unsigned short *)ip_header, ip_header->ihl * 4);

            // 发送数据包到目的主机
            struct ifreq ifr,ifr_mac;
            struct sockaddr_ll dest;

            // 获取网卡接口索引
            memset(&ifr, 0, sizeof(ifr));
            snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "ens33");
            if (ioctl(sockfd, SIOCGIFINDEX, &ifr) < 0) {
                perror("ioctl");
                return 1;
            }

            // 获取网卡接口MAC 地址
            memset(&ifr_mac, 0, sizeof(ifr_mac));
            snprintf(ifr_mac.ifr_name, sizeof(ifr_mac.ifr_name), "ens33");
            if (ioctl(sockfd, SIOCGIFHWADDR, &ifr_mac) < 0) {
                perror("ioctl");
                return 1;
            }

            // 设置目标MAC 地址（假设目标地址已知）
            unsigned char target_mac[ETH_ALEN] = {0x00, 0x0c, 0x29, 0x43, 0xa2, 0x9f}; // 替换为实际的目标MAC 地址
            memset(&dest, 0, sizeof(dest));
            dest.sll_ifindex = ifr.ifr_ifindex;
            dest.sll_halen = ETH_ALEN;
            memcpy(dest.sll_addr, target_mac, ETH_ALEN);

            // 构造新的以太网帧头
            memcpy(eth_header->h_dest, target_mac, ETH_ALEN); // 目标MAC 地址
            memcpy(eth_header->h_source, ifr_mac.ifr_hwaddr.sa_data, ETH_ALEN); // 源MAC地址
            eth_header->h_proto = htons(ETH_P_IP); // 以太网类型为IP
            printf("Interface name: %s, index: %d\n", ifr.ifr_name, ifr.ifr_ifindex);
            if (sendto(sockfd, buffer, data_size, 0, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
                perror("Sendto error");
                return 1;
            }
            printf("Datagram forwarded.\n");
            sleep(5);
        }
    }

    close(sockfd);
    free(buffer);
    return 0;
}