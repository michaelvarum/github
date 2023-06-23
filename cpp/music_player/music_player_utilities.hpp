#pragma once

#include <cstddef>

namespace MusicPlayerUtilities
{
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
    CHOOSE_PLAY_LIST = 8,
    EXIT = 9,
    EVENTS_AMOUT = 10

};
 
static const size_t FUNCIONALITY_AMOUT = EVENTS_AMOUT;
}