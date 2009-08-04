/*
 * cog_globals.h
 *
 *  Created on: 9-Jul-2009
 *      Author: prasenjit
 */

#ifndef COG_GLOBALS_H_
#define COG_GLOBALS_H_


#define INTERRUPT_FREQ 50 //ms

/*************** EKF CONSTANTS ****************/

//Matrix Sizes
#define STATE_N 3
#define STATE_M 1

#define COVAR_N 3
#define COVAR_M 3

#define CONTROL_N 2
#define CONTROL_M 1

//Variance
#define X_MOTION_VAR 0.05
#define Y_MOTION_VAR 0.05
#define THETA_MOTION_VAR 0.05

#define X_MEAS_VAR 0.07
#define Y_MEAS_VAR 0.03
#define THETA_MEAS_VAR 0.01


/**************ZONE CONSTANTS******************/
/*#define ZONE_1_X 761929.8125
#define ZONE_1_Y -4573214

#define ZONE_2_X 761930.3750
#define ZONE_2_Y -4573194.5

#define ZONE_3_X 761897.25
#define ZONE_3_Y 4573202.5

#define ZONE_4_X 761953.3125
#define ZONE_4_Y -4573227

#define ZONE_5_X 761935.25
#define ZONE_5_Y -4573235*/

#define ZONE_1_X 761893.75
#define ZONE_1_Y -4572969.5

#define ZONE_2_X 761848.31
#define ZONE_2_Y -4572957.5

#define ZONE_3_X 761865.38
#define ZONE_3_Y -4572979.5

/**************SPEED CONSTANTS*****************/
#define FAST_SPEED 15
#define SLOW_SPEED 12
#define DRAG_SPEED 15 //amp this up after testing.

#endif /* COG_GLOBALS_H_ */
