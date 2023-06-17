#include <SFML/Audio.hpp>
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
    bool IsPlay() const;
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

bool AudioFile::IsPlay() const
{
    return m_isPlay;
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


std::vector<AudioFile*>& PlayList::GetAudioFilesVec()
{
    return m_audioFiles;
}


//-------------------------------------------------------------------------------------------------------


class MusicPlayer
{
public:
    MusicPlayer();
    ~MusicPlayer();
    void AddPlayList(std::string plName);
    //void RemovePlayList(std::string plName);
    void AddAudioFile(std::string plName, std::string audioFileName);
    //void RemoveAudioFile(std::string plName, std::string audioFileName);
    void Run();
    void Play();
    void Stop();
    void Next();
    void Prev();
    bool IsPlay() const;
    //void NextPlayList();

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
{
    if (!m_playLists.empty())
    {
        // Set the current playlist to the first one in the map
        m_currPL = m_playLists.begin()->second;
    }
}

MusicPlayer::~MusicPlayer()
{
    // Stop the playback and wait for the thread to finish
    Stop();
    if (m_thread.joinable())
    {
        m_thread.join();
    }
    
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
        while (!m_isPlay || !m_currPL || m_currPL->GetAudioFilesVec().empty())
        {
            std::cout << "in Run in while of m_conditionVariable\n";
            m_conditionVariable.wait(lock);
        }
        std::cout << "in Run after while of m_conditionVariable\n";
        m_currIndxAF = 0;

        while (m_currIndxAF < m_currPL->GetAudioFilesVec().size() && !m_isExit)
        {
            m_currAF = m_currPL->GetAudioFilesVec()[m_currIndxAF];
            if (!(m_currAF->IsPlay()))
            {
                m_currAF->Play();
            }

            const sf::Sound* sound = &(m_currAF->GetSound());
            while (sound->getStatus() == sf::Sound::Playing)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                if (!IsPlay())
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
    if (m_isPlay)
    {
        m_currAF->Stop();
        m_isPlay = false;
        m_conditionVariable.notify_one();
    }
}

void MusicPlayer::Next()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_currIndxAF < m_currPL->GetAudioFilesVec().size() - 1)
    {
        bool wasPlay = IsPlay();
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
        bool wasPlay = IsPlay();
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


bool MusicPlayer::IsPlay() const
{
    return m_isPlay;
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
        std::cout << "Enter the name of the playlist: ";
        std::getline(std::cin, playlistName);
        std::unique_lock<std::mutex> lock(m_mutex);

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
    
}


int main()
{
    MusicPlayer musicPlayer;

    // Create a playlist
    musicPlayer.AddPlayList("My Playlist");
    // Add songs to the playlist
    musicPlayer.AddAudioFile("My Playlist", "song1.wav");
    musicPlayer.AddAudioFile("My Playlist", "song2.wav");
    //musicPlayer.AddAudioFile("My Playlist", "song3.wav");
    // Start playing
    std::this_thread::sleep_for(std::chrono::seconds(2));
    musicPlayer.Play();

    /* // Wait for some time
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Stop playing
    musicPlayer.Stop();

    // Wait for some time
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Start playing again
    musicPlayer.Play();

    // Wait for some time
    std::this_thread::sleep_for(std::chrono::seconds(3));

    // Skip to the next song
    musicPlayer.Next();

    // Wait for some time
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Skip to the previous song
    musicPlayer.Prev();

    // Stop playing
    musicPlayer.Stop();

    // Remove a song from the playlist
    //musicPlayer.RemoveAudioFile("My Playlist", "song2.wav");

    // Remove the playlist
    //musicPlayer.RemovePlayList("My Playlist"); */

    return 0;
}