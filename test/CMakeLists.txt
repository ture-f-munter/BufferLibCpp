enable_testing()

set(SRCS
	Test_ArrayBuffer.cpp
)

set(HDRS
	helpers.h
)

set(BUFFERLIB_SRCS
	array_buffer.h
	bufferlib_common.h
)

set(EXTRA_FILES
	BufferLib.natvis
	CMakeLists.txt
)

list(TRANSFORM SRCS PREPEND "${CMAKE_CURRENT_LIST_DIR}/")
list(TRANSFORM BUFFERLIB_SRCS PREPEND "${BUFFERLIB_INCLUDE_PATH}/")
list(TRANSFORM EXTRA_FILES PREPEND "${PROJECT_SOURCE_DIR}/")

message(${CMAKE_ROOT})

set(TARGET_EXE Test_BufferLib)
add_executable(
  ${TARGET_EXE}
  ${SRCS}
)

target_sources(${TARGET_EXE}
	PRIVATE
	${BUFFERLIB_SRCS}
	${EXTRA_FILES}
)

target_include_directories(${TARGET_EXE}
	PRIVATE
	${BUFFERLIB_INCLUDE_PATH}
)

target_link_libraries(${TARGET_EXE}
  PRIVATE
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(${TARGET_EXE})

source_group(
    TREE ${PROJECT_SOURCE_DIR}
    FILES ${SRCS} ${HDRS} ${BUFFERLIB_SRCS} ${EXTRA_FILES}
)