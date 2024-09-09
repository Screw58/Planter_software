
#FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)
#set(includedirs ../../components/esp-aws-iot/port/include)
#set(includedirs1 ${IDF_PATH}/components/nvs_flash/include)
 idf_component_register(SRCS "main.c"
                             "app_wifi.c"
                             "err_handler.c"
                             "sensors.c"
                             "adc.c"
                             "i2c.c"
                             "mqtt.c"
                             "battery.c"
                             "led.c"
                    INCLUDE_DIRS "." "../include" 
                    PRIV_REQUIRES "nvs_flash" 
                    REQUIRES "mbedtls" "mqtt" "esp_adc" "esp_driver_i2c" "esp_wifi" "esp_driver_ledc" "esp_driver_gpio" "esp_driver_gptimer"
                    )
