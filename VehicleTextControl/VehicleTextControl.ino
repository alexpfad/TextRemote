/******************************************************************************* 

 - - Vehicle SMS Controller v0.2 - -

 Author:      Alex Pfadenhauer
              (pfaden@gmail.com)

 This setup uses a Sparkfun Redboard along with a Sparkfun Cellular shield to 
 interface with a vehicle via a keyless fob. The arduino periodically checks for
 texts, and when one is recieved, it performs the appropriate action based upon
 the texted command. Much thanks for Sparkfun for their quality hardware and
 helpful documentation!

 Hardware:
  - Sparkfun Redboard
  - Sparkfun MG2639 Cellular Shield
  - OEM Remote Fob for 2014 Toyota Tundra
 
 Revisions:   v0.2 (1/2017)  -   SMS send & receive now work; SMS commands can be identified & processed.
              v0.1 (1/2017)  -   Pre-release, not yet ready. Help, run, stop, unlock, lock

 List of commands:
    HELP          Lists all of the possible commands
    RUN           Starts the engine and sends a confirmation text
    STOP          Stops the engine and sends a confirmation text
    UNLOCK        Unlocks the vehicle and sends a confirmation text
    LOCK          Locks the vehicle and sends a confirmation text
    
********************************************************************************/
#include <SoftwareSerial.h>         // used by the shield to communicate with the cellular module
#include <SFE_MG2639_CellShield.h>  // include the Sparkfun cellular shield library

#define DEBUG       1               // defines the debug variable

static char MYCELL[12] = "17034835484";         // sets my cell phone number to a constant char array
static int  LTIME = 100;                        // defines time (in ms) to send the lock signal
static int  UTIME = 100;                        // defines time (in ms) to send the unlock signal
static int  STIME = 100;                        // defines time (in ms) to send the signal to start the engine
static int  DTIME = 100;                        // defines time (in ms) to delay in between sending signals
char        LastPhoneNum[12] = "15555555555";   // creates a char array that holds the last phone number a command was recieved from
boolean     BrokenPanic = 0;                    // turned on when something goes wrong and the board can't function
int         messageIndex;
byte        unreadMsg;

void setup() {    // runs once to set up before looping begins
    if(DEBUG){    // for verification with computer
        Serial.begin(9600);             
        Serial.println(F("Type anything to begin..."));
        while(!( Serial.available() )){ ; }  // waits until the board and computer are ready to communicate
        Serial.read();
        Serial.println(F("Redboard booting up...."));
    }
    int cellbegin = cell.begin();   // starts up the cell module
    if(DEBUG){ Serial.println(F("Cellular shield starting up....")); }
    delay(5000);                   // give the cell module time to start up
    if (!(1 == cellbegin)){
       BrokenPanic = 1;
       if(DEBUG){ Serial.println(F("Cellular shield unable to start")); }
    }
    cell.getPhoneNumber(LastPhoneNum);
    if(DEBUG){
        Serial.print(F("Phone Number attached to the SIM is: "));
        Serial.println(LastPhoneNum);
    }
    sms.setMode(SMS_TEXT_MODE);     // sets the text mode to ASCII
    if(DEBUG){ Serial.println(F("Board & shield started.")); }
}

