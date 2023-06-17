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
    UserInterface();
private:
    void RunInterface();
    std::thread m_threadRunInterface;
};

UserInterface::UserInterface()
: m_threadRunInterface(std::thread(&UserInterface::RunInterface, this))
{}

void UserInterface::RunInterface()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML GUI with Buttons");

    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/msttcorefonts/arial.ttf")) {
        // Handle font loading error
    }

    // Texts for buttons
    const int numButtons = 9;
    sf::Text buttonText[numButtons];
    
    std::string buttonNames[] =
    {
        "AddPlayList", "AddAudioFile", "RemovePlayList", "RemoveAudioFile",
        "Run", "Play", "Stop", "Next", "Prev"
    };

    for (int i = 0; i < numButtons; i++) 
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
    sf::RectangleShape buttons[numButtons];
    for (int i = 0; i < numButtons; i++) {
        buttons[i].setSize(sf::Vector2f(200, 50));
        buttons[i].setFillColor(sf::Color::Green);

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
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

                    for (int i = 0; i < numButtons; i++) {
                        if (buttons[i].getGlobalBounds().contains(mousePosF))
                        {
                            // Button i+1 was clicked
                            // Add your button functionality here
                            std::cout << buttonNames[i] << std::endl;
                        }
                    }
                }
            }
        }

        window.clear();

        for (int i = 0; i < numButtons; i++) {
            window.draw(buttons[i]);
            window.draw(buttonText[i]);
        }

        window.display();
    }
}





//-------------------------------------------------------------------------------------------------------




class Reactor
{
public:
    enum EVENT
    {
        READ = 0,
        WRITE = 1,
        ERROR = 2,
        NUM_OF_EVENTS = 3
    };

    Reactor();
    void Add(int fd, EVENT event, std::function<void(int)> func); // subscribe
    void Remove(int fd, EVENT event, std::function<void(int)> func); // unsubscribe
    void Run(timeval *timeout);  // loop with select, should be blocking
    void Stop();

private:
    struct Subscriber
    {
    public:
        int m_fd;
        EVENT m_event;
        std::function<void(int)> m_func;
        bool operator==(const Subscriber subscriber_arg)const;
    };
    bool m_is_running;
    std::vector<Subscriber> m_subscribers; 
    
};

Reactor::Reactor()
: m_is_running(false), m_subscribers(0)
{}


void Reactor::Add(int fd, EVENT event, std::function<void(int)> func)
{
    m_subscribers.push_back(Subscriber({fd, event, func}));
}


void Reactor::Remove(int fd, EVENT event, std::function<void(int)> func)
{
    Subscriber subscriber_arg {fd, event, func};
    for(auto iter = m_subscribers.begin(); iter != m_subscribers.end(); ++iter)
    {
        if (*iter == subscriber_arg)
        {
            m_subscribers.erase(iter);
            return;
        }
    }
    throw std::runtime_error("Dosen't find subscribers to remove\n");
}

void Reactor::Run(timeval *timeout)
{
    fd_set fds_event[3] = {0};

    m_is_running = true;
    while (m_is_running)
    {
        for(size_t i = 0; NUM_OF_EVENTS > i; ++i)
        {
            FD_ZERO(&fds_event[i]);
        }

        // Init the fd_sets with relevant File Descriptors.
        for(auto iter = m_subscribers.begin(); iter != m_subscribers.end(); ++iter)
        {
            FD_SET(iter->m_fd, &fds_event[iter->m_event]);
        }

        int fds_ready = select(FD_SETSIZE, &fds_event[READ], &fds_event[WRITE], &fds_event[ERROR], timeout);
        if (0 > fds_ready)    // select failed. 
        {
            throw std::runtime_error("select failed\n");
        }
        
        if (0 == fds_ready)     // There are no calls from any File Descriptor. 
        {
            //std::cout << "Nothing happened\n";
        }

        // Execute operations of the subscribers.
        for(auto iter = m_subscribers.begin(); iter != m_subscribers.end(); ++iter)
        {
            if (FD_ISSET(iter->m_fd, &fds_event[iter->m_event]))
            {
                iter->m_func(iter->m_fd);
            }
        }
    }
}


void Reactor::Stop()
{
    m_is_running = false;
}


bool Reactor::Subscriber::operator==(const Subscriber subscriber_arg)const
{
    return (m_fd == subscriber_arg.m_fd &&
            m_event == subscriber_arg.m_event &&
            (reinterpret_cast<void*>(*(m_func.target<void(*)(int)>())) 
            ==
            reinterpret_cast<void*>(*(subscriber_arg.m_func.target<void(*)(int)>()))
            )
           );
}





//-------------------------------------------------------------------------------------------------------





