#ifndef AUDIO_H
#define AUDIO_H

typedef enum{
	CONNECT,
	RUNNING,
	END,
	NONE
}audioToPlay;

void play_internet_connect(void);
void play_run(void);
void play_end(void);
void stop_music(void);

void play_audio();
void initAudio();

extern unsigned int changeAudio;

#endif