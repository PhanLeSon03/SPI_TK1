/*
    Copyright (c) 2006 Michael P. Thompson <mpthompson@gmail.com>

    Permission is hereby granted, free of charge, to any person 
    obtaining a copy of this software and associated documentation 
    files (the "Software"), to deal in the Software without 
    restriction, including without limitation the rights to use, copy, 
    modify, merge, publish, distribute, sublicense, and/or sell copies 
    of the Software, and to permit persons to whom the Software is 
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be 
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF 
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
    DEALINGS IN THE SOFTWARE.

    $Id:$

    The concepts for this code were derived from the June, 2001 
    Embedded Systems Programming magazine article titled 
    "Kalman Filtering" by Dan Simon.
*/


#include <math.h>
#include <stdio.h>

#define SAMPLE_COUNT 512

// Sample data gathered directly from the SparkFun IMU 5 Degrees of Freedom
// IMU every 1/50th of a second using 10-bit ADC on an ATmega168.  The first
// column is the rate from the gyro (degrees/sec) and the second column is the 
// accelerometer pitch attitude from horizontal in degrees.



float kalman_update(float gyro_rate, float accel_angle)
// Update the State Estimation and compute the Kalman Gain.
// The estimated angle is returned.
{
    // Inputs.
    float u = gyro_rate;
    float y = accel_angle;

    // Output.
    static float x_00 = 0.0;
    static float x_10 = 0.0;

    // Persistant states.
    static float P_00 = 0.001;
    static float P_01 = 0.003;
    static float P_10 = 0.003;
    static float P_11 = 0.003;

    // Constants.  

    // These are the delta in seconds between samples.
    const float A_01 = -0.019968;
    const float B_00 = 0.019968;

    // Data read from 1000 samples of the accelerometer had a variance of 0.07701688.
    const float Sz = 0.07701688;

    // Data read from 1000 samples of the gyroscope had a variance of 0.00025556.
    // XXX The values below were pulled from the autopilot site, but I'm not sure how to
    // XXX plug them into the process noise covariance matrix.  This needs to be
    // XXX further explored.
    const float Sw_00 = 0.001;
    const float Sw_01 = 0.003;
    const float Sw_10 = 0.003;
    const float Sw_11 = 0.003;

    // Temp.
    float s_00;
    float inn_00;
    float K_00;
    float K_10;
    float AP_00;
    float AP_01;
    float AP_10;
    float AP_11;
    float APAT_00;
    float APAT_01;
    float APAT_10;
    float APAT_11;
    float KCPAT_00;
    float KCPAT_01;
    float KCPAT_10;
    float KCPAT_11;

    // Update the state estimate by extrapolating current state estimate with input u.
    // x = A * x + B * u
    x_00 += (A_01 * x_10) + (B_00 * u);

    // Compute the innovation -- error between measured value and state.
    // inn = y - c * x
    inn_00 = y - x_00;

    // Compute the covariance of the innovation.
    // s = C * P * C' + Sz
    s_00 = P_00 + Sz;

    // Compute AP matrix for use below.
    // AP = A * P
    AP_00 = P_00 + A_01 * P_10;
    AP_01 = P_01 + A_01 * P_11;
    AP_10 = P_10;
    AP_11 = P_11;

    // Compute the kalman gain matrix.
    // K = A * P * C' * inv(s)
    K_00 = AP_00 / s_00;
    K_10 = AP_10 / s_00;
    
    // Update the state estimate.
    // x = x + K * inn
    x_00 += K_00 * inn_00;
    x_10 += K_10 * inn_00;

    // Compute the new covariance of the estimation error.
    // P = A * P * A' - K * C * P * A' + Sw
    APAT_00 = AP_00 + (AP_01 * A_01);
    APAT_01 = AP_01;
    APAT_10 = AP_10 + (AP_11 * A_01);
    APAT_11 = AP_11;
    KCPAT_00 = (K_00 * P_00) + (K_00 * P_01) * A_01;
    KCPAT_01 = (K_00 * P_01);
    KCPAT_10 = (K_10 * P_00) + (K_10 * P_01) * A_01;
    KCPAT_11 = (K_10 * P_01);
    P_00 = APAT_00 - KCPAT_00 + Sw_00;
    P_01 = APAT_01 - KCPAT_01 + Sw_01;
    P_10 = APAT_10 - KCPAT_10 + Sw_10;
    P_11 = APAT_11 - KCPAT_11 + Sw_11;

    // Return the estimate.
    return x_00;
}

int main(int argc, char **argv)
{
    int i;
    float gyro_input;
    float accel_input;
    float kalman_output;

    for (i = 0; i < SAMPLE_COUNT; ++i)
    {
        // Get the gyro and accelerometer input.
        gyro_input = sample_data[i][0];
        accel_input = sample_data[i][1];

        // Update the Kalman filter and get the output.
        kalman_output = kalman_update(gyro_input, accel_input);

        // Print out input data and the kalman output.
        printf("%d gyro=%7.3f deg/sec    accel=%7.3f deg    kalman_output=%5.1f deg\n", 
               i, gyro_input, accel_input, kalman_output);
    }

    return 0;
}

