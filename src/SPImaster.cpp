#include "SPImaster.h"

SPI::SPI(spi_host_device_t host, int mosi_pin, int miso_pin, int sclk_pin, int cs_pin)
    : spi_host(host), cs_pin(cs_pin) {
    spi_bus_config_t bus_cfg  ;
        bus_cfg.mosi_io_num = mosi_pin;
        bus_cfg.miso_io_num = miso_pin;
        bus_cfg.sclk_io_num = sclk_pin;
        bus_cfg.quadwp_io_num = -1;
        bus_cfg.quadhd_io_num = -1;
        bus_cfg.max_transfer_sz = 64;
    

    esp_err_t ret = spi_bus_initialize(spi_host, &bus_cfg, SPI_DMA_DISABLED);
    if (ret != ESP_OK) {
        throw std::runtime_error("Failed to initialize SPI bus");
    }
}

void SPI::attachDevice(uint32_t clock_speed_hz, int spi_mode) {
    spi_device_interface_config_t dev_cfg ;
    

        dev_cfg.clock_speed_hz = static_cast<int>(clock_speed_hz);
        dev_cfg.mode = static_cast<int>(spi_mode);
        dev_cfg.spics_io_num = cs_pin;
        dev_cfg.queue_size = 1;
    

    esp_err_t ret = spi_bus_add_device(spi_host, &dev_cfg, &spi_handle);
    if (ret != ESP_OK) {
        throw std::runtime_error("Failed to attach SPI device");
    }
}

void SPI::transfer(const uint8_t* tx_data, uint8_t* rx_data, size_t length) {
    spi_transaction_t transaction ;
    
        transaction.length = length * 8;
        transaction.tx_buffer = tx_data;
        transaction.rx_buffer = rx_data;
    

    esp_err_t ret = spi_device_transmit(spi_handle, &transaction);
    if (ret != ESP_OK) {
        throw std::runtime_error("SPI transfer failed");
    }
}

SPI::~SPI() {
    spi_bus_remove_device(spi_handle);
    spi_bus_free(spi_host);
}
