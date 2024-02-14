#pragma once

extern SoLoud::Soloud SOLOUD;

void InitAudio();
void DeinitAudio();

class AudioPlayer{
public:
    AudioPlayer(): m_WavIndex(0),m_WavStreamIndex(0){}

    /**
     * Load an audio file in memory
     * Suggested for short audio files.
     * \param path the path to the audio file
    */
    [[nodiscard]] int LoadAudio(const char *path);
    /**
     * Load an audio file. The audio file is streamed from the disk.
     * Suggested for long audio files.
     * \param path the path to the audio file
    */
    [[nodiscard]] int LoadAudioLong(const char *path);

    /**
     * Play an audio loaded with LoadAudio
     * \param index the index of the audio file returned by LoadAudio
    */
    void PlayAudio(int index);
    /**
     * Play an audio loaded with LoadAudioLong
     * \param index the index of the audio file returned by LoadAudioLong
    */
    void PlayAudioLong(int index);

    /**
     * Stop an audio loaded with LoadAudio
     * If the audio is playing multiple times, it will stop all the instances.
     * \param index the index of the audio file returned by LoadAudio
    */
    void StopAudio(int index); 
    /**
     * Stop an audio loaded with LoadAudioLong
     * If the audio is playing multiple times, it will stop all the instances.
     * \param index the index of the audio file returned by LoadAudioLong
    */
    void StopAudioLong(int index);

    /**
     * Free all the audio files loaded with LoadAudio
    */
    void ClearAudio();
    /**
     * Free all the audio files loaded with LoadAudioLong
    */
    void ClearAudioLong();

private:

    std::vector<SoLoud::Wav>m_Wavs; ///<Container for audio files loaded with LoadAudio
    std::vector<SoLoud::WavStream>m_WavStreams; ///<Container for audio files loaded with LoadAudioLong

    int m_WavIndex;
    int m_WavStreamIndex;


};