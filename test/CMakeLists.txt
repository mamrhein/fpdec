# Michael Amrhein. Copyright (C) 2020.

file(GLOB_RECURSE TEST_SRC *.cpp *.cxx *.cc *.C *.c *.h *.hpp)
set(TEST_BIN ${PROJECT_NAME}_test)
set(TEST_LIBS ${PROJECT_NAME} ${PROJECT_NAME}++)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

# configure the executable
link_directories(${MAINFOLDER}/lib)
include_directories("${CATCH_DIR}/include")
include_directories("${MAINFOLDER}/include")
include_directories("${MAINFOLDER}/src/libfpdec")
add_executable(${TEST_BIN} ${TEST_SRC})
target_link_libraries(${TEST_BIN} ${TEST_LIBS})

# configure unit tests via CTest
add_test(NAME AllTests COMMAND ${TEST_BIN})

# run catch tests directly
add_custom_target(catch "${MAINFOLDER}/bin/${TEST_BIN}" DEPENDS ${TEST_BIN} COMMENT "Executing unit tests..." VERBATIM SOURCES ${TEST_SRC})
