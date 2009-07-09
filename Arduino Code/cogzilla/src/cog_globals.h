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

#endif /* COG_GLOBALS_H_ */
