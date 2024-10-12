#include "esp_config.h"
#ifdef USE_JDS8060
#include "esp_jds8060.h"
#include <string.h>

/*
Turn CH1 on and OFF
:w10=1,0,\n\n
:w10=0,0,\n\n
:w10=0,1,\n\n
:w10=0,0,\n\n
:w13=10001000, // Set to 10,001  hz CH0
:w14=10001000, // Set to 10,001  hz CH1
:w11=0, // Set CH0 to Sine
:w11=1, // Set CH0 to Square
:w12=0, // Set CH0 to Sine
:w12=1, // Set CH0 to Square
:w15=1568, // Set CH0 Amplitude to 1.568V
:w15=10568, // Set CH0 Amplitude to 10.568V
:w22=200,\n\n // Set Phase to 2 degrees
:w21=200,\n\n
:w22=250,\n\n // Set Phase to 2.5 Degrees
:w21=250,\n\n
*/

volatile SDeviceState gDeviceState;

void fy6800_write(char* data, uint8_t len)
{
  Serial.write((uint8_t*)data, len);
  Serial.write("\n\n");
  delay(40); // Give AWG some time to send back OK etc then flush serial buffer
  Serial.flush();
}

void setCh1Wave(EWaveType wave)
{
  char command[20];
  sprintf(command, ":w11=%i,", wave);
  gDeviceState.ch1Wave = wave;
  fy6800_write(command, strlen(command));
}

void setCh2Wave(EWaveType wave)
{
  char command[20];
  sprintf(command, ":w12=%i,", wave);
  gDeviceState.ch2Wave = wave;
  fy6800_write(command, strlen(command));
}

void setCh1Output(uint32_t output)
{
  // The old way was to independently control channels, but for some reason
  // my DSD1104 only sends a command to turn CH1 on so this is modified to
  // turn both CH1 & CH2 on at the same time
  char command[20];
  if (output) {
    gDeviceState.ch1Output = 1;
    gDeviceState.ch2Output = 1;
    sprintf(command, ":w10=1,1,", gDeviceState.ch2Output);
    fy6800_write(command, strlen(command));
  } else {
    gDeviceState.ch1Output = 0;
    gDeviceState.ch2Output = 0;
    sprintf(command, ":w10=0,0,", gDeviceState.ch2Output);
    fy6800_write(command, strlen(command));
  }
}

void setCh2Output(uint32_t output)
{
  char command[20];
  if (output) {
    gDeviceState.ch2Output = 1;
    sprintf(command, ":w10=%i,1,", gDeviceState.ch2Output);
    fy6800_write(command, strlen(command));
  } else {
    gDeviceState.ch2Output = 0;
    sprintf(command, ":w10=%i,0,", gDeviceState.ch2Output);
    fy6800_write(command, strlen(command));
  }
}

/* Set frequency in Hz */
void setCh1Freq(uint32_t frequency)
{
  char command[20];
  sprintf(command, ":w13=%u000,", frequency);
  gDeviceState.ch1Freq = frequency;
  fy6800_write(command, strlen(command));
}

/* Set frequency in Hz */
void setCh2Freq(uint32_t frequency)
{
  char command[20];
  sprintf(command, ":w14=%u000,", frequency);
  gDeviceState.ch2Freq = frequency;
  fy6800_write(command, strlen(command));
}

/* Ampl is in mV: 12.345V = 12345 */
void setCh1Ampl(uint32_t ampl)
{
  char command[20];
  sprintf(command, ":w15=%u,", ampl);
  gDeviceState.ch1Ampl = ampl;
  fy6800_write(command, strlen(command));
}

void setCh2Ampl(uint32_t ampl)
{
  char command[20];
  sprintf(command, ":w16=%u,", ampl);
  gDeviceState.ch2Ampl = ampl;
  fy6800_write(command, strlen(command));
}

/* Phase is in 0.1deg: 12.5deg = 125 */
void setCh1Phase(uint32_t phase)
{
  char command[20];
  sprintf(command, ":w22=%u0,", phase);
  gDeviceState.ch1Phase = phase;
  fy6800_write(command, strlen(command));
}

void setCh2Phase(uint32_t phase)
{
  char command[20];
  sprintf(command, ":w22=%u0,", phase);
  gDeviceState.ch2Phase = phase;
  fy6800_write(command, strlen(command));
}

void setCh1Offset(int32_t offset)
{
 char command[20];
 if ((offset > 0) && (offset < 10)) { // JDS8060 can't do lower than 10mv increments
   offset=10;
 } else if ((offset < 0) && (offset > -10)) {
  offset=-10;
 }
 int32_t voltage = 10000 + offset;
 voltage = voltage/10;
 sprintf(command, ":w17=%i,", voltage);
 gDeviceState.ch1Offset = offset;
 fy6800_write(command, strlen(command));
}

void setCh2Offset(int32_t offset)
{
 char command[20];
 if ((offset > 0) && (offset < 10)) { // JDS8060 can't do lower than 10mv increments
   offset=10;
 } else if ((offset < 0) && (offset > -10)) {
  offset=-10;
 }
 int32_t voltage = 10000 + offset;
 voltage = voltage/10;
 sprintf(command, ":w18=1%u,", voltage);
 gDeviceState.ch2Offset = offset;
 fy6800_write(command, strlen(command));
}

void initDevice(void)
{
  Serial.write((uint8_t*)"\n", 1);
  setCh1Output(0);
  setCh1Wave(EWaveType_Sine);
  setCh1Freq(1000);
  setCh1Ampl(1000);
  setCh1Offset(0);
  setCh2Output(0);
  setCh2Wave(EWaveType_Sine);
  setCh2Freq(1000);
  setCh2Ampl(1000);
  setCh2Offset(0);
}
#endif USE_JDS8060