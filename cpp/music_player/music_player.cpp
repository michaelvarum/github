#include "music_player.hpp"

//-------------------------------- MusicPlayer Class Definition --------------------------------//

MusicPlayer::MusicPlayer()
: m_playLists(), m_currSong(nullptr), m_currPlayList(nullptr), m_interface()
{
    m_interface = new UserInterface();
}

void MusicPlayer::AddPlayList(string plName)
{
    PlayList newPlayList(plName);
    m_playLists[plName] = newPlayList;
}

void MusicPlayer::RemovePlayList(string plName)
{
    if (m_playLists.find(plName) != m_playLists.end())
    {
        m_playLists.erase(plName);
    }
}


void MusicPlayer::AddSong(string plName, string songName)
{
    auto it = m_playLists.find(plName);
    if (it != m_playLists.end())
    {
        Song* newSong = new Song(songName, "path_to_song");
        it->second.AddSong(newSong);
    }
}


void MusicPlayer::RemovePlayListSong(string plName, string songName)
{
    auto it = m_playLists.find(plName);
    if (it != m_playLists.end())
    {
        it->second.RemoveSong(songName);
    }
}


void MusicPlayer::Play(string Plname)
{
    auto it = m_playLists.find(Plname);
    if (it != m_playLists.end())
    {
        m_currPlayList = &(it->second);
        m_currSong->Play();
    }
}


void MusicPlayer::Stop()
{

}


void MusicPlayer::NextSong()
{
    if (m_currPlayList != nullptr)
    {
        m_currSong->Stop();
        m_currPlayList->Next();
        m_currSong = &(m_currPlayList->GetCurrentSong());
        m_currSong->Play();
    }
}



void MusicPlayer::NextPlayList()
{

}


void MusicPlayer::Run() // this method will invoke the members methods when approriate event is occour
{
    // increment semaphore that hold the interface

    while(1)    //needs to change the condition 
    {
        // event loop from the interface
    }


}


//-------------------------------- PlayList Class Definition --------------------------------//

PlayList::PlayList(string playlistName)
: m_playlistName(), m_playList(), m_map(), m_currSong()
{}


void PlayList::AddSong(Song* song) 
{
    m_playList.push_back(*song);
    m_map[song->GetName()] = --m_playList.end();
}


void PlayList::RemoveSong(string songName)
{
    auto it = m_map.find(songName);
    if (it != m_map.end())
    {
        Song& song = *(it->second);
        m_playList.erase(it->second);
        m_map.erase(it);
        delete &song;
    }
}


void PlayList::Next()
{}


void PlayList::PlayPL()
{
    for (auto it = m_playList.begin(); it != m_playList.end(); ++it)
    {
        it->Play();
    }
    
}


void PlayList::Stop()
{}


//-------------------------------- UserInterface Class Definition --------------------------------//

UserInterface::UserInterface()
{}


void UserInterface::RunInterface()
{
    // semaphore wait

    // create a user interface with bottun to choose a play list,
    // add a song to the choosen playlist, play, stop
}


//-------------------------------- Song Class Definition --------------------------------//

Song::Song(std::string name, std::string path) 
: m_name(name), m_path(path)
{}


void Song::Play()
{
    m_sound.play();
}


void Song::Stop()
{
    m_sound.stop();
}


string Song::GetName()
{
    return m_name;
}