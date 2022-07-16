# USB SS Gamepad
![組み込み例](https://user-images.githubusercontent.com/78714269/179347633-de9f5a54-9d00-4a38-ac10-f1a9d2fb87a7.png)

This project remakes your memorable SS Controller (aka Saturn pad) into a usb gamepad:video_game:  
Let's make a USB SS Gamepad and play retro games as well as recent games distributed on Steam. 

There is a Japanese support page in my website:crossed_flags:  
https://geekyfab.com/support_ss_gamepad  
You can read more details there.   

I also made a USB SFC(SNES) Gamepad [here](https://github.com/takusan213/usb-sfc-pad).  

# Demo
![mario_play](https://user-images.githubusercontent.com/78714269/179347927-e5b2c88f-ef48-4632-ad3e-50c2a95bd300.gif)

 
# Features
In this project, I share circuit, BOM and firmware.  
I will share the PCB gerber data as soon as it is ready...  

You order your PCB with the gerber data, buy the materials on BOM, burn the firmware and assemble all, then you can get your USB SS Gamepad.
Though you need to use a driver and a soldering iron, it is fun and worth a try.


# PCB
![編集後_部品面2_トリミング](https://user-images.githubusercontent.com/78714269/179347685-6aabf8bc-b9a2-4c2b-8bdd-f7c7f21ef905.png)

![編集後_ボタン面](https://user-images.githubusercontent.com/78714269/179347687-01cab26c-ae13-43e7-a023-e4baac48ee76.png)


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
1. Remove the five screws and uncover your saturn pad.  
![kumitate1](https://user-images.githubusercontent.com/78714269/179348472-7009828f-4c05-46de-bb2b-78cf241487e9.jpg)

![kumitate2](https://user-images.githubusercontent.com/78714269/179348477-6b210a56-29ad-41c9-89b3-5e7031e6a34a.jpg)


2. remove the original PCB and put in the USB SS Gamepad PCB.  
![kumitate3](https://user-images.githubusercontent.com/78714269/179348482-82c8f7bc-3f49-46b4-8780-88f4707333d2.jpg)

![kumitate4](https://user-images.githubusercontent.com/78714269/179348488-217f2fbe-7464-4c6a-ab62-2dff81e90f9a.jpg)


3. Cover the saturn pad and tighten the screws.  
![kumitate5_2](https://user-images.githubusercontent.com/78714269/179348809-a94612ee-4487-47a2-8815-99f53d779b70.jpg)

# Author
* Takusan = [GEEKY Fab.](geekyfab.com)  
 
# License
The software files of "USB SS Gamepad" are under [Apache license 2.0](https://www.apache.org/licenses/LICENSE-2.0).  
The hardware files of "USB SS Gamepad" are under [CC BY-NC-SA 4.0](https://creativecommons.org/licenses/by-nc-sa/4.0/).  
