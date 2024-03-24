
#FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)
#set(includedirs ../../components/esp-aws-iot/port/include)
#set(includedirs1 ${IDF_PATH}/components/nvs_flash/include)
 idf_component_register(SRCS "main.c"
                             "app_wifi.c"
                             "aws.c"
                             "err_handler.c"
                             "sensors.c"
                    INCLUDE_DIRS "." "../include" 
                    PRIV_REQUIRES "nvs_flash" "esp_adc_cal" "esp-aws-iot"
                    REQUIRES "mbedtls" 
                    )

#idf_component_register(SRCS ${app_sources})

target_add_binary_data(${COMPONENT_TARGET} "../../tmp/AmazonRootCA1.pem" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "../../tmp/Plant_sensor01.private.key" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "../../tmp/Plant_sensor01.cert.pem" TEXT)