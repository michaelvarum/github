#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <limits>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <condition_variable>
#include <string>
#include <map>
#include <functional>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>

class MusicPlayer;



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





//-------------------------------------------------------------------------------//






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





//-------------------------------------------------------------------------------------------------------






class UserInterface
{
public:
    UserInterface(std::atomic<int>* event, 
                  size_t socketsAmout, 
                  boost::interprocess::interprocess_semaphore *semaphore, 
                  std::mutex* eventsMutex,
                  std::condition_variable* eventsConditionVariable,
                  std::atomic<bool>* isExit);
    ~UserInterface();
private:
    void RunInterface();
    std::thread m_threadRunInterface;
    std::atomic<int>* m_event;
    size_t m_eventsAmout;
    boost::interprocess::interprocess_semaphore *m_semaphore;
    std::mutex* m_eventsMutex;
    std::condition_variable* m_eventsConditionVariable;
    std::atomic<bool>* m_isExit;
};

UserInterface::UserInterface(std::atomic<int>* event, 
                             size_t eventsAmout, 
                             boost::interprocess::interprocess_semaphore *semaphore, 
                             std::mutex* eventsMutex,
                             std::condition_variable* eventsConditionVariable,
                             std::atomic<bool>* isExit)
: m_threadRunInterface(std::thread(&UserInterface::RunInterface, this)),
  m_event(event),
  m_eventsAmout(eventsAmout),
  m_semaphore(semaphore),
  m_eventsMutex(eventsMutex),
  m_eventsConditionVariable(eventsConditionVariable),
  m_isExit(isExit)
{}

UserInterface::~UserInterface()
{
    m_threadRunInterface.join();
}

void UserInterface::RunInterface()
{
    m_semaphore->wait();
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML GUI with Buttons");

    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/msttcorefonts/arial.ttf")) 
    {
        // Handle font loading error
    }

    // Texts for buttons
    sf::Text buttonText[m_eventsAmout];    // Minus 1 because there is exit button. 
    
    std::string buttonNames[] =
    {
        "AddPlayList", "AddAudioFile", "RemovePlayList", "RemoveAudioFile",
        "Play", "Stop", "Next", "Prev", "Exit"
    };

    for (int i = 0; i < m_eventsAmout; i++) 
    {
        buttonText[i].setFont(font);
        buttonText[i].setCharacterSize(18);
        buttonText[i].setString(buttonNames[i]);

        // Determine the position based on the column and row
        int column = i % 2;
        int row = i / 2;
        buttonText[i].setPosition(120 + column * 400, 205 + row * 70);
    }

    // Buttons
    sf::RectangleShape buttons[m_eventsAmout];
    for (int i = 0; i < m_eventsAmout; i++) 
    {
        buttons[i].setSize(sf::Vector2f(200, 50));
        buttons[i].setFillColor(sf::Color::Red);

        // Determine the position based on the column and row
        int column = i % 2;
        int row = i / 2;
        buttons[i].setPosition(100 + column * 400, 200 + row * 70);
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || *m_isExit)
            {
                event.type = sf::Event::Closed;
                m_eventsMutex->lock();
                *m_event = 8;   // its exit
                window.close();
                m_eventsConditionVariable->notify_one();
                m_eventsMutex->unlock();
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                    for (int i = 0; i < m_eventsAmout; i++) 
                    {
                        if (buttons[i].getGlobalBounds().contains(mousePosF))
                        {
                            // Button i+1 was clicked
                            // Add your button functionality here
                            m_eventsMutex->lock();
                            std::cout << buttonNames[i] << std::endl;
                            *m_event = i;   // Update the event
                            m_eventsConditionVariable->notify_one();
                            m_eventsMutex->unlock();
                        }
                    }
                }
            }
        }

        window.clear();

        for (int i = 0; i < m_eventsAmout; i++) 
        {
            window.draw(buttons[i]);
            window.draw(buttonText[i]);
        }

        window.display();
    }
}




