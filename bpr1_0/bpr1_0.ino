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

  //LAUNCH SPLASH SCREEN
  splashScreen(); //launches splash screen function

    //GET MODE (panorama, hdr, rtv, timelapse or trig)
  mode = getMode(); //gets user input for desired mode of robot

  //TESTING MODE DISPLAY DELETE!
  lcd.clear(); //clears LCD DELETE
  lcd.print(mode); //DELETE
  delay(5000); //DELETE!

  //GET MODE PREFERENCES
  modePref = getModePref(mode); //passes desired mode to function to get preferences for that mode

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
  switch (modePref) { //comparing case values with lrTrack
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
  //DECLARING & INITIALIZING VARIABLES
  int modePref = 0; //stores value indicating preferences for mode chosen by user, is returned to calling function

  //WRITE CODE TO GET PREFERENCES HERE

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
  //DECLARING & INITIALIZING VARIABLES
  int modePref = 0; //stores value indicating preferences for mode chosen by user, is returned to calling function

  //WRITE CODE TO GET PREFERENCES HERE

  return(modePref); //returns rtv preferences user requested to calling function
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
  //DECLARING & INITIALIZING VARIABLES
  int modePref = 0; //stores value indicating preferences for mode chosen by user, is returned to calling function

  //WRITE CODE TO GET PREFERENCES HERE

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
  //DECLARING & INITIALIZING VARIABLES
  int modePref = 0; //stores value indicating preferences for mode chosen by user, is returned to calling function

  //WRITE CODE TO GET PREFERENCES HERE

  return(modePref); //returns trig preferences user requested to calling function
}










