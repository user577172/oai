#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Folly::folly" for configuration "Release"
set_property(TARGET Folly::folly APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Folly::folly PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libfolly.so"
  IMPORTED_SONAME_RELEASE "libfolly.so"
  )

list(APPEND _cmake_import_check_targets Folly::folly )
list(APPEND _cmake_import_check_files_for_Folly::folly "${_IMPORT_PREFIX}/lib/libfolly.so" )

# Import target "Folly::folly_test_util" for configuration "Release"
set_property(TARGET Folly::folly_test_util APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Folly::folly_test_util PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libfolly_test_util.so"
  IMPORTED_SONAME_RELEASE "libfolly_test_util.so"
  )

list(APPEND _cmake_import_check_targets Folly::folly_test_util )
list(APPEND _cmake_import_check_files_for_Folly::folly_test_util "${_IMPORT_PREFIX}/lib/libfolly_test_util.so" )

# Import target "Folly::follybenchmark" for configuration "Release"
set_property(TARGET Folly::follybenchmark APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Folly::follybenchmark PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libfollybenchmark.so"
  IMPORTED_SONAME_RELEASE "libfollybenchmark.so"
  )

list(APPEND _cmake_import_check_targets Folly::follybenchmark )
list(APPEND _cmake_import_check_files_for_Folly::follybenchmark "${_IMPORT_PREFIX}/lib/libfollybenchmark.so" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
