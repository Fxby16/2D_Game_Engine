#pragma once

#include <soloud.h>
#include <soloud_wav.h>
#include <soloud_wavstream.h>

#include <vector>

extern SoLoud::Soloud soloud;

void InitAudio();
void DeinitAudio();

class AudioPlayer{
private:

    std::vector<SoLoud::Wav>Wavs;
    std::vector<SoLoud::WavStream>WavStreams;

    int WavIndex;
    int WavStreamIndex;

public:
    AudioPlayer(): WavIndex(0),WavStreamIndex(0){}

    /**
     * \return index of the audio
    */
    int LoadAudio(const char *path); 

    /**
     * recommended for longer audios
     * \return index of the audio
    */
    int LoadAudioLong(const char *path);

    void PlayAudio(int index); 

    /**
     * recommended for longer audios
    */
    void PlayAudioLong(int index);

    /**
     * Stop all instances of the audio source
    */
    void StopAudio(int index); 

    /**
     * Stop all instances of the audio source
    */
    void StopAudioLong(int index);

    void ClearAudio();
    void ClearAudioLong();
};