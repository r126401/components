/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_touch.h"
#include "esp_lcd_touch_gt911.h"

static const char *TAG = "GT911";

/* GT911 registers */
//#define GT911_POINT_INFO (0x814E)
//#define GT911_CONFIG_VERSION  (0x8047)
//#define GT911_PRODUCT_ID (0x8140)

/*******************************************************************************
* Function definitions
*******************************************************************************/
static esp_err_t esp_lcd_touch_gt911_read_data(esp_lcd_touch_handle_t tp);
static bool esp_lcd_touch_gt911_get_xy(esp_lcd_touch_handle_t tp, uint16_t *x, uint16_t *y, uint16_t *strength, uint8_t *point_num, uint8_t max_point_num);
static esp_err_t esp_lcd_touch_gt911_del(esp_lcd_touch_handle_t tp);

/* I2C read/write */
static esp_err_t touch_gt911_i2c_read(esp_lcd_touch_handle_t tp, uint16_t reg, uint8_t *data, uint8_t len);
static esp_err_t touch_gt911_i2c_write(esp_lcd_touch_handle_t tp, uint16_t reg, uint8_t data);

/* GT911 reset */
static esp_err_t touch_gt911_reset(esp_lcd_touch_handle_t tp);
/* Read status and config register */
static esp_err_t touch_gt911_read_cfg(esp_lcd_touch_handle_t tp);


static esp_err_t touch_gt911_i2c_write_block(esp_lcd_touch_handle_t tp, uint16_t reg, uint8_t *data, uint8_t len);

/*******************************************************************************
* Public API functions
*******************************************************************************/



uint8_t config_buff_gt911[GT911_CONFIG_SIZE];






uint8_t get_low_byte(uint16_t value) {

	uint8_t low = (uint8_t)(value & 0xFF);

	ESP_LOGI(TAG, "LOWBYTE=%02X", low);

	return low;

}


uint8_t get_high_byte(uint16_t value) {

	uint8_t high;
	high = (uint8_t)(value >> 8);

	ESP_LOGI(TAG, "HIGHBYTE=%02X", high);


	return high;

}






uint8_t touch_gt911_calculate_checksum() {

	uint16_t checksum_calc = 0;
	uint8_t checksum;

	for (uint8_t i=0; i<GT911_CONFIG_SIZE -1; i++) {
		checksum_calc += config_buff_gt911[i];
	  }

	checksum_calc = (~checksum_calc) + 1;
	checksum = get_low_byte(checksum_calc);
	config_buff_gt911[GT911_CONFIG_CHKSUM - GT911_CONFIG_START] = checksum;
	//ESP_LOGW(TAG, "touch_gt911_calculate_checksum: checksum is %02x", checksum);


	return checksum;

}



