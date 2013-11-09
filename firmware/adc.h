//ADC Library for the AtMega368
//Written by Ryan Owens

unsigned int adcRead(char channel);
unsigned long adcVoltage(unsigned int adc_value);
void adcInit(char reference, char align, char clock);