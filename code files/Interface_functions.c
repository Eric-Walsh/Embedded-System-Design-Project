// *************************************************************************//
// Filename: Interface_functions.c //
// Author: E.Walsh //
// Version: 3/14/21 written //
// Processor: NXP MKL25Z4 //
// Compiler: Keil uVision5 //
// Library: CMSIS core and device startup //
// also needs ADC_Functions.c, LCD_Functions.c, Calculations.c and Recordings.c //
// Hardware: MKL25Z128VLH4 Microcontroller connected to 4 switches, a DAC, an ADC, 2 LEDS, and a 2x8 LCD   //
// Operation: Handles the displaying of information to the LCD and the result of switch interrupts. //
// Also stores the arrays that hold the ASCII character codes for the main menu and the file menu  //
// *************************************************************************//
#include <MKL25Z4.h>
#include <stdio.h>
#include <string.h>
#include "LCD_Functions.h"
#include "Calculations.h"
#include "ADC_Functions.h"
#include "Recordings.h"
#include "Interface_Functions.h"

//enum that holds the menu states
enum Menu_State{Main, File} Menu;
//Main menu:
/* BPM:___  \\
\\ S:___ F:_ */
//File select menu:
/* File 1:  \\
\\ File 2:   */

//arrays that hold the menu visuals
uint32_t Main_Menu[16];
uint32_t File_Menu[16];

//global variables
int currentFile, cursorPos, SendFlag;

/*----------------------------------------------------------------------------
    Initializes the UI
    Also presets the sample rate and save file              E.Walsh
*----------------------------------------------------------------------------*/
void init_UI(void){
    //Enum to show which state the menu currently is in
		Menu = Main;
    init_Menu();
    //currentFile determines which array recordings are sent to
    currentFile = 1;
    //cursorPos determines where the cursor is when on the file select menu
    cursorPos = 1;
    //send flag is 1 when a recording is requested ti eb sent to the DAC
		SendFlag = 0;
    init_SampleRate();
    UpdateMenu(Main_Menu);
		delayMs(100);
		UpdateMenu(Main_Menu);
}

/*----------------------------------------------------------------------------
    Takes the switch input and activates the correct function
    dependant on the Menu state                             E.Walsh
*----------------------------------------------------------------------------*/
void Input(int switch_num){
	int MenuFlag;
		if(Menu == Main){
			MenuFlag = 0;
		} else {
			MenuFlag = 1;
		}
    switch (Menu){
        case Main:
            if(switch_num == 1){
                //increase sample rate
                ChangeSampleRate(1);
            } else if(switch_num == 2){
                //decrease sample rate
                ChangeSampleRate(-1);
            } else if(switch_num == 3){
                //enter file state
                MenuFlag = 1;
            } else if(switch_num == 4){
                //Start recording
                StartRecording(currentFile);
            }
            break;
        
        case File: 
            if(switch_num == 1){
                //move courser up
                MoveCursor(1);
            } else if(switch_num == 2){
                //move coursor down
                MoveCursor(-1);
            } else if(switch_num == 3){
                //select file and enter menu state
                SelectFile();
                MenuFlag = 0;
            } else if(switch_num == 4){
								if(SendFlag){
									SendFlag = 0;
								} else if (!SendFlag){
									SendFlag = 1;
								}
                //Send selected file to DAC
                sendToDAC(currentFile, SendFlag);
            }
            break;
    }
		
		if(MenuFlag){
			Menu = File;
		}else if(!MenuFlag){
			Menu = Main;
		}
		
    //update UI
    switch (Menu) {
        case Main:
            UpdateMenu(Main_Menu);
				delayMs(100);
						UpdateMenu(Main_Menu);
            break;
        case File:
            UpdateMenu(File_Menu);
				delayMs(100);
						UpdateMenu(File_Menu);
            break;

    }

}


/*----------------------------------------------------------------------------
    Updates the menu                                        E.Walsh
*----------------------------------------------------------------------------*/
void UpdateMenu(uint32_t MenuString[]){
    displayString(MenuString, 16);
}

/*----------------------------------------------------------------------------
    Updates the displayed BPM                               E.Walsh
*----------------------------------------------------------------------------*/
void UpdateBPM(int beats){
    //an array to hold the hexideximal ACII values of the BPM
    uint32_t beatArray[3] = {0x0, 0x0, 0x0};
    beatArray[2] = ((beats % 10) | 0x30);
    beats /= 10;
    beatArray[1] = ((beats % 10) | 0x30);
    beats /= 10;
    beatArray[0] = ((beats % 10) | 0x30);

    Main_Menu[4] = beatArray[0];
    Main_Menu[5] = beatArray[1];
    Main_Menu[6] = beatArray[2];
	if(Menu == Main){
    UpdateMenu(Main_Menu);
		delayMs(100);
		UpdateMenu(Main_Menu);
	};
}

