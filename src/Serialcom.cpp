#include "Serialcom.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdexcept>
#include <vector>

UARTMaster::UARTMaster(uart_port_t uart_num, int tx_pin, int rx_pin, int baud_rate, int buffer_size)
    : uart_num(uart_num), tx_pin(tx_pin), rx_pin(rx_pin), baud_rate(baud_rate), buffer_size(buffer_size) {}

void UARTMaster::begin() {
    // Configure UART parameters
    uart_config_t uart_config ;
    
        uart_config.baud_rate = baud_rate;
        uart_config.data_bits = UART_DATA_8_BITS;
        uart_config.parity = UART_PARITY_DISABLE;
        uart_config.stop_bits = UART_STOP_BITS_1;
        uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
        uart_config.rx_flow_ctrl_thresh = 0;
        uart_config.source_clk = UART_SCLK_APB;
  

    // Install UART driver and configure the UART
    esp_err_t ret = uart_driver_install(uart_num, buffer_size, buffer_size, 0, NULL, 0);
    if (ret != ESP_OK) {
        throw std::runtime_error("Failed to install UART driver");
    }

    ret = uart_param_config(uart_num, &uart_config);
    if (ret != ESP_OK) {
        throw std::runtime_error("Failed to configure UART parameters");
    }

    ret = uart_set_pin(uart_num, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (ret != ESP_OK) {
        throw std::runtime_error("Failed to set UART pins");
    }
}

void UARTMaster::send(const std::string& data) {
    uart_write_bytes(uart_num, data.c_str(), data.length());
}

std::string UARTMaster::receive(int max_length) {
    std::vector<char> buffer(max_length);  // Temporary buffer
    int length = uart_read_bytes(uart_num, (uint8_t*)buffer.data(), max_length, pdMS_TO_TICKS(100));  // Blocking read

    if (length > 0) {
        // Successfully received data
        return std::string(buffer.begin(), buffer.begin() + length);
    } else if (length < 0) {
        // Handle UART read error
        throw std::runtime_error("UART read error");
    }

    // No data received
    return "";
}

UARTMaster::~UARTMaster() {
    uart_driver_delete(uart_num);  // Clean up UART resources
}