esp_err_t esp_lcd_touch_new_i2c_gt911(const esp_lcd_panel_io_handle_t io, const esp_lcd_touch_config_t *config, esp_lcd_touch_handle_t *out_touch)
{
    esp_err_t ret = ESP_OK;

    assert(io != NULL);
    assert(config != NULL);
    assert(out_touch != NULL);

    /* Prepare main structure */
    esp_lcd_touch_handle_t esp_lcd_touch_gt911 = heap_caps_calloc(1, sizeof(esp_lcd_touch_t), MALLOC_CAP_DEFAULT);
    ESP_GOTO_ON_FALSE(esp_lcd_touch_gt911, ESP_ERR_NO_MEM, err, TAG, "no mem for GT911 controller");

    /* Communication interface */
    esp_lcd_touch_gt911->io = io;

    /* Only supported callbacks are set */
    esp_lcd_touch_gt911->read_data = esp_lcd_touch_gt911_read_data;
    esp_lcd_touch_gt911->get_xy = esp_lcd_touch_gt911_get_xy;
    esp_lcd_touch_gt911->del = esp_lcd_touch_gt911_del;

    /* Mutex */
    esp_lcd_touch_gt911->data.lock.owner = portMUX_FREE_VAL;

    /* Save config */
    memcpy(&esp_lcd_touch_gt911->config, config, sizeof(esp_lcd_touch_config_t));

    /* Prepare pin for touch interrupt */
    if (esp_lcd_touch_gt911->config.int_gpio_num != GPIO_NUM_NC) {
        const gpio_config_t int_gpio_config = {
            .mode = GPIO_MODE_INPUT,
            .intr_type = GPIO_INTR_NEGEDGE,
            .pin_bit_mask = BIT64(esp_lcd_touch_gt911->config.int_gpio_num)
        };
        ret = gpio_config(&int_gpio_config);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "GPIO config failed");

        /* Register interrupt callback */
        if (esp_lcd_touch_gt911->config.interrupt_callback) {
            esp_lcd_touch_register_interrupt_callback(esp_lcd_touch_gt911, esp_lcd_touch_gt911->config.interrupt_callback);
        }
    }

    /* Prepare pin for touch controller reset */
    if (esp_lcd_touch_gt911->config.rst_gpio_num != GPIO_NUM_NC) {
        const gpio_config_t rst_gpio_config = {
            .mode = GPIO_MODE_OUTPUT,
            .pin_bit_mask = BIT64(esp_lcd_touch_gt911->config.rst_gpio_num)
        };
        ret = gpio_config(&rst_gpio_config);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "GPIO config failed");
    }

    /* Reset controller */
    ret = touch_gt911_reset(esp_lcd_touch_gt911);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "GT911 reset failed");

    /* Read config */
    ret = touch_gt911_i2c_read(esp_lcd_touch_gt911, GT911_CONFIG_START, config_buff_gt911, GT911_CONFIG_SIZE);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "GT911 read complete config failed");
    touch_gt911_print_config();

    touch_gt911_change_resolution(esp_lcd_touch_gt911, config->x_max, config->y_max);





    /* Read status and config info */
    ret = touch_gt911_read_cfg(esp_lcd_touch_gt911);
    ESP_GOTO_ON_ERROR(ret, err, TAG, "GT911 init failed");

err:
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error (0x%x)! Touch controller GT911 initialization failed!", ret);
        if (esp_lcd_touch_gt911) {
            esp_lcd_touch_gt911_del(esp_lcd_touch_gt911);
        }
    }

    *out_touch = esp_lcd_touch_gt911;

    return ret;
}

static esp_err_t esp_lcd_touch_gt911_read_data(esp_lcd_touch_handle_t tp)
{
    esp_err_t err;
    uint8_t buf[41];
    uint8_t touch_cnt = 0;
    uint8_t clear = 0;
    size_t i = 0;

    assert(tp != NULL);

    err = touch_gt911_i2c_read(tp, GT911_POINT_INFO, buf, 1);
    ESP_RETURN_ON_ERROR(err, TAG, "I2C read error!");

    /* Any touch data? */
    if ((buf[0] & 0x80) == 0x00) {
        touch_gt911_i2c_write(tp, GT911_POINT_INFO, clear);
    } else {
        /* Count of touched points */
        touch_cnt = buf[0] & 0x0f;
        if (touch_cnt > 5 || touch_cnt == 0) {
            touch_gt911_i2c_write(tp, GT911_POINT_INFO, clear);
            return ESP_OK;
        }

        /* Read all points */
        err = touch_gt911_i2c_read(tp, GT911_POINT_INFO + 1, &buf[1], touch_cnt * 8);
        ESP_RETURN_ON_ERROR(err, TAG, "I2C read error!");

        /* Clear all */
        err = touch_gt911_i2c_write(tp, GT911_POINT_INFO, clear);
        ESP_RETURN_ON_ERROR(err, TAG, "I2C read error!");

        portENTER_CRITICAL(&tp->data.lock);

        /* Number of touched points */
        touch_cnt = (touch_cnt > CONFIG_ESP_LCD_TOUCH_MAX_POINTS ? CONFIG_ESP_LCD_TOUCH_MAX_POINTS : touch_cnt);
        tp->data.points = touch_cnt;

        /* Fill all coordinates */
        for (i = 0; i < touch_cnt; i++) {
            tp->data.coords[i].x = ((uint16_t)buf[(i * 8) + 3] << 8) + buf[(i * 8) + 2];
            tp->data.coords[i].y = (((uint16_t)buf[(i * 8) + 5] << 8) + buf[(i * 8) + 4]);
            tp->data.coords[i].strength = (((uint16_t)buf[(i * 8) + 7] << 8) + buf[(i * 8) + 6]);
        }

        portEXIT_CRITICAL(&tp->data.lock);
    }

    return ESP_OK;
}

