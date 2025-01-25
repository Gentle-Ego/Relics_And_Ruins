#include <SFML/Graphics.hpp>
#include "../lib/SfmlFun.h"
#include <optional>
#include <iostream>
using namespace std;
using namespace sf;

int main() {
    // Creazione della finestra
    RenderWindow window(VideoMode({800, 600}), "Input Box Example");

    // Caricamento del font
    Font font;
    if (!font.openFromFile("../assets/Fonts/TextBox.ttf")) {
        cerr << "Errore nel caricamento del font\n";
        return -1;
    }

    // Creazione dell'oggetto Text per visualizzare l'input
    Text text(font, "", 24);
    text.setFont(font);
    text.setCharacterSize(24);
    text.setFillColor(Color::White);
    text.setPosition(Vector2f(50, 50));

    // Stringa per memorizzare l'input dell'utente
    string inputString;

    String playerInput;
    Text playerText (font, "", 24);

    RectangleShape textBox;
    textBox.setFillColor(Color::White);
    textBox.setSize(Vector2f(300, 50));
    textBox.setPosition(Vector2f(50, 50));
    textBox.setOutlineColor(Color(0,0,0,0));
    textBox.setOutlineThickness(2);

    bool textBoxSelected = false;
    Vector2i mousePosition;

    // Loop principale
    while (window.isOpen())
    {
        mousePosition = Mouse::getPosition(window);
        while (optional<Event> event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
            } else if (const auto* textEvent = event->getIf<Event::TextEntered>()) {
                if (textEvent->unicode < 128 && textBoxSelected) {
                    playerInput += static_cast<char>(textEvent->unicode);
                    playerText.setString(playerInput);
                }
            }
        }
        if(checkForMouseClick(textBox, window, mousePosition))
        {
            textBoxSelected = true;
            textBox.setOutlineColor(Color::Blue);
        } else if(Mouse::isButtonPressed(Mouse::Button::Left))
        {
            textBoxSelected = false;
            textBox.setOutlineColor(Color(0,0,0,0));
        }

        window.clear();
        window.draw(textBox);
        window.draw(playerText);
        window.display();
    }
    return 0;
}
