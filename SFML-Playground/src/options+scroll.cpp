#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>

using namespace sf;
using namespace std;

const float scrollSpeed = 30.0f; // Velocità dello scroll
const float optionHeight = 30.0f; // Altezza di ogni opzione
const float scrollOffsetMax = 200.0f; // Offset massimo per lo scroll (per esempio)

// Struttura per rappresentare un'opzione
struct Option {
    RectangleShape box;
    Text text;

    Option(Font& font) : text(font, ""){
        text.setFont(font);
        text.setCharacterSize(20);
        text.setFillColor(Color::Black);
        box.setSize(Vector2f(780, 30)); // Dimensione del rettangolo
        box.setFillColor(Color(180, 180, 255));
    }

    void setText(const std::string& str) {
        text.setString(str);
    }

    void setPosition(float x, float y) {
        box.setPosition(Vector2f(x, y));
        text.setPosition(Vector2f(x + 10, y + 5)); // Distanza interna dal bordo del rettangolo
    }

    void draw(RenderWindow& window) {
        window.draw(box);
        window.draw(text);
    }
};

// Funzione per gestire il click sulle opzioni
bool checkForMouseClick(RectangleShape rectangle, RenderWindow& window, Vector2i mousePos) {
    if (rectangle.getGlobalBounds().contains(static_cast<Vector2f>(mousePos))) {
        if (Mouse::isButtonPressed(Mouse::Button::Left)) {
            return true;
        }
    }
    return false;
}

void handleScroll(float delta, std::vector<Option>& options, float& scrollOffset) {
    // Calcolare la somma dell'altezza totale delle opzioni
    float totalHeight = options.size() * optionHeight;
    float visibleHeight = 600.f;  // Altezza della finestra visibile

    // Calcolare il massimo offset dello scroll
    float maxScroll = std::max(0.f, totalHeight - visibleHeight);

    // Gestire lo scroll verso l'alto
    if (delta > 0) {
        scrollOffset = std::max(scrollOffset - scrollSpeed, 0.f);
    }
    // Gestire lo scroll verso il basso
    else if (delta < 0) {
        scrollOffset = std::min(scrollOffset + scrollSpeed, maxScroll);
    }
}

void drawOptions(RenderWindow& window, std::vector<Option>& options, float scrollOffset) {
    // Determina l'area visibile della finestra
    float visibleHeight = 600.f;  // Imposta l'altezza della finestra visibile

    for (size_t i = 0; i < options.size(); ++i) {
        // Calcola la posizione Y di ciascuna opzione
        float yPosition = 50 + (i * optionHeight) - scrollOffset;

        // Verifica se l'opzione è visibile nella finestra (tra 50 e 600 px)
        if (yPosition >= 50 && yPosition <= visibleHeight) {
            options[i].setPosition(10, yPosition);  // Imposta la posizione dell'opzione
            options[i].draw(window);  // Disegna l'opzione
        }
    }
}

// Funzione per creare le opzioni
vector<Option> createOptions(const vector<string>& optionTexts, Font& font, float startX, float startY, float width, float height) {
    vector<Option> options;
    float padding = 5.f;
    float currentY = startY;

    for (const auto& optionText : optionTexts) {
        Option opt(font);
        opt.box.setSize({width, height});
        opt.box.setPosition(Vector2f(startX, currentY));
        opt.box.setFillColor(Color(100, 100, 100));

        opt.text.setFont(font);
        opt.text.setString(optionText);
        opt.text.setCharacterSize(20);
        opt.text.setFillColor(Color::White);
        opt.text.setPosition(Vector2f(startX + 10, currentY + 5));

        currentY += height + padding;
        options.push_back(opt);
    }

    Option extraOption1(font);
    extraOption1.setText("Extra Opzione 1");
    extraOption1.setPosition(startX, currentY);
    options.push_back(extraOption1);

    Option extraOption2(font);
    extraOption2.setText("Extra Opzione 2");
    extraOption2.setPosition(startX, currentY + height + padding);
    options.push_back(extraOption2);

    return options;
}

// Funzione per gestire il click del mouse
void handleMouseClick(RenderWindow& window, vector<Option>& options) {
    Vector2i mousePos = Mouse::getPosition(window);
    for (auto& opt : options) {
        if (checkForMouseClick(opt.box, window, mousePos)) {
            cout << "Opzione selezionata: " << opt.text.getString().toAnsiString() << endl;
        }
    }
}

int main() {
    RenderWindow window(VideoMode({800, 600}), "Relics & Ruins", Style::Default);
    window.setFramerateLimit(60);
    Font font;

    if (!font.openFromFile("arial.ttf")) {
        cerr << "Errore nel caricamento del font!" << endl;
        return -1;
    }

    vector<string> optionsText;
    for (int i = 1; i <= 60; ++i) {
        optionsText.push_back("Opzione " + to_string(i));
    }

    vector<Option> options = createOptions(optionsText, font, 50.f, 50.f, 780.f, 40.f);

    RectangleShape textBox;
    textBox.setSize({700.f, 100.f});
    textBox.setPosition(Vector2f(50.f, 480.f));
    textBox.setFillColor(Color(50, 50, 50));

    Text textBoxText(font, "", 24);
    textBoxText.setFont(font);
    textBoxText.setString("Testo della textBox");
    textBoxText.setCharacterSize(20);
    textBoxText.setFillColor(Color::White);
    textBoxText.setPosition(Vector2f(60.f, 500.f));

    float scrollOffset = 0.f; // Offset di scroll

    while (window.isOpen()) {
        optional<Event> event;
        while ((event = window.pollEvent())) {
            if (event->is<Event::Closed>()) {
                window.close();
            }

            if (Mouse::isButtonPressed(Mouse::Button::Left)) {
                handleMouseClick(window, options);
            }
            
            if (const auto* scrollEvent = event->getIf<sf::Event::MouseWheelScrolled>())
            {
                handleScroll(scrollEvent->delta, options, scrollOffset);
            }
        }

        window.clear(); // Pulisci la finestra
        window.draw(textBox);
        window.draw(textBoxText);

        // Disegna le opzioni scorrevoli
        drawOptions(window, options, scrollOffset);

        window.display(); // Mostra il contenuto della finestra
    }

    return 0;
}
