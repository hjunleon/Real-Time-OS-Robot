#ifndef ULTRASONIC_H
#define ULTRASONIC_H

//volatile unsigned int cur_distance;// = 0;

void initTPM2(void);
void stopTPM2(void);
void resetTPM2(void);
void startTPM2(void);
void disableTPM2(void);
void enableTPM2IRQ(void);
void initEcho(void);
void initTrigger(void);
void initUltrasound(void);
void startUltrasound(void);
void stopUltrasound(void);
unsigned int getTPMC1Value(void);
unsigned int getUltraDist(void);
unsigned int getPITTick(void);
void startMeasure(void);
void getDistance(void);
void timeToDistance(unsigned int duration);
void onTrigger(void);
void offTrigger(void);

void initPIT(void);
void disablePIT(void);
void clearTIF(void);

extern volatile unsigned int ultra_dist;//extern_get_distance(void);

#endif
