#include <stdio.h>
#include <string.h>
#include "stm32h7xx.h"
#include "config.h"
#include "eth_driver.h"

static const char *TAG = "ETH";

// 简化的以太网驱动（生产环境需要完整的 HAL）
volatile uint32_t eth_interrupt_flag = 0;
volatile uint32_t eth_link_status = 0;

void eth_init(void) {
    // 启用以太网外设时钟（在 stm32h7_init.c 中完成）
    
    // 配置 MAC 地址
    uint8_t mac_addr[6] = ETH_MAC_ADDR;
    
    // 设置低字节
    ETH->MACA0LR = (mac_addr[3] << 24) | (mac_addr[2] << 16) | 
                   (mac_addr[1] << 8) | mac_addr[0];
    
    // 设置高字节
    ETH->MACA0HR = (mac_addr[5] << 8) | mac_addr[4];
    
    printf("[%s] Ethernet initialized\n", TAG);
    printf("[%s] MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n", TAG,
           mac_addr[0], mac_addr[1], mac_addr[2], 
           mac_addr[3], mac_addr[4], mac_addr[5]);
    printf("[%s] IP Address: %s\n", TAG, IP_ADDRESS);
}

void eth_send_packet(const uint8_t *data, uint16_t length) {
    // 生产环境需要正确的 DMA 配置和传输描述符链
    (void)data;  // 避免未使用警告
    (void)length;
}

void eth_receive_packet(uint8_t *buffer, uint16_t max_length) {
    // 生产环境需要实现 DMA 接收
    (void)buffer;
    (void)max_length;
}

int eth_link_is_up(void) {
    return eth_link_status;
}

void eth_set_link_status(int up) {
    eth_link_status = up;
    printf("[%s] Link status: %s\n", TAG, up ? "UP" : "DOWN");
}

