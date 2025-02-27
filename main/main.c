#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "neopixel.h"

#define DELAY_S 10000
#define NEOPIXEL_PIN (CONFIG_NEOPIXEL_GPIO_PIN)
#define PIXEL_COUNT (CONFIG_PIXEL_COUNT)


static const char *TAG = "neopixel_test";
static tNeopixelContext neopixel;

void printChipInfo(void)
{
    /* Print chip information */
    esp_chip_info_t chip_info;
    uint32_t flash_size;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), %s%s%s%s, ",
           CONFIG_IDF_TARGET,
           chip_info.cores,
           (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "WiFi/" : "",
           (chip_info.features & CHIP_FEATURE_BT) ? "BT" : "",
           (chip_info.features & CHIP_FEATURE_BLE) ? "BLE" : "",
           (chip_info.features & CHIP_FEATURE_IEEE802154) ? ", 802.15.4 (Zigbee/Thread)" : "");

    unsigned major_rev = chip_info.revision / 100;
    unsigned minor_rev = chip_info.revision % 100;
    printf("silicon revision v%d.%d, ", major_rev, minor_rev);
    if(esp_flash_get_size(NULL, &flash_size) != ESP_OK) {
        printf("Get flash size failed");
        return;
    }

    printf("%" PRIu32 "MB %s flash\n", flash_size / (uint32_t)(1024 * 1024),
           (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());
}

#if !defined(MAX)
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

static bool test1(uint32_t iterations)
{
   tNeopixel pixel[] =
   {
       { 0, NP_RGB(50, 0,  0) }, /* red */
       { 0, NP_RGB(0,  50, 0) }, /* green */
       { 0, NP_RGB(0,  0, 50) }, /* blue */
       { 0, NP_RGB(0,  0,  0) }, /* off */
   };

   if(NULL == neopixel)
   {
      ESP_LOGE(TAG, "[%s] Initialization failed\n", __func__);
      return false;
   }

   ESP_LOGI(TAG, "[%s] Starting", __func__);
   for(int iter = 0; iter < iterations; ++iter)
   {
      for(int i = 0; i < PIXEL_COUNT; ++i)
      {
         neopixel_SetPixel(neopixel, &((tNeopixel){i, NP_RGB(50+iter, 0+iter, 0+iter)}), 1);
         vTaskDelay(pdMS_TO_TICKS(200));
      }
   }
   ESP_LOGI(TAG, "[%s] Finished", __func__);

   return true;
}

void app_main(void)
{
    neopixel = neopixel_Init(PIXEL_COUNT, NEOPIXEL_PIN);
    for (;1;) {
        /* printf("Hello world!\n"); */
        /* printChipInfo(); */
        /* printf("Again in %d seconds...\n", DELAY_S/10000); */
        /* vTaskDelay(DELAY_S / portTICK_PERIOD_MS); */
        /* fflush(stdout); */

        test1(5);
    }
    neopixel_Deinit(neopixel);
}
