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

#include "system.h"

#define BUTTON_X        PORTAbits.RA5
#define BUTTON_TR       PORTAbits.RA4
#define BUTTON_Y        PORTCbits.RC5
#define BUTTON_Z        PORTCbits.RC4
#define BUTTON_C        PORTCbits.RC3
#define BUTTON_B        PORTCbits.RC6
#define BUTTON_A        PORTCbits.RC7
#define BUTTON_START    PORTBbits.RB7
#define BUTTON_LEFT     PORTBbits.RB6
#define BUTTON_UP       PORTBbits.RB5
#define BUTTON_RIGHT    PORTBbits.RB4
#define BUTTON_DOWN     PORTCbits.RC2
#define BUTTON_TL       PORTCbits.RC1
