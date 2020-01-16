
// ADC initialization function 
// Input: none
// Output: none
void ADC_Init(void);

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC_In(void);

//Convert
//Input: Raw ADC value to convert
//Output: Number value corresponding to Switch Positon
uint32_t Convert(uint32_t input);



