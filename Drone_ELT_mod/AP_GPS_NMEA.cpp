// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

//
// NMEA parser, adapted by Michael Smith from TinyGPS v9:
//
// TinyGPS - a small GPS library for Arduino providing basic NMEA parsing
// Copyright (C) 2008-9 Mikal Hart
// All rights reserved.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//

/// @file	AP_GPS_NMEA.cpp
/// @brief	NMEA protocol parser
///
/// This is a lightweight NMEA parser, derived originally from the
/// TinyGPS parser by Mikal Hart.
///

/*
SA,A,1,14,12,25,,,,,,,,,,28.64,19.97,20.52*24
$GNGSA,A,1,79,69,,,,,,,,,,,28.64,19.97,20.52*24
$GPGSV,1,1,04,12,71,275,26,14,30,307,30,24,72,170,28,25,30,266,37*70
$GLGSV,1,1,04,68,07,254,,69,20,302,35,77,47,105,,79,24,304,41*6C
$GNGLL,,,,,232852.40,V,N*5C
$GNRMC,232852.60,V,,,,,,,011215,,,N*6F
$GNVTG,,,,,,,,,N*2E
$GNGGA,232852.60,,,,,0,05,19.97,,,,,,*71
$GNGSA,A,1,14,12,25,,,,,,,,,,28.63,19.97,20.52*23
$GNGSA,A,1,79,69,,,,,,,,,,,28.63,19.97,20.52*23
$GPGSV,1,1,04,12,71,275,26,14,30,307,30,24,72,170,28,25,30,266,37*70
$GLGSV,1,1,04,68,07,254,,69,20,302,35,77,47,105,,79,24,304,41*6C
$GNGLL,,,,,232852.60,V,N*5E
$GNRMC,232852.80,V,,,,,,,011215,,,N*61
$GNVTG,,,,,,,,,N*2E
$GNGGA,232852.80,,,,,0,05,19.96,,,,,,*7E
$GNGSA,A,1,14,12,25,,,,,,,,,,28.62,19.96,20.51*20
$GNGSA,A,1,79,69,,,,,,,,,,,28.62,19.96,20.51*20
$GPGSV,1,1,04,12,71,275,26,14,30,307,30,24,72,170,28,25,30,266,37*70
$GLGSV,1,1,04,68,07,254,,69,20,302,36,77,47,105,,79,24,304,42*6C
$GNGLL,,,,,232852.80,V,N*50
$GNRMC,232853.00,V,,,,,,,011215,,,N*68
$GNVTG,,,,,,,,,N*2E
$GNGGA,232853.00,,,,,0,05,19.96,,,,,,*77
$GNGSA,A,1,14,12,25,,,,,,,,,,28.62,19.96,20.51*20
$GNGSA,A,1,79,69,,,,,,,,,,,28.62,19.96,20.51*20
$GPGSV,1,1,04,12,71,275,26,14,30,307,30,24,72,170,28,25,30,266,37*70
$GLGSV,1,1,04,68,07,254,,69,20,302,36,77,47,105,,79,24,304,42*6C
$GNGLL,,,,,232853.00,V,N*59
$GNRMC,232853.20,V,,,,,,,011215,,,N*6A
$GNVTG,,,,,,,,,N*2E
$GNGGA,232853.20,,,,,0,05,19.95,,,,,,*76
$GNGSA,A,1,14,12,25,,,,,,,,,,28.61,19.95,20.50*21
$GNGSA,A,1,79,69,,,,,,,,,,,28.61,19.95,20.50*21
$GPGSV,1,1,04,12,71,275,26,14,30,307,30,24,72,170,28,25,30,266,37*70
$GLGSV,1,1,04,68,07,254,,69,20,302,36,77,47,105,,79,24,304,42*6C
$GNGLL,,,,,232853.20,V,N*5B
$GNRMC,232853.40,V,,,,,,,011215,,,N*6C
$GNVTG,,,,,,,,,N*2E
$GNGGA,232853.40,,,,,0,05,19.95,,,,,,*70
$GNGSA,A,1,14,12,25,,,,,,,,,,28.60,19.95,20.50*20
$GNGSA,A,1,79,69,,,,,,,,,,,28.60,19.95,20.50*20
$GPGSV,1,1,04,12,71,275,26,14,30,307,30,24,72,170,28,25,30,266,37*70
$GLGSV,1,1,04,68,07,254,,69,20,302,36,77,47,105,,79,24,304,41*6F
$GNGLL,,,,,232853.40,V,N*5D
$GNRMC,232853.60,V,,,,,,,011215,,,N*6E
$GNVTG,,,,,,,,,N*2E
$GNGGA,232853.60,,,,,0,05,19.94,,,,,,*73
$GNGSA,A,1,14,12,25,,,,,,,,,,28.60,19.94,20.49*29
$GNGSA,A,1,79,69,,,,,,,,,,,28.60,19.94,20.49*29
$GPGSV,1,1,04,12,71,275,26,14,30,307,30,24,72,170,28
*/

