
#FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)
#set(includedirs ../../components/esp-aws-iot/port/include)
#set(includedirs1 ${IDF_PATH}/components/nvs_flash/include)
 idf_component_register(SRCS     "RGB_LED/led.c"
                    INCLUDE_DIRS "." "./RGB_LED" 
                    REQUIRES "esp_driver_ledc" "esp_driver_gpio" "esp_driver_gptimer" 
                    )
