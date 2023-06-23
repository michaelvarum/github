#include "audio_file.hpp"

AudioFile::AudioFile(const std::string& filename)
: m_filename(filename), m_isPlay(false)
{}

AudioFile::~AudioFile()
{}

bool AudioFile::load()
{
    return m_soundBuffer.loadFromFile(m_filename);
}

void AudioFile::Play()
{
    m_sound.setBuffer(m_soundBuffer); // Set the sound buffer before playing
    m_sound.play();
    m_isPlay = true;
}

void AudioFile::Stop()
{
    m_sound.pause();
    m_isPlay = false;
}

const sf::Sound& AudioFile::GetSound() const
{
    return m_sound;
}

std::string AudioFile::GetFileName()
{
    return m_filename;
}