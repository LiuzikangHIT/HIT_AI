#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>  // 包含ioctl相关的头文件
#include <linux/if_packet.h>  // 包含struct sockaddr_ll的定义
#include <linux/if_ether.h>   // 包含struct ethhdr的定义
  
#define BUFFER_SIZE 65536
  
struct route_entry {
    uint32_t dest;
    uint32_t gateway;
    uint32_t netmask;
    char interface[IFNAMSIZ];
};

int route_table_size;
struct route_entry *route_table;
  
unsigned short checksum(unsigned short *buf, int nwords) {
    unsigned long sum;
    for (sum = 0; nwords > 0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

struct route_entry *lookup_route(uint32_t dest_ip) {
    for (int i = 0; i < route_table_size; i++) {
        if ((dest_ip & route_table[i].netmask) == (route_table[i].dest & route_table[i].netmask)) {
            return &route_table[i];
        }
    }
    return NULL;
}

int main() {
    // 初始化route_table  
    route_table = malloc(sizeof(struct route_entry) * 2);
    route_table[0].dest = inet_addr("192.168.1.2");
    route_table[0].gateway = inet_addr("192.168.1.1");
    route_table[0].netmask = inet_addr("255.255.255.0");
    strncpy(route_table[0].interface, "ens33", IFNAMSIZ - 1);
    route_table[1].dest = inet_addr("192.168.2.2");
    route_table[1].gateway = inet_addr("192.168.2.1");
    route_table[1].netmask = inet_addr("255.255.255.0");
    strncpy(route_table[1].interface, "ens37", IFNAMSIZ - 1);
    route_table_size = 2;

    int sockfd;
    struct sockaddr_ll saddr;
    unsigned char *buffer = (unsigned char *)malloc(BUFFER_SIZE);
    struct ethhdr *eth_header;  // 定义eth_header指针
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    while (1) {  
        socklen_t addrlen = sizeof(saddr);
        int data_size = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&saddr, &addrlen);  
        if (data_size < 0) {  
            perror("Recvfrom error");  
            return 1;  
        }

        // 获取MAC地址
        struct ethhdr *src_eth_header = (struct ethhdr *)buffer;
        
        // 获取IP地址
        struct iphdr *ip_header = (struct iphdr *)(buffer + sizeof(struct ethhdr));
        struct in_addr src_addr, dest_addr;
        char *src_ip = malloc(32 * sizeof(char)), *dest_ip = malloc(32 * sizeof(char));
        src_addr.s_addr = ip_header->saddr;
        dest_addr.s_addr = ip_header->daddr;
        strcpy(src_ip, inet_ntoa(src_addr));
        strcpy(dest_ip, inet_ntoa(dest_addr));
        // printf("Captured packet from %s to %s\n", src_ip, dest_ip);

        // 确保是UDP数据包
        uint16_t src_port = 0, dest_port = 0;
        if (ip_header->protocol == IPPROTO_UDP) {
            // 获取UDP头部
            struct udphdr *udp_header = (struct udphdr *)(buffer + sizeof(struct ethhdr) + (ip_header->ihl * 4));

            // 获取源端口号和目的端口号
            src_port = ntohs(udp_header->source);
            dest_port = ntohs(udp_header->dest);

            struct route_entry *route = lookup_route(ip_header->daddr);
            if (route == NULL) {
                fprintf(stderr, "No route to host\n");
                continue;
            }
    
            // 修改TTL
            ip_header->ttl -= 1;
            ip_header->check = 0;
            ip_header->check = checksum((unsigned short *)ip_header, ip_header->ihl * 4);
    
            // 发送数据包到目的主机
            struct ifreq ifr, ifr_mac;

            // 获取网卡接口索引
            memset(&ifr, 0, sizeof(ifr));  
            snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), route->interface);
            if (ioctl(sockfd, SIOCGIFINDEX, &ifr) < 0) {
                perror("ioctl");
                return 1;
            }
    
            // 获取网卡接口MAC 地址
            memset(&ifr_mac, 0, sizeof(ifr_mac));
            snprintf(ifr_mac.ifr_name, sizeof(ifr_mac.ifr_name), route->interface);
            if (ioctl(sockfd, SIOCGIFHWADDR, &ifr_mac) < 0) {
                perror("ioctl");
                return 1;
            }
            
            // 设置目标MAC 地址（假设目标地址已知,此处做了简化处理）
            unsigned char target_mac[ETH_ALEN];
            unsigned char dest_mac[ETH_ALEN] = {0x00, 0x0c, 0x29, 0x7c, 0x73, 0x65}; //替换为实际的目标MAC 地址
            unsigned char src_mac[ETH_ALEN] = {0x00, 0x0c, 0x29, 0x6f, 0xf5, 0x53}; //替换为实际的目标MAC 地址
            if (strcmp(dest_ip, "192.168.2.2") == 0) {
                for(int i = 0; i < ETH_ALEN; i++) {
                    target_mac[i] = dest_mac[i];
                }
            }
            else {
                for(int i = 0; i < ETH_ALEN; i++) {
                    target_mac[i] = src_mac[i];
                }
            }
            memset(&saddr, 0, sizeof(saddr));
            saddr.sll_ifindex = ifr.ifr_ifindex;
            saddr.sll_halen = ETH_ALEN;
            memcpy(saddr.sll_addr, target_mac, ETH_ALEN);
    
            // 构造新的以太网帧头
            eth_header = (struct ethhdr *)buffer;  // 设置eth_header指向buffer的起始位置
            memcpy(eth_header->h_dest, target_mac, ETH_ALEN); // 目标MAC 地址
            memcpy(eth_header->h_source, ifr_mac.ifr_hwaddr.sa_data, ETH_ALEN); // 源MAC 地址
            eth_header->h_proto = htons(ETH_P_IP); // 以太网类型为IP
            printf("Interface name: %s, index: %d\n", ifr.ifr_name, ifr.ifr_ifindex);
            
            // 获取 UDP 数据
            char *message = (char *)(buffer + sizeof(struct ethhdr) + (ip_header->ihl * 4) + sizeof(struct udphdr));
            size_t message_len = data_size - (sizeof(struct ethhdr) + (ip_header->ihl * 4) + sizeof(struct udphdr));

            // 输出信息
            printf("-----------------------------------------------------------\n");
            if (message_len > 0) {
                message[message_len] = '\0';  // Null-terminate the string
                printf("Received message: %s\n", message);
            }
            printf("源MAC地址：%02X:%02X:%02X:%02X:%02X:%02X，", src_eth_header->h_source[0], src_eth_header->h_source[1], src_eth_header->h_source[2], src_eth_header->h_source[3], src_eth_header->h_source[4], src_eth_header->h_source[5]);
            printf("目的MAC地址：%02X:%02X:%02X:%02X:%02X:%02X\n", eth_header->h_dest[0], eth_header->h_dest[1], eth_header->h_dest[2], eth_header->h_dest[3], eth_header->h_dest[4], eth_header->h_dest[5]);
            printf("源IP地址：%s，目的IP地址：%s\n", src_ip, dest_ip);
            printf("TTL：%d\n", ip_header->ttl);
            printf("源端口号：%d，目的端口号：%d\n", src_port, dest_port);
            printf("-----------------------------------------------------------\n");
            
            if (sendto(sockfd, buffer, data_size, 0, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
                perror("Sendto error");
                return 1;
            }

            sleep(5);
        }
    }
  
    close(sockfd);
    free(buffer);
    free(route_table);  // 释放route_table的内存
    return 0;
}