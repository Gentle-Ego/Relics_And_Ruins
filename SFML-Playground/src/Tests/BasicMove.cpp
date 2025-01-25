#include <SFML/Graphics.hpp>
#include <optional>
#include<iostream>
using namespace sf;
using namespace std;

const int width = 800, height = 600;

// character.getTexture()->getSize().x * character.getScale().x

bool isNotOut(Sprite character,char direction) {
    float charX= character.getTexture().getSize().x * character.getScale().x;
    float charY= character.getTexture().getSize().y * character.getScale().y;
    if ((character.getPosition().x < 0 && direction=='A')
     || (character.getPosition().x > width-charX && direction=='D')
     || (character.getPosition().y < 0 && direction=='W') 
     || (character.getPosition().y > height-charY && direction=='S')) {
        return false;
    }
    return true;
}

int main() {
    // Creazione della finestra
    RenderWindow window(VideoMode({width, height}), "SFML Test");
    window.setFramerateLimit(60);

    // Caricamento della texture
    Texture textureBack("CharBack.png"), 
            textureLeft("CharLeft.png"),
            textureRight("CharRight.png"),
            textureWalk("CharWalk.png"),
            textureFront("CharFront.png");
    
    if (!textureBack.loadFromFile("CharBack.png")) {
        return -1; // Esci se la texture non viene caricata
    }
    if (!textureLeft.loadFromFile("CharLeft.png")) {
        return -1; // Esci se la texture non viene caricata
    }
    if (!textureRight.loadFromFile("CharRight.png")) {
        return -1; // Esci se la texture non viene caricata
    }
    if (!textureWalk.loadFromFile("CharWalk.png")) {
        return -1; // Esci se la texture non viene caricata
    }
    if (!textureFront.loadFromFile("CharFront.png")) {
        return -1; // Esci se la texture non viene caricata
    }


    // Configurazione dello sprite
    Sprite character(textureBack);
    window.draw(character);
    character.setPosition(Vector2f(width/4, height/4)); // Posizione iniziale

    // Variabili per il movimento e l'animazione
    float speed = 200.0f; // Velocità del movimento
    float animationTimer = 0.0f;
    const float frameDuration = 0.1f; // Durata di ogni frame (in secondi)
    Clock clock;

    while (window.isOpen()) {
        // Gestione degli eventi
        optional<Event> event;
        while ((event = window.pollEvent())) {
            if (event->is<Event::Closed>()) {
                window.close();
            }
        }

        // Calcolo del delta time
        float deltaTime = clock.restart().asSeconds();
        animationTimer += deltaTime;

        // Movimento del personaggio
        Vector2f movement(0.0f, 0.0f);

        if (Keyboard::isKeyPressed(Keyboard::Key::W) && isNotOut(character, 'W')) {
            movement.y -= speed * deltaTime;
            character.setTexture(textureWalk, true);    
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::S) && isNotOut(character,'S')) {
            movement.y += speed * deltaTime;
            character.setTexture(textureFront, true);
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::A) && isNotOut(character, 'A')) {
            movement.x -= speed * deltaTime;
            character.setTexture(textureLeft, true);
        } 
        if (Keyboard::isKeyPressed(Keyboard::Key::D) && isNotOut(character, 'D')) {
            movement.x += speed * deltaTime;
            character.setTexture(textureRight, true);  
        }

        // Muovi il personaggio
        character.move(movement);

        // Disegna la scena
        window.clear();
        window.draw(character);
        window.display();
    }
    return 0;
}
