#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <limits>
#include <thread>
#include <chrono>
#include <condition_variable>
#include <string>
#include <map>
#include <functional>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>

#include "music_player_utilities.hpp"
#include "audio_file.hpp"
#include "play_list.hpp"
#include "user_interface.hpp"

class MusicPlayer;
class AudioFile;
class PlayList;
class UserInterface;


class MusicPlayer
{
public:

    MusicPlayer();
    ~MusicPlayer();
    void Run();
    void AddPlayList(std::string plName);
    void AddAudioFile(std::string plName, std::string audioFileName);
    void RemovePlayList(std::string plName);
    void RemoveAudioFile(std::string plName, std::string audioFileName);
    void Play();
    void Stop();
    void Next();
    void Prev();
    void Exit();

private:
    void UpdateCurrPL();
    void PlayerManager();
    std::map<std::string, PlayList*> m_playLists;
    PlayList* m_currPL;             // PL represent playlist
    AudioFile* m_currAF;            // AF represent AudioFile
    std::atomic<size_t> m_currIndxAF;
    std::atomic<bool> m_isPlay;
    std::condition_variable m_conditionVariable;
    std::condition_variable m_eventsConditionVariable;
    std::mutex m_mutex;
    std::mutex m_eventsMutex;
    std::thread m_playerManagerThread;
    std::thread m_eventsThread;
    std::atomic<bool> m_isExit;
    boost::interprocess::interprocess_semaphore m_semaphore;
    std::atomic<int> m_event;
    UserInterface m_userInterface;
    std::string m_addPLName;            // Can do that in one buffer.
    std::string m_addAFName;
    std::string m_currPLName;

};
std::vector<AudioFile*>& PlayList::GetAudioFilesVec()
{
    return m_audioFiles;
}

MusicPlayer::MusicPlayer()
    : m_playLists(),
      m_currPL(nullptr),
      m_currAF(nullptr),
      m_currIndxAF(0),
      m_isPlay(false),
      m_conditionVariable(),
      m_eventsConditionVariable(),
      m_mutex(),
      m_eventsMutex(),
      m_playerManagerThread(std::thread(&MusicPlayer::PlayerManager, this)),
      m_eventsThread(),
      m_isExit(false),
      m_semaphore(0),
      m_event(MusicPlayerUtilities::NO_EVENT),
      m_userInterface(&m_event, 
                      MusicPlayerUtilities::FUNCIONALITY_AMOUT, 
                      &m_semaphore, 
                      &m_eventsMutex, 
                      &m_eventsConditionVariable, 
                      &m_isExit,
                      m_addPLName,
                      m_addAFName,
                      m_currPLName)
{}



MusicPlayer::~MusicPlayer()
{
    // Stop the playback and wait for the thread to finish
    Stop();
    std::cout << "In ~MusicPlayer before m_thread.join()\n";        // Has to remove the print
    if (m_playerManagerThread.joinable())
    {
        m_playerManagerThread.join();
    }
    // Has to remove the print
    std::cout << "In ~MusicPlayer after m_playerManagerThread.join() and before m_eventsThread.join()\n";   
    if (m_eventsThread.joinable())
    {
        m_eventsThread.join();
    }
    std::cout << "In ~MusicPlayer after m_eventsThread.join()\n";   // Has to remove the print
    // Clean up the playLists
    for (auto& pair : m_playLists)
    {
        delete pair.second;
    } 
}

void MusicPlayer::Run()
{
    m_semaphore.post();

    std::unique_lock<std::mutex> lock(m_eventsMutex);
    while (!m_isExit)
    {
        m_event = MusicPlayerUtilities::NO_EVENT;
        while (m_event == MusicPlayerUtilities::NO_EVENT && !m_isExit)
        {
            std::cout << "in Run in while of m_eventsConditionVariable\n";  // Has to remove the print
            m_eventsConditionVariable.wait(lock);
        }

        switch (m_event) 
        {
            case MusicPlayerUtilities::ADD_PLAY_LIST:
                AddPlayList(m_addPLName);
                break;
            case MusicPlayerUtilities::ADD_AUDIO_FILE:
                AddAudioFile(m_addPLName, m_addAFName);
                break;
            case MusicPlayerUtilities::REMOVE_PLAY_LIST:
                RemovePlayList(m_addPLName);
                break;
            case MusicPlayerUtilities::REMOVE_AUDIO_FILE:
                RemoveAudioFile(m_addPLName, m_addAFName);
                break;
            case MusicPlayerUtilities::PLAY:
                Play();
                break;
            case MusicPlayerUtilities::STOP:
                Stop();
                break;
            case MusicPlayerUtilities::NEXT:
                Next();
                break;
            case MusicPlayerUtilities::PREV:
                Prev();
                break;
            case MusicPlayerUtilities::CHOOSE_PLAY_LIST:
                UpdateCurrPL();
                break;
            case MusicPlayerUtilities::EXIT:
                Exit();
                break;
        }
    }

}

