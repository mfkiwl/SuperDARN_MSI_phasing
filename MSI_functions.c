/* 
 * Calculate the timedelay needed for a given angle relative to boresite
 *   angle_degress: angle in degrees
 *   spacing_meters: antenna spacing in meters
 *   card: phasing card number numbered from 0. Card 0 is west most phasing card
 * Note:
 *   Assumes 16 cards in main array, numbered 0-15
 *   Assumes 4 cards in interf array, numbered 16-19
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "MSI_functions.h"
/* settings which I could probably move to an ini file */
int32_t    MSI_phasecodes=8091;
int32_t    MSI_num_angles=24;
int32_t    MSI_num_cards=20;
double     MSI_bm_sep_degrees=3.24;
double     MSI_spacing_meters=12.8016;
double     MSI_max_freq=20E6;
double     MSI_min_freq=8E6;
double     MSI_lo_freq=10E6;
double     MSI_hi_freq=16E6;
int32_t    MSI_max_freq_steps=128;
double     MSI_freq_window=.25*1E6;
double     MSI_target_pwr_dB=-2.0;
double     MSI_target_tdelay0_nsecs=10.0;
double     MSI_tdelay_tolerance_nsec=3.0;
double     MSI_pwr_tolerance_dB=1.5;


/* Hardwired stuff */

double MSI_timedelay_bits_nsecs[13]={
                         0.25,
                         0.45,
                         0.8,
                         1.5,
                         2.75,
                         5.0,
                         8.0,
                         15.0,
                         25.0,
                         45.0,
                         80.0,
                         140.0,
                         250.0
                       };

double MSI_atten_bits_dB[6]={
                         0.5,
                         1.0,
                         2.0,
                         4.0,
                         8.0,
                         16.0,
                       };


int32_t MSI_attencode(double target_dB) {
  int32_t attencode;
  double remain;
  int i;
  attencode=0;
  remain=target_dB;
  for (i=5;i>=0;i--) {
    if ((remain - MSI_atten_bits_dB[i]) > 0.0) {
      attencode+=pow(2,i);
      remain-=MSI_atten_bits_dB[i];
    }
  }
  return attencode;
};

int32_t MSI_phasecode(double target_nsec) {
  int32_t phasecode;
  phasecode=0;
  double remain;
  int i;
  remain=target_nsec;
  for (i=12;i>=0;i--) {
    if ((remain - MSI_timedelay_bits_nsecs[i]) > 0.0) {
      phasecode+=pow(2,i);
      remain-=MSI_timedelay_bits_nsecs[i];
    }
  }
  return phasecode;
};

double MSI_timedelay_needed(double angle_degrees,double spacing_meters,int32_t card) {

/*
 *  * *  angle from broadside (degrees)  spacing in meters
 *   * */
  double deltat=0;
  double needed=0;
  double c=0.299792458; // meters per nanosecond
  int32_t antenna=-1;
  double radians=0.0;
  if (card > 15) antenna=card-10;
  else antenna=card;
  deltat=(spacing_meters/c)*sin((fabs(angle_degrees)*3.14159)/180.0); //nanoseconds
  if (angle_degrees > 0) needed=antenna*deltat;
  if (angle_degrees < 0) needed=(15-antenna)*deltat;
  if (needed < 0) {
    fprintf(stderr,"Error in Time Needed Calc: %lf %lf\n",needed,deltat);
  }
  return needed;
}