/*----------------------------------------------------------------------------
    Updates the displayed Sample Rate                       E.Walsh
*----------------------------------------------------------------------------*/
void UpdateSampleRate(int rate){
    //array to hold the ACII values for the new sample rate
    uint32_t newRate[3];
    RateConverter(newRate, rate);

    Main_Menu[10] = newRate[0];
    Main_Menu[11] = newRate[1];
    Main_Menu[12] = newRate[2];
}

/*----------------------------------------------------------------------------
    Moves the cursor within the file menu                   E.Walsh
*----------------------------------------------------------------------------*/
void MoveCursor(int change){
    if(cursorPos == 1 && change != -1){
        cursorPos += change;
    } else if(cursorPos == 2 && change != 1){
        cursorPos += change;
    }

    if(cursorPos == 1){
        File_Menu[7] = 0x2D;
        File_Menu[6] = 0x3C;
				File_Menu[15] = 0x20;
        File_Menu[14] = 0x20;
    } else if (cursorPos == 2){
				File_Menu[7] = 0x20;
        File_Menu[6] = 0x20;
        File_Menu[15] = 0x2D;
        File_Menu[14] = 0x3C;
    }
		
		UpdateMenu(File_Menu);
		delayMs(100);
		UpdateMenu(File_Menu);
}

/*----------------------------------------------------------------------------
    Changes the current save file                            E.Walsh
*----------------------------------------------------------------------------*/
void SelectFile(void){
    if(cursorPos != currentFile){
        currentFile = cursorPos;
        if(currentFile == 1){
            File_Menu[5] = 0x2A;
						File_Menu[13] = 0x20;
						Main_Menu[15] = 0x31;
        } else if (currentFile == 2){
            File_Menu[13] = 0x2A;
						File_Menu[5] = 0x20;
						Main_Menu[15] = 0x32;
        }
    }
		UpdateMenu(File_Menu);
		delayMs(100);
		UpdateMenu(File_Menu);
}

