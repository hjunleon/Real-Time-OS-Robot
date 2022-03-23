#ifndef ULTRASONIC_H
#define ULTRASONIC_H

//volatile unsigned int cur_distance;// = 0;

void initTPM2(void);
void stopTPM2(void);
void resetTPM2(void);
void startTPM2(void);
void disableTPM2(void);
void initEcho(void);
void initTrigger(void);
void initUltrasound(void);
void startUltrasound(void);
unsigned int getTPMC1Value(void);
void startMeasure(void);
void getDistance(void);
void timeToDistance(unsigned int duration);
void onTrigger(void);
void offTrigger(void);

void initPIT(void);
void clearTIF(void);

extern volatile unsigned int ultra_dist;//extern_get_distance(void);

#endif
