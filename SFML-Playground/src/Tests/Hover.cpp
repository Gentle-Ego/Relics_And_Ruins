#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;
using namespace sf;

int main() {
    // Creazione della finestra
    RenderWindow window(VideoMode({800, 600}), "Rettangolo cliccabile con testo");
    window.setFramerateLimit(60);

    // Creazione del rettangolo
    RectangleShape rectangle(Vector2f(200.f, 100.f));
    rectangle.setFillColor(Color::Blue);
    rectangle.setPosition(Vector2f(300.f, 250.f));

    // Creazione del testo
    Font font;
    if (!font.openFromFile("../assets/Fonts/TextBox.ttf")) { // Assicurati di avere un font chiamato "Arial.ttf"
        std::cerr << "Errore nel caricamento del font" << std::endl;
        return -1;
    }
    Text text(font, "Cliccami", 24);
    text.setFillColor(Color::White);

    // Centrare il testo nel rettangolo
    FloatRect textBounds = text.getLocalBounds();
    //text.setOrigin(textBounds.left + textBounds.width / 2, textBounds.top + textBounds.height / 2);
    text.setPosition(Vector2f(rectangle.getPosition().x + rectangle.getSize().x / 2,
                     rectangle.getPosition().y + rectangle.getSize().y / 2));

    // Loop principale della finestra
    while (window.isOpen()) 
    {
        optional<Event> event;
        while ((event = window.pollEvent())) {
            if (event->is<Event::Closed>()) {
                window.close();
            }
        }

            // Gestione del clic sul rettangolo
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                Vector2i mousePos = Mouse::getPosition(window);
                if (rectangle.getGlobalBounds().contains(static_cast<Vector2f>(mousePos))) {
                    // Azione: stampa un messaggio sulla console
                    std::cout << "Rettangolo cliccato!" << std::endl;
                }
            }

        // Disegna tutto nella finestra
        window.clear();
        window.draw(rectangle);
        window.draw(text);
        window.display();
    }

    return 0;
}