//#include <AP_Common.h>

//#include <AP_Progmem.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h> 

#include "AP_GPS_NMEA.h"
#include <FastSerial.h>
// SiRF init messages //////////////////////////////////////////////////////////
//
// Note that we will only see a SiRF in NMEA mode if we are explicitly configured
// for NMEA.  GPS_AUTO will try to set any SiRF unit to binary mode as part of
// the autodetection process.
//
/*
 const char AP_GPS_NMEA::_SiRF_init_string[] PROGMEM =
    "$PSRF103,0,0,1,1*25\r\n"   // GGA @ 1Hz
    "$PSRF103,1,0,0,1*25\r\n"   // GLL off
    "$PSRF103,2,0,0,1*26\r\n"   // GSA off
    "$PSRF103,3,0,0,1*27\r\n"   // GSV off
    "$PSRF103,4,0,1,1*20\r\n"   // RMC off
    "$PSRF103,5,0,1,1*20\r\n"   // VTG @ 1Hz
    "$PSRF103,6,0,0,1*22\r\n"   // MSS off
    "$PSRF103,8,0,0,1*2C\r\n"   // ZDA off
    "$PSRF151,1*3F\r\n"                 // WAAS on (not always supported)
    "$PSRF106,21*0F\r\n"                // datum = WGS84
    "";
*/
// MediaTek init messages //////////////////////////////////////////////////////
//
// Note that we may see a MediaTek in NMEA mode if we are connected to a non-DIYDrones
// MediaTek-based GPS.
//
/*
const char AP_GPS_NMEA::_MTK_init_string[] PROGMEM =
    "$PMTK314,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n" // GGA & VTG once every fix
    "$PMTK330,0*2E\r\n"                                                                         // datum = WGS84
    "$PMTK313,1*2E\r\n"                                                                 // SBAS on
    "$PMTK301,2*2E\r\n"                                                                 // use SBAS data for DGPS
    "";
*/
// ublox init messages /////////////////////////////////////////////////////////
//
// Note that we will only see a ublox in NMEA mode if we are explicitly configured
// for NMEA.  GPS_AUTO will try to set any ublox unit to binary mode as part of
// the autodetection process.
//
// We don't attempt to send $PUBX,41 as the unit must already be talking NMEA
// and we don't know the baudrate.
//
/*
const prog_char AP_GPS_NMEA::_ublox_init_string[] PROGMEM =
    "$PUBX,40,gga,0,1,0,0,0,0*7B\r\n"   // GGA on at one per fix
    "$PUBX,40,vtg,0,1,0,0,0,0*7F\r\n"   // VTG on at one per fix
    "$PUBX,40,rmc,0,0,0,0,0,0*67\r\n"   // RMC off (XXX suppress other message types?)
    "";
*/
// NMEA message identifiers ////////////////////////////////////////////////////
//
const char AP_GPS_NMEA::_gprmc_string[] PROGMEM = "GPRMC";
const char AP_GPS_NMEA::_gnrmc_string[] PROGMEM = "GNRMC";
const char AP_GPS_NMEA::_gpgga_string[] PROGMEM = "GPGGA";
const char AP_GPS_NMEA::_gngga_string[] PROGMEM = "GNGGA";
const char AP_GPS_NMEA::_gpvtg_string[] PROGMEM = "GPVTG";

// Convenience macros //////////////////////////////////////////////////////////
//
#define DIGIT_TO_VAL(_x)        (_x - '0')

