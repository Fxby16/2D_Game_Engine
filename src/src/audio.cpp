#include <pch.hpp>
#include <audio.hpp>

SoLoud::Soloud SOLOUD;

void InitAudio(){
    SOLOUD.init();
}

void DeinitAudio(){
    SOLOUD.deinit();
}

int AudioPlayer::LoadAudio(const char *path){
    PROFILE_FUNCTION();

    m_Wavs.resize(m_WavIndex+1);
    m_Wavs[m_WavIndex].load(path);
    ++m_WavIndex;

    return m_WavIndex-1;
}

int AudioPlayer::LoadAudioLong(const char *path){
    PROFILE_FUNCTION();

    m_WavStreams.resize(m_WavStreamIndex+1);
    m_WavStreams[m_WavStreamIndex].load(path);
    ++m_WavStreamIndex;

    return m_WavStreamIndex-1;
}

void AudioPlayer::PlayAudio(int index){
    PROFILE_FUNCTION();

    SOLOUD.play(m_Wavs[index]);
}

void AudioPlayer::PlayAudioLong(int index){
    PROFILE_FUNCTION();

    SOLOUD.play(m_WavStreams[index]);
}

void AudioPlayer::StopAudio(int index){
    PROFILE_FUNCTION();

    m_Wavs[index].stop();
}

void AudioPlayer::StopAudioLong(int index){
    PROFILE_FUNCTION();

    m_WavStreams[index].stop();
}

void AudioPlayer::ClearAudio(){
    PROFILE_FUNCTION();

    m_Wavs.clear();
    m_WavIndex=0;
}

void AudioPlayer::ClearAudioLong(){
    PROFILE_FUNCTION();

    m_WavStreams.clear();
    m_WavStreamIndex=0;
}