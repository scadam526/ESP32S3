# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/Users/Shawn/.platformio/packages/framework-espidf@3.40407.240606/components/bootloader/subproject")
  file(MAKE_DIRECTORY "C:/Users/Shawn/.platformio/packages/framework-espidf@3.40407.240606/components/bootloader/subproject")
endif()
file(MAKE_DIRECTORY
  "C:/project/ESP32S3/dispTest/image/espidf_arduino_test/.pio/build/adafruit_feather_esp32s3_reversetft/bootloader"
  "C:/project/ESP32S3/dispTest/image/espidf_arduino_test/.pio/build/adafruit_feather_esp32s3_reversetft/bootloader-prefix"
  "C:/project/ESP32S3/dispTest/image/espidf_arduino_test/.pio/build/adafruit_feather_esp32s3_reversetft/bootloader-prefix/tmp"
  "C:/project/ESP32S3/dispTest/image/espidf_arduino_test/.pio/build/adafruit_feather_esp32s3_reversetft/bootloader-prefix/src/bootloader-stamp"
  "C:/project/ESP32S3/dispTest/image/espidf_arduino_test/.pio/build/adafruit_feather_esp32s3_reversetft/bootloader-prefix/src"
  "C:/project/ESP32S3/dispTest/image/espidf_arduino_test/.pio/build/adafruit_feather_esp32s3_reversetft/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/project/ESP32S3/dispTest/image/espidf_arduino_test/.pio/build/adafruit_feather_esp32s3_reversetft/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/project/ESP32S3/dispTest/image/espidf_arduino_test/.pio/build/adafruit_feather_esp32s3_reversetft/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
