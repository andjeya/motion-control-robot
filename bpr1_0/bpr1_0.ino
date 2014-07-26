/* TO-DO IN PROGRESS
 * Write bluetooth function.
 * Complete functions getPanoPref, getRtvPref, getTimelapsePref and getTrigPref. See variable documentation and code for getMode. 
 * Other functions you will need:
 * Horizontal Panorama algorithm
 * Vertical Panorama algorithm
 * Brenzier Panorama algorithm
 * Lighting trigger function (and ability to move camera)
 * RTV straight function
 * RTV curve function
 * Timelapse straight function (will need further information about how slowly it should move, etc.)
 * Timelapse curve function (will need further information about how slowly it should move, etc.)
 * HDR pref function (specify brackets, runs right before every algorithm)
 * Take picture function (pass HDR pref info to know # of shots)
 * Bluetooth connection function? Specify bluetooth info? 
 * Reminders function (make sure manual focus, if using timelapse, flip servo switch, etc.)
 * Write preferences function for bluetooth? (ie. connection settings if needed?). If so, update getModePref case #4
 */

/***************************************************************************
 * ROBOTIC PANORAMA DEVICE CODE
 * Version 1.0
 * ANDJEY ASHWILL
 * ***REMOVED***
 * JUNE 27, 2014
 *
 * DESCRIPTION: This software programs the Ardunio Uno microcontroller platform for use with the Brenzier Panoramic Robot, 
 * allowing for the capture of panoramas, brenzier panoramas, timelapse images and lighting strike captures using standard 
 * single images or multiple images to create full HDR. The software also allows remote control over the physical positioning 
 * of a camera via a computer and may be adapted in future iterations to include full PTP USB support for the Canon 450D and 
 * control over camera/robot features via a bluetooth module and an android app. 
 ***************************************************************************/

//INCLUDING LIBRARIES
#include <Wire.h> //including support for I2C controller
#include <Adafruit_MCP23017.h> //library for adafruit LCD controller
#include <Adafruit_RGBLCDShield.h> //library for adafruit LCD controller

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield(); //NEEDED FOR LCD CONTROLLER. MORE DOCUMENTIATION NEEDED. 

//DEFINING BACKLIGHT COLORS
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

/***************************************************************************
 *     Function Information
 *     Name of Function: setup
 *     Function Return Type: void
 * 
 *     Parameters (list data type, name, and comment one per line):
 *       1. Function accepts no parameters 
 *
 *     Function Description: Function runs once to set stuff up like backlight color,
 *     cursor position and serial communication.  
 ***************************************************************************/
void setup() { //This stuff is just run once to set stuff up
  Serial.begin(9600); //configure serial communication for debugging, output & general communication
  lcd.begin(16, 2); //set up the LCD's number of columns and rows
  lcd.setBacklight(WHITE);
  lcd.setCursor(0, 0); //sets cursor to column 0, line 0 
}

/***************************************************************************
 *     Function Information 
 *     Name of Function: loop
 *     Function Return Type: void
 * 
 *     Parameters (list data type, name, and comment one per line):
 *       1. Function accepts no parameters 
 *
 *     Function Description: Function is basically like 'main' for arduino. 
 *     It loops forever because you can't end it - the processor always has to be doing
 *     something. 
 ***************************************************************************/
void loop() {
  //INITIAL SETUP
  uint8_t buttons = lcd.readButtons(); //initializing values for input from buttons
  lcd.clear(); //clears LCD
  lcd.setBacklight(WHITE); //white backlight
  buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.

  //DECLARING & INITIALIZING VARIABLES
  int mode = 0; //stores user desired mode, 1 2 3 4 5 or 6. panorama, HDR, rtv, BT, timelapse, triggers
  int modePref = 0; //stores user specified preferences for chosen mode
  int conType = 1; //stores user indicated connection type, analog or usb (1 or 2)
  int camType = 1; //stores user indicated camera type, aps-c or fullframe (1 or 2)
  int lensLength = 0; //stores user indicated lens length
  int hdrPref = 1; //number of hdr brackets user specified
  long baseShutter = -1L; //base shutter speed in uS, long type must be used because there are many digits

  //LAUNCH SPLASH SCREEN
  splashScreen(); //launches splash screen function

    //GET MODE (panorama, hdr, rtv, timelapse or trig)
  mode = getMode(); //gets user input for desired mode of robot

  //TESTING MODE DISPLAY DELETE!
  lcd.clear(); //clears LCD DELETE
  lcd.print(mode); //DELETE
  delay(500); //DELETE!

  //GET MODE PREFERENCES
  modePref = getModePref(mode); //passes desired mode to function to get preferences for that mode

    //TESTING MODEPREF DISPLAY DELETE!
  lcd.clear(); //clears LCD DELETE
  lcd.print("mode pref "); //DELETE
  lcd.print(modePref); //DELETE
  delay(500); //DELETE!

  //GET CONNECTION TYPE
  conType = getConType(); //gets connection type, analog or USB

  //TESTING CONTYPE DISPLAY DELETE!
  lcd.clear(); //clears LCD DELETE
  lcd.print("ConType "); //DELETE
  lcd.print(conType); //DELETE
  delay(500); //DELETE!

  //GET CAMERA TYPE
  camType = getCamType(); //gets camera type, aps-c or fullframe

    //TESTING CAMTYPE DISPLAY DELETE!
  lcd.clear(); //clears LCD DELETE
  lcd.print("CamType "); //DELETE
  lcd.print(camType); //DELETE
  delay(500); //DELETE!

  //GET LENS LENGTH
  lensLength = getLensLength(); //requests lens length from user

  //TESTING LENSLENGTH DISPLAY DELETE!
  lcd.clear(); //clears LCD DELETE
  lcd.print("lensLength "); //DELETE
  lcd.print(lensLength); //DELETE
  delay(500); //DELETE!

  //GET HDR PREF FROM USER
  hdrPref = getHdrPref(); //requests number of hdr brackets from the user

  //TESTING HDRPREF DISPLAY DELETE!
  lcd.clear(); //clears LCD DELETE
  lcd.print("hdrPref "); //DELETE
  lcd.print(hdrPref); //DELETE
  delay(500); //DELETE!

  //GET BASE SHUTTER FROM USER
  if(conType == 1 && hdrPref > 1){ //runs if connection is analog and HDR is specified
    baseShutter = getBaseShutter(); //requests user for base shutter speed, value returned in uS
  }

  //TESTING GETBASESHUTTER DELETE!
  lcd.clear(); //clears LCD DELETE
  lcd.print("bSh "); //DELETE
  lcd.print(baseShutter); //DELETE
  delay(500); //DELETE!
}