static bool esp_lcd_touch_gt911_get_xy(esp_lcd_touch_handle_t tp, uint16_t *x, uint16_t *y, uint16_t *strength, uint8_t *point_num, uint8_t max_point_num)
{
    assert(tp != NULL);
    assert(x != NULL);
    assert(y != NULL);
    assert(point_num != NULL);
    assert(max_point_num > 0);

    portENTER_CRITICAL(&tp->data.lock);

    /* Count of points */
    *point_num = (tp->data.points > max_point_num ? max_point_num : tp->data.points);

    for (size_t i = 0; i < *point_num; i++) {
        x[i] = tp->data.coords[i].x;
        y[i] = tp->data.coords[i].y;

        if (strength) {
            strength[i] = tp->data.coords[i].strength;
        }
    }

    /* Invalidate */
    tp->data.points = 0;

    portEXIT_CRITICAL(&tp->data.lock);

    return (*point_num > 0);
}

static esp_err_t esp_lcd_touch_gt911_del(esp_lcd_touch_handle_t tp)
{
    assert(tp != NULL);

    /* Reset GPIO pin settings */
    if (tp->config.int_gpio_num != GPIO_NUM_NC) {
        gpio_reset_pin(tp->config.int_gpio_num);
    }

    /* Reset GPIO pin settings */
    if (tp->config.rst_gpio_num != GPIO_NUM_NC) {
        gpio_reset_pin(tp->config.rst_gpio_num);
    }

    free(tp);

    return ESP_OK;
}

/*******************************************************************************
* Private API function
*******************************************************************************/

