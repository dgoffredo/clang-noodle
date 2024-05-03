# This CMakeLists.txt is based on [include-what-you-use][1].
#
# On Debian/Ubuntu, it has the following (dpkg) dependencies:
#
# - clang
# - libclang-dev
# - llvm-dev
#
# [1]: https://github.com/include-what-you-use/include-what-you-use/blob/93d8793081e5aa457114ef99d1e887390b370e4a/CMakeLists.txt

cmake_minimum_required(VERSION 3.20.0)

# TODO: Might need to put these back if CMake warns (that's what they're for, I think).
#
# cmake_policy(SET CMP0048 NEW)
# if (POLICY CMP0077)
#   cmake_policy(SET CMP0077 NEW)
# endif()

project(clang-noodle)

find_package(LLVM CONFIG REQUIRED)
find_package(Clang CONFIG REQUIRED)

list(APPEND CMAKE_MODULE_PATH ${LLVM_DIR})
include(AddLLVM)
include(HandleLLVMOptions)

set(clang_noodle_llvm_dir ${LLVM_DIR})
set(clang_noodle_include_dirs
  ${LLVM_INCLUDE_DIRS}
  ${CLANG_INCLUDE_DIRS}
)

message(STATUS
  "configuring for LLVM ${LLVM_VERSION} from ${clang_noodle_llvm_dir}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Synthesize clang-resource-headers target if necessary.
if (NOT TARGET clang-resource-headers)
  # Use only LLVM_VERSION_MAJOR for the resource directory structure; some
  # platforms include suffix in LLVM_VERSION.
  set(clang_headers_src "${LLVM_LIBRARY_DIR}/clang/${LLVM_VERSION_MAJOR}/include")
  set(clang_headers_dst "${CMAKE_BINARY_DIR}/lib/clang/${LLVM_VERSION_MAJOR}/include")

  file(GLOB_RECURSE in_files RELATIVE "${clang_headers_src}"
    "${clang_headers_src}/*"
  )

  set(out_files)
  foreach (file ${in_files})
    set(src "${clang_headers_src}/${file}")
    set(dst "${clang_headers_dst}/${file}")

    add_custom_command(OUTPUT "${dst}"
      DEPENDS "${src}"
      COMMAND ${CMAKE_COMMAND} -E copy_if_different "${src}" "${dst}"
      COMMENT "Copying ${src}..."
    )
    list(APPEND out_files "${dst}")
  endforeach()

  add_custom_target(clang-resource-headers ALL DEPENDS ${out_files})
endif()

set(LLVM_LINK_COMPONENTS
  Option
  Support
  AllTargetsAsmParsers
  AllTargetsDescs
  AllTargetsInfos
)

add_llvm_executable(clang-noodle
  LoopConvert.cpp
)

# Add a dependency on clang-resource-headers to ensure the builtin headers are
# available when IWYU is executed from the build dir.
add_dependencies(clang-noodle clang-resource-headers)

# LLVM requires C++17, so follow suit.
set_target_properties(clang-noodle PROPERTIES
  CXX_STANDARD_REQUIRED ON
  CXX_STANDARD 17
  CXX_EXTENSIONS OFF
)

separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})

target_compile_definitions(clang-noodle PRIVATE
  ${LLVM_DEFINITIONS_LIST}
)
target_include_directories(clang-noodle PRIVATE
  ${clang_noodle_include_dirs}
)

# Link statically 
target_link_libraries(clang-noodle PRIVATE
  clangAST
  clangASTMatchers
  clangBasic
  clangFrontend
  clangSerialization
  clangTooling
  # TODO: The libraries above are from the LibTooling tutorial.
  # The libraries below are from the original include-what-you-use CMakeLists.txt
  #
  # clangBasic
  # clangLex
  # clangAST
  # clangSema
  # clangFrontend
  # clangFrontendTool
  # clangDriver

  # Revision [1] in clang moved PCHContainerOperations from Frontend
  # to Serialization, but this broke builds that set
  # -DBUILD_SHARED_LIBS=on.  Revision [2] is a followup that works
  # around the issue by adding an explicit dependency on Serialization
  # wherever there was a dependency on Frontend.  Since we depend on
  # Frontend, we need an explicit dependency on Serialization too.
  # [1] https://llvm.org/viewvc/llvm-project?view=revision&revision=348907
  # [2] https://llvm.org/viewvc/llvm-project?view=revision&revision=348915
  # clangSerialization

  # clangToolingInclusionsStdlib
)

# Old code...
# 
# set(LLVM_LINK_COMPONENTS support)
# 
# add_clang_executable(loop-convert
#   LoopConvert.cpp
#   )
# target_link_libraries(loop-convert
#   PRIVATE
#   clangAST
#   clangASTMatchers
#   clangBasic
#   clangFrontend
#   clangSerialization
#   clangTooling
#   )
