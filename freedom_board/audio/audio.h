#ifndef AUDIO_H
#define AUDIO_H

typedef enum{
	CONNECT,
	RUNNING,
	END,
}audioToPlay;

void play_internet_connect(void);
void play_run(void);
void play_end(void);

void play_audio();
void initAudio();

#endif