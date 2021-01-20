#include "drawMenu.hpp"
#include "GameEngine.hpp"
#include "Chunk.hpp"
#include "Chunks.hpp"
#include "files.hpp"

#include <iostream>
#include <fstream>

void continueBtn(void);
void newBtn(void);
void createWindow(void);
bool checkFile(void);
sf::Texture texture_white;
sf::Texture texture_dark;
sf::Font font;
sf::Texture background;
bool save = false;

sf::RenderWindow window(sf::VideoMode(1800, 900), "Mincraft Menu", sf::Style::Default);

void drawWindow::draw(){
    ma::Menu m(&window);
    m.background.setTexture(background);

    ma::MenuHolder mh(&m);
    m.mainMenu = &mh;
    save = checkFile();
    if (save) {
        mh.appendElement(new ma::TextButton(&texture_white, font, new ma::Function([] {continueBtn();}), "continue"));
    } else {
        mh.appendElement(new ma::TextButton(&texture_dark, font, new ma::Function([] {continueBtn();}), "continue"));
    }
    mh.appendElement(new ma::TextButton(&texture_white, font, new ma::Function([] {newBtn(); }), "start"));
    mh.appendElement(new ma::TextButton(&texture_white, font, new ma::Function([] {exit(0); }), "exit"));

    auto size = window.getSize();

    bool mouseButtonPressed = 0;
    bool escapeButtonReleased = 0;
    while (window.isOpen()) {
        if (!save)
            save = checkFile();
        mouseButtonPressed = 0;
        escapeButtonReleased = 0;
        sf::Event event;
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed) {
                window.close();
            }else if(event.type == sf::Event::MouseButtonReleased)
            {
                if (event.key.code == sf::Mouse::Left) {
                    mouseButtonPressed = 1; }
            }else
            if (event.type == sf::Event::KeyReleased){

                if (event.key.code == sf::Keyboard::Escape) { escapeButtonReleased = 1; }
            }else if(event.type == sf::Event::Resized) {
                 size = window.getSize();
            }
        }

        if(!m.update(mouseButtonPressed, escapeButtonReleased)){
            window.close();
        }
        window.setView(sf::View({ 0.f, 0.f, (float)size.x, (float)size.y }));
        window.display();
        window.clear();
    }
}

void drawWindow::loadUIData(){
    texture_white.loadFromFile("button_white.png");
    texture_dark.loadFromFile("button_dark.png");
    font.loadFromFile("font.ttf");
    background.loadFromFile("background.png");
}

void continueBtn(){
    if (save) {
        Chunks *chunks = new Chunks(16,8,16);
        unsigned char* buffer = new unsigned char[chunks->volume * CHUNK_VOL];
        if(read_binary_file("world.bin", (char*)buffer, chunks->volume * CHUNK_VOL)){
            chunks->read(buffer);
        }
        startGame(true, chunks);
    }
    
}

void newBtn(){
    Chunks *chunks = new Chunks(16,8,16);
    startGame(false, chunks);
}

bool checkFile(){
    std::ifstream file;
    file.open("world.bt");
    if (file.is_open()) {
        file.close();
        return true;
    }
    file.close();
    return false;
}
