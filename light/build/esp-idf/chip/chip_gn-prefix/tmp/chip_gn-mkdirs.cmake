# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/antonio/Documents/Projects/matter_ws/esp/esp-matter/connectedhomeip/connectedhomeip"
  "/home/antonio/Documents/Projects/matter_ws/light/build/esp-idf/chip"
  "/home/antonio/Documents/Projects/matter_ws/light/build/esp-idf/chip/chip_gn-prefix"
  "/home/antonio/Documents/Projects/matter_ws/light/build/esp-idf/chip/chip_gn-prefix/tmp"
  "/home/antonio/Documents/Projects/matter_ws/light/build/esp-idf/chip/chip_gn-prefix/src/chip_gn-stamp"
  "/home/antonio/Documents/Projects/matter_ws/light/build/esp-idf/chip/chip_gn-prefix/src"
  "/home/antonio/Documents/Projects/matter_ws/light/build/esp-idf/chip/chip_gn-prefix/src/chip_gn-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/antonio/Documents/Projects/matter_ws/light/build/esp-idf/chip/chip_gn-prefix/src/chip_gn-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/antonio/Documents/Projects/matter_ws/light/build/esp-idf/chip/chip_gn-prefix/src/chip_gn-stamp${cfgdir}") # cfgdir has leading slash
endif()
