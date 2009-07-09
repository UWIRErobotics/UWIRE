#include "main.h"

/**************** UPDATE FLAGS/VARIABLES ********/
bool new_gps_data;
float curr_xpos_reading =  761929.06;
float curr_ypos_reading =  -4573213;
float curr_velocity_reading;
float curr_heading_reading = 3.78771354267809;
float curr_xpos_best_estimate = 0;
float curr_ypos_best_estimate = 0;
float curr_zone=0;


void rc_drive()
{
	char input = Serial0.read();
	int set_angle = 1300;
	int set_speed = 0;

	neuro_bot.set_turn_angle(set_angle);
	neuro_bot.set_speed(set_speed);
	while(input!='e')
	{
		if (Serial0.available() > 0)
		{
			input = Serial0.read();
			if (input == 'a')
			{
				neuro_bot.set_turn_angle(set_angle+=25);
				Serial0.print ("Angle Set to:");
				Serial0.println(set_angle);
			}
			else if (input =='d')
			{
				neuro_bot.set_turn_angle(set_angle-=25);
				Serial0.print("Angle Set to:");
				Serial0.println(set_angle);
			}
			else if(input =='w')
			{
				neuro_bot.set_speed(set_speed+=5);
				Serial0.print("Speed Set to:");
				Serial0.println(set_speed);
			}
			else if(input =='s')
			{
				neuro_bot.set_speed(set_speed-=5);
				Serial0.print("Speed Set to:");
				Serial0.println(set_speed);
			}
			Serial0.flush();
		}
	}
	neuro_bot.set_speed(0);
}

void insula_read()
{
	char insula_read;
	byte32 buffer_32;
	byte16 buffer_16;

	if (Serial3.available())
	{
		insula_read = Serial3.read();
		if (insula_read == GPS_speed)
		{
			while(Serial3.available() < 2) {};
			buffer_16.high = Serial3.read();
			buffer_16.low = Serial3.read();
			curr_velocity_reading = (float)buffer_16.container;
		}
		else if (insula_read == GPS_course)
		{
			while(Serial3.available() < 2){};
			buffer_16.high = Serial3.read();
			buffer_16.low = Serial3.read();
			curr_heading_reading = (float)buffer_16.container;
			curr_heading_reading /= 100;
		}
		else if (insula_read == GPS_latitude)
		{
			while(Serial3.available() < 4){};
			buffer_32.highest = Serial3.read();
			buffer_32.high = Serial3.read();
			buffer_32.low = Serial3.read();
			buffer_32.lowest = Serial3.read();
			curr_xpos_reading = (float)buffer_32.container;
		}
		else if(insula_read == GPS_longitude)
		{
			while(Serial3.available() < 4){};
			buffer_32.highest = Serial3.read();
			buffer_32.high = Serial3.read();
			buffer_32.low = Serial3.read();
			buffer_32.lowest = Serial3.read();
			curr_ypos_reading = (float)buffer_32.container;
			new_gps_data = true;
		}
	}
}

void update_zones()
{
	float distance_to_zone;

	distance_to_zone = sqrt (sq(curr_xpos_best_estimate - ZONE_2_X) + sq(curr_ypos_best_estimate - ZONE_2_Y));
	if (distance_to_zone<=5)
		curr_zone = 2;

	distance_to_zone = sqrt (sq(curr_xpos_best_estimate - ZONE_3_X) + sq(curr_ypos_best_estimate - ZONE_3_Y));
	if (distance_to_zone<=5)
		curr_zone = 3;

	distance_to_zone = sqrt (sq(curr_xpos_best_estimate - ZONE_4_X) + sq(curr_ypos_best_estimate - ZONE_4_Y));
	if (distance_to_zone<=5)
		curr_zone = 4;

	distance_to_zone = sqrt (sq(curr_xpos_best_estimate - ZONE_5_X) + sq(curr_ypos_best_estimate - ZONE_5_Y));
	if (distance_to_zone<=5)
		curr_zone = 5;

	//Calculate Distance to zone1...most important
	distance_to_zone = sqrt (sq(curr_xpos_best_estimate - ZONE_1_X) + sq(curr_ypos_best_estimate - ZONE_1_Y));
	if (distance_to_zone<=5)
		curr_zone = 1;
}

