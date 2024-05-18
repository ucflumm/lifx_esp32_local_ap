# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/esp_idf/v5.2/esp-idf/components/bootloader/subproject"
  "C:/git/idf_projects/lifx_esp32_local_ap/build/bootloader"
  "C:/git/idf_projects/lifx_esp32_local_ap/build/bootloader-prefix"
  "C:/git/idf_projects/lifx_esp32_local_ap/build/bootloader-prefix/tmp"
  "C:/git/idf_projects/lifx_esp32_local_ap/build/bootloader-prefix/src/bootloader-stamp"
  "C:/git/idf_projects/lifx_esp32_local_ap/build/bootloader-prefix/src"
  "C:/git/idf_projects/lifx_esp32_local_ap/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/git/idf_projects/lifx_esp32_local_ap/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/git/idf_projects/lifx_esp32_local_ap/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