void MusicPlayer::PlayerManager()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while (!m_isExit)
    {
        // Wait if playback is not active or if the current playlist is empty
        while (!m_isExit && (!m_isPlay || nullptr != m_currPL || m_currPL->GetAudioFilesVec().empty()))
        {
            std::cout << "in PlayerManager in while of m_conditionVariable\n";
            m_conditionVariable.wait(lock);
        }
        std::cout << "in PlayerManager after while of m_conditionVariable\n";
        m_currIndxAF = 0;

        while (!m_isExit && (m_currIndxAF < m_currPL->GetAudioFilesVec().size()))
        {
            m_currAF = m_currPL->GetAudioFilesVec()[m_currIndxAF];
            if (!m_isPlay)
            {
                m_currAF->Play();
            }

            const sf::Sound* sound = &(m_currAF->GetSound());
            while (sound->getStatus() == sf::Sound::Playing && !m_isExit)
            {
                if (!m_isPlay && !m_isExit)
                {
                    m_conditionVariable.wait(lock);
                }
            }

            ++m_currIndxAF;
        }
    }
}

void MusicPlayer::Play()
{

    std::lock_guard<std::mutex> lock(m_mutex);
    // Has to remove the print
    std::cout << "in Play before if(!m_isPlay && m_currIndxAF < m_currPL->GetAudioFilesVec().size())\n";
    if (!m_isPlay && m_currIndxAF < m_currPL->GetAudioFilesVec().size())
    {
        // Has to remove the print
        std::cout << "in Play inside if(!m_isPlay && m_currIndxAF < m_currPL->GetAudioFilesVec().size())\n";
        m_currAF = m_currPL->GetAudioFilesVec()[m_currIndxAF];
        m_currAF->Play();
        m_isPlay = true;
        m_conditionVariable.notify_one();
    }
    // Has to remove the print
    std::cout << "in Play after if(!m_isPlay && m_currIndxAF < m_currPL->GetAudioFilesVec().size())\n";
}

void MusicPlayer::Stop()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "in Stop before if (m_isPlay)\n";  // Has to remove the print
    if (m_isPlay)
    {
        std::cout << "in Stop inside if (m_isPlay)\n";  // Has to remove the print
        m_currAF->Stop();
        m_isPlay = false;
        m_conditionVariable.notify_one();
    }
    std::cout << "in Stop after if (m_isPlay)\n";   // Has to remove the print
}

void MusicPlayer::Next()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "in Next Method\n";                // Has to remove the print
    if (m_currIndxAF < m_currPL->GetAudioFilesVec().size() - 1)
    {
        bool wasPlay = m_isPlay;
        m_currAF->Stop();
        ++m_currIndxAF;
        m_currAF = m_currPL->GetAudioFilesVec()[m_currIndxAF];
        if(wasPlay)
        {
            m_isPlay = true;
            m_currAF->Play();
            m_conditionVariable.notify_one();
        }
    }
}

void MusicPlayer::Prev()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "in Prev Method\n";            // Has to remove the print
    if (m_currIndxAF > 0)
    {
        bool wasPlay = m_isPlay;
        m_currAF->Stop();
        --m_currIndxAF;
        m_currAF = m_currPL->GetAudioFilesVec()[m_currIndxAF];
        if(wasPlay)
        {
            m_isPlay = true;
            m_currAF->Play();
            m_conditionVariable.notify_one();
        }
    }
}

void MusicPlayer::AddPlayList(std::string plName)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "in AddPlayList Method\n";         // Has to remove the print
    if (m_playLists.find(plName) == m_playLists.end())
    {
        PlayList* playlist = new PlayList(plName);
        m_playLists[plName] = playlist;
    }
}


void MusicPlayer::AddAudioFile(std::string plName, std::string audioFileName)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "in AddAudioFile Method\n";        // Has to remove the print
    auto it = m_playLists.find(plName);
    if (it != m_playLists.end())
    {
        it->second->AddAudioFile(audioFileName);
    }
}

void MusicPlayer::UpdateCurrPL()
{
    m_mutex.lock();
    auto it = m_playLists.find(m_currPLName);
    if (it != m_playLists.end())
    {
        m_currPL = it->second;
        std::cout << "Current playlist set to: " << m_currPLName << std::endl;
        m_conditionVariable.notify_one();
    }
    else
    {
        std::cout << "Playlist not found: " << m_currPLName << std::endl;
    }
    m_mutex.unlock();
}

void MusicPlayer::RemovePlayList(std::string plName)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "in RemovePlayList Method\n";
    auto it = m_playLists.find(plName);
    if (it != m_playLists.end())
    {
        if (it->second == m_currPL)
        {
            Stop(); // Stop playback if the current playlist is being removed
            m_currPL = nullptr;
        }
        delete it->second;
        m_playLists.erase(it);
    }
}

void MusicPlayer::RemoveAudioFile(std::string plName, std::string audioFileName)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "in RemoveAudioFile Method\n";
    auto it = m_playLists.find(plName);
    if (it != m_playLists.end())
    {
        PlayList* playlist = it->second;
        std::vector<AudioFile*>& audioFiles = playlist->GetAudioFilesVec();
        for (auto audioFileIt = audioFiles.begin(); audioFileIt != audioFiles.end(); ++audioFileIt)
        {
            AudioFile* audioFile = *audioFileIt;
            if (audioFile->GetFileName() == audioFileName)
            {
                if (audioFile == m_currAF)
                {
                    Stop(); // Stop playback if the current audio file is being removed
                    m_currAF = nullptr;
                }
                delete audioFile;
                audioFiles.erase(audioFileIt);
                break;
            }
        }
    }
}

void MusicPlayer::Exit()
{
    std::cout << "In exit\n";
    m_isExit = true;
    m_conditionVariable.notify_all();
}



int main()
{
    MusicPlayer musicPlayer;
    musicPlayer.Run();

    return 0;
}