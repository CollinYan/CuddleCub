 #include <SPI.h>
#include <SD.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include <Adafruit_MMA8451.h>
#include <Adafruit_NeoPixel.h>
#include <AltSoftSerial.h>
#include "DFRobotDFPlayerMini.h"
// pins and static variables, change these if using a different board than Aruduino Mega //
static  int  songLength = 5;
static int lightsPin = 40;
static int numberOfLights = 12;
static int bleIRQ = 26;
static int bleCS = 33;
static int SDCardPin = 24;
static float songTime = 300000;
// AudioPlayback object 
AltSoftSerial altSerial;
DFRobotDFPlayerMini myDFPlayer;
// SD card file 
File myFile;
// Bluetooth Creadtion
Adafruit_BluefruitLE_SPI ble(bleCS, bleIRQ, -1);
// Accelorometer object creation
Adafruit_MMA8451 mma = Adafruit_MMA8451();
// Lighting object creation
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numberOfLights, lightsPin, NEO_GRB + NEO_KHZ800);
int on = -2;
long lightStartDelay;
long lightEnd;
long lightsCounter ;
String color = "";
String wcolor = "";
String sleepDelay;
String sleepMusic;

int onm = -2;
long musicStartDelay = -1;
long musicEnd = 0;
long musicCounter = 0;
int musicIndex = -1;




int onr = -2.;
long recordStartDelay = -1;
long recordEnd = 0;
long recordCounter = 0;
boolean recording = false;


void Write_SD(String inp) {
  Serial.println(F("writing to SD Card :"));
  myFile = SD.open("data.txt", FILE_WRITE);
  if (!myFile){
    Serial.println(F("ERROR: File does not exist to write to"));
  }
  myFile.println(inp);
  Serial.print(inp);
  Serial.println();
  myFile.close();
}

// converts 3 numbers into a string with a space between each number. Ex numbers(1,2,3) returns the string "1.00 2.00 3.00"
String numbersToString(float x, float y, float z) {
  String s = String(x) + " " + String(y) + " " + String(z);
  return s;
}

void parseCommand(char c[], int csize, String decider) {
  String ld = "";
  int index;

  for (int y = 5; y < csize; y++) {
    if ((String) c[y] == " ") {
      index = y + 1;
      break;
    } else {
      ld += c[y];
    }
  }
  if (decider.equals("l")) {
    if (on != 1){
      lightStartDelay = -(long) (ld.toInt());
    } else if ((String) c[2] == "o" && (String) c[3] == "f"){
      lightStartDelay = -(long) (ld.toInt());
    }
     
  } else if (decider.equals("r")) {
   
    recordStartDelay = -(long) (ld.toInt());
    
  } else if (decider.equals("m") ) {
    if (onm != 1){
    musicStartDelay = -(long) (ld.toInt());
    } else if ((String) c[2] == "o" && (String) c[3] == "f"){
      musicStartDelay = -(long) (ld.toInt());
    }
    }
    
  
  ld = "";
  for (int z = index; z < csize; z++) {
    if ((String) c[z] == " ") {
      index = z + 1;
      break;
    } else {
      ld += c[z];
    }
  }
  if (decider.equals("l")) {
    color = c[index];
    lightEnd = -(long) (ld.toInt())+60;
    
  } else if (decider.equals("r")) {
   
    recordEnd = -(long) (ld.toInt())+60;
   
  } else if (decider.equals("m")) {
      String musicIndexString;
      musicIndexString += c[index];
      musicIndex = musicIndexString.toInt();
      musicEnd = -(long) (ld.toInt())+60;
   
  }
}

void musicOn(long counter, int startDelay, int musicIndex, int endTime, boolean command) {
  //Serial.println(F("Music on function"));
  //printDetail(myDFPlayer.readType(), myDFPlayer.read());
  if (counter == startDelay) {
    Serial.println(F("Playing music"));
    myDFPlayer.playMp3Folder(musicIndex);
    Serial.println(musicIndex);
    Serial.println(F("Playing music@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"));
    myDFPlayer.play(musicIndex);
  } else if (counter == songTime && counter != endTime) {
    myDFPlayer.playMp3Folder(musicIndex);
  }
  else if (counter == endTime && counter != 0 ) {
    onm = -2;
    musicCounter = 0;
    myDFPlayer.pause();
    
  }
}

