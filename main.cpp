#include <iostream>
#include "DRFL.h"

// 반드시 이 'main' 함수가 존재해야 합니다!
int main() {
    std::cout << "--- Doosan Robot App Start ---" << std::endl;
    
    // 라이브러리가 잘 로드되었는지 버전 출력 시도
    // (참고: 두산 SDK 버전에 따라 함수명이 다를 수 있으니 
    // 에러가 나면 이 줄을 주석 처리하고 테스트하세요)
    // std::cout << "SDK Version: " << get_library_version() << std::endl;

    DRAFramework::CDRFL robot;
    std::cout << "Robot instance created successfully!" << std::endl;

    return 0;
}