/*----------------------------------------------------------------------------
    Turns the sample rate from decimal to hex
    It's incredibly inefficient                               E.Walsh
*----------------------------------------------------------------------------*/
void RateConverter(uint32_t *hexArray, int rate){
    if(rate == 0){
        hexArray[0] = 0x20;
        hexArray[1] = 0x35;
        hexArray[2] = 0x30;
    } else if (rate == 1){
        hexArray[0] = 0x31;
        hexArray[1] = 0x30;
        hexArray[2] = 0x30;
    } else if (rate == 2){
        hexArray[0] = 0x31;
        hexArray[1] = 0x35;
        hexArray[2] = 0x30;
    } else if (rate == 3){
        hexArray[0] = 0x32;
        hexArray[1] = 0x30;
        hexArray[2] = 0x30;
    } else if (rate == 4){
        hexArray[0] = 0x32;
        hexArray[1] = 0x35;
        hexArray[2] = 0x30;
    } else if (rate == 5){
        hexArray[0] = 0x33;
        hexArray[1] = 0x30;
        hexArray[2] = 0x30;
    } else if (rate == 6){
        hexArray[0] = 0x33;
        hexArray[1] = 0x35;
        hexArray[2] = 0x30;
    } else if (rate == 7){
        hexArray[0] = 0x34;
        hexArray[1] = 0x30;
        hexArray[2] = 0x30;
    } else if (rate == 8){
        hexArray[0] = 0x34;
        hexArray[1] = 0x35;
        hexArray[2] = 0x30;
    } else if (rate == 9){
        hexArray[0] = 0x35;
        hexArray[1] = 0x30;
        hexArray[2] = 0x30;
    } else if (rate == 10){
        hexArray[0] = 0x35;
        hexArray[1] = 0x35;
        hexArray[2] = 0x30;
    } else if (rate == 11){
        hexArray[0] = 0x36;
        hexArray[1] = 0x30;
        hexArray[2] = 0x30;
    } else if (rate == 12){
        hexArray[0] = 0x36;
        hexArray[1] = 0x35;
        hexArray[2] = 0x30;
    } else if (rate == 13){
        hexArray[0] = 0x37;
        hexArray[1] = 0x30;
        hexArray[2] = 0x30;
    } else if (rate == 14){
        hexArray[0] = 0x37;
        hexArray[1] = 0x35;
        hexArray[2] = 0x30;
    } else if (rate == 15){
        hexArray[0] = 0x38;
        hexArray[1] = 0x30;
        hexArray[2] = 0x30;
    } else if (rate == 16){
        hexArray[0] = 0x38;
        hexArray[1] = 0x35;
        hexArray[2] = 0x30;
    } else if (rate == 17){
        hexArray[0] = 0x39;
        hexArray[1] = 0x30;
        hexArray[2] = 0x30;
    } else if (rate == 18){
        hexArray[0] = 0x39;
        hexArray[1] = 0x35;
        hexArray[2] = 0x30;
    } else if (rate == 19){
        hexArray[0] = 0x31;
        hexArray[1] = 0x30;
        hexArray[2] = 0x68;
    } else if (rate == 20){
        hexArray[0] = 0x31;
        hexArray[1] = 0x35;
        hexArray[2] = 0x68;
    } else if (rate == 21){
        hexArray[0] = 0x32;
        hexArray[1] = 0x30;
        hexArray[2] = 0x68;
    } else if (rate == 22){
        hexArray[0] = 0x32;
        hexArray[1] = 0x35;
        hexArray[2] = 0x68;
    } else if (rate == 23){
        hexArray[0] = 0x33;
        hexArray[1] = 0x30;
        hexArray[2] = 0x68;
    } else if (rate == 24){
        hexArray[0] = 0x33;
        hexArray[1] = 0x35;
        hexArray[2] = 0x68;
    } else if (rate == 25){
        hexArray[0] = 0x34;
        hexArray[1]= 0x30;
        hexArray[2] = 0x68;
    } else if (rate == 26){
        hexArray[0] = 0x34;
        hexArray[1] = 0x35;
        hexArray[2] = 0x68;
    } else if (rate == 27){
        hexArray[0] = 0x35;
        hexArray[1] = 0x30;
        hexArray[2] = 0x68;
    } else if (rate == 28){
        hexArray[0] = 0x35;
        hexArray[1] = 0x35;
        hexArray[2] = 0x68;
    } else if (rate == 29){
        hexArray[0] = 0x36;
        hexArray[1] = 0x30;
        hexArray[2] = 0x68;
    } else if (rate == 30){
        hexArray[0] = 0x36;
        hexArray[1] = 0x35;
        hexArray[2] = 0x68;
    } else if (rate == 31){
        hexArray[0] = 0x37;
        hexArray[1] = 0x30;
        hexArray[2] = 0x68;
    } else if (rate == 32){
        hexArray[0] = 0x37;
        hexArray[1] = 0x35;
        hexArray[2] = 0x68;
    } else if (rate == 33){
        hexArray[0] = 0x38;
        hexArray[1] = 0x30;
        hexArray[2] = 0x68;
    } else if (rate == 34){
        hexArray[0] = 0x38;
        hexArray[1] = 0x35;
        hexArray[2] = 0x68;
    } else if (rate == 35){
        hexArray[0] = 0x39;
        hexArray[1] = 0x30;
        hexArray[2] = 0x68;
    } else if (rate == 36){
        hexArray[0] = 0x39;
        hexArray[1] = 0x35;
        hexArray[2] = 0x68;
    } else if (rate == 37){
        hexArray[0] = 0x31;
        hexArray[1] = 0x30;
        hexArray[2] = 0x6B;
    }
}

/*----------------------------------------------------------------------------
    Initializes UI graphics                                   E.Walsh
*----------------------------------------------------------------------------*/
void init_Menu(void){
	//Sets the framework for the main menu
	Main_Menu[0] = 0x42;
	Main_Menu[1] = 0x50;
	Main_Menu[2] = 0x4D;
	Main_Menu[3] = 0x3A;
	Main_Menu[4] = 0x2D;
	Main_Menu[5] = 0x2D;
	Main_Menu[6] = 0x2D;
	Main_Menu[7] = 0x20;
	Main_Menu[8] = 0x53;
	Main_Menu[9] = 0x3A;
	Main_Menu[10] = 0x31;
	Main_Menu[11] = 0x30;
	Main_Menu[12] = 0x68;
	Main_Menu[13] = 0x20;
	Main_Menu[14] = 0x46;
	Main_Menu[15] = 0x31;
	
	//Sets the framework for the File menu
	File_Menu[0] = 0x46;
	File_Menu[1] = 0x69;
	File_Menu[2] = 0x6C;
	File_Menu[3] = 0x65;
	File_Menu[4] = 0x31;
	File_Menu[5] = 0x2A;
	File_Menu[6] = 0x3C;
	File_Menu[7] = 0x2D;
	File_Menu[8] = 0x46;
	File_Menu[9] = 0x69;
	File_Menu[10] = 0x6C;
	File_Menu[11] = 0x65;
	File_Menu[12] = 0x32;
	File_Menu[13] = 0x20;
	File_Menu[14] = 0x20;
	File_Menu[15] = 0x20;
}
