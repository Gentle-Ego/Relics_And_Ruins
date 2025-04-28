#include <SFML/Graphics.hpp>
#include <optional>
#include <iostream>
#include <vector>

using namespace sf;
using namespace std;

float globalDesiredHeight = 0.0f;
bool leftMouseReleased = false;
void isMouseReleased(bool &released) {
    leftMouseReleased = released;
}
json leaderboards_data = load_leaderboards_data("../include/ideal_leads.json");
bool leaderboardIn = false;

struct Option {
    RectangleShape box;
    Text text;

    Option(Font& font, float sizeX, float sizeY): text(font, ""){
        text.setFont(font);
        text.setCharacterSize(20);
        text.setFillColor(Color(230, 205, 147));
        box.setSize(Vector2f(sizeX, sizeY)); // Dimensione del rettangolo
        box.setFillColor(Color(76,65,62));
    }

    void setOptionPosition(float x, float y) {
        box.setPosition(Vector2f(x, y));
        text.setPosition(Vector2f(x + 10, y + 5)); // Distanza interna dal bordo del rettangolo
    }

    void draw(RenderWindow& window) {
        window.draw(box);
        window.draw(text);
    }
};

void rescaleSprite(Sprite &sprite, const Vector2u &windowSize, float widthPercent, float heightPercent)
{
    // Calculate the desired dimensions in pixels.
    // For example, widthPercent = 0.80 means 80% of the window width,
    // and heightPercent = 0.05 means 5% of the window height.
    float desiredWidth = windowSize.x * widthPercent;
    float desiredHeight = windowSize.y * heightPercent;
    globalDesiredHeight = desiredHeight;

    // Retrieve the sprite's texture to obtain its original size.
    const Texture& texture = sprite.getTexture();
    // check if the texture is valid
    if (texture.getSize().x == 0 || texture.getSize().y == 0) {
        return;
    }

    // Get the texture's original size.
    Vector2u textureSize = texture.getSize();
    float originalWidth = static_cast<float>(textureSize.x);
    float originalHeight = static_cast<float>(textureSize.y);

    // Calculate the scale factors needed for each axis.
    float scaleX = desiredWidth / originalWidth;
    float scaleY = desiredHeight / originalHeight;

    // Apply the scaling to the sprite.
    sprite.setScale(Vector2f(scaleX, scaleY));
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
    textBoxText.setCharacterSize(static_cast<unsigned int>(window.getSize().y * 0.1f)); // Set size to 10% of window height
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

void slowPrintInTextBox(Clock& clock, const float delay, string& fullText, string& currentText, Text& textBoxText, RenderWindow& window)
{
    if (!fullText.empty() && clock.getElapsedTime().asSeconds() >= delay) 
    {
        currentText += fullText[0];  // Aggiungi la prossima lettera
        fullText.erase(0, 1);        // Rimuovi la lettera aggiunta
        textBoxText.setString(currentText);
        clock.restart();             // Resetta il timer
    }
}

// Funzione per centrare il testo in un rettangolo
void centerTextInRectangle(Text& text, const RectangleShape& rectangle) {
    FloatRect textBounds = text.getLocalBounds();
    //text.setOrigin(textBounds.left + textBounds.width / 2, textBounds.top + textBounds.height / 2);
    text.setPosition(Vector2f(rectangle.getPosition().x + rectangle.getSize().x / 2,
                            rectangle.getPosition().y + rectangle.getSize().y / 2));
}

bool checkForMouseClick(const RectangleShape& rectangle, const RenderWindow& window, const Vector2i& mousePos, bool &mouseReleased)
{
    if (rectangle.getGlobalBounds().contains(static_cast<Vector2f>(mousePos))) {
        if (mouseReleased) {
            mouseReleased = false; // Reset the mouseReleased flag
            return true;
        }
    }
    return false;
}

bool isMouseHovering(const RectangleShape& rect, const RenderWindow& window, const Vector2i& mousePosition) {
    FloatRect bounds = rect.getGlobalBounds();
    return bounds.contains(static_cast<Vector2f>(mousePosition));
}

Text selectCharacter(Text &characterNamesList, string &selection) 
{
    json characters;
    ifstream char_file("../include/characters.json");
    if (char_file.is_open()) 
    {
        char_file >> characters;
        char_file.close();
        string char_name;
        characterNamesList.setString("Available characters:\n");
        for (const auto &character : characters["characters"]) 
        {
            characterNamesList.setString(characterNamesList.getString() + string(character["name"]) + " (Level " + to_string(character["level"].get<int>()) + ")\n");
        }

    } else if(char_file.fail()) {
        throw runtime_error("Error loading characters.json file.");
    } else {
        selection = "YES";
        return characterNamesList; // Non ci sono personaggi, ritorna al menu principale
    } 
    return characterNamesList;
}

void printTutorialText(Clock &clock, Character &character, Text &textBoxText, RenderWindow &window, Font textBoxFont, RectangleShape textBox, string &selection, string &fullText, string &currentText, float &elapsedTime, int &tutorialTextStep, string &mainSelection) 
{

    Vector2i mousePosition = Mouse::getPosition(window);
    const auto cursorHand = Cursor::createFromSystem(Cursor::Type::Hand).value();
    const auto cursorArrow = Cursor::createFromSystem(Cursor::Type::Arrow).value();

    Text tutorialSelectionText(textBoxFont, "\nWould you like to skip the tutorial?\n", 40);
    tutorialSelectionText.setPosition(Vector2f(window.getSize().x /4, window.getSize().y /4));
    tutorialSelectionText.setFillColor(Color::Black);

    RectangleShape tutBoxSelectionYes(Vector2f(100, 50));
    tutBoxSelectionYes.setFillColor(Color(0, 0, 0, 0));
    tutBoxSelectionYes.setPosition(Vector2f(tutorialSelectionText.getPosition().x, tutorialSelectionText.getPosition().y + 100));
    
    RectangleShape tutBoxSelectionNo(Vector2f(66, 50));
    tutBoxSelectionNo.setFillColor(Color(0, 0, 0, 0));
    tutBoxSelectionNo.setPosition(Vector2f(tutorialSelectionText.getPosition().x + 200, tutorialSelectionText.getPosition().y + 100));

    Text tutorialSelectionTextYes(textBoxFont, "YES", 40);
    tutorialSelectionTextYes.setPosition(Vector2f(tutBoxSelectionYes.getPosition().x + 5, tutBoxSelectionYes.getPosition().y + 5));
    tutorialSelectionTextYes.setFillColor(Color::Black);

    Text tutorialSelectionTextNo(textBoxFont, "NO", 40);
    tutorialSelectionTextNo.setPosition(Vector2f(tutBoxSelectionNo.getPosition().x + 5, tutBoxSelectionNo.getPosition().y + 5));
    tutorialSelectionTextNo.setFillColor(Color::Black);

    const float delay = 0.005f;

    string text1 = "\nIt's a brisk morning, and the first rays of sunlight begin to warm "
        "the chilly air as you make your way to \nthe association. \nThe path is "
        "familiar, but today, every step feels heavier, charged with "
        "anticipation. After years of\nwaiting, you're finally here, standing "
        "at the threshold, 18 and ready to join.\nThe building stands tall and "
        "welcoming, with the association's emblem proudly displayed by the "
        "\nentrance. You take a deep breath and step inside, feeling a strange "
        "mix of nerves and excitement. \nThe reception area is bustling, with "
        "people chatting and moving about, each seemingly caught up in \ntheir"
        "own purpose. \nYou feel an odd sense of belonging, this is where you've "
        "always wanted to be, and today, it's happening.\nApproaching the "
        "front desk there is a red haired cute girl waiting, you hand over "
        "your ID with a subtle \ngrin, savoring the moment.\n\n Click to Continue";
    
    string text2 = "The receptionist smiles knowingly, having seen this scene many times before, and "
        "says: \n\"Happy birthday! Excited to finally join? My name's Rosie, "
        "and I'll be your guide through \nthe new chapter of your life!\nFollow "
        "me, we have to finish some formal paper works, then I'll be honored "
        "to let you know in depth \nyour job and how to do it well!\"\n\nA rush "
        "of pride washes over you as you nod, and she gestures toward a set of "
        "double doors at the end \nof the hall.\nYou walk through, and the room "
        "beyond has an almost ceremonial feel. You see walls lined with framed \n"
        "photos of previous members, a legacy of sorts, and you feel a "
        "connection to the history, as though your \nname, too, will someday "
        "join those ranks, becoming a DUNGEONS CLEARER!\nThe official "
        "enrollment process is straightforward but significant: signing your "
        "name in the registry,\n filling out some final paperwork, and "
        "confirming your dedication to the association's values. \nWhen you "
        "finish, Rosie hands you a membership badge with your name engraved on "
        "it, still warm from \nthe print. It feels real, solid; an achievement."
        "\n\n Click To Continue";
    
    string text3 = "Rosie:\n\"Well, welcome to the Association " + character.name +
        "!\nNow let me introduce you to the association, as you may know, your "
        "objective is to clear the dungeons \nthat are in the basement, do not "
        "worry I'll show you them as soon as we can. \nTo clear those dungeons "
        "you'll need to defeat the Dungeon's keeper, you may see it like a "
        "boss of the \ndungeon, they are very strong creatures able to destroy "
        "one's career in a matter of seconds.\nDo not worry, the other "
        "monsters of the dungeons are very easy to beat, \nas long as they not "
        "come in horde, because they are like insects, weak alone, strong in "
        "group.\n\nNow, you can easily understand that killing those monsters "
        "is not an easy task without the proper \nequipment. For this reason we "
        "of the association will give you 100 Coins to buy your first armor, "
        "\nweapon and maybe utility.\nSince we are in the capital Eran you'll be "
        "able to find many shops and stores of any kind, \nyou may choose your "
        "equipment according to your fighting style.\n\nOk now, please take a "
        "rest in our motel, The Golden Pidgeon, tomorrow I'll help you find "
        "\nthe shops, and after the dungeons.\""
        "\n\n Click To Sleep at the Golden Pidgeon and Continue";

    string text4 = "The soft morning light seeps through the small window of your room at "
        "The Golden Pigeon, casting a \ngentle glow over the simple but cozy "
        "space. \nStretching, you feel the excitement and nerves of the previous "
        "day slowly start to return. \nToday's the day you'll begin your first "
        "steps as a Dungeon Clearer.\n\nAfter freshening up, you make your way "
        "downstairs, greeted by the sounds and smells of a bustling \nmorning. "
        "\nYou spot Rosie by the entrance, waiting with a welcoming smile.\n\n"
    "Rosie:\n\"Good morning! Did you sleep well? It's a big day "
        "today,\" she says with a grin. \n\"Now, let me take you on a short "
        "tour around Eran, and then we'll head down \nto the association "
        "basement where the dungeons are located.\""
        "\n\n Click To Continue";
    
    string text5 = "You follow Rosie out of The Golden Pigeon and into the heart of "
        "Eran, the capital city. \nThe streets are bustling with people of "
        "all kinds: merchants calling out their goods, \nadventurers "
        "gathered in small groups, \nand children running around, eyes wide "
        "with wonder. \nRosie points out several important spots as you go, "
        "including the various guilds, \na few inns, and, most importantly, "
        "the local armory and supply shops.\n\n"
    "Rosie:\n\"As a Monster Hunter, you'll want to invest wisely in "
        "your equipment. This is where you'll find \neverything from "
        "weapons to potions and magical items. Just remember, your first "
        "purchase should \nmatch your style, whether it's something more "
        "defensive or offensive.\"\n\n"
        "Click To Continue";
    
    string text6 = "After the short tour, Rosie leads you back to the association "
        "building. Passing through the main hall, \nyou reach a set of thick, "
        "reinforced doors marked with an emblem. \nRosie pushes them open, "
        "revealing a dark staircase descending into the basement.\n\n"
        "Rosie:\n\"Here's where your journey begins,\" she says with a note of "
        "pride. \n\"The dungeons lie below, and each dungeon has its own unique "
        "challenges. \nThe first one isn't as tough as the others, but the "
        "Dungeon Keeper still poses a significant threat.\"\n\n"
        "You feel the weight of your responsibility settling on your "
        "shoulders. The basement air is cool and \nsmells faintly of stone and "
        "earth, as if centuries of battles and challenges have left their mark "
        "\non the atmosphere. You take a deep breath, preparing yourself.\n\n"
        "Rosie:\n\"But first, you'll need the right equipment. The association "
        "has provided you with 100 Coins, so head \nover to the shops and make "
        "sure you're well prepared. \nOnce you're ready, come back here, and "
        "I'll show you the way to the dungeon entrance.\"\n\n"
        "Click To Continue";

    if (selection == "")
    {
        window.clear();
        window.draw(textBox);
        window.draw(tutorialSelectionText);
        window.draw(tutBoxSelectionYes);
        window.draw(tutorialSelectionTextYes);
        window.draw(tutBoxSelectionNo);
        window.draw(tutorialSelectionTextNo);

        // Check for mouse hover over rectangle
        if (isMouseHovering(tutBoxSelectionYes, window, mousePosition))
            window.setMouseCursor(cursorHand);
        else if (isMouseHovering(tutBoxSelectionNo, window, mousePosition))
            window.setMouseCursor(cursorHand);
        else
            window.setMouseCursor(cursorArrow);

        // Check for mouse click rectangle
        if (checkForMouseClick(tutBoxSelectionYes, window, mousePosition, leftMouseReleased)){
            selection = "YES";
            window.setMouseCursor(cursorArrow);
        }else if (checkForMouseClick(tutBoxSelectionNo, window, mousePosition, leftMouseReleased)){
            selection = "NO";
            fullText = text1;
            currentText = "";
            elapsedTime = 0;  // Resetta il tempo
            clock.restart();
            window.setMouseCursor(cursorArrow);
        }
    } else if (selection == "NO") 
    {
        if (!character.hasItem("Association Badge"))
        {
            character.addItem({{"type", "badge"}, {"name", "Association Badge"}}, character);
        }
        window.clear();
        window.draw(textBox);

        elapsedTime += clock.restart().asSeconds();

        if (!fullText.empty() && elapsedTime >= delay) 
        {
            currentText += fullText[0];  // Aggiungi la prossima lettera
            fullText.erase(0, 1);        // Rimuovi la lettera aggiunta
            textBoxText.setString(currentText);
            elapsedTime = 0;             // Resetta il contatore
        }
        window.draw(textBoxText);
        if(fullText.empty() && checkForMouseClick(textBox, window, mousePosition, leftMouseReleased))
        {
            window.clear();
            window.draw(textBox);
            textBoxText.setString("");
            textBoxText.setPosition(Vector2f(textBox.getPosition().x + 10, textBox.getPosition().y + 10));
            textBoxText.setCharacterSize(24);
            currentText = "";
            switch(tutorialTextStep)
            {
                case 1:
                    fullText = text2;
                    tutorialTextStep ++;
                    break;
                case 2:
                    fullText = text3;
                    tutorialTextStep ++;
                    break;
                case 3:
                    fullText = text4;
                    tutorialTextStep ++;
                    break;
                case 4:
                    fullText = text5;
                    tutorialTextStep ++;
                    break;
                case 5:
                    fullText = text6;
                    tutorialTextStep ++;
                    break;
                case 6:
                    character.current_dungeon = -2;
                    character.write_character_to_json(character);
                    tutorialTextStep++;
                    mainSelection = "SHOP";
                    break;
                default:
                    break;
            }
                
        }
    } else if (character.current_dungeon == -5 && !character.hasItem("Association Badge")) {
        character.addItem({{"type", "utility"}, {"name", "Association Badge"}},
                        character);
    } else if (selection == "YES") 
    {
        character.current_dungeon = -2;
        character.write_character_to_json(character);
        mainSelection = "SHOP";
    }
    // da aggiungere il cambio di mainSelection a Shop, dopo che la stampa del tutorial è finita

    return;
}

vector<Option> createOptions(const vector<string>& optionTexts, Font& font, 
    const Vector2f& containerPos, const Vector2f& containerSize)
{
    vector<Option> options;

    // Define padding as 2% of the container height.
    float padding = containerSize.y * 0.02f; 
    // First option gets extra top padding.
    float firstOptionPadding = padding * 3; 

    // Compute the available vertical space (subtracting top padding and inter-option padding).
    float availableHeight = containerSize.y - firstOptionPadding - (padding * optionTexts.size());
    // Height if evenly divided among options.
    float computedOptionHeight = availableHeight / optionTexts.size();

    // Define a maximum option height: roughly container height/6 minus some padding.
    float maxOptionHeight = containerSize.y / 6.0f - padding;
    // Use the smaller of the computed height or the maximum allowed.
    float optionHeight = std::min(computedOptionHeight, maxOptionHeight);

    // Option width is nearly the full container width minus horizontal padding.
    float optionWidth = containerSize.x - 2 * padding;

    // Starting vertical position for the first option.
    float currentY = containerPos.y + firstOptionPadding;

    for (const auto& optionText : optionTexts) {
        // Create an Option with the computed dimensions.
        Option opt(font, optionWidth, optionHeight);
        opt.box.setSize({ optionWidth, optionHeight });
        // Position the option relative to the container's position plus horizontal padding.
        opt.box.setPosition({ containerPos.x + padding, currentY });

        // Set up text properties.
        opt.text.setFont(font);
        opt.text.setString(optionText);

        // Set initial character size to 50% of the option height.
        unsigned int charSize = static_cast<unsigned int>(optionHeight * 0.5f);
        opt.text.setCharacterSize(charSize);

        // Check text bounds and reduce character size if text width exceeds available width.
        FloatRect textBounds = opt.text.getLocalBounds();
        while (textBounds.size.x > optionWidth - 2 * padding && charSize > 0) {
            charSize--;
            opt.text.setCharacterSize(charSize);
            textBounds = opt.text.getLocalBounds();
        }

        // Position text within the option box:
        // add extra horizontal padding and vertically center the text.
        opt.text.setPosition({
            containerPos.x + 2 * padding,
            currentY + (optionHeight - textBounds.size.y) / 2 - textBounds.position.y
        });

        // Advance the current Y position for the next option.
        currentY += optionHeight + padding;
        options.push_back(opt);
    }

    return options;
}


// Funzione per gestire il click del mouse
string handleOptionMouseClick(RenderWindow& window, vector<Option>& options) {
    Vector2i mousePos = Mouse::getPosition(window);
    string x = "";
    for (auto& opt : options) {
        if (checkForMouseClick(opt.box, window, mousePos, leftMouseReleased)) {
            x = opt.text.getString().toAnsiString();
            cout << "Opzione selezionata: " << x << endl;
            return x;
        }
    }
    return "";
}

void drawOptions(RenderWindow& window, std::vector<Option>& options) {
    for (auto& option : options) {
        option.draw(window);
    }
}

void shopSelected(string &filename, Character &character, RenderWindow &window, Font textBoxFont, 
                RectangleShape &background, Text &mainBoxText, 
                RectangleShape &mainBox, RectangleShape &lowerBox, RectangleShape &upperBox, 
                RectangleShape &upperTitleBox, Text &upperTitleBoxText, Text &upperBoxText, 
                vector<Option> &options, Texture &backroundShopTexture)
{
    // These static variables persist across frames for shop mode handling.
    static string shopMode = "";           // "" means initial menu; "buy" or "sell" when in that mode.
    static vector<Option> itemOptions;       // Options for items (buying or selling).
    static vector<json> currentItems;        // Items to be displayed in the current mode.
    
    // Draw the common shop UI.
    background.setTexture(&backroundShopTexture);
    window.clear();
    window.draw(background);
    window.draw(mainBox);
    window.draw(mainBoxText);
    window.draw(lowerBox);
    window.draw(upperBox);
    window.draw(upperBoxText);
    window.draw(upperTitleBox);
    window.draw(upperTitleBoxText);

    if (shopMode == "") {
        // In the initial state, show the Buy/Sell/Go Back options.
        drawOptions(window, options);
        if (leftMouseReleased) {
            string option = handleOptionMouseClick(window, options);
            if (option == "Buy") {
                shopMode = "buy";
                mainBoxText.setString("Select an item to buy!\nCoins: " + to_string(character.coins));
            } else if (option == "Sell") {
                shopMode = "sell";
                mainBoxText.setString("Select an item to sell!\nCoins: " + to_string(character.coins));
            } else if (option == "Go Back") {
                filename = "";
                shopMode = "";
                return;
            }
            leftMouseReleased = false;
        }
    }
    else if (shopMode == "buy") {
        // Load items available for buying.
        // Load items available for buying.
        try {
            currentItems = loadShopItems(filename, character.level);
        } catch (const std::exception& e) {
            cerr << "Error loading shop items from file: " << filename << ". Exception: " << e.what() << endl;
            mainBoxText.setString("Failed to load shop items. Please try again later.");
            return;
        }
        vector<string> itemOptionStrings;
        for (const auto &item : currentItems) {
            string optionText = string(item["name"]) + " - " + to_string(item["value"].get<int>()) + " coins" + "\nIn possession: " + to_string(character.getItemCount(string(item["name"])));
            itemOptionStrings.push_back(optionText);
        }
        // Append a "Back" option.
        itemOptionStrings.push_back("Back");
        // Create options for items using the lowerBox as the container.
        itemOptions = createOptions(itemOptionStrings, textBoxFont, lowerBox.getPosition(), lowerBox.getSize());
        drawOptions(window, itemOptions);
        
        if (leftMouseReleased) {
            string selected = handleOptionMouseClick(window, itemOptions);
            if (selected == "Back") {
                shopMode = ""; // Return to Buy/Sell selection.
            } else {
                // Find the index of the selected item.
                int index = -1;
                for (int i = 0; i < static_cast<int>(itemOptionStrings.size()); ++i) {
                    if (itemOptionStrings[i] == selected) {
                        index = i;
                        break;
                    }
                }
                if (index != -1 && index < static_cast<int>(currentItems.size())) {
                    json selectedItem = currentItems[index];
                    int itemCost = selectedItem["value"].get<int>();
                    if (character.coins >= itemCost) {
                        character.coins -= itemCost;
                        character.addItem(selectedItem, character);
                        mainBoxText.setString("Purchased " + string(selectedItem["name"]) + " for " + to_string(itemCost) + " coins!\nCoins left: " + to_string(character.coins));
                        character.write_character_to_json(character);
                    } else {
                        mainBoxText.setString("Not enough coins to buy " + string(selectedItem["name"]) + "!\nCoins left: " + to_string(character.coins));
                    }
                }
            }
            leftMouseReleased = false;
        }
    }
    else if (shopMode == "sell") {
        // Get items available for selling (matching the shop type).
        string selection = filename.substr(11, filename.length() - 17); // Extract the shop type from the filename. (eg: "../include/armors.json" -> "armor")
        currentItems = character.findItemsType(selection);
        vector<string> itemOptionStrings;
        for (const auto &item : currentItems) {
            int itemValue = item["value"].get<int>();
            int sellingPrice = static_cast<int>(itemValue * 0.75);
            string optionText = string(item["name"]) + " - " + to_string(sellingPrice) + " coins" + "\nIn possession: " + to_string(character.getItemCount(string(item["name"])));
            itemOptionStrings.push_back(optionText);
        }
        // Append a "Back" option.
        itemOptionStrings.push_back("Back");
        itemOptions = createOptions(itemOptionStrings, textBoxFont, lowerBox.getPosition(), lowerBox.getSize());
        drawOptions(window, itemOptions);

        if (leftMouseReleased) {
            string selected = handleOptionMouseClick(window, itemOptions);
            if (selected == "Back") {
                shopMode = ""; // Return to the initial Buy/Sell menu.
            } else {
                int index = -1;
                for (int i = 0; i < static_cast<int>(itemOptionStrings.size()); ++i) {
                    if (itemOptionStrings[i] == selected) {
                        index = i;
                        break;
                    }
                }
                if (index != -1 && index < static_cast<int>(currentItems.size())) {
                    json selectedItem = currentItems[index];
                    int itemValue = selectedItem["value"].get<int>();
                    int sellingPrice = static_cast<int>(itemValue * 0.75);
                    character.coins += sellingPrice;
                    if(character.getItemCount(string(selectedItem["name"])) == 1) {
                        character.removeItem(string(selectedItem["name"]), character);
                    } else {
                        character.addItem(selectedItem, character, -1); // Remove one instance of the item.
                    }
                    mainBoxText.setString("Sold " + string(selectedItem["name"]) + " for " + to_string(sellingPrice) + " coins!" + "\nCoins now: " + to_string(character.coins));
                    character.write_character_to_json(character);
                }
            }
            leftMouseReleased = false;
        }
    }
    
    // (Note: Do not call window.display() here since your main loop calls it after shopSelected.)
}


void shops(Character &character, RenderWindow &window, Font textBoxFont, 
            RectangleShape &background, Texture &backgroundTexture,
            RectangleShape &upperBox, Text &upperBoxText, RectangleShape &upperTitleBox, Text &upperTitleBoxText,
            RectangleShape &lowerBox, RectangleShape &mainBox, Text &mainBoxText, string &filename,
            int &shopChoice, Texture &backroundShopTexture)
{
    window.clear();
    character.current_dungeon = -2;
    character.write_character_to_json(character);
    Texture armorTexture, weaponTexture, potionTexture, foodTexture, usableTexture, utilityTexture, mainTexture;  

    vector<string> shopOptions = {"DragonForge Armory", "The Weapons of Valoria", "The Alchemist's Kiss",
                                    "Feast & Famine", "Relics & Rarities", "The Rusty Nail", "Exit the Shop"};
    vector<Option> options = createOptions(shopOptions, textBoxFont, lowerBox.getPosition(), lowerBox.getSize());
    vector<string> YN_Options = {"Buy", "Sell", "Go Back"};
    vector<Option> YesNoOptions = createOptions(YN_Options, textBoxFont, lowerBox.getPosition(), lowerBox.getSize());

    if (shopChoice != 0 && filename != ""){
        shopSelected(filename, character, window, textBoxFont, background, mainBoxText, mainBox, lowerBox, upperBox, upperTitleBox, upperTitleBoxText, upperBoxText, YesNoOptions, backroundShopTexture);
        return;
    } else {
        shopChoice = 0;
        background.setTexture(&backgroundTexture);
        mainBoxText.setString("Welcome to the shops area! Choose a shop to visit:\n");
        window.draw(background);
        window.draw(mainBox);
        window.draw(mainBoxText);
        window.draw(lowerBox);
        window.draw(upperBox);
        window.draw(upperBoxText);
        window.draw(upperTitleBox);
        window.draw(upperTitleBoxText);
        drawOptions(window, options);
    } 
    if (leftMouseReleased) {
        if(find(shopOptions.begin(), shopOptions.end(), handleOptionMouseClick(window, options)) != shopOptions.end()) {
            if (shopChoice == 0) {
                leftMouseReleased = true;
                string selectedOption = handleOptionMouseClick(window, options);
                if (selectedOption == "DragonForge Armory") {
                    filename = "../include/armors.json";
                    shopChoice = 1;
                    if (!armorTexture.loadFromFile("../assets/Textures/Backgrounds/Valoria/armory.jpg")){
                        return;
                    }
                    backroundShopTexture = armorTexture;
                    mainBoxText.setString("Welcome to Dragon Forge\nWould you like to Buy or Sell?\n");
                } else if (selectedOption == "The Weapons of Valoria") {
                    filename = "../include/weapons.json";
                    shopChoice = 2;
                    if (!weaponTexture.loadFromFile("../assets/Textures/Backgrounds/Valoria/weaponary.jpg")){
                        return;
                    }
                    backroundShopTexture = weaponTexture;
                    mainBoxText.setString("Welcome to The Weapons of Valoria\nWould you like to Buy or Sell?\n");
                } else if (selectedOption == "The Alchemist's Kiss") {
                    filename = "../include/potions.json";
                    shopChoice = 3;
                    if (!potionTexture.loadFromFile("../assets/Textures/Backgrounds/Valoria/alchemist.jpg")){
                        return;
                    }
                    backroundShopTexture = potionTexture;
                    mainBoxText.setString("Welcome to The Alchemist's Kiss\nWould you like to Buy or Sell?\n");
                } else if (selectedOption == "Feast & Famine") {
                    filename = "../include/foods.json";
                    shopChoice = 4;
                    if (!foodTexture.loadFromFile("../assets/Textures/Backgrounds/Valoria/tavern.jpg")){
                        return;
                    }
                    backroundShopTexture = foodTexture;
                    mainBoxText.setString("Welcome to Feast & Famine\nWould you like to Buy or Sell?\n");
                } else if (selectedOption == "Relics & Rarities") {
                    filename = "../include/usables.json";
                    shopChoice = 5;
                    if (!usableTexture.loadFromFile("../assets/Textures/Backgrounds/Valoria/magic.jpg")){
                        return;
                    }
                    backroundShopTexture = usableTexture;
                    mainBoxText.setString("Welcome to Relics & Rarities\nWould you like to Buy or Sell?\n");
                } else if (selectedOption == "The Rusty Nail") {
                    filename = "../include/utilities.json";
                    shopChoice = 6;
                    if (!utilityTexture.loadFromFile("../assets/Textures/Backgrounds/Valoria/utilities.jpg")){
                        return;
                    }
                    backroundShopTexture = utilityTexture;
                    mainBoxText.setString("Welcome to The Rusty Nail\nWould you like to Buy or Sell?\n");
                } else if (selectedOption == "Exit the Shop") {
                    shopChoice = 0;
                    character.current_dungeon = 0;
                    return;
                }
            } else {
                if (shopChoice == 1) {
                    filename = "../include/armors.json";
                    shopChoice = 1;
                    if (!armorTexture.loadFromFile("../assets/Textures/Backgrounds/Valoria/armory.jpg")) {
                        return;
                    }
                    backroundShopTexture = armorTexture;
                    mainBoxText.setString("Welcome to Dragon Forge\nWould you like to Buy or Sell?\n");
                } else if (shopChoice == 2) {
                    filename = "../include/weapons.json";
                    shopChoice = 2;
                    if (!weaponTexture.loadFromFile("../assets/Textures/Backgrounds/Valoria/weaponary.jpg")) {
                        return;
                    }
                    backroundShopTexture = weaponTexture;
                    mainBoxText.setString("Welcome to The Weapons of Valoria\nWould you like to Buy or Sell?\n");
                } else if (shopChoice == 3) {
                    filename = "../include/potions.json";
                    shopChoice = 3;
                    if (!potionTexture.loadFromFile("../assets/Textures/Backgrounds/Valoria/alchemist.jpg")) {
                        return;
                    }
                    backroundShopTexture = potionTexture;
                    mainBoxText.setString("Welcome to The Alchemist's Kiss\nWould you like to Buy or Sell?\n");
                } else if (shopChoice == 4) {
                    filename = "../include/foods.json";
                    shopChoice = 4;
                    if (!foodTexture.loadFromFile("../assets/Textures/Backgrounds/Valoria/tavern.jpg")) {
                        return;
                    }
                    backroundShopTexture = foodTexture;
                    mainBoxText.setString("Welcome to Feast & Famine\nWould you like to Buy or Sell?\n");
                } else if (shopChoice == 5) {
                    filename = "../include/usables.json";
                    shopChoice = 5;
                    if (!usableTexture.loadFromFile("../assets/Textures/Backgrounds/Valoria/magic.jpg")) {
                        return;
                    }
                    backroundShopTexture = usableTexture;
                    mainBoxText.setString("Welcome to Relics & Rarities\nWould you like to Buy or Sell?\n");
                } else if (shopChoice == 6) {
                    filename = "../include/utilities.json";
                    shopChoice = 6;
                    if (!utilityTexture.loadFromFile("../assets/Textures/Backgrounds/Valoria/utilities.jpg")) {
                        return;
                    }
                    backroundShopTexture = utilityTexture;
                    mainBoxText.setString("Welcome to The Rusty Nail\nWould you like to Buy or Sell?\n");
                }
            }
            character.write_character_to_json(character);
        }
        return;
    }
}

void mainMenu (Character &character, RenderWindow &window, Font textBoxFont, 
    RectangleShape &background, Texture &backgroundTexture,
    RectangleShape &upperBox, Text &upperBoxText, RectangleShape &upperTitleBox, Text &upperTitleBoxText,
    RectangleShape &lowerBox, RectangleShape &mainBox, Text &mainBoxText)
{
    window.clear();
    character.current_dungeon = 0;
    character.write_character_to_json(character);

    Texture mainTexture;
    if (!mainTexture.loadFromFile("../assets/Textures/Backgrounds/Valoria/CapitalLobby.jpg")){
        cerr << "Error loading main texture." << endl;
        return;
    }
    backgroundTexture = mainTexture;
    background.setTexture(&backgroundTexture);
    mainBoxText.setString("You are now in the main square of Valoria! Here you can take many streets to go wherever you go!\nChoose an option from the ones on your right -->\n");

    vector<string> mainOptions = {"Go to the Shops", "Go to the Monster-Hunters Association", "Exit the Game"};
    vector<Option> options = createOptions(mainOptions, textBoxFont, lowerBox.getPosition(), lowerBox.getSize());


    window.draw(background);
    window.draw(mainBox);
    window.draw(mainBoxText);
    window.draw(lowerBox);
    window.draw(upperBox);
    window.draw(upperBoxText);
    window.draw(upperTitleBox);
    window.draw(upperTitleBoxText);
    drawOptions(window, options);

    if(leftMouseReleased){
        if(find(mainOptions.begin(), mainOptions.end(), handleOptionMouseClick(window, options)) != mainOptions.end()){
            leftMouseReleased = true;
            string selectedOption = handleOptionMouseClick(window, options);
            if (selectedOption == "Go to the Shops") {
                character.current_dungeon = -2;
                return;
            } else if (selectedOption == "Go to the Monster-Hunters Association") {
                character.current_dungeon = -3;
                return;
            } else if (selectedOption == "Exit the Game") {
                character.write_character_to_json(character);
                window.close();
                return;
            }
        }
    }
}

void showLeaderboard (vector<int> &leadChoice, json &leaderboards_data, RenderWindow &window, Font textBoxFont, 
    RectangleShape &background, Texture &backgroundTexture,
    RectangleShape &upperBox, Text &upperBoxText, RectangleShape &upperTitleBox, Text &upperTitleBoxText,
    RectangleShape &lowerBox, RectangleShape &mainBox, Text &mainBoxText) {
    window.clear();
    Texture leaderboardTexture;
    if (!leaderboardTexture.loadFromFile("../assets/Textures/Backgrounds/Valoria/inside-hunter.jpg")){
        cerr << "Error loading leaderboard texture." << endl;
        return;
    }
    backgroundTexture = leaderboardTexture;
    background.setTexture(&backgroundTexture);

    // Display leaderboard title
    mainBoxText.setString("Here are the Leaderboards you asked for!\nExplore the best characters based on your selection -->\n");
    window.draw(background);
    window.draw(mainBox);
    window.draw(mainBoxText);

    // Extract leaderboard data based on leadChoice
    string difficulty = (leadChoice[0] == 1) ? "Easy" : (leadChoice[0] == 2) ? "Normal" : (leadChoice[0] == 3) ? "Hard" : "Extreme";
    string category = (leadChoice[1] == 1) ? "total_game" : "dungeons";
    string metric = (category == "total_game") ? ((leadChoice[2] == 1) ? "turns" : (leadChoice[2] == 2) ? "coins_spent" : (leadChoice[2] == 3) ? "kills_death_ratio" : (leadChoice[2] == 4) ? "total_kills" : (leadChoice[2] == 5) ? "total_coins_acquired" : "level") : (leadChoice[3] == 1) ? "turns_to_complete" : "kills_death_ratio";
    if (leadChoice[1] == 2) {
        string dungeonID = to_string(leadChoice[2]);
    }

    // Create the button to go back to the LeaderboardMenu

    // Display leaderboard data
}

void leaderboardsMenu (vector<int> &leadChoice, json &leaderboards_data, RenderWindow &window, Font textBoxFont, 
RectangleShape &background, Texture &backgroundTexture,
RectangleShape &upperBox, Text &upperBoxText, RectangleShape &upperTitleBox, Text &upperTitleBoxText,
RectangleShape &lowerBox, RectangleShape &mainBox, Text &mainBoxText) {
    // Possibilities of leadChoice: [2, 1, 1, 0] -> [Normal, Total Game, Turns, nothing]
    //                              [3, 2, 10, 2] -> [Hard, Single Dungeon, Dungeon 10, K/D Ratio]
    //                              [1/2/3/4, 1/2, *1*/*2*/3/4/5/6/7/8/9/10, 0/1/2]

    window.clear();
    Texture leaderboardTexture;
    if (!leaderboardTexture.loadFromFile("../assets/Textures/Backgrounds/Valoria/inside-hunter.jpg")){
        cerr << "Error loading leaderboard texture." << endl;
        return;
    }
    backgroundTexture = leaderboardTexture;
    background.setTexture(&backgroundTexture);
    mainBoxText.setString("Here are the Leaderboards!\nHere you can see the best players of the game!\nChoose an option from the ones on your right -->\n");

    vector<string> leaderboardOptions = {"Easy", "Normal", "Hard", "Extreme", "Back"};
    vector<string> leaderboardOptions1 = {"Total Game", "Single Dungeon", "Back"};
    vector<string> leaderboardOptions2 = {"Turns", "Coins Spent", "K/D Ratio", "Total Kills", "Total Coins acquired", "Level", "Back"};
    vector<string> leaderboardOptions2b = {"Dungeon 1", "Dungeon 2", "Dungeon 3", "Dungeon 4", "Dungeon 5", "Dungeon 6", "Dungeon 7", "Dungeon 8", "Dungeon 9", "Dungeon 10", "Back"};
    vector<string> leaderboardOptions3b = {"Turns to Complete", "K/D Ratio", "Back"};

    vector<Option> options = createOptions(leaderboardOptions, textBoxFont, lowerBox.getPosition(), lowerBox.getSize());
    vector<Option> options1 = createOptions(leaderboardOptions1, textBoxFont, lowerBox.getPosition(), lowerBox.getSize());
    vector<Option> options2 = createOptions(leaderboardOptions2, textBoxFont, lowerBox.getPosition(), lowerBox.getSize());
    vector<Option> options2b = createOptions(leaderboardOptions2b, textBoxFont, lowerBox.getPosition(), lowerBox.getSize());
    vector<Option> options3b = createOptions(leaderboardOptions3b, textBoxFont, lowerBox.getPosition(), lowerBox.getSize());

    if (leftMouseReleased){
        if (leaderboardIn) {
            if (leadChoice[0] == 0){
                if (find(leaderboardOptions.begin(), leaderboardOptions.end(), handleOptionMouseClick(window, options)) != leaderboardOptions.end()) {
                    leftMouseReleased = true;
                    string selectedOption = handleOptionMouseClick(window, options1);
                    if (selectedOption == "Back") {
                        leaderboardIn = false;
                        return;
                    } else if (selectedOption == "Easy") {
                        leadChoice.push_back(1);
                    } else if (selectedOption == "Normal") {
                        leadChoice.push_back(2);
                    } else if (selectedOption == "Hard") {
                        leadChoice.push_back(3);
                    } else if (selectedOption == "Extreme") {
                        leadChoice.push_back(4);
                    }
                }
            } else if (find(leaderboardOptions1.begin(), leaderboardOptions1.end(), handleOptionMouseClick(window, options1)) != leaderboardOptions1.end()){
                leftMouseReleased = true;
                string selectedOption = handleOptionMouseClick(window, options1);
                if (selectedOption == "Back") {
                    leadChoice.pop_back();
                    return;
                } else if (selectedOption == "Total Game") {
                    leadChoice.push_back(1);
                } else if (selectedOption == "Single Dungeon") {
                    leadChoice.push_back(2);
                }
            } else if (find(leaderboardOptions2.begin(), leaderboardOptions2.end(), handleOptionMouseClick(window, options2)) != leaderboardOptions2.end()){
                leftMouseReleased = true;
                string selectedOption = handleOptionMouseClick(window, options1);
                if (selectedOption == "Back") {
                    leadChoice.pop_back();
                    return;
                } else if (selectedOption == "Turns") {
                    leadChoice.push_back(1);
                } else if (selectedOption == "Coins Spent") {
                    leadChoice.push_back(2);
                } else if (selectedOption == "K/D Ratio") {
                    leadChoice.push_back(3);
                } else if (selectedOption == "Total Kills") {
                    leadChoice.push_back(4);
                } else if (selectedOption == "Total Coins acquired") {
                    leadChoice.push_back(5);
                } else if (selectedOption == "Level") {
                    leadChoice.push_back(6);
                }
            } else if (find(leaderboardOptions2b.begin(), leaderboardOptions2b.end(), handleOptionMouseClick(window, options2b)) != leaderboardOptions2b.end()){
                leftMouseReleased = true;
                string selectedOption = handleOptionMouseClick(window, options1);
                if (selectedOption == "Back") {
                    leadChoice.pop_back();
                    return;
                } else if (selectedOption == "Dungeon 1") {
                    leadChoice.push_back(1);
                } else if (selectedOption == "Dungeon 2") {
                    leadChoice.push_back(2);
                } else if (selectedOption == "Dungeon 3") {
                    leadChoice.push_back(3);
                } else if (selectedOption == "Dungeon 4") {
                    leadChoice.push_back(4);
                } else if (selectedOption == "Dungeon 5") {
                    leadChoice.push_back(5);
                } else if (selectedOption == "Dungeon 6") {
                    leadChoice.push_back(6);
                } else if (selectedOption == "Dungeon 7") {
                    leadChoice.push_back(7);
                } else if (selectedOption == "Dungeon 8") {
                    leadChoice.push_back(8);
                } else if (selectedOption == "Dungeon 9") {
                    leadChoice.push_back(9);
                } else if (selectedOption == "Dungeon 10") {
                    leadChoice.push_back(10);
                }
            } else if (find(leaderboardOptions3b.begin(), leaderboardOptions3b.end(), handleOptionMouseClick(window, options3b)) != leaderboardOptions3b.end()){
                leftMouseReleased = true;
                string selectedOption = handleOptionMouseClick(window, options1);
                if (selectedOption == "Back") {
                    leadChoice.pop_back();
                    return;
                } else if (selectedOption == "Turns to Complete") {
                    leadChoice.push_back(1);
                } else if (selectedOption == "K/D Ratio") {
                    leadChoice.push_back(2);
                }
            }
        }
    }

    if ((leadChoice[1] == 1 && leadChoice[2] != 0) || (leadChoice[2] != 0 && leadChoice[3] != 0)){
        showLeaderboard(leadChoice, leaderboards_data, window, textBoxFont, background, backgroundTexture, upperBox, upperBoxText, upperTitleBox, upperTitleBoxText, lowerBox, mainBox, mainBoxText);
        return;
    }

    window.draw(background);
    window.draw(mainBox);
    window.draw(mainBoxText);
    window.draw(lowerBox);
    window.draw(upperBox);
    window.draw(upperBoxText);
    window.draw(upperTitleBox);
    window.draw(upperTitleBoxText);
    if (leadChoice[0] == 0) {
        drawOptions(window, options);
    } else if (leadChoice[1] == 0) {
        drawOptions(window, options1);
    } else if (leadChoice[1] == 1 && leadChoice[2] == 0) {
        drawOptions(window, options2);
    } else if (leadChoice[2] == 0) {
        drawOptions(window, options2b);
    } else if (leadChoice[1] == 2){
        drawOptions(window, options3b);
    }
}

void mhaMenu (Character &character, RenderWindow &window, Font textBoxFont, 
    RectangleShape &background, Texture &backgroundTexture,
    RectangleShape &upperBox, Text &upperBoxText, RectangleShape &upperTitleBox, Text &upperTitleBoxText,
    RectangleShape &lowerBox, RectangleShape &mainBox, Text &mainBoxText)
{
    window.clear();
    character.current_dungeon = -3;
    character.write_character_to_json(character);

    Texture mainTexture;
    if (!mainTexture.loadFromFile("../assets/Textures/Backgrounds/Valoria/out-hunter.jpg")){
        cerr << "Error loading main texture." << endl;
        return;
    }
    backgroundTexture = mainTexture;
    background.setTexture(&backgroundTexture);
    mainBoxText.setString("You are now in the Monster-Hunters Association! Here you can find many things to do!\nChoose an option from the ones on your right -->\n");

    vector<string> mainOptions = {"Go to the Dungeons", "Check the Leaderboards", "Go to the Pub", "Exit the Association"};
    vector<Option> options = createOptions(mainOptions, textBoxFont, lowerBox.getPosition(), lowerBox.getSize());

    window.draw(background);
    window.draw(mainBox);
    window.draw(mainBoxText);
    window.draw(lowerBox);
    window.draw(upperBox);
    window.draw(upperBoxText);
    window.draw(upperTitleBox);
    window.draw(upperTitleBoxText);
    drawOptions(window, options);

    if(leftMouseReleased){
        if(find(mainOptions.begin(), mainOptions.end(), handleOptionMouseClick(window, options)) != mainOptions.end()){
            leftMouseReleased = true;
            string selectedOption = handleOptionMouseClick(window, options);
            if (selectedOption == "Go to the Dungeons") {
                character.current_dungeon = -6;
                return;
            } else if (selectedOption == "Check the Leaderboards") {
                leaderboards_data = load_leaderboards_data("../include/ideal_leads.json");
                leaderboardIn = true;
                return;
            } else if (selectedOption == "Go to the Pub") {
                character.current_dungeon = -4;
                return;
            } else if (selectedOption == "Exit the Association") {
                mainMenu(character, window, textBoxFont, background, backgroundTexture, upperBox, upperBoxText, upperTitleBox, upperTitleBoxText, lowerBox, mainBox, mainBoxText);
                return;
            }
        }
    }
}

void pubMenu (Character &character, RenderWindow &window, Font textBoxFont, 
    RectangleShape &background, Texture &backgroundTexture,
    RectangleShape &upperBox, Text &upperBoxText, RectangleShape &upperTitleBox, Text &upperTitleBoxText,
    RectangleShape &lowerBox, RectangleShape &mainBox, Text &mainBoxText, int &youWantToBeDrunk, int &timesPassedOut)
{
    window.clear();
    //character.current_dungeon = 0;
    character.write_character_to_json(character);

    Texture mainTexture;
    if (!mainTexture.loadFromFile("../assets/Textures/Backgrounds/Valoria/alcohol.jpg")){
        cerr << "Error loading main texture." << endl;
        return;
    }
    backgroundTexture = mainTexture;
    background.setTexture(&backgroundTexture);
    if (youWantToBeDrunk == 0 && timesPassedOut == 0){
        mainBoxText.setString("You are now in the Pub. What would you like to do?\nChoose an option from the ones on your right -->\n");
    }

    vector<string> mainOptions = {"Buy ale (2 coins)", "Buy beer (4 coins)", "Buy whisky (20 coins)", "Back to the Association hall"};
    vector<Option> options = createOptions(mainOptions, textBoxFont, lowerBox.getPosition(), lowerBox.getSize());

    window.draw(background);
    window.draw(mainBox);
    window.draw(mainBoxText);
    window.draw(lowerBox);
    window.draw(upperBox);
    window.draw(upperBoxText);
    window.draw(upperTitleBox);
    window.draw(upperTitleBoxText);
    drawOptions(window, options);

    if(leftMouseReleased){
        if(find(mainOptions.begin(), mainOptions.end(), handleOptionMouseClick(window, options)) != mainOptions.end()){
            leftMouseReleased = true;
            string selectedOption = handleOptionMouseClick(window, options);
            if (selectedOption == "Buy ale (2 coins)") 
            {
                if (character.coins >= 2) 
                {
                    character.coins -= 2;
                    mainBoxText.setString("You drink ale!\nCoins left: " + to_string(character.coins));
                    youWantToBeDrunk +=2;
                    if (youWantToBeDrunk >= 20)
                    {
                        mainBoxText.setString("You drunk too much alcohol and passed out, you lost 5HP\n");
                        character.health -= 5;
                        timesPassedOut += 1;
                        youWantToBeDrunk = 0;
                        if (timesPassedOut >= 3)
                        {
                            mainBoxText.setString("You really love alcohol, maybe too much, everything goes dark and you die\n\n");
                            timesPassedOut = 0;
                            character.health = 0;
                        }
                    }
                    character.write_character_to_json(character);
                } else 
                {
                    mainBoxText.setString("Not enough coins to buy ale!\nCoins left: " + to_string(character.coins));
                }
                character.write_character_to_json(character);
                return;
            } else if (selectedOption == "Buy beer (4 coins)") 
            {
                if (character.coins >= 4) 
                {
                    character.coins -= 4;
                    mainBoxText.setString("You drink beer!\nCoins left: " + to_string(character.coins));
                    youWantToBeDrunk +=4;
                    if (youWantToBeDrunk >= 20)
                    {
                        mainBoxText.setString("You drunk too much alcohol and passed out, you lost 5HP\n");
                        character.health -= 5;
                        timesPassedOut += 1;
                        youWantToBeDrunk = 0;
                        if (timesPassedOut >= 3)
                        {
                            mainBoxText.setString("You really love alcohol, maybe too much, everything goes dark and you die\n\n");
                            timesPassedOut = 0;
                            character.health = 0;
                        }
                    }
                    character.write_character_to_json(character);
                } else 
                {
                    mainBoxText.setString("Not enough coins to buy beer!\nCoins left: " + to_string(character.coins));
                }
                character.write_character_to_json(character);
                return;
            } else if (selectedOption == "Buy whisky (20 coins)") 
            {
                if (character.coins >= 20) 
                {
                    character.coins -= 20;
                    mainBoxText.setString("You drink whisky!\nCoins left: " + to_string(character.coins));
                    youWantToBeDrunk +=20;
                    if (youWantToBeDrunk >= 20)
                    {
                        mainBoxText.setString("You drunk too much alcohol and passed out, you lost 5HP\n");
                        character.health -= 5;
                        timesPassedOut += 1;
                        youWantToBeDrunk = 0;
                        if (timesPassedOut >= 3)
                        {
                            mainBoxText.setString("You really love alcohol, maybe too much, everything goes dark and you die\n\n");
                            timesPassedOut = 0;
                            character.health = 0;
                        }
                    }
                    character.write_character_to_json(character);
                } else 
                {
                    mainBoxText.setString("Not enough coins to buy whisky!\nCoins left: " + to_string(character.coins));
                }
                character.write_character_to_json(character);
                return;
            }else if (selectedOption == "Back to the Association hall") 
            {
                character.current_dungeon = -3;
                return;
            }
        }
    }

}

void respawner (Character &character, RenderWindow &window, Font textBoxFont)
{
    Vector2i mousePos = Mouse::getPosition(window);
    Option respawnOption(textBoxFont, window.getSize().x/9, window.getSize().x/16);
    respawnOption.text.setString("Respawn");
    respawnOption.setOptionPosition(window.getSize().x - window.getSize().x/9, window.getSize().y - window.getSize().x/16);

    if (leftMouseReleased)
    {
        if (checkForMouseClick(respawnOption.box, window, mousePos, leftMouseReleased))
        {
            // Diverse cose in relazione alla difficoltà
            character.current_dungeon = 0;
            character.health = 10;
            character.write_character_to_json(character);
            return;
        }
    }
    window.draw(respawnOption.text);
}

void dungeonMenu (Character &character, RenderWindow &window, Font textBoxFont, 
    RectangleShape &background, Texture &backgroundTexture,
    RectangleShape &upperBox, Text &upperBoxText, RectangleShape &upperTitleBox, Text &upperTitleBoxText,
    RectangleShape &lowerBox, RectangleShape &mainBox, Text &mainBoxText)
{
    window.clear();
    character.current_dungeon = -6;
    character.write_character_to_json(character);

    Texture mainTexture;
    if (!mainTexture.loadFromFile("../assets/Textures/Backgrounds/Dungeons/hall.jpg")){
        cerr << "Error loading main texture." << endl;
        return;
    }
    backgroundTexture = mainTexture;
    background.setTexture(&backgroundTexture);
    mainBoxText.setString("You are now in the Dungeon Hall! Here you can select which dungeon you want to enter!\nChoose an option from the ones on your right -->\n");

    vector<string> mainOptions = {"Dungeon 1", "Dungeon 2", "Dungeon 3", "Dungeon 4", "Dungeon 5", "Dungeon 6", "Dungeon 7", "Dungeon 8", "Dungeon 9", "Dungeon 10", "Back to the Association"};
    vector<Option> options = createOptions(mainOptions, textBoxFont, lowerBox.getPosition(), lowerBox.getSize());

    window.draw(background);
    window.draw(mainBox);
    window.draw(mainBoxText);
    window.draw(lowerBox);
    window.draw(upperBox);
    window.draw(upperBoxText);
    window.draw(upperTitleBox);
    window.draw(upperTitleBoxText);
    drawOptions(window, options);

    if(leftMouseReleased){
        if(find(mainOptions.begin(), mainOptions.end(), handleOptionMouseClick(window, options)) != mainOptions.end()){
            leftMouseReleased = true;
            string selectedOption = handleOptionMouseClick(window, options);
            if (selectedOption == "Dungeon 1") {
                character.current_dungeon = 1;
                return;
            } else if (selectedOption == "Dungeon 2") {
                character.current_dungeon = 2;
                return;
            } else if (selectedOption == "Dungeon 3") {
                character.current_dungeon = 3;
                return;
            }else if (selectedOption == "Dungeon 4") {
                character.current_dungeon = 4;
                return;
            }else if (selectedOption == "Dungeon 5") {
                character.current_dungeon = 5;
                return;
            }
            else if (selectedOption == "Dungeon 6") {
                character.current_dungeon = 6;
                return;
            } else if (selectedOption == "Dungeon 7") {
                character.current_dungeon = 7;
                return;
            } else if (selectedOption == "Dungeon 8") {
                character.current_dungeon = 8;
                return;
            } else if (selectedOption == "Dungeon 9") {
                character.current_dungeon = 9;
                return;
            } else if (selectedOption == "Dungeon 10") {
                character.current_dungeon = 10;
                return;
            } else if (selectedOption == "Back to the Association") {
                mhaMenu(character, window, textBoxFont, background, backgroundTexture, upperBox, upperBoxText, upperTitleBox, upperTitleBoxText, lowerBox, mainBox, mainBoxText);
                return;
            }
        }
    }
}
