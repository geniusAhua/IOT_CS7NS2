# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)
set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/components"
                         "${CMAKE_CURRENT_LIST_DIR}/components/lib/"
                         "$ENV{IDF_PATH}/components/esp-aws-iot/libraries/backoffAlgorithm"
						 "$ENV{IDF_PATH}/components/esp-aws-iot/libraries/coreMQTT"
						 "$ENV{IDF_PATH}/components/esp-aws-iot/libraries/common/posix_compat")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(hardware)

target_add_binary_data(${CMAKE_PROJECT_NAME}.elf "./cert/AmazonRootCA1.pem" TEXT)
target_add_binary_data(${CMAKE_PROJECT_NAME}.elf "./cert/Client.crt" TEXT)
target_add_binary_data(${CMAKE_PROJECT_NAME}.elf "./cert/Client.key" TEXT)