void drag_drive()
{

}

void track_drive()
{
	//ekf matrices;

	float mean_predicted [STATE_N][STATE_M] = {{0.0},{0.0},{0.0}};
	float mean_best [STATE_N][STATE_M] = {{0.0},{0.0},{0.0}};
	float last_best [STATE_N][STATE_M] = {{761897.0},{-4573234.0},{0.00}}; //fill in with first set of gps values!

	float variance_predicted[STATE_N][STATE_N] = {{1.0,0,0},{0,1.0,0},{0,0,1.0}};
	float variance_best[STATE_N][STATE_N] = {{1.0,0,0},{0,1.0,0},{0,0,1.0}};

	float input_matrix[STATE_N][STATE_M] = {{0.0},{0.0},{0.0}};

	float motion_model_covar[COVAR_N][COVAR_M] = {{X_MOTION_VAR,0,0},{0,Y_MOTION_VAR,0},{0,0,THETA_MOTION_VAR}};
	float meas_model_covar[COVAR_N][COVAR_M] = {{X_MEAS_VAR,0,0},{0,Y_MEAS_VAR,0},{0,0,THETA_MEAS_VAR}};

	float control_u[CONTROL_N][CONTROL_M] = {{1.0},{0.0}};

	float Gt[COVAR_N][COVAR_M] = {{1.0,0,-1.0*control_u[0][0]*sin(mean_best[3][1])},{0,1.0,1.0*control_u[0][0]*cos(mean_best[3][1])},{0,0,1.0}};
	float Ht[COVAR_N][COVAR_M] = {{1.0,0,0},{0,1.0,0},{0,0,1.0}};

	float kalman_gain[STATE_N][STATE_N] = {{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};

	float identity_matrix[STATE_N][STATE_N] = {{1.0,0,0},{0,1.0,0},{0,0,1.0}};

	//temporary holders
	float temp[STATE_N][STATE_N] = {{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};
	float temp1[STATE_N][STATE_N] = {{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};
	float temp2[STATE_N][STATE_N] = {{0.0,0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}};

	float single_temp[STATE_N][STATE_M] = {{0.0},{0.0},{0.0}};
	float single_temp1[STATE_N][STATE_M] = {{0.0},{0.0},{0.0}};

	char user_input = 'a';

	while (user_input!='e')
	{
		user_input = Serial0.read();
		insula_read();
		if (false) //propagate EKF
		{
			/**********Propagate Motion Model*********************/
			mean_predicted[0][0] = last_best[0][0] + control_u[0][0]*cos(last_best[2][0]);
			mean_predicted[1][0] = last_best[1][0] + control_u[0][0]*sin(last_best[2][0]);
			mean_predicted[2][0] = last_best[2][0] + control_u[1][0];

			//update motion model derivative term
			Gt[0][2] = -1.0*control_u[0][0]*sin(mean_best[2][0]);
			Gt[1][2] = 1.0*control_u[0][0]*cos(mean_best[2][0]);

			//update variance belief term
			matrix_multiply((float*)Gt,(float*)variance_best,STATE_N,STATE_N,STATE_N,(float*)temp);
			matrix_transpose((float*)Gt,STATE_N,STATE_N,(float*)temp1);
			matrix_multiply((float*)temp,(float*)temp1,STATE_N,STATE_N,STATE_N,(float*)temp2);
			matrix_addition((float*)temp2,(float*)motion_model_covar,STATE_N,STATE_N,(float*)variance_predicted);

			//update measurement
			input_matrix[0][0] = curr_xpos_reading;
			input_matrix[1][0] = curr_ypos_reading;
			input_matrix[2][0] = curr_heading_reading;

			/********Amalgamate Measurement Model********/

			//Calculate Kalman Gain
			matrix_multiply((float*)Ht,(float*)variance_predicted,STATE_N,STATE_N,STATE_N,(float*)temp);
			matrix_transpose((float*)Ht,STATE_N,STATE_N,(float*)temp1); //really pointless :s
			matrix_multiply((float*)temp,(float*)temp1,STATE_N,STATE_N,STATE_N,(float*)temp2);
			matrix_addition((float*)temp2,(float*)meas_model_covar,STATE_N,STATE_N,(float*)temp);

			matrix_inversion((float*)temp,STATE_N,(float*)temp2);

			matrix_multiply ((float*)variance_predicted,(float*)temp1,STATE_N,STATE_N,STATE_N,(float*)temp);

			matrix_multiply((float*)temp,(float*)temp2,STATE_N,STATE_N,STATE_N,(float*)kalman_gain);

			matrix_print((float*)kalman_gain,STATE_N,STATE_N);
			//Calculate best mean
			matrix_subtraction((float*)input_matrix,(float*)mean_predicted,STATE_N,STATE_M,(float*)single_temp);
			matrix_multiply((float*)kalman_gain,(float*)single_temp,STATE_N,STATE_N,STATE_M,(float*)single_temp1);
			matrix_addition((float*)mean_predicted,(float*)single_temp1,STATE_N,STATE_M,(float*)mean_best);

			//matrix_print((float*)mean_best,STATE_N,STATE_M);

			matrix_multiply((float*)kalman_gain,(float*)Ht,STATE_N,STATE_N,STATE_N,(float*)temp);
			matrix_subtraction((float*)identity_matrix,(float*)temp,STATE_N,STATE_N,(float*)temp1);
			matrix_multiply((float*)temp1,(float*)variance_predicted,STATE_N,STATE_N,STATE_N,(float*)variance_best);

			//Update Variables and cleanup
			curr_xpos_best_estimate = mean_best[0][0];
			curr_ypos_best_estimate = mean_best[1][0];
			control_u[0][0] = curr_velocity_reading;
			control_u[1][0] = last_best[2][0] - mean_best[2][0];

			last_best[0][0] = mean_best[0][0];
			last_best[1][0] = mean_best[1][0];
			last_best[2][0] = mean_best[2][0];
			new_gps_data = false;
			update_zones();
		}

		if(new_gps_data)
		{
			Serial0.print("Speed:");
			Serial0.println(curr_velocity_reading);

			Serial0.print("Heading:");
			Serial0.println(curr_heading_reading);

			Serial0.print("Latitude:");
			Serial0.println(curr_xpos_reading);

			Serial0.print("Longitude");
			Serial0.println(curr_ypos_reading);
			new_gps_data = false;

		}
		/*Serial0.print ("Current Zone:");
		Serial0.println(curr_zone);*/


	}
}

void wgslla2xyz(float wlat, float wlon)
{
  float A_EARTH = 6378137.0;
  float walt=0.0;
  float flattening = 1/298.257223563;
  float NAV_E2 = (2-flattening)*flattening;

  float deg2rad = PI/180;

  float slat = sin(wlat*deg2rad);
  float clat = cos(wlat*deg2rad);
  float r_n = sqrt(1 - NAV_E2*slat*slat);

  r_n = A_EARTH/r_n;

  curr_xpos_reading = (r_n + walt)*clat*cos(wlon*deg2rad);
  curr_ypos_reading = (r_n + walt)*clat*sin(wlon*deg2rad);
}


void CLI()
{

	char input = Serial0.read();

	if (input == 'r')
	{
		Serial0.println("Entering RC Drive");
		rc_drive();
		Serial0.println("Exiting RC Drive");
	}

	if (input == 'd')
	{
		Serial0.println("Entering Drag Drive");
		drag_drive();
	}

	if (input == 't')
	{
		Serial0.println("Entering Track Drive");
		track_drive();
		Serial0.println("Exiting Track Drive");
	}
}

void setup()
{
	Serial0.begin(19200);   //user console
	Serial3.begin(BRAIN_BAUD);    //insula comm
	Serial0.println("Cogzilla Console");

	Serial0.println("Setting Up Vehicle");
	neuro_bot.setup_vehicle();
	Serial0.println("Vehicle is ready");

	Serial0.println("Setting up Camera");
	//cmu_cam1.setup_cmu_cam(&Serial2);
	Serial0.println("Camera is ready");
}


void loop()
{
	if(Serial0.available() > 0)
		CLI();
}


int main(void)
{
	init();
	setup();

	for (;;)
		loop();

	return 0;
}
