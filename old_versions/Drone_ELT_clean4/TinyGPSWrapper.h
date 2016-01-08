/*
TinyGPS - a small GPS library for Arduino providing basic NMEA parsing
Based on work by and "distance_to" and "course_to" courtesy of Maarten Lamers.
Suggestion to add satellites(), course_to(), and cardinal(), by Matt Monson.
Precision improvements suggested by Wayne Holder.
Copyright (C) 2008-2013 Mikal Hart
All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef TinyGPSWrapper_h
#define TinyGPSWrapper_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "TinyGPS.h"
#include <stdlib.h>
TinyGPS tinyGPS;
//const char *  strum = "f4ji$GPGGA,183730,3907.356,N,12102.482,W,1,05,1.6,646.4,M,-24.1,M,,*75\nff3fs$GPRMC,183729,A,3907.356,N,12102.482,W,000.0,360.0,080301,015.5,E*6F";
//const char *strum4 = "$GNGGA,232924.80,5212.87930,N,02110.59355,E,1,06,3.64,94.9,M,34.1,M,,*7F$GNRMC,232925.00,A,5212.87935,N,02110.59339,E,0.208,,011215,,,A*6C";
const char *strum5 = //"$GNGGA,232924.80,5212.87930,N,02110.59355,E,1,06,3.64,94.9,M,34.1,M,,*7F\n$GNRMC,232925.00,A,5212.87935,N,02110.59339,E,0.208,,011215,,,A*6C\n";
"$GPGSV,2,1,05,02,13,138,22,12,67,270,33,14,28,311,34,24,76,177,13*7D\n\
$GPGSV,2,2,05,25,26,263,35*4E\n\
$GNRMC,225101.80,A,5212.88580,N,02110.57865,E,0.065,,081215,,,A*6D\n\
$GLGSV,1,1,02,68,22,308,37,,,,29*5D\n\
$GNGLL,5212.88581,N,02110.57863,E,225101.60,A,A*71\n";


//"$GNGGA,232924.80,5212.87930,N,02110.59355,E,1,06,3.64,94.9,M,34.1,M,,*7F";
// GNRMC - to! i musi byc newline!
//"$GNGGA,232924.80,5212.87930,N,02110.59355,E,1,06,3.64,94.9,M,34.1,M,,*7F$GNRMC,232925.00,A,5212.87935,N,02110.59339,E,0.208,,011215,,,A*6C";
//$GNRMC,232925.00,A,5212.87935,N,02110.59339,E,0.208,,011215,,,A*6C\
//$GNGLL,5212.88581,N,02110.57863,E,225101.60,A,A*71";
//$GNRMC,225101.80,A,5212.88580,N,02110.57865,E,0.065,,081215,,,A*6D\

//"$GNGGA,225101.60,5212.88581,N,02110.57863,E,1,05,4.02,132.5,M,34.1,M,,*44";
//"$GNGSA,A,3,14,12,25,24,,,,,,,,,6.57,4.02,5.20*1E"\
//"$GNGSA,A,3,68,,,,,,,,,,,,6.57,4.02,5.20*17"\
//"$GPGSV,2,1,05,02,13,138,22,12,67,270,33,14,28,311,34,24,76,177,13*7D"\
//"$GPGSV,2,2,05,25,26,263,35*4E";

//"$GLGSV,1,1,02,68,22,308,36,,,,29*5C"\
//"$GNGLL,,,,,225057.20,V,N*51"\
//"$GNRMC,225057.40,V,,,,,,,081215,,,N*6F"\
//"$GNVTG,,,,,,,,,N*2E";
//"$GNGGA,225057.40,,,,,0,04,45.86,,,,,,*70"\
//"$GNGSA,A,1,14,12,25,,,,,,,,,,62.68,45.86,42.73*28"\
//"$GNGSA,A,1,68,,,,,,,,,,,,62.68,45.86,42.73*27"\
//"$GPGSV,2,1,05,02,13,138,21,12,67,270,33,14,28,311,33,24,76,177,08*73"\
//"$GPGSV,2,2,05,25,26,263,35*4E"\
//"$GLGSV,1,1,02,68,22,308,36,,,,29*5C";
/*"$GNGSA,A,3,14,24,12,25,,,,,,,,,5.89,3.64,4.63*1F"\
"$GNGSA,A,3,79,69,,,,,,,,,,,5.89,3.64,4.63*19"\
"$GPGSV,1,1,04,12,71,276,29,14,30,307,28,24,72,170,28,25,30,266,37*75"\
"$GLGSV,2,1,06,68,07,254,,69,20,302,28,77,47,105,18,78,68,331,*62"\
"$GLGSV,2,2,06,79,24,305,39,81,17,183,*62"\
"$GNGLL,5212.87930,N,02110.59355,E,232924.80,A,A*7F"\
"$GNRMC,232925.00,A,5212.87935,N,02110.59339,E,0.208,,011215,,,A*6C"\
"$GNVTG,,T,,M,0.208,N,0.385,K,A*39";
*/

TinyGPS * giveTinyGPS(){
  return &tinyGPS;
}

void readAndParseTest(){
 for(int i = 0; i < 400; i++){
//    Serial.write(strum2[i]);
         bool result = tinyGPS.encode(strum5[i]);
         if(result == true){
           Serial.write("pozycja=");
            Serial.print(i);
            Serial.print(strum5[i]);
             Serial.write("fix=");
           Serial.print(tinyGPS.satellites());
    unsigned long age;
    float flat, flon;
    tinyGPS.f_get_position(&flat, &flon, &age);
    Serial.print("LAT=");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    Serial.print(" LON=");
  Serial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    Serial.print(" SAT=");
    Serial.print(tinyGPS.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : tinyGPS.satellites());
    Serial.print(" PREC=");
    Serial.print(tinyGPS.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : tinyGPS.hdop());
    break;
/*    //GPS mode
    Serial.print(" Constellations=");
    Serial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0 : tinyGPS.constellations());
  */

         }
  }
  Serial.print("koniec testu");
}

bool readAndParse(){
//  readAndParseTest();
 while(Serial.available() > 0) { 
  
  //    mavlink_active = 1;//test
  //    lastMAVBeat = millis();
        uint8_t c = Serial.read();
        Serial.write(c);
        bool result = tinyGPS.encode(c);
        if(result)
         return true;      
        //        Serial.write(c);
 }
  return false;
}


#endif
