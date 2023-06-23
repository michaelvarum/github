#pragma once
#include <string>
#include <vector>

#include "audio_file.hpp"

class PlayList
{
public:
    PlayList(std::string plName);
    ~PlayList();
    void AddAudioFile(const std::string& filename);
    std::vector<AudioFile*>& GetAudioFilesVec();

private:
    std::vector<AudioFile*> m_audioFiles;
    std::string m_plName;
};