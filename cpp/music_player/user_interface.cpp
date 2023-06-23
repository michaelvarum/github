#include <iostream>

#include "user_interface.hpp"

UserInterface::UserInterface(std::atomic<int>* event, 
                             size_t eventsAmout, 
                             boost::interprocess::interprocess_semaphore *semaphore, 
                             std::mutex* eventsMutex,
                             std::condition_variable* eventsConditionVariable,
                             std::atomic<bool>* isExit,
                             std::string& addPLName,
                             std::string& addAFName,
                             std::string& currPLName)
: m_threadRunInterface(std::thread(&UserInterface::RunInterface, this)),
  m_event(event),
  m_eventsAmout(eventsAmout),
  m_semaphore(semaphore),
  m_eventsMutex(eventsMutex),
  m_eventsConditionVariable(eventsConditionVariable),
  m_isExit(isExit),
  m_buttonText(),
  m_addPLName(addPLName),
  m_addAFName(addAFName),
  m_currPLName(currPLName)
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

    
    std::string buttonNames[] =
    {
        "Add Playlist", "Add Audio File", "Remove Playlist", "Remove Audio File",
        "Play", "Stop", "Next", "Prev", "Choose Playlist", "Exit"
    };

    for (int i = 0; i < m_eventsAmout; i++) 
    {
        m_buttonText[i].setFont(font);
        m_buttonText[i].setCharacterSize(18);
        m_buttonText[i].setString(buttonNames[i]);

        // Determine the position based on the column and row
        int column = i % 2;
        int row = i / 2;
        m_buttonText[i].setPosition(120 + column * 400, 205 + row * 70);
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
                *m_event = MusicPlayerUtilities::EXIT;   // its exit
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
                            // Button i was clicked
                            m_eventsMutex->lock();
                            std::cout << buttonNames[i] << std::endl;
                            *m_event = i;   // Update the event of button click.
                            if (*m_event == MusicPlayerUtilities::ADD_PLAY_LIST || *m_event == MusicPlayerUtilities::REMOVE_PLAY_LIST)
                            {
                                HandleAddRemovePlaylist();
                            }
                            else if (*m_event == MusicPlayerUtilities::ADD_AUDIO_FILE || *m_event == MusicPlayerUtilities::REMOVE_AUDIO_FILE)
                            {
                                HandleAddRemoveAudioFile();
                            }
                            else if (*m_event == MusicPlayerUtilities::CHOOSE_PLAY_LIST)
                            {
                                HandleChoosePlaylist();
                            }
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
            window.draw(m_buttonText[i]);
        }

        window.display();
    }
}

void UserInterface::HandleAddRemovePlaylist()
{
    OpenTextBox("Enter Playlist Name", m_addPLName);
}

void UserInterface::HandleAddRemoveAudioFile()
{
    OpenTextBox("Enter Playlist Name", m_addPLName);
    OpenTextBox("Enter Audio file name", m_addAFName);
}

void UserInterface::HandleChoosePlaylist()
{
    OpenTextBox("Choose a Playlist", m_currPLName);
}

void UserInterface::OpenTextBox(std::string text, std::string& bufferToFill)
{
    // Create the first window for entering the playlist name
    sf::RenderWindow audioFileWindow(sf::VideoMode(400, 200), text);

    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/msttcorefonts/arial.ttf"))
    {
        // Handle font loading error
    }

    sf::Text label1(text + ":", font, 16);
    label1.setPosition(20, 30);

    sf::RectangleShape inputBox1(sf::Vector2f(300, 30));
    inputBox1.setPosition(20, 60);
    inputBox1.setFillColor(sf::Color::White);
    inputBox1.setOutlineThickness(1);
    inputBox1.setOutlineColor(sf::Color::Black);

    sf::Text enteredText1("", font, 16);
    enteredText1.setPosition(30, 65);
    enteredText1.setFillColor(sf::Color::Black);



    while (audioFileWindow.isOpen())
    {
        sf::Event event;
        while (audioFileWindow.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                audioFileWindow.close();
            }
            else if (event.type == sf::Event::TextEntered)
            {
                if (event.text.unicode < 128 && event.text.unicode != '\r' && event.text.unicode != '\b')
                {
                    enteredText1.setString(enteredText1.getString() + static_cast<char>(event.text.unicode));
                }
                else if (event.text.unicode == '\b' && !enteredText1.getString().isEmpty())
                {
                    enteredText1.setString(enteredText1.getString().substring(0, enteredText1.getString().getSize() - 1));
                }
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter)
            {
                // Handle playlist name submission
                bufferToFill = enteredText1.getString();
                // Perform necessary actions with the playlist name
                // ...
                audioFileWindow.close();

            }
        }

        audioFileWindow.clear();
        audioFileWindow.draw(label1);
        audioFileWindow.draw(inputBox1);
        audioFileWindow.draw(enteredText1);
        audioFileWindow.display();
    }

}