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

