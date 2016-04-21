/*/*

 Copyright (c) 2012 Andy Little 11/11/2012

(
  Some parts of this work are based on: 
  http://code.google.com/p/arducam-osd/source/browse/trunk/ArduCAM_OSD/MAVLink.ino
  Copyright (c) 2011. Sandro Benigno
)

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>
 
*/

#include <Arduino.h>
#include <cstdint>
#include <mavlink.h>

#include <aircraft.h>

mavlink_system_t mavlink_system = {12,1,0,0};

void comm_send_ch(mavlink_channel_t chan, uint8_t ch)
{
#if defined __AVR_ATmega32U4_
  Serial1.write(ch);
#else
  Serial.write(ch);
#endif
}

namespace {

  
   uint32_t  num_heartbeats = 0U;
   
}

// return number of heartbeats since boot
uint32_t get_num_heartbeats()
{
   return  num_heartbeats;
}

namespace{

   uint8_t  apm_mav_type;
   uint8_t  apm_mav_system; 
   uint8_t  apm_mav_component;

   struct mav_sr_t{
      uint8_t stream_number;
      uint16_t rate;
   };

// The same as the minimosd

   mav_sr_t MAVStreams[] = {
      {MAV_DATA_STREAM_RAW_SENSORS,0x02},
      {MAV_DATA_STREAM_EXTENDED_STATUS,0x02},
      {MAV_DATA_STREAM_RC_CHANNELS,0x05},
      {MAV_DATA_STREAM_POSITION,0x02},
      {MAV_DATA_STREAM_EXTRA1, 0x05},
      {MAV_DATA_STREAM_EXTRA2,0x02}
   };

   void request_mavlink_rate(uint8_t mav_system, uint8_t mav_component, uint8_t stream_number, uint16_t rate );

   void request_mavlink_rate(uint8_t system, uint8_t component, uint8_t stream_number, uint16_t rate)
   {
        mavlink_msg_request_data_stream_send(MAVLINK_COMM_0,system, component, stream_number, rate, 1);
   }

   bool rate_request_sent_flag = false;

   void  request_mavlink_rates()
   {
     static const size_t  NumStreams = sizeof(MAVStreams) / sizeof(mav_sr_t);
     for (size_t i=0; i < NumStreams; ++i) {
         request_mavlink_rate(apm_mav_system, apm_mav_component,MAVStreams[i].stream_number, MAVStreams[i].rate);
     }
     rate_request_sent_flag = true;
   }

     void do_mavlink_heartbeat(mavlink_message_t* pmsg);
     void do_mavlink_sys_status(mavlink_message_t *pmsg);
   #ifndef MAVLINK10
     void do_mavlink_gps_raw(mavlink_message_t * pmsg);
     void do_mavlink_gps_status(mavlink_message_t * pmsg);
   #else
     void do_mavlink_gps_raw_int(mavlink_message_t * pmsg);
   #endif
     void do_mavlink_vfr_hud(mavlink_message_t * pmsg);
     void do_mavlink_attitude(mavlink_message_t * pmsg);

} // ~namespace