void loop() {      // loops continually while connected to power
    
    delay(10000);   //delays for 10 seconds at the beginning of each loop so it isn't constantly checking
    unreadMsg = 1;  //sets it up so the index checking loop will start during each loop of the main program
    
    if( 1 == BrokenPanic ){
        // turn on the buzzer, once hardware is ready
        if(DEBUG){ Serial.println(F("BrokenPanic is set! Something is fucked up!")); }
    }
    else{
        while(unreadMsg > 0){   // checks indices if we have already found and dealt with one message, or haven't checked yet this loop
            for (messageIndex = 1; messageIndex < 257; messageIndex++){  // checks each index
                if(DEBUG){  Serial.println(messageIndex);  } // prints debug messages (each index as it runs through)
                if(0 < sms.read(messageIndex)){  // if sms.read returns a positive value, there is something at that index
                    unreadMsg = 2;  // 2 means a message has been found
                    break;  // stops checking through each index
                }
            }
            if(1 == unreadMsg){      // if we rolled through all the indices and there were no messages
                unreadMsg = 0;      // sets it to 0 to say that no messages have been found, so don't try to check again or to read one 
            }
            else if(2 == unreadMsg) {                // if there is a message
                if(DEBUG){ Serial.println(("There is an unread message at index ") + String(messageIndex)); }
                strcpy(LastPhoneNum, sms.getSender());      // we update the number from which the last message was recieved
                String messageContents = sms.getMessage();
                if (!(isNumberApproved())) {                // the number that texted the last command isn't on the list!
                    if(DEBUG){
                        Serial.println(F("Last message from an unapproved number!"));
                        Serial.println("The number is: " + String(LastPhoneNum));
                        Serial.println(" My number is: " + String(MYCELL));
                        Serial.print(F("The message is:"));
                        Serial.println(messageContents);
                        Serial.println(F("Note: denied number notification text is not sent when debug mode is on"));
                        Serial.println("");
                    }
                    else{
                    sendText(MYCELL, String(LastPhoneNum) + (F(" just contacted the Vehicle SMS Controller with the following text:")));
                    sendText(MYCELL, messageContents);
                    }
                }
                else {  // the number is on the list! Now, let's perform the command
                    if(DEBUG){
                        Serial.print(F("messageContents[0] = "));
                        Serial.println(messageContents[0]);
                    }
                    if ('H' == messageContents[0]) {
                        if(DEBUG){ Serial.println(F("Available commands are:   HELP, RUN, STOP, LOCK, UNLOCK")); }
                        else{ sendText(MYCELL, (F("Available commands are:   HELP, RUN, STOP, LOCK, UNLOCK"))); }
                    }
                    else if ('R' == messageContents[0]) {   // Start the engine
                        startEngine();
                    }
                    else if ('S' == messageContents[0]) {   // Stop the engine
                        stopEngine();
                    }
                    else if ('U' == messageContents[0]) {   // Unlock the vehicle
                        unlockVehicle();
                    }
                    else if ('L' == messageContents[0]) {   // Lock the vehicle
                        lockVehicle();
                    }
                    else{     // The message text did not begin with one of the above letters.
                        if(DEBUG){ Serial.println(F("command not understood!")); }
                        else{ sendText(MYCELL, (F("Last command not understood. Text 'HELP' for a list of available commands"))); }
                    }
                }
                if(DEBUG){Serial.println(("Now deleting the last message at index:" + String(messageIndex)));}
                sms.deleteMessage(messageIndex);     // delete the message so it doesn't clutter up the SIM mailbox!
                unreadMsg = 1;  // tells the program we are done with this message, but we need to make sure there aren't more left
            }  // end of code that reads the message
        } // loop to check for unread messages
        if(DEBUG){  Serial.println(F("no unread messages"));  }
        delay(1000);
        } // code that handles no unread messages
} // loop()



void sendText(char theRecipient[12], String theMessage){
    if(DEBUG){ Serial.println("sending text:" + String(theMessage)); }
    sms.start(theRecipient);   // start sending the message, and specify the recipient phone number
    sms.print(theMessage);     // write the body of the message using the text passed to the function
    int smssend = sms.send();  // sends the message and returns a flag (pos for send success, neg for send failure)
    if(0 > smssend){ BrokenPanic = 1; }   // if the sending failed, the BrokenPanic variable is set
    return;
}

void sendLock(int duration) {
    if(DEBUG){ Serial.println(F("Now Locking")); }
    delay(duration);
  return;
}


void sendUnlock(int duration) {
    if(DEBUG){ Serial.println(F("Now Unlocking")); }
    delay(duration);
  return;
}


boolean isNumberApproved(){
    if(DEBUG){ Serial.print("Last rec. num is:" + String(LastPhoneNum)); }
    if (0 == (strcmp(MYCELL,LastPhoneNum))) {  // check and see if the last phone number is mine
        if(DEBUG){ Serial.println(F("The number was on the list!")); }
        return true;
    }
    if(DEBUG){ Serial.println(F("The number was NOT on the list!")); }
    return false;
}


void unlockVehicle() {
    sendLock(UTIME);     // sends the unlock signal to open the vehicle
    if(DEBUG){ Serial.println(F("The vehicle has been unlocked")); }
    else{ sendText(LastPhoneNum, (F("The vehicle is now unlocked."))); }
    return;
}


void lockVehicle() {
    sendLock(LTIME);     // sends the lock signal to lock the vehicle
    if(DEBUG){ Serial.println(F("The vehicle has been locked")); }
    else{ sendText(LastPhoneNum, (F("The vehicle is now locked."))); }
    return;
}


void startEngine() {     // to start engine, lock twice within 2 seconds, then hold lock for 3 seconds
    sendLock(LTIME);     // sends the lock signal to lock the vehicle
    delay(DTIME);        // pause to make sure commands sent by remote are distinct
    sendLock(LTIME);     // sends the lock signal to lock the vehicle
    delay(DTIME);        // pause to make sure commands sent by remote are distinct
    sendLock(STIME);     // sends the lock signal to start the vehicle
    // if hardware functionality is added to check vehicle power (which comes on only when engine is on), add checking for that here
    if(DEBUG){ Serial.println(F("The vehicle has been turned on")); }
    else{ sendText(LastPhoneNum, (F("The vehicle has been turned on."))); }
    return;
}


void stopEngine() {       // to stop the engine, one simply has to unlock the vehicle
    sendUnlock(UTIME);    // unlock the car to turn the engine off
    delay(DTIME);         // pause to make sure commands sent by remote are distinct
    sendLock(LTIME);      // lock the doors again to make sure we're all secure!
    if(DEBUG){ Serial.println(F("The vehicle has been turned off")); }
    else{ sendText(LastPhoneNum, (F("The vehicle has been turned off."))); }
    return;
}

