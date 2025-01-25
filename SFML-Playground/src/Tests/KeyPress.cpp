#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <optional>
#include <iostream>
using namespace sf;
using namespace std;

int main() {
    // Creazione della finestra
    RenderWindow window(VideoMode({800, 600}), "SFML Test");
    // Caricamento del font
    Font font;
    if (!font.openFromFile("arial.ttf")) {
        return -1; // Esci se il font non viene caricato
    }

    // Creazione del testo con il costruttore corretto
    Text text(font, "A", 100); // Font, stringa, dimensione caratteri
    text.setFillColor(Color::Red); // Colore rosso
    text.setPosition({350.0f, 250.0f}); // Posizione come Vector2f
    Texture texture("images.png");
    Sprite sprite(texture);

    bool displayText = false; // Controllo per mostrare il testo
    bool displayImage = false;
    // Ciclo principale della finestra
    while (window.isOpen()) 
    {
        // Gestione degli eventi
        optional<Event> event;
        
        while ((event = window.pollEvent())) {
            // Gestione della chiusura finestra
            if (event->is<Event::Closed>()) {
                window.close();
            }

            if (Keyboard::isKeyPressed(Keyboard::Key::B))
            {
                displayImage = true;
            } else if (Keyboard::isKeyPressed(Keyboard::Key::Space))
            {
                text.setString("Space");
                displayText = true;
            } 

            // Pulizia della finestra
           
        }
            window.clear(Color::Black);
            
            // Disegna il testo se un tasto è stato premuto
            if (displayText) {
                window.draw(text);
                displayText=false;
            }
            if (displayImage) {
                window.draw(sprite);
            }

            // Aggiorna il display della finestra
            window.display();
    }
    return 0;
}
