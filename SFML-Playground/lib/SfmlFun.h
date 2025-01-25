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
    background.setPosition(Vector2f(offsetX, offsetY));
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

// Funzione per inizializzare il testo e la textBox
void initializeTextBoxForTutorial(RectangleShape& textBox, Text& textBoxText, const RenderWindow& window, string& fullText, string& currentText) 
{
    fullText = "Welcome to \n Relics & Ruins";
    currentText = "";  // Inizia con testo vuoto

    // Configura la textBox
    textBox.setSize(Vector2f(window.getSize().x - 2 * 10, window.getSize().y - 2 * 10));
    textBox.setPosition(Vector2f(10, 10));

    // Configura il testo
    textBoxText.setCharacterSize(150);
    textBoxText.setPosition(Vector2f(textBox.getPosition().x + 10, textBox.getPosition().y + 10));
    textBoxText.setString(currentText);
}

// Funzione per gestire la stampa graduale del testo
bool handleTextPrintingForTutorial(Clock& clock, const float delay, string& fullText, string& currentText, Text& textBoxText, RenderWindow& window) 
{
    if (!fullText.empty() && clock.getElapsedTime().asSeconds() >= delay) {
        currentText += fullText[0];  // Aggiungi la prossima lettera
        fullText.erase(0, 1);        // Rimuovi la lettera aggiunta
        textBoxText.setString(currentText);
        clock.restart();             // Resetta il timer
    }

    //window.draw(textBoxText);
    return fullText.empty(); // Restituisce true se la stampa è completata
}

void slowPrintInTextBox(Text& textBoxText, const string& toPrintText, const float delay, Clock& clock) 
{
    string fullText = toPrintText;
    string currentText = "";

    while (!fullText.empty()) {
        if (clock.getElapsedTime().asSeconds() >= delay) {
            currentText += fullText[0];  // Aggiungi la prossima lettera
            fullText.erase(0, 1);        // Rimuovi la lettera aggiunta
            textBoxText.setString(currentText);
            clock.restart();             // Resetta il timer
        }
    }
}

// Funzione per centrare il testo in un rettangolo
void centerTextInRectangle(Text& text, const RectangleShape& rectangle) {
    FloatRect textBounds = text.getLocalBounds();
    //text.setOrigin(textBounds.left + textBounds.width / 2, textBounds.top + textBounds.height / 2);
    text.setPosition(Vector2f(rectangle.getPosition().x + rectangle.getSize().x / 2,
                            rectangle.getPosition().y + rectangle.getSize().y / 2));
}

bool checkForMouseClick(RectangleShape rectangle, RenderWindow& window, Vector2i mousePos)
{
    if (rectangle.getGlobalBounds().contains(static_cast<Vector2f>(mousePos))) 
    {
        if (Mouse::isButtonPressed(Mouse::Button::Left)) 
        {
            return true;
        }  
    }
    return false;
}

bool isMouseHovering(const sf::RectangleShape& rect, const sf::RenderWindow& window, const sf::Vector2i& mousePosition) {
    sf::FloatRect bounds = rect.getGlobalBounds();
    return bounds.contains(static_cast<sf::Vector2f>(mousePosition));
}