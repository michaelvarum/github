#pragma once

#include <SFML/Graphics.hpp>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>

#include "music_player_utilities.hpp"

class UserInterface
{
public:
    UserInterface(std::atomic<int>* event, 
                  size_t socketsAmout, 
                  boost::interprocess::interprocess_semaphore *semaphore, 
                  std::mutex* eventsMutex,
                  std::condition_variable* eventsConditionVariable,
                  std::atomic<bool>* isExit,
                  std::string& addPLName,
                  std::string& addAFName,
                  std::string& currPLName);
    ~UserInterface();
    void HandleAddRemovePlaylist();
    void HandleAddRemoveAudioFile();
    void HandleChoosePlaylist();

private:
    void RunInterface();
    void OpenTextBox(std::string text, std::string& bufferToFill);
    std::thread m_threadRunInterface;
    std::atomic<int>* m_event;
    size_t m_eventsAmout;
    boost::interprocess::interprocess_semaphore *m_semaphore;
    std::mutex* m_eventsMutex;
    std::condition_variable* m_eventsConditionVariable;
    std::atomic<bool>* m_isExit;
    sf::Text m_buttonText[MusicPlayerUtilities::FUNCIONALITY_AMOUT];
    std::string& m_addPLName;
    std::string& m_addAFName;
    std::string& m_currPLName;
};