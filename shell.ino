
String sdata="";  // Initialised to nothing.
byte test;
bool sdPresent = false;

#include <SPI.h>
#include <SD.h>
#include <MemoryFree.h>
#include <avr/wdt.h>

File root;

void printHelp() {
  Serial.println(F("Command Interpreter adapted from https://www.best-microcontroller-projects.com/arduino-string.html"));
  Serial.println(F("Make sure the line ending that your monitor sends is \r, for sake of program compatibility."));
  Serial.println(F("Commands are v (set a value in memory), h (display help)"));
  Serial.println(F(", d (list contents of SD card if there is one, c. (list contents of a given file.),"));
  Serial.println(F("r (reboot), t (text editor, edits text of a given file.), R (delete a given file)"));  
}

void rmFile(String file) {
  SD.remove(file);
  
}

void textEdit(String file) {
  String fileIn;
  Serial.println(F("||Barebones Typewriter-style text editor||"));
  Serial.println(F("Backspace is disabled because technical raisins."));
  Serial.println(F("Type \"quitwrite\" to exit the editor."));
  File editfile = SD.open(file, FILE_WRITE);
  if(file) {
  
  while(1) {
   
    fileIn = Serial.readStringUntil("\r");
    if(fileIn == "quitwrite\r") {
      break;
    }
    else {
      if(fileIn != "\n") {
          editfile.println(fileIn + "\n");
      }
    }
    
    
  }
  
  editfile.close();
  }
  else {
  Serial.println("[ERROR] Failed to open file");
}
}
void printFile(String file) {
   
      File dataFile = SD.open(file);
      // if the file is available, write to it:
      if (dataFile) {
         while (dataFile.available()) {
         Serial.write(dataFile.read());
      }   }    
      // if the file isn't open, pop up an error:
      else {
          Serial.print("\n[ERROR] Failed to open ");
          Serial.println(file);
     }
     dataFile.close();
            }



void reboot() {
  wdt_disable();
  wdt_enable(WDTO_15MS);
  while (1) {}
}

void printDirectory(File dir, int numTabs) {
  while (true) {

    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    for (uint8_t i = 0; i < numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs + 1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
}




void setup (void) {
  Serial.begin(9600);
  Serial.print("boot");
  delay(500);
  Serial.print(".");
  delay(500);
  Serial.print(".");
  delay(500);
  Serial.print(".");
  delay(500);
  Serial.println(".\n");

  Serial.print("free memory (bytes): ");
  Serial.println(freeMemory());

  if(freeMemory() < 512) {
    Serial.println(F("[WARNING] There is currently under 512 bytes of RAM."));
    Serial.println(F("Please consider running this on a more powerful board.\n"));
  }
  
  Serial.println(F("[NOTE] This program assumes the use of an Arduino UNO SD card"));
  Serial.println(F("shield available with the initialization pin at pin 10."));
  Serial.println(F("\nInitializing SD card...\n"));

  sdPresent = true;
  if (!SD.begin(10)) {
    Serial.println(F("[ERROR] NO SD CARD AVAILABLE!\n"));
    Serial.println(F("initialization failed! continuing without SD card!\n"));
    sdPresent = false;
  }
  Serial.println(F("Initialization done."));
  Serial.println(F("Opening root directory of SD card..."));
  root = SD.open("/");


  Serial.println(F("Done!\n"));

  printHelp();

}

void loop(void) {
String valStr;
String fileName;
String editfileName;
String delFile;
int val;
byte ch;
   if (Serial.available()) {
      ch = Serial.read();

      sdata += (char)ch;

      if (ch=='\r') {  // Command received and ready.
         sdata.trim();

         // Process command in sdata.
         switch( sdata.charAt(0) ) {
          
         case 'c':
             if (sdata.length()>1){
               fileName = sdata.substring(1);
               
            }
            root.close();
            if(sdPresent == true) {
            printFile(fileName);
            }
            else{
              Serial.println(F("[ERROR] SD card not present!"));
            }
           
            root = SD.open("/");
            break;
         case 'v':
            if (sdata.length()>1){
               valStr = sdata.substring(1);
               val = valStr.toInt();
            }
            Serial.print(F("Val "));
            Serial.println(val);
            break;
         case 'h':
         printHelp();
            break;
         case 'd':
            if(sdPresent == true) {
              
            printDirectory(root, 0);
            
            root = SD.open("/");
            root.rewindDirectory();
         } 
         else {
          Serial.println(F("[ERROR] SD card not present!"));
         }
            break;
         case 'r':
            reboot();
            break;
         default: 
         Serial.print(sdata);
         Serial.println(" is an invalid command!");
         break;
          // switch
         case 't':
          if (sdata.length()>1){
               editfileName = sdata.substring(1);
               
            }
            root.close();
            textEdit(editfileName);
            root = SD.open("/");
         
         break;
        case 'R':
           if (sdata.length()>1){
               delFile = sdata.substring(1);
            }
         rmFile(delFile);
         break;
         }
         sdata = ""; // Clear the string ready for the next command.
      } // if \r
   }  // available
}
