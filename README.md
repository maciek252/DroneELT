# DroneELT
ELT for drones

DroneELT is an analogue of ELT rescue system for aircrafts. Its core is a Orange LRS module with an antenna and a separate battery, protected by a foam from crash damage, and receiving the MAVLINK data from the drone's flight controller. As soon as the MAVLINK messages stop coming, DroneELT starts to transmit over a predefined PMR frequency the last GPS position encoded in the BCD code.