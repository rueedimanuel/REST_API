# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\REST_API_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\REST_API_autogen.dir\\ParseCache.txt"
  "REST_API_autogen"
  )
endif()
