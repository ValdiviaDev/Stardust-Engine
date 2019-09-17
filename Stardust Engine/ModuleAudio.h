#ifndef __ModuleAudio_H__
#define __ModuleAudio_H__

#include "Module.h"
#include "SDL_mixer\include\SDL_mixer.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f
typedef unsigned int FX;

struct GameSounds {
	FX boostSound;
	FX constraintCollision;
	FX deathSound;
	FX getBoost;
	FX appearCapsule;
	FX breakCapsule;
	FX restartSound;
	FX endGameSound;
	FX runOutBoost;
};

class ModuleAudio : public Module
{
public:

	ModuleAudio(Application* app, bool start_enabled = true);
	~ModuleAudio();

	bool Init();
	bool Start();
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

	//Charge FX
	void ChargeFX();

	//Get FX
	GameSounds GetFX();

private:

	Mix_Music*			music;
	p2List<Mix_Chunk*>	fx;

	GameSounds gameSounds;

};

#endif // __ModuleAudio_H__