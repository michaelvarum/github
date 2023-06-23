#pragma once
#include <string>
#include <atomic>
#include <SFML/Audio.hpp>

class AudioFile
{
public:
    AudioFile(const std::string& filename);
    ~AudioFile();
    bool load();
    void Play();
    void Stop();
    const sf::Sound& GetSound() const;
    std::string GetFileName();

private:
    std::string m_filename;
    sf::SoundBuffer m_soundBuffer;
    sf::Sound m_sound;
    std::atomic<bool> m_isPlay;
};