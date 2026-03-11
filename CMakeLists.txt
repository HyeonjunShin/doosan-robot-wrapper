cmake_minimum_required(VERSION 3.10)
project(doosan_robot_control)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. 시스템 아키텍처 및 OS 버전 감지
if(UNIX AND NOT APPLE)
    if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "aarch64" OR CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "arm64")
        set(ARCH "arm64")
    else()
        set(ARCH "amd64")
    endif()

    # Ubuntu 버전 감지 (예: 20.04, 22.04 등)
    file(READ "/etc/os-release" OS_RELEASE_CONTENT)
    string(REGEX MATCH "VERSION_ID=\"([0-9]+\\.[0-9]+)\"" _ ${OS_RELEASE_CONTENT})
    set(OS_VER ${CMAKE_MATCH_1})
    
    set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/Linux/64bits/${ARCH}/${OS_VER}")
    message(STATUS "Detected Linux ${ARCH} (Ubuntu ${OS_VER}) - Lib path: ${LIB_DIR}")

elseif(WIN32)
    set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/Windows/64bits")
    message(STATUS "Detected Windows 64bits - Lib path: ${LIB_DIR}")
endif()

# 2. 실행 파일 생성 (main.cpp가 루트에 있다고 가정)
add_executable(robot_app main.cpp)

# 3. 헤더 파일 경로 설정
target_include_directories(robot_app PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

# 4. 라이브러리 연결
if(UNIX)
    target_link_libraries(robot_app PRIVATE
        "${LIB_DIR}/libDRFL.a"
        "${LIB_DIR}/libPocoFoundation.so"
        "${LIB_DIR}/libPocoNet.so"
        pthread dl  # 리눅스 필수 시스템 라이브러리
    )
    
    # 실행 시 .so 파일을 찾을 수 있도록 RPATH 설정
    set_target_properties(robot_app PROPERTIES INSTALL_RPATH "${LIB_DIR}")
    set_target_properties(robot_app PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)

elseif(WIN32)
    target_link_libraries(robot_app PRIVATE "${LIB_DIR}/DRFLWin64.lib")
    # DLL 파일들은 실행 파일(.exe) 옆으로 복사해야 실행됨
endif()