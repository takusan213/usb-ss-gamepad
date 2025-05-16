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
    
    // Clear all button fields by zeroing the raw bytes
    // 効率的にメモリクリア (2バイト分)
    gamepad_input->val[0] = 0;  // ボタンデータ
    gamepad_input->val[1] = 0;  // ハットスイッチデータ

    // Directly set start button (not using mapping)
    // START ボタンだけは特別扱い - 常にそのまま使う
    gamepad_input->members.buttons.start = BUTTON_IsPressed(BUTTON_START);        

    if(flags.sw_flag == false) {
        // マッピングを使用してメモリ効率の良い実装
        // Button 0: A
        SetHIDButtonField(gamepad_input, Mapping_GetUsage(0), BUTTON_IsPressed(BUTTON_A));
        // Button 1: B
        SetHIDButtonField(gamepad_input, Mapping_GetUsage(1), BUTTON_IsPressed(BUTTON_B));
        // Button 2: C
        SetHIDButtonField(gamepad_input, Mapping_GetUsage(2), BUTTON_IsPressed(BUTTON_C));
        // Button 3: X
        SetHIDButtonField(gamepad_input, Mapping_GetUsage(3), BUTTON_IsPressed(BUTTON_X));
        // Button 4: Y
        SetHIDButtonField(gamepad_input, Mapping_GetUsage(4), BUTTON_IsPressed(BUTTON_Y));
        // Button 5: Z
        SetHIDButtonField(gamepad_input, Mapping_GetUsage(5), BUTTON_IsPressed(BUTTON_Z));
        // Button 6: L1
        SetHIDButtonField(gamepad_input, Mapping_GetUsage(6), BUTTON_IsPressed(BUTTON_TL));
        // Button 7: R1
        SetHIDButtonField(gamepad_input, Mapping_GetUsage(7), BUTTON_IsPressed(BUTTON_TR));

        // D-Padを直接HATスイッチとして処理（マッピングなし、メモリ効率良）
        if(BUTTON_IsPressed(BUTTON_UP) && BUTTON_IsPressed(BUTTON_LEFT)) {
            gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_NORTH_WEST;
        } else if(BUTTON_IsPressed(BUTTON_UP) && BUTTON_IsPressed(BUTTON_RIGHT)) {
            gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_NORTH_EAST;
        } else if(BUTTON_IsPressed(BUTTON_DOWN) && BUTTON_IsPressed(BUTTON_LEFT)) {
            gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_SOUTH_WEST;
        } else if(BUTTON_IsPressed(BUTTON_DOWN) && BUTTON_IsPressed(BUTTON_RIGHT)) {
            gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_SOUTH_EAST;
        } else if(BUTTON_IsPressed(BUTTON_UP)) {
            gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_NORTH;
        } else if(BUTTON_IsPressed(BUTTON_RIGHT)) {
            gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_EAST;
        } else if(BUTTON_IsPressed(BUTTON_DOWN)) {
            gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_SOUTH;
        } else if(BUTTON_IsPressed(BUTTON_LEFT)) {
            gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_WEST;
        }

    } else if(flags.sw_flag == true) {
        // 代替モード - マッピングを使用
        // Button 0: A - 基本ボタンは通常通り
        SetHIDButtonField(gamepad_input, Mapping_GetUsage(0), BUTTON_IsPressed(BUTTON_A));
        // Button 1: B
        SetHIDButtonField(gamepad_input, Mapping_GetUsage(1), BUTTON_IsPressed(BUTTON_B));
        // Button 2: C
        SetHIDButtonField(gamepad_input, Mapping_GetUsage(2), BUTTON_IsPressed(BUTTON_C));
        // 特殊ボタンのマッピング
        // Button 8: L2 (TL)
        SetHIDButtonField(gamepad_input, Mapping_GetUsage(8), BUTTON_IsPressed(BUTTON_TL));
        // Button 9: R2 (TR)
        SetHIDButtonField(gamepad_input, Mapping_GetUsage(9), BUTTON_IsPressed(BUTTON_TR));
        // Button 10: left_stick (Y)
        SetHIDButtonField(gamepad_input, Mapping_GetUsage(10), BUTTON_IsPressed(BUTTON_Y));
        // Button 11: right_stick (X)
        SetHIDButtonField(gamepad_input, Mapping_GetUsage(11), BUTTON_IsPressed(BUTTON_X));
        // Button 12: home (Z)
        SetHIDButtonField(gamepad_input, Mapping_GetUsage(12), BUTTON_IsPressed(BUTTON_Z));      
    }


    // アナログスティック処理 - より省メモリな実装
    // X軸 (左右)
    gamepad_input->members.analog_stick.X = 128;  // デフォルト中央
    if(BUTTON_IsPressed(BUTTON_LEFT)){
        gamepad_input->members.analog_stick.X = 0;   // 左
    }else if(BUTTON_IsPressed(BUTTON_RIGHT)){
        gamepad_input->members.analog_stick.X = 255; // 右
    }
    
    // Y軸 (上下)
    gamepad_input->members.analog_stick.Y = 128;  // デフォルト中央
    if(BUTTON_IsPressed(BUTTON_UP)){
        gamepad_input->members.analog_stick.Y = 0;   // 上
    }else if(BUTTON_IsPressed(BUTTON_DOWN)){
        gamepad_input->members.analog_stick.Y = 255; // 下
    }
    
    // 他のアナログ値を標準の128に設定
    gamepad_input->members.analog_stick.Z = 128;
    gamepad_input->members.analog_stick.Rz = 128;
    
    // // HATスイッチの初期化は、クロスキー処理なしの場合にのみ必要
    // 上記でD-padをHATスイッチとして処理した場合は、ここで初期化しない
    
    // 特別な組み合わせボタン - 例：START+L同時押しでHOMEボタン
    if (flags.sw_flag == false && BUTTON_IsPressed(BUTTON_START) && BUTTON_IsPressed(BUTTON_TL)) {
        SetHIDButtonField(gamepad_input, Mapping_GetUsage(12), true);
    }

    // initialize z axis and rz axis 
    gamepad_input->val[5] = 0x80;
    gamepad_input->val[6] = 0x80;        

    // hat switch mode
    switch(flags.crosskey_flag){
        case 0:     //xy mode
        /*DO NOTHING*/            
        break;
        
        case 1:     //hat sw mode
            if((gamepad_input->val[3] == 0x00) && (gamepad_input->val[4] == 0x80)){
                gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_WEST;
            } else if((gamepad_input->val[3] == 0x00) && (gamepad_input->val[4] == 0x00)){
                gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_NORTH_WEST;
            } else if ((gamepad_input->val[3] == 0x80) && (gamepad_input->val[4] == 0x00)){
                gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_NORTH;
            } else if ((gamepad_input->val[3] == 0xFF) && (gamepad_input->val[4] == 0x00)){
                gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_NORTH_EAST;
            } else if ((gamepad_input->val[3] == 0xFF) && (gamepad_input->val[4] == 0x80)){
                gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_EAST;
            } else if ((gamepad_input->val[3] == 0xFF) && (gamepad_input->val[4] == 0xFF)){
                gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_SOUTH_EAST;
            } else if ((gamepad_input->val[3] == 0x80) && (gamepad_input->val[4] == 0xFF)){
                gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_SOUTH;
            } else if ((gamepad_input->val[3] == 0x00) && (gamepad_input->val[4] == 0xFF)){
                gamepad_input->members.hat_switch.hat_switch = HAT_SWITCH_SOUTH_WEST;
            } 

            gamepad_input->val[3] = 0x80;
            gamepad_input->val[4] = 0x80;
            
            break;
    
        case 2:     // z rz mode
            gamepad_input->val[5] = gamepad_input->val[3];
            gamepad_input->val[6] = gamepad_input->val[4];

            gamepad_input->val[3] = 0x80;
            gamepad_input->val[4] = 0x80;            
            
            break;
            
        default:
            /*DO NOTHING*/
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