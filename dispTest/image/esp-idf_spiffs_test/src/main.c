#include "esp_spiffs.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "Filesystem";

void app_main() {
    vTaskDelay(1000);
    ESP_LOGI(TAG, "Initializing SPIFFS...");
    esp_vfs_spiffs_conf_t config = {
        .base_path = "/storage",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    esp_err_t result = esp_vfs_spiffs_register(&config);

    if (result != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize SPIFFS: %s", esp_err_to_name(result));
        return;
    }

    size_t total = 0, used = 0;
    result = esp_spiffs_info(config.partition_label, &total, &used);
    if (result != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get partition info: %s", esp_err_to_name(result));
    } else {
        ESP_LOGI(TAG, "Partition total bytes: %d, used bytes: %d", total, used);
    }

    FILE* f = fopen("/storage/nuggets.bmp", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return;
    }

    // Get file size
    fseek(f, 0, SEEK_END);        // move pointer to end
    long fileSize = ftell(f);     // get current position (file size in bytes)
    fseek(f, 0, SEEK_SET);        // rewind to start

    ESP_LOGI(TAG, "File opened successfully. Size: %d", (int)fileSize);
    fclose(f);
}