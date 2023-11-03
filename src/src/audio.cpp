#include <audio.hpp>

SoLoud::Soloud soloud;

void InitAudio(){
    soloud.init();
}

void DeinitAudio(){
    soloud.deinit();
}

int AudioPlayer::LoadAudio(const char *path){
    Wavs.resize(WavIndex+1);
    Wavs[WavIndex].load(path);
    ++WavIndex;

    return WavIndex-1;
}

int AudioPlayer::LoadAudioLong(const char *path){
    WavStreams.resize(WavStreamIndex+1);
    WavStreams[WavStreamIndex].load(path);
    ++WavStreamIndex;

    return WavStreamIndex-1;
}

void AudioPlayer::PlayAudio(int index){
    soloud.play(Wavs[index]);
}

void AudioPlayer::PlayAudioLong(int index){
    soloud.play(WavStreams[index]);
}

void AudioPlayer::StopAudio(int index){
    Wavs[index].stop();
}

void AudioPlayer::StopAudioLong(int index){
    WavStreams[index].stop();
}

void AudioPlayer::ClearAudio(){
    Wavs.clear();
    WavIndex=0;
}

void AudioPlayer::ClearAudioLong(){
    WavStreams.clear();
    WavStreamIndex=0;
}