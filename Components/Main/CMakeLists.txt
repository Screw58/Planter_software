
#FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)
#set(includedirs ../../components/esp-aws-iot/port/include)
#set(includedirs1 ${IDF_PATH}/components/nvs_flash/include)
 idf_component_register(SRCS "main.c" "McuState.c"
                    INCLUDE_DIRS "." 
                    # PRIV_REQUIRES "nvs_flash" 
                    REQUIRES "Battery" "ErrHandler" "Network_custom" "Peripherals" "Sensors" "Signaling" "driver"
                    )