/*
$GNGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*2E
$GNGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*2E
$GPGSV,2,1,08,02,19,134,,03,00,021,,06,28,096,,12,73,280,*70
$GPGSV,2,2,08,14,31,304,,15,07,193,,17,22,047,,25,32,267,*79
$GLGSV,2,1,06,67,05,252,,69,13,352,,76,43,108,,77,72,337,*65
$GLGSV,2,2,06,78,19,305,,87,50,163,*6C
$GNGLL,,,,,233831.00,V,N*5C


z netu:

$GPGGA,183730,3907.356,N,12102.482,W,1,05,1.6,646.4,M,-24.1,M,,*75
$GPRMC,183729,A,3907.356,N,12102.482,W,000.0,360.0,080301,015.5,E*6F
$GPVTG,096.5,T,083.5,M,0.0,N,0.0,K,D*22


*/

// Public Methods //////////////////////////////////////////////////////////////
/*
void AP_GPS_NMEA::init(AP_HAL::UARTDriver *s, enum GPS_Engine_Setting nav_setting)
{
    _port = s;

    // send the SiRF init strings
    _port->print_P((const prog_char_t *)_SiRF_init_string);

    // send the MediaTek init strings
    _port->print_P((const prog_char_t *)_MTK_init_string);

    // send the ublox init strings
    _port->print_P((const prog_char_t *)_ublox_init_string);
}
*/

bool AP_GPS_NMEA::read(void)
{
    int16_t numc;
    bool parsed = false;

//    numc = _port->available();
    while (numc--) {
//        if (_decode(_port->read())) {
//            parsed = true;
//        }
    }
    return parsed;
}


void AP_GPS_NMEA::test(){
  Serial.print("pocz testu");  
const char *  strum = "f4ji$GPGGA,183730,3907.356,N,12102.482,W,1,05,1.6,646.4,M,-24.1,M,,*75\nff3fs$GPRMC,183729,A,3907.356,N,12102.482,W,000.0,360.0,080301,015.5,E*6F";
const char *strum4 = "$GNGGA,232924.80,5212.87930,N,02110.59355,E,1,06,3.64,94.9,M,34.1,M,,*7Fde$GNRMC,232925.00,A,5212.87935,N,02110.59339,E,0.208,,011215,,,A*6C";
//const char *strum2 = "$GPGSV,1,1,04,12,71,275,26,14,30,307,30,24,72,170,28,25,30,266,37*70$GLGSV,1,1,03,68,07,254,,69,20,302,35,79,24,304,41*5C$GNGLL,,,,,232851.80,V,N*53$GNRMC,232852.00,V,,,,,,,011215,,,N*69$GNVTG,,,,,,,,,N*2E$GNGGA,232852.00,,,,,0,05,19.98,,,,,,*78$"\
//"GNGSA,A,1,14,12,25,,,,,,,,,,28.65,19.98,20.53*2B$GNGSA,A,1,79,69,,,,,,,,,,,28.65,19.98,20.53*2B$";//GPGSV,1,1,04,12,71,275,26,14,30,307,30,24,72,170,28,25,30,266,37*70$GLGSV,1,1,03,68,07,254,,69,20,302,35,79,24,304,41*5C$GNGLL,,,,,232852.00,V,N*58$GNRMC,232852.20,V,,,,,,,011215,,,N*6B$GNVTG,,,,,,,,,N*2E$GNGGA,232852.20,,,,,0,05,19.98,,,,,,*7A$GNGSA,A,1,14,12,25,,,,,,,,,,28.64,19.98,20.52*2B$GNGSA,A,1,79,69,,,,,,,,,,,28.64,19.98,20.52*2B$GPGSV,1,1,04,12,71,275,26,14,30,307,30,24,72,170,28,25,30,266,37*70";

/*
const char *strum3 =  "$GNRMC,232924.80,A,5212.87930,N,02110.59355,E,0.231,,011215,,,A*60"\
"$GNVTG,,T,,M,0.231,N,0.428,K,A*33"\
"$GNGGA,232924.80,5212.87930,N,02110.59355,E,1,06,3.64,94.9,M,34.1,M,,*7F"\
// $GPGGA,183730,3907.356,N,12102.482,W,1,05,1.6,646.4,M,-24.1,M,,*75
"$GNGSA,A,3,14,24,12,25,,,,,,,,,5.89,3.64,4.63*1F"\
"$GNGSA,A,3,79,69,,,,,,,,,,,5.89,3.64,4.63*19"\
"$GPGSV,1,1,04,12,71,276,29,14,30,307,28,24,72,170,28,25,30,266,37*75"\
"$GLGSV,2,1,06,68,07,254,,69,20,302,28,77,47,105,18,78,68,331,*62"\
"$GLGSV,2,2,06,79,24,305,39,81,17,183,*62"\
"$GNGLL,5212.87930,N,02110.59355,E,232924.80,A,A*7F"\
"$GNRMC,232925.00,A,5212.87935,N,02110.59339,E,0.208,,011215,,,A*6C"\
"$GNVTG,,T,,M,0.208,N,0.385,K,A*39"\
"$GNGGA,232925.00,5212.87935,N,02110.59339,E,1,06,3.64,95.1,M,34.1,M,,*70"\
"$GNGSA,A,3,14,24,12,25,,,,,,,,,5.89,3.64,4.63*1F";
*/
/*
$GNGSA,A,3,79,69,,,,,,,,,,,5.89,3.64,4.63*19
$GPGSV,1,1,04,12,71,276,29,14,30,307,28,24,72,170,28,25,30,266,37*75
$GLGSV,2,1,06,68,07,254,,69,20,302,28,77,47,105,17,78,68,331,*6D
$GLGSV,2,2,06,79,24,305,39,81,17,183,*62
$GNGLL,5212.87935,N,02110.59339,E,232925.00,A,A*79
$GNRMC,232925.20,A,5212.87938,N,02110.59323,E,0.202,,011215,,,A*62
$GNVTG,,T,,M,0.202,N,0.375,K,A*3C
$GNGGA,232925.20,5212.87938,N,02110.59323,E,1,06,3.64,95.3,M,34.1,M,,*76
$GNGSA,A,3,14,24,12,25,,,,,,,,,5.89,3.64,4.63*1F
$GNGSA,A,3,79,69,,,,,,,,,,,5.89,3.64,4.63*19
$GPGSV,1,1,04,12,71,276,29,14,30,307,28,24,72,170,28,25,30,266,37*75
$GLGSV,2,1,06,68,07,254,,69,20,302,28,77,47,10";
*/
  for(int i = 0; i < 400; i++){
//    Serial.write(strum2[i]);
         bool result = readELT(strum4[i]);
         if(result == true){
          Serial.write(i);
             Serial.write("fix=");
           Serial.print(fix);
         }
  }
  Serial.print("koniec testu");
}



