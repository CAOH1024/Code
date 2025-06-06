#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include "esp_err.h"

#define BLINK_GPIO GPIO_NUM_2  // 你连接LED的GPIO口

uint16_t blink_flag = 0;  // 用于标记LED状态

static void IRAM_ATTR gptimer_alarm_callback(void)
{
    // 切换LED状态
    blink_flag = !blink_flag;
    gpio_set_level(BLINK_GPIO, blink_flag);
}

void app_main(void)
{
    // 配置 GPIO2 为输出模式
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BLINK_GPIO),  // 设置GPIO2
        .mode = GPIO_MODE_OUTPUT,              // 设置为输出模式
        .pull_up_en = GPIO_PULLUP_DISABLE,     // 禁用上拉
        .pull_down_en = GPIO_PULLDOWN_DISABLE, // 禁用下拉
        .intr_type = GPIO_INTR_DISABLE         // 禁用中断
    };
    gpio_config(&io_conf);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,  // 使用默认时钟源
        .resolution_hz = 1000,                // 设置分辨率为1kHz
        .direction = GPTIMER_COUNT_UP,        // 计数方向为向上计数
        .intr_priority = 1,
    };

    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

    gptimer_alarm_config_t alarm_config = {
        .alarm_count = 5000,  // 设置闹钟计数为500
        .reload_count = true,
        .flags.auto_reload_on_alarm = true
    };
    gptimer_set_alarm_action(gptimer, &alarm_config);

    gptimer_event_callbacks_t cbs = {
        .on_alarm = gptimer_alarm_callback  // 不需要处理闹钟事件
    };

    ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));
    ESP_ERROR_CHECK(gptimer_enable(gptimer));
    ESP_ERROR_CHECK(gptimer_start(gptimer));
    
    while (1) {

    }
}
 