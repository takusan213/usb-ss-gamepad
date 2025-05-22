/*******************************************************************************
Copyright 2021 Geeky Fab. (geekyfab.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*******************************************************************************/

#ifndef MY_APP_DEVICE_GAMEPAD_C
#define	MY_APP_DEVICE_GAMEPAD_C

#include "app_device_joystick.h"
#include "system.h"
#include "buttons.h"
#include "usb.h"
#include "usb_device_hid.h"
#include "mapping.h"
#include "hid_rpt_map.h"
#include "usb_framework/inc/usb_ch9.h"
#include "usb_framework/inc/usb_device.h"

typedef struct _Flags{
    uint8_t crosskey_flag :2 ;
    uint8_t sw_flag :1 ;
    uint8_t :5 ;
} Flags;


Flags flags;

// The HIDFeatureReceive function has been moved to usb_events.c
// to handle both Interface 0 and Interface 1 Feature reports

void App_DeviceGamepadInit(void){
    flags.crosskey_flag = 0;
    flags.sw_flag = false;
}

/**
 * Helper function to set button field based on usage
 * @param gamepad_input Pointer to the joystick report
 * @param usage HID usage value (1-14)
 * @param pressed Whether the button is pressed
 */
// 注意: この関数は現在使用されていませんが、マッピング機能のために残しています
static void SetHIDButtonField(INPUT_CONTROLS* gamepad_input, uint8_t usage, bool pressed) {
    // Button bits are stored in bitfields, so we have to set each one individually based on usage
    switch(usage) {
        case 1: gamepad_input->members.buttons.a = pressed; break;
        case 2: gamepad_input->members.buttons.b = pressed; break;
        case 3: gamepad_input->members.buttons.c = pressed; break;
        case 4: gamepad_input->members.buttons.x = pressed; break;
        case 5: gamepad_input->members.buttons.y = pressed; break;
        case 6: gamepad_input->members.buttons.z = pressed; break;
        case 7: gamepad_input->members.buttons.L1 = pressed; break;
        case 8: gamepad_input->members.buttons.R1 = pressed; break;
        case 9: gamepad_input->members.buttons.start = pressed; break;
        case 10: gamepad_input->members.buttons.L2 = pressed; break;
        case 11: gamepad_input->members.buttons.R2 = pressed; break;
        case 12: gamepad_input->members.buttons.home = pressed; break;
        case 13: gamepad_input->members.buttons.right_stick = pressed; break;
        case 14: gamepad_input->members.buttons.left_stick = pressed; break;
        default: break; // Invalid usage
    }
}

