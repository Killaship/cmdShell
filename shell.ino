#include <MemoryFree.h>

String sdata="";  // Initialised to nothing.
byte test;
bool sdPresent = false;

#include <SPI.h>
#include <SD.h>

File root;



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
    Serial.println("[WARNING] There is currently under 512 bytes of RAM.");
    Serial.println("Please consider running this on a more powerful board.\n");
  }
  
  Serial.println("[NOTE] This program assumes the use of an Arduino UNO SD card");
  Serial.println("shield available with the initialization pin at pin 10.");
  Serial.println("\nInitializing SD card...\n");
    
  Serial.println("[NOTE] Make sure your serial terminal outputs a CR character at the end of a line.");
  Serial.println("The Arduino IDE serial monitor is preferable as it lets you choose what line ending to use.\n");

  if (!SD.begin(10)) {
    Serial.println("[ERROR] NO SD CARD AVAILABLE!\n");
    Serial.println("initialization failed! continuing without SD card!\n");
  }
  Serial.println("Initialization done.");
  Serial.println("Opening root directory of SD card...");
  root = SD.open("/");


  Serial.println("Done!\n");

  Serial.println("Command Interpreter adapted from https://www.best-microcontroller-projects.com/arduino-string.html");
  Serial.println("Commands are t (test), v (set a value in memory), h (display help)");
  Serial.println(", d (list contents of SD card if there is one, and, c. (list contents of a given file.)");

}

void loop(void ) {
byte ch;
String valStr;
int val;

   if (Serial.available()) {
      ch = Serial.read();

      sdata += (char)ch;

      if (ch=='\r') {  // Command received and ready.
         sdata.trim();

         // Process command in sdata.
         switch( sdata.charAt(0) ) {
         case 't':
            test = 1;
            Serial.println("Test");
            break;
         case 'v':
            if (sdata.length()>1){
               valStr = sdata.substring(1);
               val = valStr.toInt();
            }
            Serial.print("Val ");
            Serial.println(val);
            break;
         case 'h':
            Serial.println("Command Interpreter adapted from https://www.best-microcontroller-projects.com/arduino-string.html");
            Serial.println("Commands are t (test), v (set a value in memory), h (display help)");
            Serial.println(", d (list contents of SD card if there is one, and, c. (list contents of a given file.)");
            
            break;
         case 'd':
           
           printDirectory(root, 0);
            
          
            break;
         default: 
         Serial.print(sdata);
         Serial.println(" is an invalid command!");
         break;
         } // switch

         sdata = ""; // Clear the string ready for the next command.
      } // if \r
   }  // available
}