void musicOff(long counter, int startDelay, int index, boolean command) {
  Serial.println(F("Music Off function"));
  if (counter == startDelay ) {
    Serial.println(F("Stopping Music"));
    onm = -2;
    musicCounter = 0;
    myDFPlayer.pause();
    
      
    }
  
}
void SDtoBLE() {
  if (!ble.isConnected()) {
    Serial.println(F("no bluetooth connection"));
    return;
  }
  myFile;
  Serial.println(F("sending data"));
  myFile = SD.open("data.txt", FILE_READ);
  if (!myFile) {
    Serial.println(F("ERROR: File does not exist on SD card"));
  }
  else
  {
  Serial.println(F("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"));  
  }
  while (myFile.available()) {
    //Serial.println("sending data");
    char c = myFile.read();
    Serial.print(c);
    if (!ble.print(c)) {
      Serial.println(F("ERROR: Unable to send over bluetooth"));
    }
  }
  myFile.close();
  ble.println("ESD");
  SD.remove("data.txt");
}

void lightsOn(long counter, int startDelay, int endTime, String color, boolean command) {
  
  Serial.println(F("lights on function"));
  if (color.equals("r")) {
    Serial.println("R");
    if (counter == startDelay) {
      Serial.println(F("turning lights on"));
      for (int i = 0; i < numberOfLights; i++) {
        strip.setPixelColor(i, 255, 0, 0);
      }
      strip.show();
    } else if (counter == endTime && endTime != 0) {
      //Serial.println("turning lights off");
      for (int i = 0; i < 12; i++) {
        strip.setPixelColor(i, 0, 0, 0);
      }
     
      on = -2;
      
      strip.show();

    }
  } else if (color.equals("g")) {
    //Serial.println("R");
      if (counter == startDelay) {
        //Serial.println("beginning clause");
        for (int i = 0; i < numberOfLights; i++) {
          strip.setPixelColor(i, 0, 255, 0);
        }
        strip.show();
      } else if (counter == endTime && endTime != 0) {
        lightsCounter = 0;
        //Serial.println("end clause");
        for (int i = 0; i < 12; i++) {
          strip.setPixelColor(i, 0, 0, 0);
        }
        strip.show();
  
        on = -2;
     
      }
  }
}

void lightsOff(long counter, int startDelay, String color, boolean command) {
  //Serial.println(F("lights off func"));
  strip.begin();
  if (counter >= startDelay) {
    
    for (int i = 0; i < numberOfLights; i++) {
      strip.setPixelColor(i, 0, 0, 0);
    }
    strip.show();
    color = "";
    lightsCounter = 0;
    on = -2;
    
  }
}

void recordOn(long counter, int startDelay, long endTime, boolean command) {
  if (counter == startDelay) {
    onr  = 1;
    recording = true;
  } else if (counter == endTime && endTime != 0) {
    onr = -2;
    recording = false;
  }
}

