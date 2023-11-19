#pragma once

#include <soloud.h>
#include <soloud_wav.h>
#include <soloud_wavstream.h>

#include <vector>

extern SoLoud::Soloud SOLOUD;

void InitAudio();
void DeinitAudio();

class AudioPlayer{
private:

    std::vector<SoLoud::Wav>m_Wavs;
    std::vector<SoLoud::WavStream>m_WavStreams;

    int m_WavIndex;
    int m_WavStreamIndex;

public:
    AudioPlayer(): m_WavIndex(0),m_WavStreamIndex(0){}

    int LoadAudio(const char *path);
    int LoadAudioLong(const char *path);

    void PlayAudio(int index);
    void PlayAudioLong(int index);

    void StopAudio(int index); 
    void StopAudioLong(int index);

    void ClearAudio();
    void ClearAudioLong();
};