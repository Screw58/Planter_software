# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)
list(APPEND EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../components/esp-aws-iot")
set(COMPONENT_REQUIRES "mbedtls" "nvs_flash" "esp_adc_cal" )
#set(EXTRA_COMPONENT_DIRS "../components/esp-aws-iot")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(Planter_scratch_4_4)