/* Reset controller */
static esp_err_t touch_gt911_reset(esp_lcd_touch_handle_t tp)
{
    assert(tp != NULL);

    if (tp->config.rst_gpio_num != GPIO_NUM_NC) {
        ESP_RETURN_ON_ERROR(gpio_set_level(tp->config.rst_gpio_num, tp->config.levels.reset), TAG, "GPIO set level error!");
        vTaskDelay(pdMS_TO_TICKS(10));
        ESP_RETURN_ON_ERROR(gpio_set_level(tp->config.rst_gpio_num, !tp->config.levels.reset), TAG, "GPIO set level error!");
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    return ESP_OK;
}

static esp_err_t touch_gt911_read_cfg(esp_lcd_touch_handle_t tp)
{
    uint8_t buf[4];

    assert(tp != NULL);

    ESP_RETURN_ON_ERROR(touch_gt911_i2c_read(tp, GT911_PRODUCT_ID, (uint8_t *)&buf[0], 3), TAG, "GT911 read error!");
    ESP_RETURN_ON_ERROR(touch_gt911_i2c_read(tp, GT911_CONFIG_VERSION, (uint8_t *)&buf[3], 1), TAG, "GT911 read error!");

    ESP_LOGI(TAG, "TouchPad_ID:0x%02x,0x%02x,0x%02x", buf[0], buf[1], buf[2]);
    ESP_LOGI(TAG, "TouchPad_Config_Version:%d", buf[3]);

    return ESP_OK;
}

static esp_err_t touch_gt911_i2c_read(esp_lcd_touch_handle_t tp, uint16_t reg, uint8_t *data, uint8_t len)
{
    assert(tp != NULL);
    assert(data != NULL);

    /* Read data */
    return esp_lcd_panel_io_rx_param(tp->io, reg, data, len);
}


static esp_err_t touch_gt911_i2c_write_block(esp_lcd_touch_handle_t tp, uint16_t reg, uint8_t *data, uint8_t len) {

	esp_err_t ret;

	ret = esp_lcd_panel_io_tx_param(tp->io, reg, data, len);

	return ret;

}



static esp_err_t touch_gt911_i2c_write(esp_lcd_touch_handle_t tp, uint16_t reg, uint8_t data)
{
    assert(tp != NULL);

    // *INDENT-OFF*
    /* Write data */
    return esp_lcd_panel_io_tx_param(tp->io, reg, (uint8_t[]){data}, 1);
    // *INDENT-ON*
}





esp_err_t esp_lcd_touch_gt911_confirm_config(esp_lcd_touch_handle_t tp) {

	esp_err_t ret;

	touch_gt911_calculate_checksum();

	ret = touch_gt911_i2c_write(tp, GT911_CONFIG_CHKSUM, config_buff_gt911[GT911_CONFIG_CHKSUM-GT911_CONFIG_START]);

	ESP_GOTO_ON_ERROR(ret, err, TAG, "Error al escribir la configuracion");


	ret = touch_gt911_i2c_write(tp, GT911_CONFIG_FRESH, 1);
	ESP_GOTO_ON_ERROR(ret, err, TAG, "Error al escribir GT911_CONFIG_FRESH");



	err:
	    if (ret != ESP_OK) {
	        ESP_LOGE(TAG, "Error to execute command GT911_CONFIG_FRESH");

	    }




	return ret;


}

esp_err_t touch_gt911_change_resolution(esp_lcd_touch_handle_t tp, uint16_t width, uint16_t height) {

	esp_err_t ret;

	config_buff_gt911[GT911_X_OUTPUT_MAX_LOW - GT911_CONFIG_START] = get_low_byte(width);
	config_buff_gt911[GT911_X_OUTPUT_MAX_HIGH - GT911_CONFIG_START] = get_high_byte(width);
	config_buff_gt911[GT911_Y_OUTPUT_MAX_LOW - GT911_CONFIG_START] = get_low_byte(height);
	config_buff_gt911[GT911_Y_OUTPUT_MAX_HIGH - GT911_CONFIG_START] = get_high_byte(height);

	ret = touch_gt911_i2c_write_block(tp, GT911_CONFIG_START, config_buff_gt911, GT911_CONFIG_SIZE);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "Error to write resolution on device. Error: %d", ret);
	} else {
		esp_lcd_touch_gt911_confirm_config(tp);
	}

	return ret;



	return ESP_OK;
}

esp_err_t touch_gt911_print_config() {

	  int i;
	  char cadena[2 * GT911_CONFIG_SIZE + 1];
	  for (i=0;i< GT911_CONFIG_SIZE;i++) {

		  sprintf(&cadena[2 * i], "%02x", config_buff_gt911[i]);

	  }

	  ESP_LOGW(TAG, "configuracion actual: %s", cadena);

/*
	  for (i=0;i<GT911_CONFIG_SIZE;i++) {
		  printf("%02x:%02x\n", 0x8047+i, config_buff_gt911[i] );
	  }
*/
	  return ESP_OK;

}

esp_err_t touch_gt911_get_config(esp_lcd_touch_handle_t tp) {

	esp_err_t ret = ESP_OK;


	/* Read config */
	    ret = touch_gt911_i2c_read(tp, GT911_CONFIG_START, config_buff_gt911, GT911_CONFIG_SIZE);
	    ESP_GOTO_ON_ERROR(ret, err, TAG, "GT911 read complete config failed");


	        err:
	            if (ret != ESP_OK) {
	                ESP_LOGE(TAG, "Error (0x%x)! Touch controller GT911 initialization failed!", ret);
	            }

	   return ret;
}





