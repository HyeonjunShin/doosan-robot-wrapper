#include <iostream>
#include "DRFL.h"
#include <thread> 
#include <chrono> 
#include <assert.h>
#include <sstream>

using namespace DRAFramework;


std::string to_str(const MONITORING_ACCESS_CONTROL x)
{
    if(x == MONITORING_ACCESS_CONTROL_REQUEST)  return "MONITORING_ACCESS_CONTROL_REQUEST";
    if(x == MONITORING_ACCESS_CONTROL_DENY)  return "MONITORING_ACCESS_CONTROL_DENY";
    if(x == MONITORING_ACCESS_CONTROL_GRANT)  return "MONITORING_ACCESS_CONTROL_GRANT";
    if(x == MONITORING_ACCESS_CONTROL_LOSS)  return "MONITORING_ACCESS_CONTROL_LOSS";
    if(x == MONITORING_ACCESS_CONTROL_LAST)  return "MONITORING_ACCESS_CONTROL_LAST";
    return "to_str err";
}
std::string to_str(const ROBOT_STATE x)
{
    if(x == STATE_INITIALIZING)  return "STATE_INITIALIZING";
    if(x == STATE_STANDBY)  return "STATE_STANDBY";
    if(x == STATE_MOVING)  return "STATE_MOVING";
    if(x == STATE_SAFE_OFF)  return "STATE_SAFE_OFF";
    if(x == STATE_TEACHING)  return "STATE_TEACHING";
    if(x == STATE_SAFE_STOP)  return "STATE_SAFE_STOP";
    if(x == STATE_EMERGENCY_STOP)  return "STATE_EMERGENCY_STOP";
    if(x == STATE_HOMMING)  return "STATE_HOMMING";
    if(x == STATE_RECOVERY)  return "STATE_RECOVERY";
    if(x == STATE_SAFE_STOP2)  return "STATE_SAFE_STOP2";
    if(x == STATE_SAFE_OFF2)  return "STATE_SAFE_OFF2";
    if(x == STATE_RESERVED1)  return "STATE_RESERVED1";
    if(x == STATE_RESERVED2)  return "STATE_RESERVED2";
    if(x == STATE_RESERVED3)  return "STATE_RESERVED3";
    if(x == STATE_RESERVED4)  return "STATE_RESERVED4";
    if(x == STATE_NOT_READY)  return "STATE_NOT_READY";
    if(x == STATE_LAST)  return "STATE_LAST";
    return "to_str err";
}



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


const std::string IP_ADDRESS = "192.168.0.34";
bool get_control_access = false;
bool is_standby = false;


void OnMonitroingAccessControlCB(const MONITORING_ACCESS_CONTROL access) {
    std::cout << "[OnMonitroingAccessControlCB] Control Access : " << to_str(access) << std::endl;
    if(MONITORING_ACCESS_CONTROL_GRANT == access) {
        get_control_access = true;
        std::cout << "Got Control Access !! " << std::endl;
    }
    if(MONITORING_ACCESS_CONTROL_LOSS == access) {
        get_control_access = false;
    }
};

void OnMonitoringStateCB(const ROBOT_STATE state) {
    std::cout << "[OnMonitoringStateCB] Robot State : " << to_str(state) << std::endl;
    if(STATE_STANDBY == state) {
        is_standby = true;
        std::cout << "Successfully Servo on !! " << std::endl;
    }else {
        is_standby = false;
    }
};

