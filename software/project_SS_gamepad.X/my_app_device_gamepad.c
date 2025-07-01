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

/* ----------------------------------------------------------------------------
   mapping.h などで定義済み
   NUM_BUTTONS = 14
   BUTTON_* 列挙体は 0..13 の物理ボタン番号に対応している想定
---------------------------------------------------------------------------- */

/* ────────────────────────────────────────────────────────────────────────────
   usageByte[usage]: 
     usage (1–14) が INPUT_CONTROLS.val[] の何バイト目に対応するか
   ──────────────────────────────────────────────────────────────────────────── */
static const uint8_t usageByte[15] = {
    /*  0: 未使用 (無効) */  0,
    /*  1: A            */  0,    // val[0].bit0
    /*  2: B            */  0,    // val[0].bit1
    /*  3: C            */  0,    // val[0].bit2
    /*  4: X            */  0,    // val[0].bit3
    /*  5: Y            */  0,    // val[0].bit4
    /*  6: Z            */  0,    // val[0].bit5
    /*  7: L1           */  0,    // val[0].bit6
    /*  8: R1           */  0,    // val[0].bit7
    /*  9: Start        */  1,    // val[1].bit0
    /* 10: L2           */  1,    // val[1].bit1
    /* 11: R2           */  1,    // val[1].bit2
    /* 12: Home         */  1,    // val[1].bit3
    /* 13: Right Stick  */  1,    // val[1].bit4
    /* 14: Left Stick   */  1     // val[1].bit5
};

/* ────────────────────────────────────────────────────────────────────────────
   usageMask[usage]:
     各 usage (1–14) がそのバイト内で何ビット目かを表すマスク
   ──────────────────────────────────────────────────────────────────────────── */
static const uint8_t usageMask[15] = {
    /*  0: 無効      */  0x00,
    /*  1: A         */  1 << 0,  // val[0] bit0 → a
    /*  2: B         */  1 << 1,  // val[0] bit1 → b
    /*  3: C         */  1 << 2,  // val[0] bit2 → c
    /*  4: X         */  1 << 3,  // val[0] bit3 → x
    /*  5: Y         */  1 << 4,  // val[0] bit4 → y
    /*  6: Z         */  1 << 5,  // val[0] bit5 → z
    /*  7: L1        */  1 << 6,  // val[0] bit6 → L1
    /*  8: R1        */  1 << 7,  // val[0] bit7 → R1
    /*  9: Start     */  1 << 0,  // val[1] bit0 → start
    /* 10: L2        */  1 << 1,  // val[1] bit1 → L2
    /* 11: R2        */  1 << 2,  // val[1] bit2 → R2
    /* 12: Home      */  1 << 3,  // val[1] bit3 → home
    /* 13: RightStick*/  1 << 4,  // val[1] bit4 → right_stick
    /* 14: LeftStick */  1 << 5   // val[1] bit5 → left_stick
};

/* 物理 idx をハードボタンに変換して押下を調べる関数 */
static bool isPhysPressed(uint8_t phys)
{
    switch(phys){
        case 0:  return BUTTON_IsPressed(BUTTON_A);
        case 1:  return BUTTON_IsPressed(BUTTON_B);
        case 2:  return BUTTON_IsPressed(BUTTON_C);
        case 3:  return BUTTON_IsPressed(BUTTON_X);
        case 4:  return BUTTON_IsPressed(BUTTON_Y);
        case 5:  return BUTTON_IsPressed(BUTTON_Z);
        case 6:  return BUTTON_IsPressed(BUTTON_TL);   // L
        case 7:  return BUTTON_IsPressed(BUTTON_TR);   // R
        case 8:  return BUTTON_IsPressed(BUTTON_START);
        default: return false;   // no physical button for this index
    }
}

// The HIDFeatureReceive function has been moved to usb_events.c
// to handle both Interface 0 and Interface 1 Feature reports

void App_DeviceGamepadInit(void){
    flags.crosskey_flag = 0;
    flags.sw_flag = false;
}


void App_DeviceGamepadAct(INPUT_CONTROLS* gamepad_input){

    // No Report ID in Interface 0
    
    // Clear all button fields and data by zeroing all bytes
    memset(gamepad_input->val, 0, sizeof(gamepad_input->val));
    
    
    // D-Padの状態を取得（全ての処理で使えるように上部で定義）
    bool up = BUTTON_IsPressed(BUTTON_UP);
    bool down = BUTTON_IsPressed(BUTTON_DOWN);
    bool left = BUTTON_IsPressed(BUTTON_LEFT);
    bool right = BUTTON_IsPressed(BUTTON_RIGHT);

    for (uint8_t phys = 0; phys < NUM_BUTTONS; phys++){
        if(!isPhysPressed(phys)) continue;          // 押されていなければスキップ

        uint8_t usage = Mapping_GetUsage(phys, flags.sw_flag);  // sw_flagでモード選択
        if(!usage || usage > 14) continue;             // 無効は無視

        uint8_t idx  = usageByte[usage];               // val のバイト番号
        uint8_t mask = usageMask[usage];               // 立てるビット
        gamepad_input->val[idx] |= mask;               // ワンショットでセット
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
            
        // モード1: HATスイッチ
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
            
        // モード2: Z/RZ
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