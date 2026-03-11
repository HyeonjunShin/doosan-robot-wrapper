#include <iostream>
#include "DRFL.h"
#include <thread> 
#include <chrono> 
#include <assert.h>

using namespace DRAFramework;

CDRFLEx robot;
bool g_TpInitailizingComplted = false;
bool g_bHasControlAuthority = false;
bool g_mStat = false;

void OnTpInitializingCompleted() {
    g_TpInitailizingComplted = true;
    robot.ManageAccessControl(MANAGE_ACCESS_CONTROL_FORCE_REQUEST);
}

void OnHommingCompleted() {
    cout << "Homing completed" << endl;
}

void OnProgramStopped(const PROGRAM_STOP_CAUSE) {
    assert(robot.PlayDrlStop(STOP_TYPE_SLOW));
    cout << "Program stopped" << endl;
}

void OnMonitoringDataCB(const LPMONITORING_DATA pData) {
    return;
}

void OnMonitoringDataExCB(const LPMONITORING_DATA_EX pData) {
    return;
}

void OnMonitoringCtrlIOExCB(const LPMONITORING_CTRLIO_EX pData) {
    return;
}

void OnMonitroingAccessControlCB(const MONITORING_ACCESS_CONTROL eTrasnsitControl) {
    switch (eTrasnsitControl) {
        case MONITORING_ACCESS_CONTROL_REQUEST:
            assert(robot.ManageAccessControl(MANAGE_ACCESS_CONTROL_RESPONSE_NO));
            break;
        case MONITORING_ACCESS_CONTROL_GRANT:
            g_bHasControlAuthority = true;
            break;
        case MONITORING_ACCESS_CONTROL_DENY:
        case MONITORING_ACCESS_CONTROL_LOSS:
            g_bHasControlAuthority = false;
            if (g_TpInitailizingComplted) {
                robot.ManageAccessControl(MANAGE_ACCESS_CONTROL_FORCE_REQUEST);
            }
            break;
        default:
            break;
    }
}

void OnLogAlarm(LPLOG_ALARM tLog) {
    g_mStat = true;
    cout << "Alarm Info: group(" << (unsigned int)tLog->_iGroup 
         << "), index(" << tLog->_iIndex 
         << "), param(" << tLog->_szParam[0] << ")" << endl;
}

void OnTpPopup(LPMESSAGE_POPUP tPopup) {
    cout << "Popup Message: " << tPopup->_szText << endl;
}

void OnTpLog(const char* strLog) {
    cout << "Log Message: " << strLog << endl;
}

void OnTpProgress(LPMESSAGE_PROGRESS tProgress) {
    cout << "Progress: " << (int)tProgress->_iCurrentCount 
         << "/" << (int)tProgress->_iTotalCount << endl;
}

void OnTpGetuserInput(LPMESSAGE_INPUT tInput) {
    cout << "User Input: " << tInput->_szText << endl;
}

void OnRTMonitoringData(LPRT_OUTPUT_DATA_LIST tData) {
    return;
}

void OnDisConnected() {
    while (!robot.open_connection("192.168.0.34")) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

int main() {

    robot.set_on_homming_completed(OnHommingCompleted);
    robot.set_on_monitoring_data(OnMonitoringDataCB);
    // robot.set_on_monitoring_data_ex(OnMonitoringDataExCB);
    robot.set_on_monitoring_access_control(OnMonitroingAccessControlCB);
    robot.set_on_tp_initializing_completed(OnTpInitializingCompleted);
    robot.set_on_log_alarm(OnLogAlarm);
    robot.set_on_tp_popup(OnTpPopup);
    robot.set_on_tp_log(OnTpLog);
    robot.set_on_tp_progress(OnTpProgress);
    robot.set_on_tp_get_user_input(OnTpGetuserInput);
    robot.set_on_rt_monitoring_data(OnRTMonitoringData);
    robot.set_on_program_stopped(OnProgramStopped);
    robot.set_on_disconnected(OnDisConnected);

    assert(robot.open_connection("192.168.0.34"));
    assert(robot.setup_monitoring_version(1));

    robot.manage_access_control(MANAGE_ACCESS_CONTROL_FORCE_REQUEST);
    assert(robot.set_robot_mode(ROBOT_MODE_AUTONOMOUS));

    robot.set_robot_control(CONTROL_SERVO_ON);

    while ((robot.get_robot_state() != STATE_STANDBY)) {
        cout << "Robot state: " << robot.get_robot_state() << endl;
        this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    std::cout << "Connecting to robot..." << std::endl;
    // std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // bool result = robot.set_robot_control(CONTROL_SERVO_ON);

    // robot.set_robot_control(CONTROL_SERVO_ON);  
    // // // robot.set_robot_mode(ROBOT_MODE::ROBOT_MODE_MANUAL);

    // // LPROBOT_POSE pCurrentPos = robot.get_current_posj();

    // while (true) {
    //     LPROBOT_POSE pCurrentPos = robot.get_current_posj();
    //     if (pCurrentPos != nullptr) {
    //         printf("\rJ1: %6.2f | J2: %6.2f | J3: %6.2f | J4: %6.2f | J5: %6.2f | J6: %6.2f", 
    //             pCurrentPos->_fPosition[0], pCurrentPos->_fPosition[1], pCurrentPos->_fPosition[2],
    //             pCurrentPos->_fPosition[3], pCurrentPos->_fPosition[4], pCurrentPos->_fPosition[5]);
    //         fflush(stdout); // 화면 갱신을 위해 버퍼 비우기
    //     }
    //     std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 10Hz 주기로 업데이트
    // }


    robot.close_connection();
    return 0;
}