void read_mavlink()
{

   static bool mavlink_active = false;
   static int packet_drops = 0;
   static int  parse_error = 0;
  
   mavlink_message_t msg; 
   mavlink_status_t status;

 #if defined __AVR_ATmega32U4_  
   while (Serial1.available() > 0) {
            uint8_t ch = Serial1.read();
#else
   while (Serial.available() > 0) {
            uint8_t ch = Serial.read();
#endif

      if(mavlink_parse_char(MAVLINK_COMM_0, ch, &msg, &status)) {
         mavlink_active = true;
         switch(msg.msgid) {
            case MAVLINK_MSG_ID_HEARTBEAT:
               do_mavlink_heartbeat(&msg);
               if ( rate_request_sent_flag == false){
                  request_mavlink_rates();
               }
            break;
            case MAVLINK_MSG_ID_SYS_STATUS:
               do_mavlink_sys_status(&msg);
            break;
#ifndef MAVLINK10 
            case MAVLINK_MSG_ID_GPS_RAW:
               do_mavlink_gps_raw(&msg);
            break;
            case MAVLINK_MSG_ID_GPS_STATUS:
               do_mavlink_gps_status(&msg);
            break;
#else
            case MAVLINK_MSG_ID_GPS_RAW_INT:
               do_mavlink_gps_raw_int(&msg);
            break;
#endif          
            case MAVLINK_MSG_ID_VFR_HUD:
               do_mavlink_vfr_hud(&msg);
            break;
            case MAVLINK_MSG_ID_ATTITUDE:
               do_mavlink_attitude(&msg);
            break;
            default:
            break;
         }
      }
   }
 
  packet_drops += status.packet_rx_drop_count;
  parse_error += status.parse_error;
}

namespace {

  void do_mavlink_heartbeat(mavlink_message_t* pmsg)
  {    
      apm_mav_system    = pmsg->sysid;
      apm_mav_component = pmsg->compid;
      apm_mav_type      = mavlink_msg_heartbeat_get_type(pmsg);
#ifdef MAVLINK10             
      the_aircraft.custom_mode = mavlink_msg_heartbeat_get_custom_mode(pmsg);
      the_aircraft.nav_mode = 0;
#endif            
     ++num_heartbeats;
   }

   void do_mavlink_sys_status(mavlink_message_t *pmsg)
   {
#ifndef MAVLINK10            
      the_aircraft.battery_voltage 
      //   = quan::voltage_<float>::mV{mavlink_msg_sys_status_get_vbat(pmsg) };
        = mavlink_msg_sys_status_get_vbat(pmsg)/ 1000.f;
      the_aircraft.custom_mode = mavlink_msg_sys_status_get_mode(pmsg);
      the_aircraft.nav_mode = mavlink_msg_sys_status_get_nav_mode(pmsg);
#else

      the_aircraft.battery_voltage
       //  = quan::voltage_<float>::mV{mavlink_msg_sys_status_get_voltage_battery(pmsg)};
         = mavlink_msg_sys_status_get_voltage_battery(pmsg) / 1000.f;
#endif            
      the_aircraft.battery_remaining = mavlink_msg_sys_status_get_battery_remaining(pmsg);
   }

#ifndef MAVLINK10
#error wrong version
   void do_mavlink_gps_raw(mavlink_message_t * pmsg)
   {
     // the_aircraft.location.lat = quan::angle_<float>::deg{mavlink_msg_gps_raw_get_lat(pmsg)};
      the_aircraft.location.lat = mavlink_msg_gps_raw_get_lat(pmsg) * 1.e7f;
     // the_aircraft.location.lon = quan::angle_<float>::deg{mavlink_msg_gps_raw_get_lon(pmsg)};
       the_aircraft.location.lon = mavlink_msg_gps_raw_get_lon(pmsg) * 1.e7f;
      the_aircraft.gps.fix_type = mavlink_msg_gps_raw_get_fix_type(pmsg);
   }
   void do_mavlink_gps_status(mavlink_message_t * pmsg)
   {
      the_aircraft.gps.num_sats = mavlink_msg_gps_status_get_satellites_visible(pmsg);
   }
#else

  void do_mavlink_gps_raw_int(mavlink_message_t * pmsg)
   {
     // the_aircraft.location.gps_time_stamp = quan::time_<uint64_t>::us{mavlink_msg_gps_raw_int_get_time_usec(pmsg)};
      the_aircraft.location.gps_time_stamp = mavlink_msg_gps_raw_int_get_time_usec(pmsg);
    //  the_aircraft.location.gps_lat = quan::angle_<int32_t>::deg10e7{ mavlink_msg_gps_raw_int_get_lat(pmsg) };
      the_aircraft.location.gps_lat =  mavlink_msg_gps_raw_int_get_lat(pmsg) ;
    //  the_aircraft.location.gps_lon = quan::angle_<int32_t>::deg10e7{ mavlink_msg_gps_raw_int_get_lon(pmsg) };
      the_aircraft.location.gps_lon =  mavlink_msg_gps_raw_int_get_lon(pmsg) ;
   //   the_aircraft.location.gps_alt = quan::length_<int32_t>::mm{ mavlink_msg_gps_raw_int_get_alt(pmsg) };
      the_aircraft.location.gps_alt = mavlink_msg_gps_raw_int_get_alt(pmsg);
   //       the_aircraft.location.gps_vog = quan::velocity_<uint16_t>::cm_per_s{ mavlink_msg_gps_raw_int_get_vel(pmsg)};
      the_aircraft.location.gps_vog =  mavlink_msg_gps_raw_int_get_vel(pmsg);
//    the_aircraft.location.gps_cog =  quan::angle_<uint16_t>::cdeg{mavlink_msg_gps_raw_int_get_cog(pmsg)};
      the_aircraft.location.gps_cog =  mavlink_msg_gps_raw_int_get_cog(pmsg);
 //   the_aircraft.location.gps_hdop = quan::length_<uint16_t>::cm{ mavlink_msg_gps_raw_int_get_eph(pmsg) };
      the_aircraft.location.gps_hdop = mavlink_msg_gps_raw_int_get_eph(pmsg);
  //  the_aircraft.location.gps_vdop = quan::length_<int32_t>::cm{ mavlink_msg_gps_raw_int_get_epv(pmsg) };
      the_aircraft.location.gps_vdop =  mavlink_msg_gps_raw_int_get_epv(pmsg);
      the_aircraft.gps.fix_type = mavlink_msg_gps_raw_int_get_fix_type(pmsg);
      the_aircraft.gps.num_sats = mavlink_msg_gps_raw_int_get_satellites_visible(pmsg);
   }
#endif

   void do_mavlink_vfr_hud(mavlink_message_t * pmsg)
   {
      // mavlink_msg_vfr_hud_get_groundspeed retirns val in m/s
      the_aircraft.groundspeed 
       //  = quan::velocity_<float>::m_per_s{mavlink_msg_vfr_hud_get_groundspeed(pmsg)};
         = mavlink_msg_vfr_hud_get_groundspeed(pmsg);

      // mavlink_msg_vfr_hud_get_airspeed returns val in m/s
      the_aircraft.airspeed 
        //  = quan::velocity_<float>::m_per_s{mavlink_msg_vfr_hud_get_airspeed(pmsg)};
        =  mavlink_msg_vfr_hud_get_airspeed(pmsg);
    //  the_aircraft.heading = quan::angle_<float>::deg{mavlink_msg_vfr_hud_get_heading(pmsg)};
      the_aircraft.heading = mavlink_msg_vfr_hud_get_heading(pmsg);
      the_aircraft.throttle = mavlink_msg_vfr_hud_get_throttle(pmsg) / 100.f;

      if((the_aircraft.throttle > 1.f) && (the_aircraft.throttle < 1.5f)){
         the_aircraft.throttle = 1.f;//Temporary fix for ArduPlane 2.28
      }else {
         if((the_aircraft.throttle < 0.f) || (the_aircraft.throttle > 1.5f)){
            the_aircraft.throttle = 0.f;//Temporary fix for ArduPlane 2.28
          }
      }
      // mavlink_msg_vfr_hud_get_alt returns height in m
      // baroalt
    //  the_aircraft.location.baro_alt = quan::length_<float>::m{mavlink_msg_vfr_hud_get_alt(pmsg)};
      the_aircraft.location.baro_alt = mavlink_msg_vfr_hud_get_alt(pmsg);
   }

   void do_mavlink_attitude(mavlink_message_t * pmsg)
   {
      static const float pi = 3.141592653589793238462643383279502884197;
      static const float rad_to_deg = 180.f / pi;
     // the_aircraft.attitude.pitch = quan::angle_<float>::rad{mavlink_msg_attitude_get_pitch(pmsg)};
      the_aircraft.attitude.pitch = mavlink_msg_attitude_get_pitch(pmsg) * rad_to_deg;
      //  the_aircraft.attitude.roll = quan::angle_<float>::rad{mavlink_msg_attitude_get_roll(pmsg)};
      the_aircraft.attitude.roll = mavlink_msg_attitude_get_roll(pmsg)* rad_to_deg;
      // the_aircraft.attitude.yaw = quan::angle_<float>::rad{mavlink_msg_attitude_get_yaw(pmsg)};
      the_aircraft.attitude.yaw = mavlink_msg_attitude_get_yaw(pmsg) * rad_to_deg;
   }
}// ~namespace
