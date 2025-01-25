#include <SFML/Graphics.hpp>
#include <optional>
#include <iostream>
#include <vector>

using namespace sf;
using namespace std;

void resizeBackground(Sprite& background, const RenderWindow& window) 
{
    Vector2u windowSize = window.getSize();

    Vector2u textureSize = background.getTexture().getSize();

    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;

    float scale = max(scaleX, scaleY);

    background.setScale(Vector2f(scale, scale));

    float offsetX = (windowSize.x - textureSize.x * scale) / 2;
    float offsetY = (windowSize.y - textureSize.y * scale) / 2;
    background.setPosition(sf::Vector2f(offsetX, offsetY));
}


bool isNotOut(Sprite& character, char direction, const RenderWindow& window) {
    // Ottieni le dimensioni della finestra
    Vector2u windowSize = window.getSize();

    // Calcola le dimensioni attuali del personaggio
    float charX= character.getTexture().getSize().x * character.getScale().x;
    float charY= character.getTexture().getSize().y * character.getScale().y;

    // Controlla la posizione in base alla direzione
    if ((character.getPosition().x < 0 && direction == 'A')  // Muove verso sinistra
        || (character.getPosition().x > windowSize.x - charX && direction == 'D')  // Muove verso destra
        || (character.getPosition().y < 0 && direction == 'W')  // Muove verso l'alto
        || (character.getPosition().y > windowSize.y - charY && direction == 'S')) {  // Muove verso il basso
        return false;  // Il personaggio è fuori dai bordi
    }

    return true;  // Il personaggio è all'interno dei bordi
}


int main() 
{
    // Creazione della finestra
    RenderWindow window(VideoMode::getDesktopMode(), "Simulated Fullscreen", Style::Default);
    window.setPosition(Vector2i(0, 0)); // Posiziona la finestra nell'angolo in alto a sinistra
    //RenderWindow window(VideoMode({width, height}), "SFML Test");
    window.setFramerateLimit(60);

    vector<Texture> textureBack;
    vector<string> textureFilesBack = {"../assets/Textures/Character/Back/CharBack1.png", "../assets/Textures/Character/Back/CharBack2.png", "../assets/Textures/Character/Back/CharBack3.png", "../assets/Textures/Character/Back/CharBack4.png",};
    for (const auto& file : textureFilesBack) 
    {
        Texture texture;
        if (!texture.loadFromFile(file)) 
        {
            return -1; // Esci in caso di errore
        }
        textureBack.push_back(texture); // Inserisci la texture nel vettore
    }

    vector<Texture> textureFront;
    vector<string> textureFilesFront = {"../assets/Textures/Character/Front/CharFront1.png", "../assets/Textures/Character/Front/CharFront2.png", "../assets/Textures/Character/Front/CharFront3.png", "../assets/Textures/Character/Front/CharFront4.png",};
    for (const auto& file : textureFilesFront) 
    {
        Texture texture;
        if (!texture.loadFromFile(file)) 
        {
            return -1; // Esci in caso di errore
        }
        textureFront.push_back(texture); // Inserisci la texture nel vettore
    }
    vector<Texture> textureLeft;
    vector<string> textureFilesLeft = {"../assets/Textures/Character/Left/CharLeft1.png", "../assets/Textures/Character/Left/CharLeft2.png", "../assets/Textures/Character/Left/CharLeft3.png", "../assets/Textures/Character/Left/CharLeft4.png",};
    for (const auto& file : textureFilesLeft) 
    {
        Texture texture;
        if (!texture.loadFromFile(file)) 
        {
            return -1; // Esci in caso di errore
        }
        textureLeft.push_back(texture); // Inserisci la texture nel vettore
    }
    vector<Texture> textureRight;
    vector<string> textureFilesRight = {"../assets/Textures/Character/Right/CharRight1.png", "../assets/Textures/Character/Right/CharRight2.png", "../assets/Textures/Character/Right/CharRight3.png", "../assets/Textures/Character/Right/CharRight4.png",};
    for (const auto& file : textureFilesRight) 
    {
        Texture texture;
        if (!texture.loadFromFile(file)) 
        {
            return -1; // Esci in caso di errore
        }
        textureRight.push_back(texture); // Inserisci la texture nel vettore
    }
    
    // Configurazione dello sprite
    Sprite character(textureBack[0]);
    window.draw(character);
    character.setPosition(Vector2f(400, 200)); // Posizione iniziale

    // Variabili per il movimento e l'animazione
    float speed = 100.0f; // Velocità del movimento
    float animationTimer = 0.0f;
    const float frameDuration = 0.1f; // Durata di ogni frame (in secondi)
    Clock clock;

    // Timer per l'animazione
    Clock animationClock;
    float animationInterval = 0.5f; // Intervallo di tempo tra i frame (in secondi)
    int currentFrame = 0;

    // Direzione corrente (inizialmente "Back")
    enum Direction { Back, Front, Left, Right };
    Direction currentDirection = Back;

    while (window.isOpen()) 
    {
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

        // Cambiamento texture solo durante il movimento
        bool isMoving = false;

        if (Keyboard::isKeyPressed(Keyboard::Key::W) && isNotOut(character, 'W', window)) {
            movement.y -= speed * deltaTime;
            currentDirection = Back;
            isMoving = true;
            //character.setTexture(textureBack[0], true);    
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::S) && isNotOut(character,'S', window)) {
            //movement.y += speed * animationClock.getElapsedTime().asSeconds();
            currentDirection = Front;
            isMoving = true;
            movement.y += speed * deltaTime;
            //character.setTexture(textureFront[0], true);
        }
        if (Keyboard::isKeyPressed(Keyboard::Key::A) && isNotOut(character, 'A', window)) {
            //movement.x -= speed * animationClock.getElapsedTime().asSeconds();
            currentDirection = Left;
            isMoving = true;
            movement.x -= speed * deltaTime;
            //character.setTexture(textureLeft[0], true);
        } 
        if (Keyboard::isKeyPressed(Keyboard::Key::D) && isNotOut(character, 'D', window)) {
            //movement.x += speed * animationClock.getElapsedTime().asSeconds();
            currentDirection = Right;
            isMoving = true;
            movement.x += speed * deltaTime;
            //character.setTexture(textureRight[0], true);  
        }

        // Muovi il personaggio
        character.move(movement);

        if (isMoving && animationClock.getElapsedTime().asSeconds() > animationInterval) 
        {
            animationClock.restart(); // Resetta il timer
            currentFrame = (currentFrame + 1) % 4; // Avanza al frame successivo (ciclo)

            switch (currentDirection) {
                case Back: character.setTexture(textureBack[currentFrame], true); break;
                case Front: character.setTexture(textureFront[currentFrame], true); break;
                case Left: character.setTexture(textureLeft[currentFrame], true); break;
                case Right: character.setTexture(textureRight[currentFrame], true); break;
            }
        }
        
        Texture backgroundTextureValoria;
        if (!backgroundTextureValoria.loadFromFile("../assets/Textures/Backgrounds/Valoria/CapitalLobby.png")) 
        {
            cerr << "Errore: impossibile caricare lo sfondo!" << endl;
            return -1;
        }
        Sprite backgroundSprite(backgroundTextureValoria);

        // Ridimensiona lo sfondo
        resizeBackground(backgroundSprite, window);
    
        // Disegna la scena
        window.clear();
        window.draw(backgroundSprite);
        window.draw(character);
        window.display();
    }
    return 0;
}
