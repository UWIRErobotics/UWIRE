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
HardwareSerial *cam_serial;

void  cmu_cam::setup_cmu_cam(HardwareSerial *serial_port )
{
	cam_serial = serial_port;
	cam_serial->begin(9600);
	cam_serial->flush();

	//switch camera to raw serial mode
	cam_serial->print("RM 1");
	cam_serial->write(0x0D); //carriage return
	while (ack_recieved(true) == false)
	{
		cam_serial->flush();
		cam_serial->print("RM 1");
		cam_serial->write(0x0D); //carriage return
	}


	cam_serial->print("CR 18 44");
	cam_serial->write(0x0D);
	while (ack_recieved(true) == false)
	{
			cam_serial->flush();
			cam_serial->print("CR 18 44");
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
			counter++;
		}
		return true;
	}
	else
	{
		//Serial0.print("Instead of A, I got ");
		return false;
	}


}

bool cmu_cam::track_stop_sign()
{
	cam_serial->print("PM 1");
	cam_serial->write(0x0D); //carriage return
	while (ack_recieved(true) == false)
	{
		cam_serial->flush();
		//Serial0.println("Command Sent");
		cam_serial->print("PM 1");
		cam_serial->write(0x0D); //carriage return
	}

	//Track Colour
	cam_serial->print("TC 70 80 47 55 15 17");
	cam_serial->write(0x0D);

	while (ack_recieved(false) == false)
	{
			cam_serial->flush();
			//Serial0.println("Command Sent");
			cam_serial->print("TC 70 80 47 55 15 17");
			cam_serial->write(0x0D); //carriage return
	}

	char read_byte=read_cmu_byte();
	while (read_byte != 0xFF) //signifies beginning of packet
		read_byte=read_cmu_byte();

	if (read_cmu_byte() == 0x54)
	{
		//Serial0.println("Found T Packet, Parsing");
		for(int i=0; i<8; i++) {
				stop_sign_track_info[i] = read_cmu_byte();
				if ( stop_sign_track_info[i] == 58 || stop_sign_track_info[i] == 13 ) {
					//Serial0.println("ERROR: Unexpected colon or return carriage in middle of T packet!");
					stop_sign_in_view = false;
					return false;
				}
			}

		if (stop_sign_track_info[CMU_RESULT_MX]>0x00 && stop_sign_track_info[CMU_RESULT_MY] > 0x00 && stop_sign_track_info[CMU_RESULT_CONF] > 0x00)
			stop_sign_in_view = true;
		else
			stop_sign_in_view = false;

			//--------------------------delete me later
			/*Serial0.write(0xFF);
			Serial0.write(stop_sign_track_info[0]);
			Serial0.write(stop_sign_track_info[1]);
			Serial0.write(stop_sign_track_info[2]);
			Serial0.write(stop_sign_track_info[3]);
			Serial0.write(stop_sign_track_info[4]);
			Serial0.write(stop_sign_track_info[5]);
			Serial0.write(stop_sign_track_info[6]);
			Serial0.write(stop_sign_track_info[7]);
			Serial0.write(0xFF);
			Serial0.write(0xFF);
			Serial0.write(0xFF);
			Serial0.write(0xFF);
			Serial0.write(0x0D);*/
			//--------------------------

			return true;
	}
	else
	{
		Serial0.println("Not T Packet...wtf?!");
	}
}


void cmu_cam::flush_cam()
{
	cam_serial->flush();
}


bool cmu_cam::track_red_light()
{
	cam_serial->print("PM 1");
	cam_serial->write(0x0D); //carriage return
	while (ack_recieved(true) == false)
	{
		cam_serial->flush();
		//Serial0.println("Command Sent");
		cam_serial->print("PM 1");
		cam_serial->write(0x0D); //carriage return
	}

	//Track Colour
	cam_serial->print("TC 78 93 40 60 16 18");
	cam_serial->write(0x0D);

	while (ack_recieved(false) == false)
	{
			cam_serial->flush();
			//Serial0.println("Command Sent");
			cam_serial->print("TC 78 93 40 60 16 18");
			cam_serial->write(0x0D); //carriage return
	}

	char read_byte=read_cmu_byte();
	while (read_byte != 0xFF) //signifies beginning of packet
		read_byte=read_cmu_byte();

	if (read_cmu_byte() == 0x54)
	{
		//Serial0.println("Found T Packet, Parsing");
		for(int i=0; i<8; i++) {
				red_light_track_info[i] = read_cmu_byte();
				if ( red_light_track_info[i] == 58 || red_light_track_info[i] == 13 ) {
					Serial0.println("ERROR: Unexpected colon or return carriage in middle of T packet!");
					return false;
				}
			}

			//--------------------------delete me later
			/*Serial0.write(0xFF);
			Serial0.write(stop_sign_track_info[0]);
			Serial0.write(stop_sign_track_info[1]);
			Serial0.write(stop_sign_track_info[2]);
			Serial0.write(stop_sign_track_info[3]);
			Serial0.write(stop_sign_track_info[4]);
			Serial0.write(stop_sign_track_info[5]);
			Serial0.write(stop_sign_track_info[6]);
			Serial0.write(stop_sign_track_info[7]);
			Serial0.write(0xFF);
			Serial0.write(0xFF);
			Serial0.write(0xFF);
			Serial0.write(0xFF);
			Serial0.write(0x0D);*/
			//--------------------------

			if (red_light_track_info[CMU_RESULT_MX]>0x00 && red_light_track_info[CMU_RESULT_MY] > 0x00 && red_light_track_info[CMU_RESULT_CONF] > 0x00)
				red_light_in_view = true;
			else
				red_light_in_view = false;

			return true;
	}
	else
	{
		Serial0.println("Not T Packet...wtf?!");
	}
}