class MusicPlayer
{
public:
    MusicPlayer();
    ~MusicPlayer();
    void AddPlayList(std::string plName);
    void AddAudioFile(std::string plName, std::string audioFileName);
    void RemovePlayList(std::string plName);
    void RemoveAudioFile(std::string plName, std::string audioFileName);
    void Run();
    void Play();
    void Stop();
    void Next();
    void Prev();
    void Exit();

private:
    void UpdateCurrPL();
    std::map<std::string, PlayList*> m_playLists;
    PlayList* m_currPL;             // PL represent playlist
    AudioFile* m_currAF;            // AF represent AudioFile
    std::atomic<size_t> m_currIndxAF;
    std::atomic<bool> m_isPlay;
    std::condition_variable m_conditionVariable;
    std::mutex m_mutex;
    std::thread m_thread;
    std::thread m_threadUserPL;
    std::atomic<bool> m_isExit;
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
      m_mutex(),
      m_thread(std::thread(&MusicPlayer::Run, this)),
      m_threadUserPL(std::thread(&MusicPlayer::UpdateCurrPL, this)),
      m_isExit(false),
      m_userInterface()
{}

MusicPlayer::~MusicPlayer()
{
    // Stop the playback and wait for the thread to finish
    Stop();
    std::cout << "In ~MusicPlayer before m_thread.join()\n";
    if (m_thread.joinable())
    {
        m_thread.join();
    }
    std::cout << "In ~MusicPlayer after m_thread.join() and before m_threadUserPL.join()\n";
    if (m_threadUserPL.joinable())
    {
        m_threadUserPL.join();
    }
    std::cout << "In ~MusicPlayer after m_threadUserPL.join()\n";
    // Clean up the playLists
    for (auto& pair : m_playLists)
    {
        delete pair.second;
    }
}

void MusicPlayer::Run()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while (!m_isExit)
    {
        // Wait if playback is not active or if the current playlist is empty
        while (!m_isPlay || nullptr != m_currPL || m_currPL->GetAudioFilesVec().empty() && !m_isExit)
        {
            //std::cout << "in Run in while of m_conditionVariable\n";
            m_conditionVariable.wait(lock);
        }
        //std::cout << "in Run after while of m_conditionVariable\n";
        m_currIndxAF = 0;

        while (m_currIndxAF < m_currPL->GetAudioFilesVec().size() && !m_isExit)
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
    //std::cout << "in Play before if(!m_isPlay && m_currIndxAF < m_currPL->GetAudioFilesVec().size())\n";
    if (!m_isPlay && m_currIndxAF < m_currPL->GetAudioFilesVec().size())
    {
        //std::cout << "in Play inside if(!m_isPlay && m_currIndxAF < m_currPL->GetAudioFilesVec().size())\n";
        m_currAF = m_currPL->GetAudioFilesVec()[m_currIndxAF];
        m_currAF->Play();
        m_isPlay = true;
        m_conditionVariable.notify_one();
    }
    //std::cout << "in Play after if(!m_isPlay && m_currIndxAF < m_currPL->GetAudioFilesVec().size())\n";
}

void MusicPlayer::Stop()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    //std::cout << "in Stop before if (m_isPlay)\n";
    if (m_isPlay)
    {
        //std::cout << "in Stop inside if (m_isPlay)\n";
        m_currAF->Stop();
        m_isPlay = false;
        m_conditionVariable.notify_one();
    }
    //std::cout << "in Stop after if (m_isPlay)\n";
}

void MusicPlayer::Next()
{
    std::lock_guard<std::mutex> lock(m_mutex);
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
        //std::unique_lock<std::mutex> lock(m_mutex);

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
        //std::unique_lock<std::mutex> unlock(m_mutex);
    }
}

void MusicPlayer::RemovePlayList(std::string plName)
{
    std::lock_guard<std::mutex> lock(m_mutex);
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

    // Create a playlist
    musicPlayer.AddPlayList("My Playlist");
    // Add songs to the playlist
    musicPlayer.AddAudioFile("My Playlist", "song1.wav");
    musicPlayer.AddAudioFile("My Playlist", "song2.wav");
    musicPlayer.AddAudioFile("My Playlist", "song3.wav");

    std::this_thread::sleep_for(std::chrono::seconds(3));
    // Remove an audio file
    musicPlayer.RemoveAudioFile("My Playlist", "song2.wav");

    // Remove a playlist
    musicPlayer.RemovePlayList("My Playlist");

    // Add a new playlist
    musicPlayer.AddPlayList("New Playlist");
    // Add songs to the new playlist
    musicPlayer.AddAudioFile("New Playlist", "song4.wav");
    musicPlayer.AddAudioFile("New Playlist", "song5.wav");

    std::cout << "Needs to Entered again\n";
    // Wait for some time
    std::this_thread::sleep_for(std::chrono::seconds(3));
    // Start playing
    musicPlayer.Play();
    // Wait for some time
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Stop playback
    musicPlayer.Stop();
    // Wait for some time
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Remove an audio file
    musicPlayer.RemoveAudioFile("New Playlist", "new_song1.wav");
    // Wait for some time
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Start playing again
    musicPlayer.Play();
    // Wait for some time
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Skip to the next song
    musicPlayer.Next();
    // Wait for some time
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Skip to the previous song
    musicPlayer.Prev();
    // Wait for some time
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Exit the music player
    musicPlayer.Exit();

    return 0;
}