#include <iostream>

#include "play_list.hpp"

PlayList::PlayList(std::string plName)
: m_audioFiles(), m_plName(plName)
{}

PlayList::~PlayList()
{
    // Clean up the audio files
    for (AudioFile* audioFile : m_audioFiles)
    {
        delete audioFile;
    }
}

void PlayList::AddAudioFile(const std::string& filename)
{
    AudioFile* audioFile = new AudioFile(filename);
    if (audioFile->load())
    {
        m_audioFiles.push_back(audioFile);
    }
    else
    {
        std::cerr << "Failed to load audio file: " << filename << std::endl;
        delete audioFile;
    }
}