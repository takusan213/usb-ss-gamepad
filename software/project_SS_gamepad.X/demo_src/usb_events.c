/*******************************************************************************
Copyright 2016 Microchip Technology Inc. (www.microchip.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

To request to license the code under the MLA license (www.microchip.com/mla_license), 
please contact mla_licensing@microchip.com
*******************************************************************************/

/*********************************************************************
 * This file is modified by Geeky Fab.
 * 
 * Changes from the original source:
 *     - deleted unused header file inclusion
 ********************************************************************/

/** INCLUDES *******************************************************/
#include "system.h"

#include "usb.h"
#include "usb_device_hid.h"
#include "usb_framework/inc/usb_ch9.h"
#include "usb_framework/inc/usb_device.h"

#include "app_device_joystick.h"
#include "mapping.h"
#include "demo_src/hid_rpt_map.h"

/*******************************************************************
 * Function:        bool USER_USB_CALLBACK_EVENT_HANDLER(
 *                        USB_EVENT event, void *pdata, uint16_t size)
 *
 * PreCondition:    None
 *
 * Input:           USB_EVENT event - the type of event
 *                  void *pdata - pointer to the event data
 *                  uint16_t size - size of the event data
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is called from the USB stack to
 *                  notify a user application that a USB event
 *                  occured.  This callback is in interrupt context
 *                  when the USB_INTERRUPT option is selected.
 *
 * Note:            None
 *******************************************************************/
bool USER_USB_CALLBACK_EVENT_HANDLER(USB_EVENT event, void *pdata, uint16_t size)
{
    switch( (int) event )
    {
        case EVENT_TRANSFER:
            break;

        case EVENT_SOF:
            /* We are using the SOF as a timer to time the LED indicator.  Call
             * the LED update function here. */
//            APP_LEDUpdateUSBStatus();
            break;

        case EVENT_SUSPEND:
            /* Update the LED status for the suspend event. */
//            APP_LEDUpdateUSBStatus();

            //Call the hardware platform specific handler for suspend events for
            //possible further action (like optionally going reconfiguring the application
            //for lower power states and going to sleep during the suspend event).  This
            //would normally be done in USB compliant bus powered applications, although
            //no further processing is needed for purely self powered applications that
            //don't consume power from the host.
            SYSTEM_Initialize(SYSTEM_STATE_USB_SUSPEND);
            break;

        case EVENT_RESUME:
            /* Update the LED status for the resume event. */
//            APP_LEDUpdateUSBStatus();

            //Call the hardware platform specific resume from suspend handler (ex: to
            //restore I/O pins to higher power states if they were changed during the 
            //preceding SYSTEM_Initialize(SYSTEM_STATE_USB_SUSPEND) call at the start
            //of the suspend condition.
            SYSTEM_Initialize(SYSTEM_STATE_USB_RESUME);
            break;

        case EVENT_CONFIGURED:
            /* When the device is configured, we can (re)initialize the demo
             * code. */
            APP_DeviceJoystickInitialize();
            break;

        case EVENT_SET_DESCRIPTOR:
            break;

        case EVENT_EP0_REQUEST:
            /* We have received a non-standard USB request.  The HID driver
             * needs to check to see if the request was for it. */
            USBCheckHIDRequest();
            break;

        case EVENT_BUS_ERROR:
            break;

        case EVENT_TRANSFER_TERMINATED:
            break;

        default:
            break;
    }
    return true;
}

// Feature report buffers for two interfaces
static uint8_t featureBuf[64];            // Interface 0 受信用バッファ
static uint8_t mapFeatureBuf[64];         // Interface 1 mapping feature buffer
static USB_HANDLE featureRxHandle = 0;    // ハンドルはグローバルで共有

/* ---------- SET_REPORT handler for both interfaces ---------- */
void HIDFeatureReceive(void)
{
    uint8_t reportID = SetupPkt.W_Value.byte.LB;  // Report ID is in the low byte of wValue
    uint8_t interfaceNum = SetupPkt.W_Index.byte.LB;  // Interface number is in the low byte of wIndex
    
    // Handle Interface 0 with Report ID 2 (old implementation)
    if (interfaceNum == 0 && reportID == 0x02) {
        /* まだ転送中なら何もしない */
        if(HIDRxHandleBusy(featureRxHandle)) return;

        /* EP番号は JOYSTICK_EP で OK。64B 受信を予約 */
        featureRxHandle = HIDRxPacket(JOYSTICK_EP, featureBuf, sizeof(featureBuf));
    }
    // Handle Interface 1 with Report ID 1 (new mapping interface)
    else if (interfaceNum == 1 && reportID == 0x01) {
        // Check if this is SET_REPORT (from host to device)
        if (SetupPkt.bRequest == SET_REPORT) {
            // SET_REPORT - receive data from host via control transfer
            USBEP0Receive(mapFeatureBuf, sizeof(mapFeatureBuf), NULL);

            // Process the mapping data
            // Note: The actual feature report data will be processed after EP0 transfer completes
            // in the USB interrupt, but for simplicity we'll process it here
            Mapping_SetFromFeatureReport(mapFeatureBuf, sizeof(mapFeatureBuf));
        } 
        else if (SetupPkt.bRequest == GET_REPORT) {
            // GET_REPORT - send data to host
            // Prepare feature report data
            Mapping_GetAsFeatureReport(mapFeatureBuf);
            
            // Send the data back to the host through endpoint 0
            USBEP0SendRAMPtr(mapFeatureBuf, sizeof(mapFeatureBuf), USB_EP0_INCLUDE_ZERO);
        }
    }
}

/* ---------- ② 64B受信し終わったとき自動で呼ばれる ---------- */
void USBCB_HIDSetReportComplete(void)
{
    /* 受信済みか確認してバッファを取り出す */
    if(!HIDRxHandleBusy(featureRxHandle))
    {
        /* Report ID チェック */
        if(featureBuf[0] == 0x02)
        {
            Mapping_Save(&featureBuf[1]);     // 先頭14Bを保存
        }

        /* 次のSET_REPORTに備えてハンドルを無効化 */
        featureRxHandle = 0;
    }
}

/*******************************************************************************
End of File
*/