/***************************************************************************
 *     Function Information
 *     Name of Function: splashScreen
 *     Function Return Type: void
 * 
 *     Parameters (list data type, name, and comment one per line):
 *       1. Function accepts no parameters 
 *
 *     Function Description: Function is splash screen. Function welcomes user on LCD display,
 *     prompts user to press select to continue. (Then splashScreen exits, returning control 
 *     to main loop.)
 ***************************************************************************/
void splashScreen(){
  //INITIAL SETUP
  uint8_t buttons = lcd.readButtons(); //initializing values for input from buttons
  lcd.clear(); //clears LCD
  lcd.setBacklight(WHITE); //white backlight
  buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.  

  //DECLARING & INITIALIZING VARIABLES
  boolean loopcontrol = 0; //value for select button loop control

  //PRINTING TO LCD
  lcd.setCursor(0, 0); //sets cursor to column 0, line 0 
  lcd.print("    WELCOME!    "); //prints to LCD
  lcd.setCursor(0, 1); //sets cursor to next line - column 0, line 1 (note: line 1 is the second row, since counting begins with 0)
  lcd.print("Select To Begin"); //prints to LCD

  //INPUT LOOP - WAITING FOR SELECT BUTTON PRESS AND RELEASE
  while(loopcontrol == 0){ //runs while select button not pressed
    buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.   
    if(buttons & BUTTON_SELECT) { //runs if ANY button is pressed AND BUTTON _SELECT = 1
      buttons = lcd.readButtons(); //update state of pressed buttons. 1 if ANY button pressed, 0 if not. 
      while((buttons & BUTTON_SELECT) == 1){ //WAITS UNTIL SELECT BUTTON RELEASED (otherwise future loops may register select button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      } 
      loopcontrol = 1; //stops loop 
    }
  }

  //CLEARING LCD FOR NEXT FUNCTION
  lcd.clear(); //clears LCD
}

/***************************************************************************
 *     Function Information
 *     Name of Function: getMode
 *     Function Return Type: int
 * 
 *     Parameters (list data type, name, and comment one per line):
 *       1. Function accepts no parameters 
 *
 *     Function Description: Function requests mode from the user - panorama, 
 *     HDR, timelapse or trigger. Function returns an integer to the calling function 
 *     indicating which option the user selected. 
 ***************************************************************************/
int getMode(){
  //INITIAL SETUP
  uint8_t buttons = lcd.readButtons(); //initializing values for input from buttons
  lcd.clear(); //clears LCD
  lcd.setBacklight(WHITE); //white backlight
  buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.

  //DECLARING & INITIALIZING VARIABLES
  boolean loopcontrol = 0; //value for select button loop control
  int lrTrack = 1; //tracks left/right clicks, possible values 1 2 3
  int udTrack = 1; //tracks up/down clicks, 1 top row 2 bottom row 
  int mode = 1; //returns user selected mode (values 1 2 3 4 5 6)

  //PRINTING SCREEN NAME (SELECT MODE)
  lcd.setCursor(0, 0); //sets cursor to column 0, line 0 
  lcd.setBacklight(YELLOW); //yellow backlight
  lcd.print("  SELECT MODE   "); //prints to LCD
  delay(1000); //pauses for 1 second to display select mode
  lcd.setBacklight(WHITE); //white backlight
  lcd.clear(); //clears LCD

  //PRINTING MODE OPTIONS TO LCD
  lcd.setCursor(0, 0); //sets cursor to column 0, line 0 
  lcd.print("PANO HDR RTV BT");
  lcd.setCursor(0, 1); //sets cursor to next line - column 0, line 1 (note: line 1 is the second row, since counting begins with 0)
  lcd.print("TIMELAPSE  TRIG"); //prints to LCD

  //INPUT LOOP - WAITING FOR SELECT BUTTON PRESS AND RELEASE
  while(loopcontrol == 0){ //runs while select button not pressed
    buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.  

    //BUTTON TRACKING
    //right button tracking
    if(((buttons & BUTTON_RIGHT)>0) & ((lrTrack < 4)>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      lrTrack ++; //increment lrTrack by 1 
      while(((buttons & BUTTON_RIGHT)>0) == 1){ //WAITS UNTIL RIGHT BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    }
    //left button tracking
    if(((buttons & BUTTON_LEFT)>0) & ((lrTrack > 1)>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      lrTrack --; //decrement by 1
      while(((buttons & BUTTON_LEFT)>0) == 1){ //WAITS UNTIL LEFT BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    }
    //down button tracking
    if(((buttons & BUTTON_DOWN)>0) & ((udTrack < 2)>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      udTrack ++; //increment by 1
      while(((buttons & BUTTON_DOWN)>0) == 1){ //WAITS UNTIL DOWN BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    }
    //up button tracking
    if(((buttons & BUTTON_UP)>0) & ((udTrack > 1)>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      udTrack --; //decrement by 1
      while(((buttons & BUTTON_UP)>0) == 1){ //WAITS UNTIL UP BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    } 

    //SELECTING MODE
    lcd.blink(); //blink cursor throughout entire program
    if(udTrack == 1){ //if selection is in top row 
      switch (lrTrack) { //comparing case values with lrTrack
      case 1: //executes if lrTrack = 1
        lcd.setCursor(0, 0); //sets cursor to column 0, line 0 (pano)
        mode = 1; //mode 1, pano
        break;
      case 2: //executes if lrTrack = 2
        lcd.setCursor(5, 0); //sets cursor to column 5, line 0 (hdr)
        mode = 2; //mode 2, hdr
        break;
      case 3: //executes if lrTrack = 3
        lcd.setCursor(9, 0); //sets cursor to column 9, line 0 (rtv)
        mode = 3; //mode 3, rtv
        break;
      case 4: //executes if lrTrack = 4
        lcd.setCursor(13, 0); //sets cursor to column 13, line 0 (bt)
        mode = 4; //mode 4, bt
        break;
      }
    }
    else{ //if selection is on bottom row
      switch (lrTrack) { //comparing case values with lrTrack
      case 3: //executes if lrTrack = 3
        lcd.setCursor(11, 1); //sets cursor to column 11, line 1 (trig)
        mode = 6; //mode 6, trig
        break;
      case 4: //executes if lrTrack = 4
        lcd.setCursor(11, 1); //sets cursor to column 11, line 1 (trig)
        mode = 6; //mode 6, trig
        break;
      default: //executes if lrTrack = 1 or 2
        lcd.setCursor(0, 1); //sets cursor to column 0, line 1 (timelapse)
        mode = 5; //mode 5, timelapse
        break;
      }
    }

    //STOPPING LOOP
    if(buttons & BUTTON_SELECT) { //runs if ANY button is pressed AND BUTTON _SELECT = 1
      buttons = lcd.readButtons(); //update state of pressed buttons. 1 if ANY button pressed, 0 if not. 
      while((buttons & BUTTON_SELECT) == 1){ //WAITS UNTIL SELECT BUTTON RELEASED (otherwise future loops may register select button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      } 
      loopcontrol = 1; //stops loop 
    }
  }

  //WRAPPING UP
  lcd.clear(); //clears LCD
  lcd.noBlink(); //stop blinking!
  return(mode); //returns mode user requested to calling function (1, 2, 3, 4 or 5)
}

/***************************************************************************
 *     Function Information
 *     Name of Function: getModePref
 *     Function Return Type: int
 * 
 *     Parameters (list data type, name, and comment one per line):
 *       1. int mode (what mode the user selected, pano, timelapse, hdr, rtv, trig)
 *
 *     Function Description: Function takes the mode specified by the user in the 
 *     getMode function and requests preferences for that mode. This is accomplished
 *     by a switch statement which launches the appropriate request preferences function 
 *     for each mode and stores the returned preference value in a variable. This function 
 *     then passes that variable to the calling function. 
 ***************************************************************************/
int getModePref(int mode){
  //DECLARING & INITIALIZING VARIABLES
  int modePref = 0; //stores value indicating preferences for mode chosen by user, is returned to calling function

  //LAUNCH PROPER GET MODE PREFERENCES FUNCTION
  switch (mode) { //comparing case values with lrTrack
  case 1: //executes if mode 1, pano
    modePref = getPanoPref(); //get panorama pref
    break;
  case 2: //executes if mode 2, hdr
    modePref = 5; //we'll get detailed information about hdr brackets later, skip this function for now, just note it's hdr
    break;
  case 3: //executes if mode 3, rtv
    modePref = getRtvPref(); //get rtv pref
    break;
  case 4: //executes if mode 4, bt
    modePref = 4; //bluetooth function indicated, no preferences required
    break;
  case 5: //executes if mode 4, timelapse
    modePref = getTimelapsePref(); //get timelapse pref
    break;
  case 6: //executes if mode 5, trig
    modePref = getTrigPref(); //get trig pref
    break;
  }

  return(modePref); //returns mode preferences user requested to calling function 
}

/***************************************************************************
 *     Function Information
 *     Name of Function: getPanoPref
 *     Function Return Type: int
 * 
 *     Parameters (list data type, name, and comment one per line):
 *       1. Function accepts no parameters 
 *
 *     Function Description: Function displays options for panorama (standard horizontal, 
 *     standard vertical, brenzier) and returns preference to calling function. HDR
 *     information will be requested later, right before the program runs. See variable documentation.
 ***************************************************************************/
int getPanoPref(){
  //INITIAL SETUP
  uint8_t buttons = lcd.readButtons(); //initializing values for input from buttons
  lcd.clear(); //clears LCD
  lcd.setBacklight(WHITE); //white backlight
  buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.

  //DECLARING & INITIALIZING VARIABLES
  boolean loopcontrol = 0; //value for select button loop control
  int lrTrack = 1; //tracks left/right clicks, possible values 1 2 3
  int modePref = 0; //stores value indicating preferences for mode chosen by user, is returned to calling function

  //PRINTING PANORAMA OPTIONS TO LCD
  lcd.setCursor(0, 0); //sets cursor to column 0, line 0 
  lcd.print("PANORAMA OPTIONS");
  lcd.setCursor(0, 1); //sets cursor to next line - column 0, line 1 (note: line 1 is the second row, since counting begins with 0)
  lcd.print("HRZ VRT BRENZIER"); //prints to LCD

  //INPUT LOOP - WAITING FOR SELECT BUTTON PRESS AND RELEASE
  while(loopcontrol == 0){ //runs while select button not pressed
    buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.  

    //BUTTON TRACKING
    //right button tracking
    if(((buttons & BUTTON_RIGHT)>0) & ((lrTrack < 3)>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      lrTrack ++; //increment lrTrack by 1 
      while(((buttons & BUTTON_RIGHT)>0) == 1){ //WAITS UNTIL RIGHT BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    }
    //left button tracking
    if(((buttons & BUTTON_LEFT)>0) & ((lrTrack > 1)>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      lrTrack --; //decrement by 1
      while(((buttons & BUTTON_LEFT)>0) == 1){ //WAITS UNTIL LEFT BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    }

    //SELECTING MODE
    lcd.blink(); //blink cursor throughout entire program
    switch (lrTrack) { //comparing case values with lrTrack
    case 1: //executes if lrTrack = 1
      lcd.setCursor(0, 1); //sets cursor to column 0, line 1 (hrz)
      modePref = 1; //mode 1, standard horizontal panorama
      break;
    case 2: //executes if lrTrack = 2
      lcd.setCursor(4, 1); //sets cursor to column 5, line 1 (vrt)
      modePref = 2; //mode 2, standard vertical panorama
      break;
    case 3: //executes if lrTrack = 3
      lcd.setCursor(8, 1); //sets cursor to column 9, line 1 (brenzier)
      modePref = 3; //mode 3, brenzier panorama
      break;
    }

    //STOPPING LOOP
    if(buttons & BUTTON_SELECT) { //runs if ANY button is pressed AND BUTTON _SELECT = 1
      buttons = lcd.readButtons(); //update state of pressed buttons. 1 if ANY button pressed, 0 if not. 
      while((buttons & BUTTON_SELECT) == 1){ //WAITS UNTIL SELECT BUTTON RELEASED (otherwise future loops may register select button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      } 
      loopcontrol = 1; //stops loop 
    }
  }

  //WRAPPING UP
  lcd.clear(); //clears LCD
  lcd.noBlink(); //stop blinking!
  return(modePref); //returns panorama preferences user requested to calling function
}

/***************************************************************************
 *     Function Information
 *     Name of Function: getRtvPref
 *     Function Return Type: int
 * 
 *     Parameters (list data type, name, and comment one per line):
 *       1. Function accepts no parameters 
 *
 *     Function Description: Function displays options for rtv (straight or curve) 
 *     and returns preference to calling function. HDR information will be requested 
 *     later, right before the program runs. See variable documentation.
 ***************************************************************************/
int getRtvPref(){
  //INITIAL SETUP
  uint8_t buttons = lcd.readButtons(); //initializing values for input from buttons
  lcd.clear(); //clears LCD
  lcd.setBacklight(WHITE); //white backlight
  buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.

  //DECLARING & INITIALIZING VARIABLES
  boolean loopcontrol = 0; //value for select button loop control
  int lrTrack = 1; //tracks left/right clicks, possible values 1 2 3
  int modePref = 0; //stores value indicating preferences for mode chosen by user, is returned to calling function

  //PRINTING PANORAMA OPTIONS TO LCD
  lcd.setCursor(0, 0); //sets cursor to column 0, line 0 
  lcd.print("RTV OPTIONS");
  lcd.setCursor(0, 1); //sets cursor to next line - column 0, line 1 (note: line 1 is the second row, since counting begins with 0)
  lcd.print("STRAIGHT  CURVE"); //prints to LCD

  //INPUT LOOP - WAITING FOR SELECT BUTTON PRESS AND RELEASE
  while(loopcontrol == 0){ //runs while select button not pressed
    buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.  

    //BUTTON TRACKING
    //right button tracking
    if(((buttons & BUTTON_RIGHT)>0) & ((lrTrack < 2)>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      lrTrack ++; //increment lrTrack by 1 
      while(((buttons & BUTTON_RIGHT)>0) == 1){ //WAITS UNTIL RIGHT BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    }
    //left button tracking
    if(((buttons & BUTTON_LEFT)>0) & ((lrTrack > 1)>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      lrTrack --; //decrement by 1
      while(((buttons & BUTTON_LEFT)>0) == 1){ //WAITS UNTIL LEFT BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    }

    //SELECTING MODE
    lcd.blink(); //blink cursor throughout entire program
    switch (lrTrack) { //comparing case values with lrTrack
    case 1: //executes if lrTrack = 1
      lcd.setCursor(0, 1); //sets cursor to column 0, line 1 (straight)
      modePref = 10; //mode 10, real time video straight
      break;
    case 2: //executes if lrTrack = 2
      lcd.setCursor(10, 1); //sets cursor to column 10, line 1 (curve)
      modePref = 11; //mode 11, real time video straight
      break;
    }

    //STOPPING LOOP
    if(buttons & BUTTON_SELECT) { //runs if ANY button is pressed AND BUTTON _SELECT = 1
      buttons = lcd.readButtons(); //update state of pressed buttons. 1 if ANY button pressed, 0 if not. 
      while((buttons & BUTTON_SELECT) == 1){ //WAITS UNTIL SELECT BUTTON RELEASED (otherwise future loops may register select button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      } 
      loopcontrol = 1; //stops loop 
    }
  }

  //WRAPPING UP
  lcd.clear(); //clears LCD
  lcd.noBlink(); //stop blinking!
  return(modePref); //returns real time video preferences user requested to calling function
}

/***************************************************************************
 *     Function Information
 *     Name of Function: getTimelapsePref
 *     Function Return Type: int
 * 
 *     Parameters (list data type, name, and comment one per line):
 *       1. Function accepts no parameters 
 *
 *     Function Description: Function displays options for Timelapse (straight or curve) 
 *     and returns preference to calling function. HDR information will be requested 
 *     later, right before the program runs. Another function will be needed to get
 *     info on duration, speed, etc. See variable documentation.
 ***************************************************************************/
int getTimelapsePref(){
  //INITIAL SETUP
  uint8_t buttons = lcd.readButtons(); //initializing values for input from buttons
  lcd.clear(); //clears LCD
  lcd.setBacklight(WHITE); //white backlight
  buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.

  //DECLARING & INITIALIZING VARIABLES
  boolean loopcontrol = 0; //value for select button loop control
  int lrTrack = 1; //tracks left/right clicks, possible values 1 2 3
  int modePref = 0; //stores value indicating preferences for mode chosen by user, is returned to calling function

  //PRINTING PANORAMA OPTIONS TO LCD
  lcd.setCursor(0, 0); //sets cursor to column 0, line 0 
  lcd.print("TMLAPSE OPTIONS");
  lcd.setCursor(0, 1); //sets cursor to next line - column 0, line 1 (note: line 1 is the second row, since counting begins with 0)
  lcd.print("STRAIGHT  CURVE"); //prints to LCD

  //INPUT LOOP - WAITING FOR SELECT BUTTON PRESS AND RELEASE
  while(loopcontrol == 0){ //runs while select button not pressed
    buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.  

    //BUTTON TRACKING
    //right button tracking
    if(((buttons & BUTTON_RIGHT)>0) & ((lrTrack < 2)>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      lrTrack ++; //increment lrTrack by 1 
      while(((buttons & BUTTON_RIGHT)>0) == 1){ //WAITS UNTIL RIGHT BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    }
    //left button tracking
    if(((buttons & BUTTON_LEFT)>0) & ((lrTrack > 1)>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      lrTrack --; //decrement by 1
      while(((buttons & BUTTON_LEFT)>0) == 1){ //WAITS UNTIL LEFT BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    }

    //SELECTING MODE
    lcd.blink(); //blink cursor throughout entire program
    switch (lrTrack) { //comparing case values with lrTrack
    case 1: //executes if lrTrack = 1
      lcd.setCursor(0, 1); //sets cursor to column 0, line 1 (straight)
      modePref = 14; //mode 14, timelapse straight
      break;
    case 2: //executes if lrTrack = 2
      lcd.setCursor(10, 1); //sets cursor to column 10, line 1 (curve)
      modePref = 15; //mode 15, timelapse straight
      break;
    }

    //STOPPING LOOP
    if(buttons & BUTTON_SELECT) { //runs if ANY button is pressed AND BUTTON _SELECT = 1
      buttons = lcd.readButtons(); //update state of pressed buttons. 1 if ANY button pressed, 0 if not. 
      while((buttons & BUTTON_SELECT) == 1){ //WAITS UNTIL SELECT BUTTON RELEASED (otherwise future loops may register select button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      } 
      loopcontrol = 1; //stops loop 
    }
  }

  //WRAPPING UP
  lcd.clear(); //clears LCD
  lcd.noBlink(); //stop blinking!
  return(modePref); //returns timelapse preferences user requested to calling function
}

/***************************************************************************
 *     Function Information
 *     Name of Function: getTrigPref
 *     Function Return Type: int
 * 
 *     Parameters (list data type, name, and comment one per line):
 *       1. Function accepts no parameters 
 *
 *     Function Description: Function displays options for trig (lighting is only option for now) 
 *     and returns preference to calling function. HDR information will be requested 
 *     later, right before the program runs. See variable documentation.
 ***************************************************************************/
int getTrigPref(){
  //INITIAL SETUP
  uint8_t buttons = lcd.readButtons(); //initializing values for input from buttons
  lcd.clear(); //clears LCD
  lcd.setBacklight(WHITE); //white backlight
  buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.

  //DECLARING & INITIALIZING VARIABLES
  boolean loopcontrol = 0; //value for select button loop control
  int lrTrack = 1; //tracks left/right clicks, possible values 1 2 3
  int modePref = 0; //stores value indicating preferences for mode chosen by user, is returned to calling function

  //PRINTING PANORAMA OPTIONS TO LCD
  lcd.setCursor(0, 0); //sets cursor to column 0, line 0 
  lcd.print("TRIGGER OPTIONS");
  lcd.setCursor(0, 1); //sets cursor to next line - column 0, line 1 (note: line 1 is the second row, since counting begins with 0)
  lcd.print("LIGHTING"); //prints to LCD

  //INPUT LOOP - WAITING FOR SELECT BUTTON PRESS AND RELEASE
  while(loopcontrol == 0){ //runs while select button not pressed
    buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.  

    //BUTTON TRACKING
    //right button tracking
    if(((buttons & BUTTON_RIGHT)>0) & ((lrTrack < 1)>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      lrTrack ++; //increment lrTrack by 1 
      while(((buttons & BUTTON_RIGHT)>0) == 1){ //WAITS UNTIL RIGHT BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    }
    //left button tracking
    if(((buttons & BUTTON_LEFT)>0) & ((lrTrack > 1)>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      lrTrack --; //decrement by 1
      while(((buttons & BUTTON_LEFT)>0) == 1){ //WAITS UNTIL LEFT BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    }

    //SELECTING MODE
    lcd.blink(); //blink cursor throughout entire program
    switch (lrTrack) { //comparing case values with lrTrack
    case 1: //executes if lrTrack = 1
      lcd.setCursor(0, 1); //sets cursor to column 0, line 1 (straight)
      modePref = 7; //mode 7, lightning trigger
      break;
    }

    //STOPPING LOOP
    if(buttons & BUTTON_SELECT) { //runs if ANY button is pressed AND BUTTON _SELECT = 1
      buttons = lcd.readButtons(); //update state of pressed buttons. 1 if ANY button pressed, 0 if not. 
      while((buttons & BUTTON_SELECT) == 1){ //WAITS UNTIL SELECT BUTTON RELEASED (otherwise future loops may register select button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      } 
      loopcontrol = 1; //stops loop 
    }
  }

  //WRAPPING UP
  lcd.clear(); //clears LCD
  lcd.noBlink(); //stop blinking!
  return(modePref); //returns trigger preferences user requested to calling function
}

/***************************************************************************
 *     Function Information
 *     Name of Function: getConType
 *     Function Return Type: int
 * 
 *     Parameters (list data type, name, and comment one per line):
 *       1. Function accepts no parameters 
 *
 *     Function Description: Function requests user input for connection type 
 *     (usb or analog). Connection type returned to calling function by integer
 *     conType. (1 is analog, 2 is usb). 
 ***************************************************************************/
int getConType(){
  //INITIAL SETUP
  uint8_t buttons = lcd.readButtons(); //initializing values for input from buttons
  lcd.clear(); //clears LCD
  lcd.setBacklight(WHITE); //white backlight
  buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.

  //DECLARING & INITIALIZING VARIABLES
  boolean loopcontrol = 0; //value for select button loop control
  int lrTrack = 1; //tracks left/right clicks, possible values 1 2 3
  int conType = 0; //stores value indicating connection type chosen by user, is returned to calling function

  //PRINTING PANORAMA OPTIONS TO LCD
  lcd.setCursor(0, 0); //sets cursor to column 0, line 0 
  lcd.print("CONNECTION TYPE");
  lcd.setCursor(0, 1); //sets cursor to next line - column 0, line 1 (note: line 1 is the second row, since counting begins with 0)
  lcd.print("ANALOG  USB"); //prints to LCD

  //INPUT LOOP - WAITING FOR SELECT BUTTON PRESS AND RELEASE
  while(loopcontrol == 0){ //runs while select button not pressed
    buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.  

    //BUTTON TRACKING
    //right button tracking
    if(((buttons & BUTTON_RIGHT)>0) & ((lrTrack < 2)>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      lrTrack ++; //increment lrTrack by 1 
      while(((buttons & BUTTON_RIGHT)>0) == 1){ //WAITS UNTIL RIGHT BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    }
    //left button tracking
    if(((buttons & BUTTON_LEFT)>0) & ((lrTrack > 1)>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      lrTrack --; //decrement by 1
      while(((buttons & BUTTON_LEFT)>0) == 1){ //WAITS UNTIL LEFT BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    }

    //SELECTING MODE
    lcd.blink(); //blink cursor throughout entire program
    switch (lrTrack) { //comparing case values with lrTrack
    case 1: //executes if lrTrack = 1
      lcd.setCursor(0, 1); //sets cursor to column 0, line 1 (analog)
      conType = 1; //connection type is 1, analog
      break;
    case 2: //executes if lrTrack = 2
      lcd.setCursor(8, 1); //sets cursor to column 8, line 1 (usb)
      conType = 2; //connection type is 2, usb
      break;
    }

    //STOPPING LOOP
    if(buttons & BUTTON_SELECT) { //runs if ANY button is pressed AND BUTTON _SELECT = 1
      buttons = lcd.readButtons(); //update state of pressed buttons. 1 if ANY button pressed, 0 if not. 
      while((buttons & BUTTON_SELECT) == 1){ //WAITS UNTIL SELECT BUTTON RELEASED (otherwise future loops may register select button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      } 
      loopcontrol = 1; //stops loop 
    }
  }

  //WRAPPING UP
  lcd.clear(); //clears LCD
  lcd.noBlink(); //stop blinking!
  return(conType); //returns real time video preferences user requested to calling function
}

/***************************************************************************
 *     Function Information
 *     Name of Function: getCamType
 *     Function Return Type: int
 * 
 *     Parameters (list data type, name, and comment one per line):
 *       1. Function accepts no parameters 
 *
 *     Function Description: Function requests user input for camera type - 
 *     APS-C or fullframe. The camera type is stored as an integer in the 
 *     camType variable and returned to the callig function.
 ***************************************************************************/
int getCamType(){
  //INITIAL SETUP
  uint8_t buttons = lcd.readButtons(); //initializing values for input from buttons
  lcd.clear(); //clears LCD
  lcd.setBacklight(WHITE); //white backlight
  buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.

  //DECLARING & INITIALIZING VARIABLES
  boolean loopcontrol = 0; //value for select button loop control
  int lrTrack = 1; //tracks left/right clicks, possible values 1 2 3
  int camType = 0; //stores value indicating connection type chosen by user, is returned to calling function

  //PRINTING PANORAMA OPTIONS TO LCD
  lcd.setCursor(0, 0); //sets cursor to column 0, line 0 
  lcd.print("CAMERA TYPE");
  lcd.setCursor(0, 1); //sets cursor to next line - column 0, line 1 (note: line 1 is the second row, since counting begins with 0)
  lcd.print("APS-C FULLFRAME"); //prints to LCD

  //INPUT LOOP - WAITING FOR SELECT BUTTON PRESS AND RELEASE
  while(loopcontrol == 0){ //runs while select button not pressed
    buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.  

    //BUTTON TRACKING
    //right button tracking
    if(((buttons & BUTTON_RIGHT)>0) & ((lrTrack < 2)>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      lrTrack ++; //increment lrTrack by 1 
      while(((buttons & BUTTON_RIGHT)>0) == 1){ //WAITS UNTIL RIGHT BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    }
    //left button tracking
    if(((buttons & BUTTON_LEFT)>0) & ((lrTrack > 1)>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      lrTrack --; //decrement by 1
      while(((buttons & BUTTON_LEFT)>0) == 1){ //WAITS UNTIL LEFT BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    }

    //SELECTING MODE
    lcd.blink(); //blink cursor throughout entire program
    switch (lrTrack) { //comparing case values with lrTrack
    case 1: //executes if lrTrack = 1
      lcd.setCursor(0, 1); //sets cursor to column 0, line 1 (APS-C)
      camType = 1; //camera type is 1, APS-C
      break;
    case 2: //executes if lrTrack = 2
      lcd.setCursor(6, 1); //sets cursor to column 6, line 1 (fullframe)
      camType = 2; //camera type is 2, full frame
      break;
    }

    //STOPPING LOOP
    if(buttons & BUTTON_SELECT) { //runs if ANY button is pressed AND BUTTON _SELECT = 1
      buttons = lcd.readButtons(); //update state of pressed buttons. 1 if ANY button pressed, 0 if not. 
      while((buttons & BUTTON_SELECT) == 1){ //WAITS UNTIL SELECT BUTTON RELEASED (otherwise future loops may register select button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      } 
      loopcontrol = 1; //stops loop 
    }
  }

  //WRAPPING UP
  lcd.clear(); //clears LCD
  lcd.noBlink(); //stop blinking!
  return(camType); //returns real time video preferences user requested to calling function
}

/***************************************************************************
 *     Function Information
 *     Name of Function: getLensLength
 *     Function Return Type: int
 * 
 *     Parameters (list data type, name, and comment one per line):
 *       1. Function accepts no parameters 
 *
 *     Function Description: Function requests mode from the user - panorama, 
 *     HDR, timelapse or trigger. Function returns an integer to the calling function 
 *     indicating which option the user selected. 
 ***************************************************************************/
int getLensLength(){
  //INITIAL SETUP
  uint8_t buttons = lcd.readButtons(); //initializing values for input from buttons
  lcd.clear(); //clears LCD
  lcd.setBacklight(WHITE); //white backlight
  buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.

  //DECLARING & INITIALIZING VARIABLES
  boolean loopcontrol = 0; //value for select button loop control
  int lensLength = 17; //returns user selected lens length, integer specifies mm
  int updateScreen  = 0; //variable stores information on screen refresh - 0 screen should not refresh, 1 screen should refresh

  //PRINTING MODE OPTIONS TO LCD
  lcd.setCursor(0, 0); //sets cursor to column 0, line 0 
  lcd.print("LENS LENGTH");
  lcd.setCursor(0, 1); //sets cursor to next line - column 0, line 1 (note: line 1 is the second row, since counting begins with 0)
  lcd.print(lensLength); //prints lens length
  lcd.print(" mm"); //prints to LCD

  //INPUT LOOP - WAITING FOR SELECT BUTTON PRESS AND RELEASE
  while(loopcontrol == 0){ //runs while select button not pressed
    buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.  

    //BUTTON TRACKING
    //up button tracking
    if(((buttons & BUTTON_UP)>0) & ((lensLength <= (500-5))>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      lensLength=lensLength+5; //increment lensLength by 5
      updateScreen=1; //update screen when you get to the if statement
      while(((buttons & BUTTON_UP)>0) == 1){ //WAITS UNTIL UP BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    }      
    //down button tracking
    if(((buttons & BUTTON_DOWN)>0) & ((lensLength >= (10 + 5))>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      lensLength=lensLength - 5; //decrement by 5
      updateScreen=1; //update screen when you get to the if statement
      while(((buttons & BUTTON_DOWN)>0) == 1){ //WAITS UNTIL DOWN BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    }
    //left button tracking
    if(((buttons & BUTTON_LEFT)>0) & ((lensLength > 10)>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      lensLength --; //decrement by 1
      updateScreen=1; //update screen when you get to the if statement
      while(((buttons & BUTTON_LEFT)>0) == 1){ //WAITS UNTIL LEFT BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    }
    //right button tracking
    if(((buttons & BUTTON_RIGHT)>0) & ((lensLength < 500)>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      lensLength ++; //increment by 1
      updateScreen=1; //update screen when you get to the if statement
      while(((buttons & BUTTON_RIGHT)>0) == 1){ //WAITS UNTIL RIGHT BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    } 
    //redraw screen
    if(updateScreen==1){ //redraws screen if a button has been pushed and update is required
      lcd.clear(); //clears lcd
      lcd.setCursor(0, 0); //sets cursor to column 0, line 0 
      lcd.print("LENS LENGTH");
      lcd.setCursor(0, 1); //sets cursor to next line - column 0, line 1 (note: line 1 is the second row, since counting begins with 0)
      lcd.print(lensLength); //prints lens length
      lcd.print(" mm"); //prints to LCD
      updateScreen=0; //disables screen update on next loop
    }
    //stop loop when select button pressed
    if(buttons & BUTTON_SELECT) { //runs if ANY button is pressed AND BUTTON _SELECT = 1
      buttons = lcd.readButtons(); //update state of pressed buttons. 1 if ANY button pressed, 0 if not. 
      while((buttons & BUTTON_SELECT) == 1){ //WAITS UNTIL SELECT BUTTON RELEASED (otherwise future loops may register select button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      } 
      loopcontrol = 1; //stops loop 
    }
  }

  //WRAPPING UP
  lcd.clear(); //clears LCD
  lcd.noBlink(); //stop blinking!
  return(lensLength); //returns lens length to calling function 
}

/***************************************************************************
 *     Function Information
 *     Name of Function: getHdrPref
 *     Function Return Type: int
 * 
 *     Parameters (list data type, name, and comment one per line):
 *       1. Function accepts no parameters 
 *
 *     Function Description: Function requests user input number of HDR brackets to take.
 *     If 1 is selected, no HDR will be taken. Function returns number of brackets requested
 *     stored in the hdrPref variable to the calling function.
 ***************************************************************************/
int getHdrPref(){
  //INITIAL SETUP
  uint8_t buttons = lcd.readButtons(); //initializing values for input from buttons
  lcd.clear(); //clears LCD
  lcd.setBacklight(WHITE); //white backlight
  buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.

  //DECLARING & INITIALIZING VARIABLES
  boolean loopcontrol = 0; //value for select button loop control
  int lrTrack = 1; //tracks left/right clicks, possible values 1 2 3
  int hdrPref = 0; //stores value indicating number of HDR brackets chosen by user, is returned to calling function

  //PRINTING PANORAMA OPTIONS TO LCD
  lcd.setCursor(0, 0); //sets cursor to column 0, line 0 
  lcd.print("# HDR BRACKETS");
  lcd.setCursor(0, 1); //sets cursor to next line - column 0, line 1 (note: line 1 is the second row, since counting begins with 0)
  lcd.print("1 2 3 5 7 9"); //prints to LCD

  //INPUT LOOP - WAITING FOR SELECT BUTTON PRESS AND RELEASE
  while(loopcontrol == 0){ //runs while select button not pressed
    buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.  

    //BUTTON TRACKING
    //right button tracking
    if(((buttons & BUTTON_RIGHT)>0) & ((lrTrack < 6)>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      lrTrack ++; //increment lrTrack by 1 
      while(((buttons & BUTTON_RIGHT)>0) == 1){ //WAITS UNTIL RIGHT BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    }
    //left button tracking
    if(((buttons & BUTTON_LEFT)>0) & ((lrTrack > 1)>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      lrTrack --; //decrement by 1
      while(((buttons & BUTTON_LEFT)>0) == 1){ //WAITS UNTIL LEFT BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    }

    //SELECTING MODE
    lcd.blink(); //blink cursor throughout entire program
    switch (lrTrack) { //comparing case values with lrTrack
    case 1: //executes if lrTrack = 1
      lcd.setCursor(0, 1); //sets cursor to column 0, line 1 (1 bracket)
      hdrPref = 1; //No HDR (only 1 bracket)
      break;
    case 2: //executes if lrTrack = 2
      lcd.setCursor(2, 1); //sets cursor to column 2, line 1 (2 brackets)
      hdrPref = 2; //number of HDR brackets to take
      break;
    case 3: //executes if lrTrack =3
      lcd.setCursor(4, 1); //sets cursor to column 4, line 1 (3 brackets)
      hdrPref = 3; //number of HDR brackets to take
      break;
    case 4: //executes if lrTrack = 4
      lcd.setCursor(6, 1); //sets cursor to column 6, line 1 (5 brackets)
      hdrPref = 5; //number of HDR brackets to take
      break;
    case 5: //executes if lrTrack = 5
      lcd.setCursor(8, 1); //sets cursor to column 8, line 1 (7 brackets)
      hdrPref = 7; //number of HDR brackets to take
      break;
    case 6: //executes if lrTrack = 6
      lcd.setCursor(10, 1); //sets cursor to column 10, line 1 (9 brackets)
      hdrPref = 9; //number of HDR brackets to take
      break;
    }

    //STOPPING LOOP
    if(buttons & BUTTON_SELECT) { //runs if ANY button is pressed AND BUTTON _SELECT = 1
      buttons = lcd.readButtons(); //update state of pressed buttons. 1 if ANY button pressed, 0 if not. 
      while((buttons & BUTTON_SELECT) == 1){ //WAITS UNTIL SELECT BUTTON RELEASED (otherwise future loops may register select button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      } 
      loopcontrol = 1; //stops loop 
    }
  }

  //WRAPPING UP
  lcd.clear(); //clears LCD
  lcd.noBlink(); //stop blinking!
  return(hdrPref); //returns hdr preferences user requested to calling function
}

/***************************************************************************
 *     Function Information
 *     Name of Function: getBaseShutter
 *     Function Return Type: long baseShutter
 *     Parameters (list data type, name, and comment one per line):
 *       1. function accepts no parameters
 *
 *     Function Description: Function requests base shutter from user and returns 
 *     base shutter speed as a long value to calling function in microseconds (10^-6 seconds)! 
 *     IMPORTANT: If you intend to use the value in microseconds for delay, you may 
 *     use the delayMicroseconds() function to handle values from 3 uS to 16383 uS. 
 *     For larger values, you should use the delay() function which accepts values in mS. 
 ***************************************************************************/
long getBaseShutter(){
  //INITIAL SETUP
  uint8_t buttons = lcd.readButtons(); //initializing values for input from buttons
  lcd.clear(); //clears LCD
  lcd.setBacklight(WHITE); //white backlight
  buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.

  //DECLARING & INITIALIZING VARIABLES
  boolean loopcontrol = 0; //value for select button loop control
  long baseShutter = 12500L; //value for base shutter in uS calculated from selected shutter speed in seconds, default 1/80s, returned to calling function
  int arrayPosition = 34; //index of array to be incremented by button presses, default value of 34 references 1/80 shutter speed
  const char *shutterStrings[55] = {"30","25","20","15","13","10","8","6","5","4","3.2","2.5","2","1.6","1.3","1","0.8","0.6","0.5","0.4","0.3","1/4","1/5","1/6","1/8","1/10","1/13","1/15","1/20","1/25","1/30","1/40","1/50","1/60","1/80","1/100","1/125","1/160","1/200","1/250","1/320","1/400","1/500","1/640","1/800","1/1000","1/1250","1/1600","1/2000","1/2500","1/3200","1/4000","1/5000","1/6400","1/8000"}; //shutter strings to print. This code allocates an array of two pointers to const char - pointers will be set to the addresses of the static strings.
  float baseShutterArray[55] = {30,25,20,15,13,10,8,6,5,4,3.2,2.5,2,1.6,1.3,1,.8,.6,.5,.4,.3,.25,.2,.16666666,.125,.1,.07692308,.06666666,.05,.04,.03333333,.025,.02,.01666666,.0125,.01,.008,.00625,.005,.004,.003125,.0025,.002,.0015625,.00125,.001,.0008,.000625,.0005,.0004,.0003125,.00025,.0002,.00015625,.000125}; //base shutter values
  int updateScreen  = 0; //variable stores information on screen refresh - 0 screen should not refresh, 1 screen should refresh

  //PRINTING MODE OPTIONS TO LCD
  lcd.setCursor(0, 0); //sets cursor to column 0, line 0 
  lcd.print("SEL BASE SHUTTER");
  lcd.setCursor(0, 1); //sets cursor to next line - column 0, line 1 (note: line 1 is the second row, since counting begins with 0)
  lcd.print(shutterStrings[arrayPosition]); //prints selected shutter speed
  lcd.print("s"); //prints to LCD

  //INPUT LOOP - WAITING FOR SELECT BUTTON PRESS AND RELEASE
  while(loopcontrol == 0){ //runs while select button not pressed
    buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.  
    //BUTTON TRACKING
    //up button tracking
    if(((buttons & BUTTON_UP)>0) & ((arrayPosition <= (54-5))>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      arrayPosition=arrayPosition+5; //increment arrayPosition by 5
      updateScreen=1; //update screen when you get to the if statement
      while(((buttons & BUTTON_UP)>0) == 1){ //WAITS UNTIL UP BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    }      
    //down button tracking
    if(((buttons & BUTTON_DOWN)>0) & ((arrayPosition >= (0 + 5))>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      arrayPosition=arrayPosition - 5; //decrement by 5
      updateScreen=1; //update screen when you get to the if statement
      while(((buttons & BUTTON_DOWN)>0) == 1){ //WAITS UNTIL DOWN BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    }
    //left button tracking
    if(((buttons & BUTTON_LEFT)>0) & ((arrayPosition > 0)>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      arrayPosition --; //decrement by 1
      updateScreen=1; //update screen when you get to the if statement
      while(((buttons & BUTTON_LEFT)>0) == 1){ //WAITS UNTIL LEFT BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    }
    //right button tracking
    if(((buttons & BUTTON_RIGHT)>0) & ((arrayPosition < 54)>0)){ //normalizes important arguments to 1 if true, 0 if false for comparison
      arrayPosition ++; //increment by 1
      updateScreen=1; //update screen when you get to the if statement
      while(((buttons & BUTTON_RIGHT)>0) == 1){ //WAITS UNTIL RIGHT BUTTON RELEASED (otherwise future loops may register button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      }
    } 
    //redraw screen
    if(updateScreen==1){ //redraws screen if a button has been pushed and update is required
      lcd.clear(); //clears lcd
      lcd.setCursor(0, 0); //sets cursor to column 0, line 0 
      lcd.print("SEL BASE SHUTTER");
      lcd.setCursor(0, 1); //sets cursor to next line - column 0, line 1 (note: line 1 is the second row, since counting begins with 0)
      lcd.print(shutterStrings[arrayPosition]); //prints current base shutter value
      lcd.print("s"); //prints to LCD
      updateScreen=0; //disables screen update on next loop
    }
    //stop loop when select button pressed
    if(buttons & BUTTON_SELECT) { //runs if ANY button is pressed AND BUTTON _SELECT = 1
      buttons = lcd.readButtons(); //update state of pressed buttons. 1 if ANY button pressed, 0 if not. 
      while((buttons & BUTTON_SELECT) == 1){ //WAITS UNTIL SELECT BUTTON RELEASED (otherwise future loops may register select button!)
        buttons = lcd.readButtons(); //updates state of pressed buttons. 1 if ANY button pressed, 0 if not.
      } 
      loopcontrol = 1; //stops loop 
    }
  }

  //WRAPPING UP
  lcd.clear(); //clears LCD
  lcd.noBlink(); //stop blinking!  
  baseShutter = ((baseShutterArray[arrayPosition])*1000000); //converting seconds to microseconds, chopping off any decimals
  return(baseShutter); //returns base shutter value to calling function in microseconds 1250
}