class MusicPlayer
{
public:
    enum EVENT
    {
        NO_EVENT = -1,
        ADD_PLAY_LIST = 0,
        ADD_AUDIO_FILE = 1,
        REMOVE_PLAY_LIST = 2,
        REMOVE_AUDIO_FILE = 3,
        PLAY = 4,
        STOP = 5,
        NEXT = 6,
        PREV = 7,
        EXIT = 8
    };
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
    std::thread m_userPLThread;
    std::thread m_eventsThread;
    std::atomic<bool> m_isExit;
    boost::interprocess::interprocess_semaphore m_semaphore;
    static const size_t FUNCIONALITY_AMOUT = 9;
    std::atomic<int> m_event;
    UserInterface m_userInterface;

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
      m_userPLThread(std::thread(&MusicPlayer::UpdateCurrPL, this)),
      m_eventsThread(),
      m_isExit(false),
      m_semaphore(0),
      m_event(NO_EVENT),
      m_userInterface(&m_event, FUNCIONALITY_AMOUT, &m_semaphore, &m_eventsMutex, &m_eventsConditionVariable, &m_isExit)
{}



MusicPlayer::~MusicPlayer()
{
    // Stop the playback and wait for the thread to finish
    Stop();
    std::cout << "In ~MusicPlayer before m_thread.join()\n";
    if (m_playerManagerThread.joinable())
    {
        m_playerManagerThread.join();
    }
    std::cout << "In ~MusicPlayer after m_thread.join() and before m_threadUserPL.join()\n";
    if (m_userPLThread.joinable())
    {
        m_userPLThread.join();
    }
    std::cout << "In ~MusicPlayer after m_userPLThread.join() and before m_eventsThread.join()\n";
    if (m_eventsThread.joinable())
    {
        m_eventsThread.join();
    }
    std::cout << "In ~MusicPlayer after m_eventsThread.join()\n";
    // Clean up the playLists
    for (auto& pair : m_playLists)
    {
        delete pair.second;
    } 

    m_semaphore.post();
}

void MusicPlayer::Run()
{
    m_semaphore.post();

    std::unique_lock<std::mutex> lock(m_eventsMutex);
    while (!m_isExit)
    {
        m_event = NO_EVENT;
        while (m_event == NO_EVENT && !m_isExit)
        {
            std::cout << "in Run in while of m_eventsConditionVariable\n";
            m_eventsConditionVariable.wait(lock);
        }

        switch (m_event) 
        {
            case ADD_PLAY_LIST:
                AddPlayList("New Playlist");
                break;
            case ADD_AUDIO_FILE:
                AddAudioFile("New Playlist", "song1.wav");
                break;
            case REMOVE_PLAY_LIST:
                RemovePlayList("New Playlist");
                break;
            case REMOVE_AUDIO_FILE:
                RemoveAudioFile("New Playlist", "song1.wav");
                break;
            case PLAY:
                Play();
                break;
            case STOP:
                Stop();
                break;
            case NEXT:
                Next();
                break;
            case PREV:
                Prev();
                break;
            case EXIT:
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
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
    std::cout << "in Play before if(!m_isPlay && m_currIndxAF < m_currPL->GetAudioFilesVec().size())\n";
    if (!m_isPlay && m_currIndxAF < m_currPL->GetAudioFilesVec().size())
    {
        std::cout << "in Play inside if(!m_isPlay && m_currIndxAF < m_currPL->GetAudioFilesVec().size())\n";
        m_currAF = m_currPL->GetAudioFilesVec()[m_currIndxAF];
        m_currAF->Play();
        m_isPlay = true;
        m_conditionVariable.notify_one();
    }
    std::cout << "in Play after if(!m_isPlay && m_currIndxAF < m_currPL->GetAudioFilesVec().size())\n";
}

void MusicPlayer::Stop()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "in Stop before if (m_isPlay)\n";
    if (m_isPlay)
    {
        std::cout << "in Stop inside if (m_isPlay)\n";
        m_currAF->Stop();
        m_isPlay = false;
        m_conditionVariable.notify_one();
    }
    std::cout << "in Stop after if (m_isPlay)\n";
}

void MusicPlayer::Next()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "in Next Method\n";
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
    std::cout << "in Prev Method\n";
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
    std::cout << "in AddPlayList Method\n";
    if (m_playLists.find(plName) == m_playLists.end())
    {
        PlayList* playlist = new PlayList(plName);
        m_playLists[plName] = playlist;
        //m_conditionVariable.notify_one();
    }
}


void MusicPlayer::AddAudioFile(std::string plName, std::string audioFileName)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::cout << "in AddAudioFile Method\n";
    auto it = m_playLists.find(plName);
    if (it != m_playLists.end())
    {
        it->second->AddAudioFile(audioFileName);
        //m_conditionVariable.notify_one();
    }
}

void MusicPlayer::UpdateCurrPL()
{
    while (!m_isExit)
    {
        // Find the playlist entered by the user
        std::string playlistName;
        std::cout << "Enter the name of the playlist:\n";
        std::getline(std::cin, playlistName);
        m_mutex.lock();

        auto it = m_playLists.find(playlistName);
        if (it != m_playLists.end())
        {
            m_currPL = it->second;
            std::cout << "Current playlist set to: " << playlistName << std::endl;
            m_conditionVariable.notify_one();
        }
        else
        {
            std::cout << "Playlist not found: " << playlistName << std::endl;
        }
        m_mutex.unlock();
    }
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

    //musicPlayer.RemoveAudioFile("New Playlist", "song1.wav");

    return 0;
}