void AP_GPS_NMEA::receiveAndRead(){

      while(Serial.available() > 0) { 
  
  //    mavlink_active = 1;//test
  //    lastMAVBeat = millis();
        uint8_t c = Serial.read();
        Serial.write(c);
//        continue;
         bool result = readELT(c);
         /*
         if(result)
          Serial.write("t "); 
          else 
                    Serial.write("f "); */
                  //  Serial.write(c);
                    if(1 &&result){
      Serial.write("NumSat: = ");
      Serial.write(num_sats);
      Serial.write("fix=");
     Serial.print(fix);
    Serial.write("\n");
                    }
    }

}

bool AP_GPS_NMEA::readELT(char c){
      if(_decode(c))
        return true;
      return false;  
}

bool AP_GPS_NMEA::_decode(char c)
{
    bool valid_sentence = false;

    switch (c) {
    case ',': // term terminators
        _parity ^= c;
    case '\r':
    case '\n':
    case '*':
        if (_term_offset < sizeof(_term)) {
            _term[_term_offset] = 0;
            valid_sentence = _term_complete();
        }
        ++_term_number;
        _term_offset = 0;
        _is_checksum_term = c == '*';
        return valid_sentence;

    case '$': // sentence begin
        _term_number = _term_offset = 0;
        _parity = 0;
        _sentence_type = _GPS_SENTENCE_OTHER;
        _is_checksum_term = false;
        _gps_data_good = false;
        return valid_sentence;
    }

    // ordinary characters
    if (_term_offset < sizeof(_term) - 1)
        _term[_term_offset++] = c;
    if (!_is_checksum_term)
        _parity ^= c;

    return valid_sentence;
}