int main() {

    robot.set_on_monitoring_access_control(OnMonitroingAccessControlCB);
	robot.set_on_monitoring_state(OnMonitoringStateCB);

    // robot.set_on_homming_completed(OnHommingCompleted);
    // robot.set_on_monitoring_data(OnMonitoringDataCB);
    // // robot.set_on_monitoring_data_ex(OnMonitoringDataExCB);
    // robot.set_on_monitoring_access_control(OnMonitroingAccessControlCB);
    // robot.set_on_tp_initializing_completed(OnTpInitializingCompleted);
    // robot.set_on_log_alarm(OnLogAlarm);
    // robot.set_on_tp_popup(OnTpPopup);
    // robot.set_on_tp_log(OnTpLog);
    // robot.set_on_tp_progress(OnTpProgress);
    // robot.set_on_tp_get_user_input(OnTpGetuserInput);
    // robot.set_on_rt_monitoring_data(OnRTMonitoringData);
    // robot.set_on_program_stopped(OnProgramStopped);
    // robot.set_on_disconnected(OnDisConnected);

	bool ret = robot.open_connection(IP_ADDRESS);
	std::cout << "open connection return value " << ret << std::endl;
	if (true != ret) {
		std::cout << "Cannot open connection to robot @ " << IP_ADDRESS
					<< std::endl;
		return 1;
	}
    assert(robot.setup_monitoring_version(1));


	for (size_t retry = 0; retry < 10; ++retry, std::this_thread::sleep_for(std::chrono::milliseconds(1000))) {
		if(!get_control_access) { 
				robot.ManageAccessControl(MANAGE_ACCESS_CONTROL_FORCE_REQUEST);
				continue;
		}

        if(robot.get_robot_state() == STATE_SAFE_OFF2){
            robot.set_robot_mode(ROBOT_MODE_RECOVERY);
        }

		if(!is_standby) {
            robot.set_robot_control(CONTROL_SERVO_ON);
            continue;
		}
		if(get_control_access && is_standby)   break;
	}

    robot.set_robot_mode(ROBOT_MODE_AUTONOMOUS);


	// float targetPos[6] = {0.,0.,30.,0.,0.,0.};
	// float targetVel = 50;
	// float targetAcc = 50;
	// robot.movej(targetPos, targetVel, targetAcc);

	// targetPos[2] = 0;
	// robot.movej(targetPos, targetVel, targetAcc);


    // std::cout << "Press Enter to continue..." ;
	// std::cin.get();  // Waits for user to press Enter

    // cout << "MoveJ" << endl;
    // float angle[6] = {-92,3,-107,0,-75,9};
    // robot.movej(angle, 50, 50, 4.0);


    float move_pos[6] = {100, 200, 102, 0, -180, 0};
    float velo[2] = {1000, 1000};
    float acc[2] =  {1000, 1000};
    while (true){
        string input;
        getline(cin, input);

        if (input == "q" || input == "Q") break;
        if (input.empty()) continue;


        if(robot.get_robot_state() == STATE_RECOVERY){

            float angle[6] = {-92.0, 3.0, -107.0, 0.0, -75.0, 9.0};
            float velo[6] = {10.0, 10.0, 10.0, 10.0, 10.0, 10.0}; // 10 deg/sec
            float acc[6] = {20.0, 20.0, 20.0, 20.0, 20.0, 20.0};  // 20 deg/sec^2

            robot.movej_h2r(angle, velo, acc, 10.0);

            bool bArrived = false;
            while (!bArrived) {
                robot.hold2run();

                LPROBOT_POSE cur_j = robot.get_current_posj();
                
                float error_sum = 0;
                for(int i=0; i<6; i++) {
                    error_sum += std::abs(cur_j->_fPosition[i] - angle[i]);
                }

                if(error_sum < 0.5) { 
                    bArrived = true;
                    robot.stop(STOP_TYPE_QUICK); 
                }

                // D. 주기 조절 (API-DRFL 권장 주기는 약 10~50ms)
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }
        
        }



        stringstream ss(input);
        char cmd;
        ss >> cmd;

        if (cmd == 'r' || cmd == 'R') {
            cout << to_str(robot.get_robot_state()) << endl; 
            // cout << robot.get_tool_force() << endl;

            if(robot.get_robot_state() == STATE_SAFE_OFF2){
                robot.set_robot_mode(ROBOT_MODE_RECOVERY);
                robot.set_robot_control(CONTROL_SERVO_ON);  
                this_thread::sleep_for(std::chrono::milliseconds(2000));
                
                // robot.release_protective_stop(RELEASE_MODE_RELEASE);
                // robot.set_robot_mode(ROBOT_MODE_AUTONOMOUS);

                float angle[6] = {-92,3,-107,0,-75,9};
                float valo[6] = {0,0,0,0,0,0};
                float acc[6] = {0,0,0,0,0,0};
                robot.movej_h2r(angle, valo, acc, 10.0);
                robot.set_robot_mode(ROBOT_MODE_AUTONOMOUS);

                // move_pos[0] = 300;
                // move_pos[1] = 800;
                // move_pos[2] = 300;

                // robot.movej_h2r(move_pos, velo, acc, 10);
                // float angle[6] = {-92,3,-107,0,-75,9};
                // float valo[6] = {0,0,0,0,0,0};
                // float acc[6] = {0,0,0,0,0,0};
                // // robot.movej_h2r(angle, valo, acc, 10.0);
                // this_thread::sleep_for(std::chrono::milliseconds(2000));

            }
            // robot.move_()

            //     // std::this_thread::sleep_for(std::chrono::milliseconds(500));
            //     // robot.jog(JOG_AXIS_TASK_Z, MOVE_REFERENCE_BASE, 5.0);
            // }

            // if(robot.get_robot_state() == STATE_RECOVERY){
            //     robot.set_robot_mode(ROBOT_MODE_AUTONOMOUS);

            //     move_pos[0] = 300;
            //     move_pos[1] = 800;
            //     move_pos[2] = 300;

            //     robot.movel(move_pos, velo, acc, 0);
            //     // robot.set_robot_mode(ROBOT_MODE_AUTONOMOUS);
            //     // robot.set_robot_control(CONTROL_SERVO_ON);
            // }
        }

        if (cmd == 'o' || cmd == 'O') {
            cout << to_str(robot.get_robot_state()) << endl; 
            if(robot.get_robot_state() == STATE_RECOVERY){
                robot.set_robot_mode(ROBOT_MODE_AUTONOMOUS);
                // robot.set_robot_control(CONTROL_SERVO_ON);
            }
        }

        if (cmd == 'm' || cmd == 'M') {
            bool parse_success = true;
            for (int i = 0; i < 3; i++) {
                if (!(ss >> move_pos[i])) {
                    parse_success = false;
                    break;
                }
            }

            if (parse_success) {
                if (robot.get_robot_state() == STATE_STANDBY) {
                    cout << "Moving to: [" << move_pos[0] << ", " << move_pos[1] << ", " << move_pos[2] 
                         << ", " << move_pos[3] << ", " << move_pos[4] << ", " << move_pos[5] << "]" << endl;
                    bool ret = robot.movel(move_pos, velo, acc, 0);
                    cout << ret << endl;
                }
            }
        }

        // if(robot.get_robot_state() == STATE_STANDBY){

        //     cout<<"dajlkwjdl" << endl;
        //     robot.movel(move, valo, acc, 0);
        //     cout<<"dajlkwjdl" << endl;

        //     move[2] = 400;
        //     robot.movel(move, valo, acc, 0);
        //     cout<<"dajlkwjdl" << endl;
        // }

        // if(robot.get_robot_state() == STATE_SAFE_OFF2){
        //     robot.set_robot_mode(ROBOT_MODE_RECOVERY);
        //     robot.set_robot_control(CONTROL_SERVO_ON);
        //     cout<<robot.get_robot_state() << " " << STATE_SAFE_OFF2 << endl;
        // }
        
    }
    robot.close_connection();

    // cout << robot.get_robot_state() << endl;

    // assert(robot.set_robot_mode(ROBOT_MODE_AUTONOMOUS));

    // robot.SetRobotSystem(ROBOT_SYSTEM_REAL);
    // robot.SetRobotMode(ROBOT_MODE_MANUAL);
    // robot.manage_access_control(MANAGE_ACCESS_CONTROL_FORCE_REQUEST);

    // cout << robot.get_robot_state() << endl;

    // this_thread::sleep_for(std::chrono::milliseconds(1000));
    // bool ison_servo = robot.set_robot_control(CONTROL_SERVO_ON);

    // cout << "servo" << ison_servo << endl;
    // cout << robot.get_robot_state() << endl;

    // cout <<  "wati" << endl;
    // this_thread::sleep_for(std::chrono::milliseconds(2000));

    // cout << "MoveJ" << endl;
    // float angle[6] = {-92,3,-107,0,-75,9};
    // robot.movej(angle, valo, acc, 4.0);


    // float move[6] = {100, 800, 130, 0, -180, -90};
    // float valo[6] = {0,0,0,0,0,0};
    // float acc[6] = {0,0,0,0,0,0};
    // robot.movel(move, valo, acc, 4.0);

    // while (true)
    // {
    //     cout << robot.get_robot_state() << endl;
    //     if(robot.get_robot_state() == STATE_SAFE_OFF){
    //         assert(robot.set_robot_mode(ROBOT_MODE_AUTONOMOUS));
    //         robot.set_robot_control(CONTROL_SERVO_ON);
    // }
    // robot.MoveJ(angle, valo, acc, 5.0);
    // }
    // this_thread::sleep_for(std::chrono::milliseconds(10000));


    // while ((robot.get_robot_state() != STATE_STANDBY)) {
    //     cout << "Robot state: " << robot.get_robot_state() << endl;
    //     this_thread::sleep_for(std::chrono::milliseconds(1000));
    // }

    // std::cout << "Connecting to robot..." << std::endl;
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


    return 0;
}


