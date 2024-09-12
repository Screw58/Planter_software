
#FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)
#set(includedirs ../../components/esp-aws-iot/port/include)
#set(includedirs1 ${IDF_PATH}/components/nvs_flash/include)
 idf_component_register(SRCS "WiFi/app_wifi.c"
                             "Mqtt/mqtt.c"
                    INCLUDE_DIRS "." "./Wifi" "./Mqtt" 
                    REQUIRES "mbedtls" "mqtt" "nvs_flash" "esp_wifi" "ErrHandler"
                    )