//
// internal utilities
//
int16_t AP_GPS_NMEA::_from_hex(char a)
{
    if (a >= 'A' && a <= 'F')
        return a - 'A' + 10;
    else if (a >= 'a' && a <= 'f')
        return a - 'a' + 10;
    else
        return a - '0';
}

uint32_t AP_GPS_NMEA::_parse_decimal()
{
    char *p = _term;
    uint32_t ret = 100UL * atol(p);
    while (isdigit(*p))
        ++p;
    if (*p == '.') {
        if (isdigit(p[1])) {
            ret += 10 * (p[1] - '0');
            if (isdigit(p[2]))
                ret += p[2] - '0';
        }
    }
    return ret;
}

uint32_t AP_GPS_NMEA::_parse_degrees()
{
    char *p, *q;
    uint8_t deg = 0, min = 0;
    uint32_t frac_min = 0;
    int32_t ret = 0;

    // scan for decimal point or end of field
    for (p = _term; isdigit(*p); p++)
        ;
    q = _term;

    // convert degrees
    while ((p - q) > 2) {
        if (deg)
            deg *= 10;
        deg += DIGIT_TO_VAL(*q++);
    }

    // convert minutes
    while (p > q) {
        if (min)
            min *= 10;
        min += DIGIT_TO_VAL(*q++);
    }

    // convert fractional minutes
    // expect up to four digits, result is in
    // ten-thousandths of a minute
    if (*p == '.') {
        q = p + 1;
        for (int16_t i = 0; i < 5; i++) {
            frac_min = (int32_t)(frac_min * 10);
            if (isdigit(*q))
                frac_min += *q++ - '0';
        }
    }
    ret = (int32_t)deg * (int32_t)1000000UL + (int32_t)((min * 100000UL + frac_min) / 6UL);
    return ret;
}

