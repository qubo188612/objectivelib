cmake_minimum_required(VERSION 3.15)
project(objectivelib CXX)

list(APPEND CMAKE_PREFIX_PATH "$ENV{QT5_15_2_DIR}")  #添加qt5路径

# 查找依赖库
find_package(alg_base_common REQUIRED)
find_package(syopencv REQUIRED)
find_package(com_app_devicepluginbase REQUIRED)
find_package(com_app_paramrw REQUIRED)
find_package(pthreadvc2 REQUIRED)
find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core)
find_package(QT NAMES Qt5 REQUIRED COMPONENTS Network)
find_package(Qt5 REQUIRED COMPONENTS Core)
find_package(Qt5 REQUIRED COMPONENTS Network)

set(CMAKE_CXX_STANDARD 17)

# 指定源文件和头文件目录
set(SOURCE_DIR "${PROJECT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")

if (CONAN_CREATE_BUILD)
    message("Don't create COMMIT_ID")
else()
	#将commit_id写进头文件中
	execute_process(
		COMMAND git rev-parse --short HEAD
		OUTPUT_VARIABLE COMMIT_ID_APP_OBJECTIVELIB
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	file(WRITE "${INCLUDE_DIR}/commitid.h" "#pragma once\n#define COMMIT_ID_APP_OBJECTIVELIB \"${COMMIT_ID_APP_OBJECTIVELIB}\" ")
endif()

#设置版本信息
IF (MSVC)
	option (VERSION_RELEASE "版本是否为正式版" ON)
	IF(${VERSION_RELEASE})
		SET(VERSION_PRODUCT_TYPE Release)
	ELSE()
		SET(VERSION_PRODUCT_TYPE Test)
	ENDIF()
	message("VERSION_PRODUCT_TYPE:" ${VERSION_PRODUCT_TYPE})

	string(TIMESTAMP VERSION_PRODUCT_YEAR %y%m)
	message("VERSION_PRODUCT_YEAR:" ${VERSION_PRODUCT_YEAR})

	string(TIMESTAMP VERSION_PRODUCT_DATE %d%H)
	message("VERSION_PRODUCT_DATE:" ${VERSION_PRODUCT_DATE})


	set(MY_VERSIONINFO_RC "${SOURCE_DIR}/version.rc")
	configure_file("${SOURCE_DIR}/version.rc.in" "${MY_VERSIONINFO_RC}")
ELSEIF (UNIX)
ENDIF ()	
	
if (GPU_SUPPORT)
message("-----------------------------------------------gpu----------------------------------------------------")
	find_package(sycuda REQUIRED)
	project(objectivelensconvert LANGUAGES CXX CUDA)
	file(GLOB_RECURSE SOURCE_FILES "${SOURCE_DIR}/*.cpp" "${SOURCE_DIR}/*.cu")
	file(GLOB_RECURSE SOURCE_HEADERS "${INCLUDE_DIR}/*.h" "${INCLUDE_DIR}/*.cuh")
	# 设置 CUDA 相关选项
	set(CMAKE_CUDA_STANDARD_REQUIRED ON)	

	# 添加库
	add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${SOURCE_HEADERS} ${MY_VERSIONINFO_RC})
	target_compile_definitions(${PROJECT_NAME} PRIVATE GPU_SUPPORT)	
	
	#指定头文件路径
	target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR} )

	# 链接 spdlog、nlohmann_json
	target_link_libraries(${PROJECT_NAME} PRIVATE alg_base_common::alg_base_common sycuda::sycuda syopencv::syopencv pthreadvc2::pthreadvc2 Qt5::Core Qt5::Network)
else()
message("-----------------------------------------------cpu----------------------------------------------------")
	# 收集所有源文件和头文件
	file(GLOB_RECURSE SOURCE_FILES "${SOURCE_DIR}/*.cpp" )
	file(GLOB_RECURSE SOURCE_HEADERS "${INCLUDE_DIR}/*.h")

	# 添加库
	add_library(${PROJECT_NAME}  SHARED ${SOURCE_FILES} ${SOURCE_HEADERS} ${MY_VERSIONINFO_RC})
	#指定头文件路径
	target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR} )

	# 链接 spdlog、nlohmann_json
	target_link_libraries(${PROJECT_NAME} PRIVATE com_app_devicepluginbase::com_app_devicepluginbase com_app_paramrw::com_app_paramrw alg_base_common::alg_base_common syopencv::syopencv pthreadvc2::pthreadvc2 Qt5::Core Qt5::Network)
endif()

#安装
string(TIMESTAMP SDK_DATE  %y_%m_%d_%H)
message("SDK_DATE:" ${SDK_DATE})
set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/install/${PROJECT_NAME}_${SDK_DATE}")
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${INCLUDE_DIR}/${PROJECT_NAME}.h;${alg_base_common_INCLUDE_DIR}/dataStructure.h")
install(TARGETS ${PROJECT_NAME})
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}.pdb ${CMAKE_INSTALL_PREFIX}/bin/${PROJECT_NAME}.pdb)

# 只有在不是 conan create 的阶段才编译和安装测试可执行文件
if (CONAN_CREATE_BUILD)
    message("Not installing test package")
else()
    # 可执行文件（test_package 中的测试代码）
	file(GLOB TEST_SOURCE_LIST "test_package/src/*.cpp" "test_package/src/*.h")
    add_executable(${PROJECT_NAME}_test ${TEST_SOURCE_LIST})
	target_compile_definitions(${PROJECT_NAME}_test PRIVATE DEVELOP)

	if(EXISTS "${CMAKE_SOURCE_DIR}/test_package/src/custom_example.cpp")
		# 输出 custom_example.cpp 的具体路径
		message(STATUS "Found custom_example.cpp at: ${CMAKE_SOURCE_DIR}/test_package/src/custom_example.cpp")
		# 添加定义和源文件
		add_compile_definitions(CUSTOM_MAIN_SELF_IMPLEMENTATION)
	else()
		# 如果 custom_example.cpp 不存在，输出提示信息
		message(WARNING "custom_example.cpp not found at ${CMAKE_SOURCE_DIR}/test_package/src/custom_example.cpp. Using default implementation.")
	endif()


    target_include_directories(${PROJECT_NAME}_test PUBLIC ${CMAKE_INSTALL_PREFIX}/include)
    target_link_libraries(${PROJECT_NAME}_test PRIVATE ${PROJECT_NAME} syopencv::syopencv alg_base_common::alg_base_common)
endif()
