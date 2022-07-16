# USB SS Gamepad

This project remakes your memorable SS Controller (aka Saturn pad) into a usb gamepad:video_game:  
Let's make a USB SS Gamepad and play retro games as well as recent games distributed on Steam. 

There is a Japanese support page in my website:crossed_flags:  
https://geekyfab.com/support_ss_gamepad  
You can read more details there.   

I also made a USB SFC(SNES) Gamepad [here](https://github.com/takusan213/usb-sfc-pad).  

 
# Features
In this project, I share circuit, BOM and firmware.  
I will share the PCB gerber data as soon as it is ready...  

You order your PCB with the gerber data, buy the materials on BOM, burn the firmware and assemble all, then you can get your USB SS Gamepad.
Though you need to use a driver and a soldering iron, it is fun and worth a try.


# Tested OS / hardware
- Windows 10
- Windows 11
- Raspberry PI 3
    - Raspberry Pi OS 
    - Recalbox for RASPBERRY PI 3
    - RetroPi 4.7.1 for RASPBERRY PI 2/3
- Raspberry PI 4
    - Raspberry Pi OS
    - Recalbox for RASPBERRY PI 4/400
    - RetroPie 4.7.1 for RASPBERRY PI 4/400
- Retro Freak

# Usage
## Windows
In a Windows environment, USB SS Gamepad runs with standard driver.  
You don't need to install additional drivers.  

## Raspberry PI OS
In a Raspberry PI OS environment, you need to install 'joystick' package like this
```bash
sudo apt install joystick
```
It is also good to install 'jstest-gtk' for operation test.
```bash
sudo apt install jstest-gtk
```

## Recalbox and RetroPie
In Recalbox and RetroPie, you don't need to install 'joystick' package.  
Maybe Recalbox and RetroPie install it automatically.

# Manufacturing
To make a PCB you need to download the zipped gerber files and upload them to a PCB manufacturer like AllPCB.  
When you order the PCB, it is better to select "gold-plating" option so that conduction between pads of the PCB and rubber buttons of SFC controller keep well.  
...Having said that, I also manufactured a "tin-plating" version and it just work in the beggining:clock1:  
(I sometimes felt that the "tin-plating" version didn't response.)
 
# Assembly
1. Remove the screws and uncover your saturn pad.  

2. remove the original PCB and put in the USB SS Gamepad PCB.  


3. Cover the saturn pad and tighten the screws.  


# Author
* Takusan = [GEEKY Fab.](geekyfab.com)  
 
# License
The software files of "USB SS Gamepad" are under [Apache license 2.0](https://www.apache.org/licenses/LICENSE-2.0).  
The hardware files of "USB SS Gamepad" are under [CC BY-NC-SA 4.0](https://creativecommons.org/licenses/by-nc-sa/4.0/).  