// Processes a just-completed term
// Returns true if new sentence has just passed checksum test and is validated
bool AP_GPS_NMEA::_term_complete()
{
    // handle the last term in a message
    if (_is_checksum_term) {
        uint8_t checksum = 16 * _from_hex(_term[0]) + _from_hex(_term[1]);
        if (checksum == _parity) {
            if (_gps_data_good) {
                switch (_sentence_type) {
//                case _GPS_SENTENCE_GPRMC:
                                case _GPS_SENTENCE_GPRMC:
                    time                        = _new_time;
                    date                        = _new_date;
                    latitude            = _new_latitude * 10;   // degrees*10e5 -> 10e7
                    longitude           = _new_longitude * 10;  // degrees*10e5 -> 10e7
                    ground_speed        = _new_speed;
                    ground_course       = _new_course;
                    fix                 = GPS::FIX_3D;          // To-Do: add support for proper reporting of 2D and 3D fix
                    break;
                case  _GPS_SENTENCE_GPGGA://_GPS_SENTENCE_GPGGA,
                    altitude            = _new_altitude;
                    time                        = _new_time;
                    latitude            = _new_latitude * 10;   // degrees*10e5 -> 10e7
                    longitude           = _new_longitude * 10;  // degrees*10e5 -> 10e7
                    num_sats            = _new_satellite_count;
                    hdop                        = _new_hdop;
                    fix                 = GPS::FIX_3D;          // To-Do: add support for proper reporting of 2D and 3D fix
                    break;
                case _GPS_SENTENCE_GPVTG:
                    ground_speed        = _new_speed;
                    ground_course       = _new_course;
                    // VTG has no fix indicator, can't change fix status
                    break;
                }
            } else {
                switch (_sentence_type) {
                case _GPS_SENTENCE_GPRMC:
                case _GPS_SENTENCE_GPGGA:
                    // Only these sentences give us information about
                    // fix status.
                    fix = GPS::FIX_NONE;
                }
            }
            // we got a good message
            return true;
        }
        // we got a bad message, ignore it
        return false;
    }

    // the first term determines the sentence type
  
    if (_term_number == 0) {
        if (!strcmp_P(_term, _gprmc_string)) {
            _sentence_type = _GPS_SENTENCE_GPRMC;
        } else  if (!strcmp_P(_term, _gnrmc_string)) {
            _sentence_type = _GPS_SENTENCE_GPRMC;
        }else if (!strcmp_P(_term, _gpgga_string)) {
            _sentence_type = _GPS_SENTENCE_GPGGA;
        }else if (!strcmp_P(_term, _gngga_string)) {
            _sentence_type = _GPS_SENTENCE_GPGGA;
        }else if (!strcmp_P(_term, _gpvtg_string)) {
            _sentence_type = _GPS_SENTENCE_GPVTG;
            // VTG may not contain a data qualifier, presume the solution is good
            // unless it tells us otherwise.
            _gps_data_good = true;
        } else {
            _sentence_type = _GPS_SENTENCE_OTHER;
        }
        return false;
    }

    // 32 = RMC, 64 = GGA, 96 = VTG
    if (_sentence_type != _GPS_SENTENCE_OTHER && _term[0]) {
        switch (_sentence_type + _term_number) {
        // operational status
        //
        case _GPS_SENTENCE_GPRMC + 2: // validity (RMC)
            _gps_data_good = _term[0] == 'A';
            break;
        case _GPS_SENTENCE_GPGGA + 6: // Fix data (GGA)
            _gps_data_good = _term[0] > '0';
            break;
        case _GPS_SENTENCE_GPVTG + 9: // validity (VTG) (we may not see this field)
            _gps_data_good = _term[0] != 'N';
            break;
        case _GPS_SENTENCE_GPGGA + 7: // satellite count (GGA)
            _new_satellite_count = atol(_term);
            break;
        case _GPS_SENTENCE_GPGGA + 8: // HDOP (GGA)
            _new_hdop = _parse_decimal();
            break;

        // time and date
        //
        case _GPS_SENTENCE_GPRMC + 1: // Time (RMC)
        case _GPS_SENTENCE_GPGGA + 1: // Time (GGA)
            _new_time = _parse_decimal();
            break;
        case _GPS_SENTENCE_GPRMC + 9: // Date (GPRMC)
            _new_date = atol(_term);
            break;

        // location
        //
        case _GPS_SENTENCE_GPRMC + 3: // Latitude
        case _GPS_SENTENCE_GPGGA + 2:
            _new_latitude = _parse_degrees();
            break;
        case _GPS_SENTENCE_GPRMC + 4: // N/S
        case _GPS_SENTENCE_GPGGA + 3:
            if (_term[0] == 'S')
                _new_latitude = -_new_latitude;
            break;
        case _GPS_SENTENCE_GPRMC + 5: // Longitude
        case _GPS_SENTENCE_GPGGA + 4:
            _new_longitude = _parse_degrees();
            break;
        case _GPS_SENTENCE_GPRMC + 6: // E/W
        case _GPS_SENTENCE_GPGGA + 5:
            if (_term[0] == 'W')
                _new_longitude = -_new_longitude;
            break;
        case _GPS_SENTENCE_GPGGA + 9: // Altitude (GPGGA)
            _new_altitude = _parse_decimal();
            break;

        // course and speed
        //
        case _GPS_SENTENCE_GPRMC + 7: // Speed (GPRMC)
        case _GPS_SENTENCE_GPVTG + 5: // Speed (VTG)
            _new_speed = (_parse_decimal() * 514) / 1000;       // knots-> m/sec, approximiates * 0.514
            break;
        case _GPS_SENTENCE_GPRMC + 8: // Course (GPRMC)
        case _GPS_SENTENCE_GPVTG + 1: // Course (VTG)
            _new_course = _parse_decimal();
            break;
        }
    }

    return false;
}

#define hexdigit(x) ((x)>9?'A'+(x):'0'+(x))

/*
  detect a NMEA GPS. Adds one byte, and returns true if the stream
  matches a NMEA string
 */
bool
AP_GPS_NMEA::_detect(uint8_t data)
{
	static uint8_t step;
	static uint8_t ck;

	switch (step) {
	case 0:
		ck = 0;
		if ('$' == data) {
			step++;
		}
		break;
	case 1:
		if ('*' == data) {
			step++;
		} else {
			ck ^= data;
		}
		break;
	case 2:
		if (hexdigit(ck>>4) == data) {
			step++;
		} else {
			step = 0;
		}
		break;
	case 3:
		if (hexdigit(ck&0xF) == data) {
			return true;
		}
		step = 0;
		break;
    }
    return false;
}

