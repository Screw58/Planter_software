
#FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)
#set(includedirs ../../components/esp-aws-iot/port/include)
#set(includedirs1 ${IDF_PATH}/components/nvs_flash/include)
 idf_component_register(SRCS "err_handler.c"
                    INCLUDE_DIRS "."  
                    # PRIV_REQUIRES "nvs_flash" 
                    REQUIRES "Peripherals" "Sensors"
                    )