void App_DeviceGamepadAct(INPUT_CONTROLS* gamepad_input){

    // No Report ID in Interface 0
    
    // Clear all button fields and data by zeroing all bytes
    // 完全にメモリをクリアする
    for (int i = 0; i < sizeof(INPUT_CONTROLS); i++) {
        gamepad_input->val[i] = 0;
    }
    
    
    // D-Padの状態を取得（全ての処理で使えるように上部で定義）
    bool up = BUTTON_IsPressed(BUTTON_UP);
    bool down = BUTTON_IsPressed(BUTTON_DOWN);
    bool left = BUTTON_IsPressed(BUTTON_LEFT);
    bool right = BUTTON_IsPressed(BUTTON_RIGHT);

    // Directly set start button (not using mapping)
    // START ボタンだけは特別扱い - 常にそのまま使う
    gamepad_input->members.buttons.start = BUTTON_IsPressed(BUTTON_START);        

    if(flags.sw_flag == false) {
        // 直接紐づけに変更する - マッピング機能を回避して直接設定
        gamepad_input->members.buttons.a = BUTTON_IsPressed(BUTTON_A);
        gamepad_input->members.buttons.b = BUTTON_IsPressed(BUTTON_B);
        gamepad_input->members.buttons.c = BUTTON_IsPressed(BUTTON_C);
        gamepad_input->members.buttons.x = BUTTON_IsPressed(BUTTON_X);
        gamepad_input->members.buttons.y = BUTTON_IsPressed(BUTTON_Y);
        gamepad_input->members.buttons.z = BUTTON_IsPressed(BUTTON_Z);
        gamepad_input->members.buttons.L1 = BUTTON_IsPressed(BUTTON_TL);
        gamepad_input->members.buttons.R1 = BUTTON_IsPressed(BUTTON_TR);

        // D-PadをHATスイッチとして処理 (上で定義した変数を使用)
        
        // HATスイッチとして処理 - MODE 0の場合のみ
        // if (flags.crosskey_flag == 0) {
        //     if(up && left) {
        //         gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_NORTH_WEST;
        //     } else if(up && right) {
        //         gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_NORTH_EAST;
        //     } else if(down && left) {
        //         gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_SOUTH_WEST;
        //     } else if(down && right) {
        //         gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_SOUTH_EAST;
        //     } else if(up) {
        //         gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_NORTH;
        //     } else if(right) {
        //         gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_EAST;
        //     } else if(down) {
        //         gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_SOUTH;
        //     } else if(left) {
        //         gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_WEST;
        //     } else {
        //         gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_NULL;
        //     }
        // }

    } else if(flags.sw_flag == true) {
        // 代替モード - 直接紐づけに変更
        // 下位互換性のために基本ボタンを維持
        gamepad_input->members.buttons.a = BUTTON_IsPressed(BUTTON_A);
        gamepad_input->members.buttons.b = BUTTON_IsPressed(BUTTON_B);
        gamepad_input->members.buttons.c = BUTTON_IsPressed(BUTTON_C);
        // 特殊ボタンの直接マッピング
        gamepad_input->members.buttons.L2 = BUTTON_IsPressed(BUTTON_TL);
        gamepad_input->members.buttons.R2 = BUTTON_IsPressed(BUTTON_TR);
        gamepad_input->members.buttons.left_stick = BUTTON_IsPressed(BUTTON_Y);
        gamepad_input->members.buttons.right_stick = BUTTON_IsPressed(BUTTON_X);
        gamepad_input->members.buttons.home = BUTTON_IsPressed(BUTTON_Z);
    }

    // 特別な組み合わせボタン - 例：START+L同時押しでHOMEボタン
    if (flags.sw_flag == false && BUTTON_IsPressed(BUTTON_START) && BUTTON_IsPressed(BUTTON_TL)) {
        SetHIDButtonField(gamepad_input, Mapping_GetUsage(12), true);
    }


    // アナログスティック処理
    // ハットスイッチはデフォルトでNULL(8)に設定
    gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_NULL;

    // X/XY 初期化
    gamepad_input->members.analog_stick.X = 0x80;  // デフォルト中央
    gamepad_input->members.analog_stick.Y = 0x80;  // デフォルト中央

    // Z/Rz 初期化
    gamepad_input->members.analog_stick.Z = 0x80;   // 中央位置 (128)
    gamepad_input->members.analog_stick.Rz = 0x80;  // 中央位置 (128)


    // クロスキーモード処理
    switch(flags.crosskey_flag) {
        // モード0: アナログX/Y
        case 0:
            // X軸 (左右)
            if(left){
                gamepad_input->members.analog_stick.X = 0x00;   // 左
            }else if(right){
                gamepad_input->members.analog_stick.X = 0xFF; // 右
            }
            
            // Y軸 (上下)
            if(up){
                gamepad_input->members.analog_stick.Y = 0x00;   // 上
            }else if(down){
                gamepad_input->members.analog_stick.Y = 0xFF; // 下
            }
            break;
            
        // モード1: HATスイッチ+アナログXY (両方有効)
        case 1:
            // HATスイッチ設定
            if(up && left) {
                gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_NORTH_WEST;
            } else if(up && right) {
                gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_NORTH_EAST;
            } else if(down && left) {
                gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_SOUTH_WEST;
            } else if(down && right) {
                gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_SOUTH_EAST;
            } else if(up) {
                gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_NORTH;
            } else if(right) {
                gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_EAST;
            } else if(down) {
                gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_SOUTH;
            } else if(left) {
                gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_WEST;
            } else {
                gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_NULL;
            }
            break;
            
        // モード2: 何も反応しない
        case 2:
            // アナログスティックは中立に設定済み
            gamepad_input->members.analog_stick.Z = left ? 0 : (right ? 255 : 128);
            gamepad_input->members.analog_stick.Rz = up ? 0 : (down ? 255 : 128);
    
            break;
            
        // 不明なモード: モード0と同じ
        default:
            // ここには何も記述しない - 全てのモード処理は上のクロスキーモード処理で完了
            break;
    }
    
    return;

}

void ChangeSWMode_Button_Start(void){
    uint16_t cnt_timer =0;
    
    if (BUTTON_IsPressed(BUTTON_START)&&BUTTON_IsPressed(BUTTON_TR)){
        INTCONbits.TMR0IF = 0;          // reset timer0 interrupt flag
        TMR0bits.TMR0 = (uint8_t)5;
        cnt_timer = 0;
        while(BUTTON_IsPressed(BUTTON_START)&&BUTTON_IsPressed(BUTTON_TR)){
            if(INTCONbits.TMR0IF){          // INTCONbits.TMR0IF happens every 4ms
                cnt_timer++;
                if(cnt_timer >=250){        // 1s
                    flags.sw_flag = ~(flags.sw_flag);
                    cnt_timer =0;
                    while(BUTTON_IsPressed(BUTTON_START)&&BUTTON_IsPressed(BUTTON_TR));
                }
                INTCONbits.TMR0IF = 0;
                TMR0bits.TMR0 = (uint8_t)5;
            }
        }
    }
        
    return;
}

void ChangeSWMode_Button_Select(void){
    uint16_t cnt_timer =0;
    
    if (BUTTON_IsPressed(BUTTON_START)&&BUTTON_IsPressed(BUTTON_TL)){
        INTCONbits.TMR0IF = 0;          // reset timer0 interrupt flag
        TMR0bits.TMR0 = (uint8_t)5;
        cnt_timer = 0;
        while(BUTTON_IsPressed(BUTTON_START)&&BUTTON_IsPressed(BUTTON_TL)){
            if(INTCONbits.TMR0IF){          // INTCONbits.TMR0IF happens every 4ms
                cnt_timer++;
                if(cnt_timer >=250){        // 1s
                    switch(flags.crosskey_flag){
                        case 0: flags.crosskey_flag =1; break;
                        case 1: flags.crosskey_flag =2; break;
                        case 2: flags.crosskey_flag =0; break;
                    }
                    cnt_timer =0;
                    while(BUTTON_IsPressed(BUTTON_START)&&BUTTON_IsPressed(BUTTON_TL));
                }
                INTCONbits.TMR0IF = 0;
                TMR0bits.TMR0 = (uint8_t)5;
            }
        }
    }
        
    return;
}

#endif	/* MY_APP_DEVICE_GAMEPAD_C */