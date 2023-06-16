#include <SFML/Audio.hpp>
#include <string>
#include <iostream>
#include <thread>
#include <condition_variable>

class AudioFile
{
public:
    AudioFile(const std::string& filename);
    ~AudioFile();
    bool load();
    void play();
    void stop();
    const sf::Sound& GetSound();

private:
    std::string m_filename;
    sf::Sound m_sound;
    sf::SoundBuffer m_soundBuffer;
};

AudioFile::AudioFile(const std::string& filename)
    : m_filename(filename)
{
}

AudioFile::~AudioFile()
{
}

bool AudioFile::load()
{
    return m_soundBuffer.loadFromFile(m_filename);
}

void AudioFile::play()
{
    m_sound.setBuffer(m_soundBuffer);
    m_sound.play();
}

void AudioFile::stop()
{
    m_sound.stop();
}

const sf::Sound& AudioFile::GetSound()
{
    return m_sound;
}


class PlayList
{
public:
    PlayList();
    ~PlayList();
    void AddSong(const std::string& filename);
    void Play();
    void Stop();

private:
    std::vector<AudioFile*> m_audioFiles;
    int m_currentSongIndex;
    std::thread m_playbackThread;
    std::condition_variable m_playbackCV;
    std::mutex m_playbackMutex;
    bool m_stopPlayback;
};

PlayList::PlayList()
    : m_audioFiles(), m_currentSongIndex(0), m_playbackThread(), m_stopPlayback(false)
{
}

PlayList::~PlayList()
{
    Stop();

    for (AudioFile* audioFile : m_audioFiles)
    {
        delete audioFile;
    }
}

void PlayList::AddSong(const std::string& filename)
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

void PlayList::Play()
{
    m_playbackThread = std::thread([this]() 
    {
        for (m_currentSongIndex = 0; m_currentSongIndex < m_audioFiles.size(); ++m_currentSongIndex)
        {
            std::lock_guard<std::mutex> lock(m_playbackMutex);
            if (m_stopPlayback)
            {
                break;
            }
            m_playbackMutex.unlock(); // Unlock the mutex

            AudioFile* audioFile = m_audioFiles[m_currentSongIndex];
            audioFile->play();

            const sf::Sound* sound = &(audioFile->GetSound());
            while (sound->getStatus() == sf::Sound::Playing)
            {
                std::lock_guard<std::mutex> lock(m_playbackMutex);
                if (m_stopPlayback)
                {
                    audioFile->stop();
                    break;
                }
            }
        }

        std::lock_guard<std::mutex> lock(m_playbackMutex);
        m_stopPlayback = true;
        m_playbackCV.notify_all();
    });
}

void PlayList::Stop()
{
    m_playbackMutex.lock(); // Lock the mutex for shared data access
    m_stopPlayback = true; // Set the stop flag
    m_playbackMutex.unlock(); // Unlock the mutex

    m_playbackCV.notify_all();

    if (m_playbackThread.joinable())
    {
        m_playbackThread.join();
    }
}

int main()
{
    PlayList playlist;
    playlist.AddSong("song1.wav");
    playlist.AddSong("song2.wav");

    playlist.Play();

    // Wait for user input to stop the playlist
    std::cout << "Press enter to stop the playlist...";
    std::cin.ignore();

    playlist.Stop();

    return 0;
}