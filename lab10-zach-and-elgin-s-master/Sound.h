// Sound.h
// Runs on TM4C123 or LM4F120
// Prototypes for basic functions to play sounds from the
// original Space Invaders.
// Jonathan Valvano
// November 17, 2014
#include <stdint.h>
extern uint8_t Index;
extern uint8_t SineWave[64];
extern volatile uint32_t Counts;

void Sound_Init(void);
//void Sound_Play(const uint8_t *pt, uint32_t count);
void Sound_Morse(void);
void Sound_Killed(void);
void Sound_Explosion(void);

void Sound_Fastinvader1(void);
void Sound_Fastinvader2(void);
void Sound_Fastinvader3(void);
void Sound_Fastinvader4(void);
void Sound_Highpitch(void);

