#include <SFML/Audio.hpp>
#include <string>
#include <iostream>
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
//-------------------------------------------------------------------------------//






class PlayList
{
public:
    PlayList(std::string plName);
    ~PlayList();
    void AddAudioFile(const std::string& filename);
    void Run();
    void Play();
    void Stop();
    void Next();
    void Prev();
    bool IsPlay() const;

private:
    std::vector<AudioFile*> m_audioFiles;
    AudioFile* m_currAF;      //AF represent AudioFile
    std::atomic<size_t> m_currIndxAF;
    std::atomic<bool> m_isPlay;
    std::condition_variable m_conditionVariable;
    std::mutex m_mutex;
    std::thread m_thread;
    std::string m_plName;
};

PlayList::PlayList(std::string plName)
: m_audioFiles(), m_currAF(nullptr), m_currIndxAF(0), m_isPlay(false), m_plName(plName)
{
    // Start the thread
    m_thread = std::thread(&PlayList::Run, this);
}

PlayList::~PlayList()
{
    // Stop the playback and wait for the thread to finish
    Stop();
    if (m_thread.joinable())
        m_thread.join();

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

void PlayList::Run()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_currIndxAF < m_audioFiles.size())
    {
        m_currAF = m_audioFiles[m_currIndxAF];
        m_currAF->Play();

        const sf::Sound* sound = &(m_currAF->GetSound());
        while (sound->getStatus() == sf::Sound::Playing)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            std::unique_lock<std::mutex> lock(m_mutex);
            if (!IsPlay())
            {
                m_conditionVariable.wait(lock);
            }
        }
        ++m_currIndxAF;
    }
}

void PlayList::Play()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_isPlay && m_currIndxAF < m_audioFiles.size())
    {
        m_currAF = m_audioFiles[m_currIndxAF];
        m_currAF->Play();
        m_isPlay = true;
        m_conditionVariable.notify_one();
    }
}

void PlayList::Stop()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_isPlay)
    {
        m_currAF->Stop();
        m_isPlay = false;
        m_conditionVariable.notify_one();
    }
}

void PlayList::Next()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_currIndxAF < m_audioFiles.size() - 1)
    {
        bool wasPlay = IsPlay();
        m_currAF->Stop();
        ++m_currIndxAF;
        m_currAF = m_audioFiles[m_currIndxAF];
        if(wasPlay)
        {
            m_isPlay = true;
            m_currAF->Play();
            m_conditionVariable.notify_one();
        }
    }
}

void PlayList::Prev()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_currIndxAF > 0)
    {
        bool wasPlay = IsPlay();
        m_currAF->Stop();
        --m_currIndxAF;
        m_currAF = m_audioFiles[m_currIndxAF];
        if(wasPlay)
        {
            m_isPlay = true;
            m_currAF->Play();
            m_conditionVariable.notify_one();
        }
    }
}


bool PlayList::IsPlay() const
{
    return m_isPlay;
}



//-------------------------------------------------------------------------------------------------------


class MusicPlayer
{
public:
    MusicPlayer();
    void AddPlayList(std::string plName);
    void RemovePlayList(std::string plName);
    void AddSong(std::string plName, std::string songName);
    void RemovePlayListSong(std::string plName, std::string songName);
    void Run();
    void Play();
    void Stop();
    void NextSong();
    //void NextPlayList();

private:
    std::map<std::string, PlayList> m_playLists;
    AudioFile* m_currSong;  // AF represent AudioFile
    PlayList* m_currPL;     // PL represent playlist
};







int main()
{
    PlayList playlist("pl_1");
    playlist.AddAudioFile("song1.wav");
    playlist.AddAudioFile("song2.wav");

    // Start the playlist
    playlist.Play();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    playlist.Next();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    playlist.Prev();
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    //playlist.Stop();

    return 0;
}