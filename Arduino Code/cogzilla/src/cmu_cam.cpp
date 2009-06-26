/*
 * cmu_cam.cpp
 *
 *  Created on: 23-Jun-2009
 *      Author: prasenjit
 */
#include "cmu_cam.h"

/*** OBJECT DECLARATION ***/
        cmu_cam cmu_cam1;
/**************************/

char colour_command[] = "TC 250 250 250 250 250 250";
char result[8] = "";
HardwareSerial *cam_serial;


void cmu_cam::setup_cmu_cam(HardwareSerial *serial_port )
{
	cam_serial = serial_port;
	cam_serial->begin(9600);
	cam_serial->flush();

	Serial0.println("Port Setup");
	//switch camera to raw serial mode
	cam_serial->print("RM 1");
	cam_serial->write(0x0D); //carriage return
	while (ack_recieved(true) == false)
	{
		cam_serial->flush();
		Serial0.println("Command Sent");
		cam_serial->print("RM 1");
		cam_serial->write(0x0D); //carriage return
	}

}

char cmu_cam::read_cmu_byte()
{
	while (cam_serial->available()< 1) {
		//Serial0.println("NOT GETTING ANYTHING!");
	} ; //wait for packet

	return cam_serial->read();
}


bool cmu_cam::ack_recieved(bool colon_expected)
{
	char received_byte;
	int counter = 0;
	int bytes_in_ack= 3 + colon_expected; //5 if a colon is expected after ack

	received_byte = read_cmu_byte();
	if (received_byte == 0x41) //A ..the rest is pointless to read.
	{
		while (counter < bytes_in_ack) //A has already been read, read the rest of the bytes in ACK
		{
			received_byte = read_cmu_byte();
			Serial0.println(received_byte);
			counter++;
		}
		return true;
	}
	else
	{
		Serial0.print("Instead of A, I got ");
		Serial0.println(received_byte);
		return false;
	}

}

bool cmu_cam::track_colour()
{

	cam_serial->print("PM 1");
	cam_serial->write(0x0D); //carriage return
	while (ack_recieved(true) == false)
	{
		cam_serial->flush();
		Serial0.println("Command Sent");
		cam_serial->print("PM 1");
		cam_serial->write(0x0D); //carriage return
	}


	cam_serial->print("TC 0 0 0 0 0 0");
	cam_serial->write(0x0D);

	while (ack_recieved(false) == false)
	{
			cam_serial->flush();
			Serial0.println("Command Sent");
			cam_serial->print("TC 0 0 0 0 0 0");
			cam_serial->write(0x0D); //carriage return
	}

	char read_byte=read_cmu_byte();
	while (read_byte != 0xFF) //signifies beginning of packet
	{
		Serial0.print(read_byte);
		read_byte=read_cmu_byte();
	}

	if (read_cmu_byte() == 0x54)
	{
		Serial0.println("Found T Packet, Parsing");
		for(int i=0; i<8; i++) {
				result[i] = read_cmu_byte();
				if ( result[i] == 58 || result[i] == 13 ) {
					Serial0.println("ERROR: Unexpected colon or return carriage in middle of T packet!");
					return false;
				}
			}

			//--------------------------delete me later
			Serial0.println(" mx: ");
			Serial0.write(result[0]);
			Serial0.println(" my: ");
			Serial0.write(result[1]);
			Serial0.println(" x1_leftMost: ");
			Serial0.write(result[2]);
			Serial0.println(" y1_leftMost: ");
			Serial0.write(result[3]);
			Serial0.println(" x2_rightMost: ");
			Serial0.write(result[4]);
			Serial0.println(" y2_rightMost: ");
			Serial0.write(result[5]);
			Serial0.println(" numPixels: ");
			Serial0.write(result[6]);
			Serial0.println(" confidence: ");
			Serial0.write(result[7]);
			//--------------------------
	}
	else
	{
		Serial0.println("Not T Packet...wtf?!");
	}

}
