
#FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)
#set(includedirs ../../components/esp-aws-iot/port/include)
#set(includedirs1 ${IDF_PATH}/components/nvs_flash/include)
 idf_component_register(SRCS "Adc/adc.c"
                             "I2C/i2c.c"
                             "Gpio/gpio_user.c"
                             "Timers/GPTimers.c"
                    INCLUDE_DIRS "." "./I2C" "./Adc" "./Gpio" "./Timers"
                    REQUIRES "esp_adc" "esp_driver_i2c" "esp_driver_gpio" "driver"
                    )
