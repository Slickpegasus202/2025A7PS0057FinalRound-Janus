#include <TinyGPS++.h>
#include <SoftwareSerial.h>


 const int RXPin = 4, TXPin = 3;
  SoftwareSerial gpsSerial(RXPin, TXPin);

TinyGPSPlus gps;



int state = 0;

double currentalt = 0;
double lastalt = 0;
double apogeealt = 0;
bool payload = false;


void configgps() {
  gpsSerial.print("$PQTMNMMSG,0,0*2A\r\n");   // Disable all NMEA
  delay(200);

  gpsSerial.print("$PQTMNMMSG,1,RMC,1*29\r\n"); // Enable RMC
  delay(200);

  gpsSerial.print("$PQTMNMMSG,1,GGA,1*2C\r\n"); // Enable GGA
  delay(200);

  gpsSerial.print("$PQTMNVRAM,2,1*3F\r\n");      
  delay(200);
}

void setup() {
  Serial.begin(115200);   
  gpsSerial.begin(9600);  

 

  configgps();
  
}

void loop() {
  while (gpsSerial.available() > 0) {
    char c = gpsSerial.read();
    gps.encode(c);

    // i would ideally add a kalman filter to filter my altitude data, but no time rn :/

    if (gps.location.isUpdated() && gps.altitude.isUpdated()) {
      lastalt = currentalt;
      currentalt = gps.altitude.meters();

     
    if (currentalt > apogeealt) {
        apogeealt = currentalt;
      }

     
    switch (state) {
      case 0:
        if (currentalt > lastalt + 3){
          state = 1;
          }
          break;



        case 1:
          if (currentalt < lastalt - 3) {
            state =2;
          }
          break;

        case 2:
        if(currentalt< lastalt-5){  
          state = 3;
        }
          break;

        case 3:
          if (!payload && currentalt <= 0.75 * apogeealt) {
            state = 4;
            payload = true;
          } else if (currentalt < 1) {
            state = 5;
          }
          break;

        case 4:
          if (currentalt < 1) {
            state = 5;
          }
          break;

        case 5:
          
          break;
      }


      if (gps.time.isValid()) {
        Serial.print("Time (UTC): ");
          if (gps.time.hour() < 10) {
          Serial.print("0");
          }
        Serial.print(gps.time.hour()); 
        Serial.print(":");
      

        if (gps.time.minute() < 10){ 
          Serial.print("0");
        }
        Serial.print(gps.time.minute()); 
        Serial.print(":");
        

        if (gps.time.second() < 10) {
        Serial.print("0");
        }
        Serial.println(gps.time.second());
      }
      

      if (gps.location.isValid()) {
        Serial.print("Latitude : "); 
        Serial.println(gps.location.lat(), 6);

        Serial.print("Longitude: "); 
         Serial.println(gps.location.lng(), 6);
      }

      if (gps.altitude.isValid()) {
        Serial.print("MSL Altitude: ");
        Serial.print(currentalt);
        Serial.println("m");
      }

      Serial.print("State: ");
      switch (state) {
        case 0: Serial.println("IDLE"); break;
        case 1: Serial.println("ASCENT"); break;
        case 2: Serial.println("APOGEE"); break;
        case 3: Serial.println("DESCENT"); break;
        case 4: Serial.println("PAYLOAD_DEPLOYED"); break;
        case 5: Serial.println("LANDED"); break;
      }

      Serial.println();
    }
  }
}