void recordOff(long counter, int startDelay, long endTime, boolean command) {
  if (counter == startDelay) {
    onr = -2;
    recording = false;
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  //Serial.println();
  strip.begin();
  strip.show();
  altSerial.begin(9600);
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  if (!myDFPlayer.begin(altSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    //while(true);
  } else {
    Serial.println(F("DFPlayer Mini online."));
  }
  myDFPlayer.volume(30);
  //myDFPlayer.enableLoopAll();
  if (!ble.begin(true)){
    Serial.println(F("ERROR:Failed to begin bluetooth"));
  } 
  ble.echo(false);
  ble.verbose(false);
  if (ble.setMode(BLUEFRUIT_MODE_DATA)) {
    Serial.println(F("Switching to DATA mode!"));
    Serial.println(F("Bluetooth Online"));
  } else {
    Serial.println(F("ERROR: Failed to begin Bluetooth"));
  }
  if (!SD.begin(SDCardPin)) {
    Serial.println(F("ERROR: Failed to begin SD card"));
  } else {
    Serial.println(F("SD card online"));
  }
  if (!mma.begin()) {
    Serial.println(F("ERROR: Failed to begin accelerometer"));
  } else {
    Serial.println(F("Accelerometer online"));
  }
  mma.setRange(MMA8451_RANGE_2_G);
}

void recordHelper() {
  Serial.println(F("Record Helper Function"));
  mma.read();
  sensors_event_t event;
  mma.getEvent(&event);
  String s = numbersToString(event.acceleration.x, event.acceleration.y, event.acceleration.z);
  Serial.println(s);
  Write_SD(s);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (ble.isConnected() && ble.available()  ) {
      //Serial.println(F("recieveing command"));
      int csize = ble.available();
      char c[csize];
      for (int x = 0; x < csize; x++) {
        c[x] = (char) ble.read();
      }
      Serial.println(c);
      Serial.println("!!!!");
      //sleeptime, waketime, sleepcolor, wakecolor, sleepMusicIndex, wakeMusicIndex 
      //Serial.println("test");
      char sleepDelayc[csize];
      int x;
      int aaa =0;
      int bbb =0;
      int ccc =0;
      int ddd =0;
      for (x = 0; x < csize; x++) {
        if(c[x] != ' '){
          sleepDelayc[x] = c[x];
          Serial.println(c[x]);
        }
        else
        {
          Serial.println("sleepdelay");
          x++;
          break;  
        }
      }
      sleepDelayc[x] = '\0';
      //wakedelay
      for (x =x; x < csize; x++) {
        if(c[x] != ' ')
        {
          x = x;
          Serial.println(c[x]);
          //wakeDelay[aaa] = c[x];
          aaa++;
        }
        else
        {
          Serial.println("wd");
          x++;
          break;  
        }
      }
      //sleepcolor
      char sleepColor[csize];
      for (x=x; x < csize; x++) {
        if(c[x] != ' ')
        {
          x = x;
          Serial.println(c[x]);
          sleepColor[bbb] = c[x];
          bbb++;
        }
        else
        {
          x++;
          Serial.println("sc");
          break;  
        }
      }
      //wakecolor
      char wakeColor[csize];
      for (x=x; x < csize; x++) {
        if(c[x] != ' ')
        {
          wakeColor[ccc] = c[x];
          Serial.println(c[x]);
          ccc++;
        }
        else
        {
          x++;
          Serial.println("wc");
          break;  
        }
      }
      char sleepMusicc[csize];
      for (x=x; x < csize; x++) {
        if(c[x] != ' '){
          sleepMusicc[ddd] = c[x];
          Serial.println(c[x]);
          ddd++;
        }
        else
        {
          x++;
          Serial.println("sm");
          break;  
        }
      }
      
      //Serial.println(indicator);
      int asdf = 0;
      //Serial.println(F("music if statement"));
        if (((String)sleepDelayc).toInt()>0) {
          onm = 1;
          on = 1;
        } else {
          onm = -1;
          on = -1;
        }
        color = (String) sleepColor;
        wcolor = (String) wakeColor;
        Serial.println(color);
        Serial.println(wcolor);
        musicCounter = 0;
        lightsCounter = 0;
        sleepDelay = (String) sleepDelayc;
        sleepMusic = (String) sleepMusicc;
        Serial.println(sleepMusic);

  }
  if (recording) {
    Serial.println(F("recording"));
    recordHelper();
    
  }

  if (onm == 1) {
    Serial.println(sleepDelay);
    Serial.println(sleepMusic);
    Serial.println(F("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"));
    musicOn(musicCounter , sleepDelay.toInt(), sleepMusic.toInt(), sleepDelay.toInt()+10,false);
    lightsOn(musicCounter,sleepDelay.toInt(), sleepDelay.toInt()+10, "r" ,false);
    musicCounter += 1;
  } else if (onm <= -1) {
    Serial.println(musicStartDelay);
    musicOff(musicCounter, sleepDelay.toInt(), sleepMusic.toInt(),false);
    lightsOff(musicCounter, sleepDelay.toInt(), color,false);
    if(sleepDelay.toInt() > -1 && musicCounter>=sleepDelay.toInt()+10)
    {
      musicCounter -= 86400;
      Serial.println(musicCounter);
      onm = 1;
    }
    musicCounter += 1;
  }


  

  
  if (onm == 1) {
    recordOn(musicCounter, sleepDelay.toInt(), recordEnd,false);
    recordCounter += 1;
  } else if (onm <= -1) {
    recordOff(musicCounter, sleepDelay.toInt(), recordEnd,false);
    recordCounter += 1;
  }

 

  
    
  Serial.println();
  delay(1000);
}
