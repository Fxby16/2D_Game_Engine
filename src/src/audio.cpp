#include <audio.hpp>

SoLoud::Soloud SOLOUD;

void InitAudio(){
    SOLOUD.init();
}

void DeinitAudio(){
    SOLOUD.deinit();
}

int AudioPlayer::LoadAudio(const char *path){
    m_Wavs.resize(m_WavIndex+1);
    m_Wavs[m_WavIndex].load(path);
    ++m_WavIndex;

    return m_WavIndex-1;
}

int AudioPlayer::LoadAudioLong(const char *path){
    m_WavStreams.resize(m_WavStreamIndex+1);
    m_WavStreams[m_WavStreamIndex].load(path);
    ++m_WavStreamIndex;

    return m_WavStreamIndex-1;
}

void AudioPlayer::PlayAudio(int index){
    SOLOUD.play(m_Wavs[index]);
}

void AudioPlayer::PlayAudioLong(int index){
    SOLOUD.play(m_WavStreams[index]);
}

void AudioPlayer::StopAudio(int index){
    m_Wavs[index].stop();
}

void AudioPlayer::StopAudioLong(int index){
    m_WavStreams[index].stop();
}

void AudioPlayer::ClearAudio(){
    m_Wavs.clear();
    m_WavIndex=0;
}

void AudioPlayer::ClearAudioLong(){
    m_WavStreams.clear();
    m_WavStreamIndex=0;
}