#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <SFML/Audio.hpp>

using namespace std;

//-------------------------------- Forward Declaration, --------------------------------//
class PlayList;
class UserInterface;
class Song;

//-------------------------------- MusicPlayer Class Declaration --------------------------------//
class MusicPlayer
{
public:
    MusicPlayer();
    void AddPlayList(string plName);
    void RemovePlayList(string plName);
    void AddSong(string plName, string songName);
    void RemovePlayListSong(string plName, string songName);
    void Play(string Plname);
    void Stop();
    void NextSong();
    void NextPlayList();
    void Run();

private:
    map<string, PlayList> m_playLists;
    Song* m_currSong;
    PlayList* m_currPlayList;
    UserInterface* m_interface;
};

//-------------------------------- PlayList Class Declaration --------------------------------//
class PlayList
{
public:
    PlayList(string playlistName);
    void AddSong(Song* song);
    void RemoveSong(string songName);
    void Next();
    void PlayPL();
    void Stop();
private:
    string m_playlistName;
    vector<Song> m_playList;
    map<string, vector<Song>::iterator> m_map;
    Song* m_currSong;
};

//-------------------------------- UserInterface Class Declaration --------------------------------//
class UserInterface
{
public:
    UserInterface();
    void RunInterface();
private:
    
};

//-------------------------------- Song Class Declaration --------------------------------//
class Song
{
public:
    Song(string name, string path);
    void Play();
    void Stop();
    string GetName();
private:
    string m_name;
    string m_path;
    sf::SoundBuffer m_soundBuffer;
    sf::Sound m_sound;
};
