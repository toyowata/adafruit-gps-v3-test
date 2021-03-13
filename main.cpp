/* mbed Microcontroller Library
 * Copyright (c) 2021 ARM Limited
 * SPDX-License-Identifier: BSD-3-Clause
 */ 
#include "mbed.h"
#include "MBed_Adafruit_GPS.h"
 
using namespace std::chrono;
UnbufferedSerial * gps_Serial;
 
int main() {

    gps_Serial = new UnbufferedSerial(PG_14, PG_9); //serial object for use w/ GPS
    Adafruit_GPS myGPS(gps_Serial); //object of Adafruit's GPS class
    char c; //when read via Adafruit_GPS::read(), the class returns single character stored here
    Timer refresh_Timer; //sets up a timer for use in loop; how often do we print GPS info?
    const int refresh_Time = 2000; //refresh time in ms

    myGPS.begin(9600);  //sets baud rate for GPS communication; note this may be changed via Adafruit_GPS::sendCommand(char *)
                        //a list of GPS commands is available at http://www.adafruit.com/datasheets/PMTK_A08.pdf
   
    myGPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); //these commands are defined in MBed_Adafruit_GPS.h; a link is provided there for command creation
    myGPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
    myGPS.sendCommand(PGCMD_ANTENNA);

    printf("Connection established at 9600 baud...\n");

    thread_sleep_for(1000);

    refresh_Timer.start();  //starts the clock on the timer

    while(true) {
        c = myGPS.read();   //queries the GPS
       
        if (c) { printf("%c", c); } //this line will echo the GPS data if not paused

        //check if we recieved a new message from GPS, if so, attempt to parse it,
        if ( myGPS.newNMEAreceived() ) {
            if ( !myGPS.parse(myGPS.lastNMEA()) ) {
                continue;   
            }    
        }

        //check if enough time has passed to warrant printing GPS info to screen
        //note if refresh_Time is too low or pc.baud is too low, GPS data may be lost during printing
        if (duration_cast<milliseconds>(refresh_Timer.elapsed_time()).count() >= refresh_Time) {
        //if (refresh_Timer.read_ms() >= refresh_Time) {
            refresh_Timer.reset();
            printf("Time: %d:%d:%d.%u\n", myGPS.hour, myGPS.minute, myGPS.seconds, myGPS.milliseconds);
            printf("Date: %d/%d/20%d\n", myGPS.day, myGPS.month, myGPS.year);
            printf("Fix: %d\n", (int) myGPS.fix);
            printf("Quality: %d\n", (int) myGPS.fixquality);
            if (myGPS.fix) {
                printf("Location: %5.2f%c, %5.2f%c\n", myGPS.latitude, myGPS.lat, myGPS.longitude, myGPS.lon);
                printf("Speed: %5.2f knots\n", myGPS.speed);
                printf("Angle: %5.2f\n", myGPS.angle);
                printf("Altitude: %5.2f\n", myGPS.altitude);
                printf("Satellites: %d\n", myGPS.satellites);
            }
        }
    }
}
