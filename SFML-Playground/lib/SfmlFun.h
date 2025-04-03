#include <SFML/Graphics.hpp>
#include <optional>
#include <iostream>
#include <vector>

using namespace sf;
using namespace std;

float globalDesiredHeight;
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
    const Texture texture = sprite.getTexture();
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
        cerr << "Error loading characters.json file." << endl;
        exit(1);
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
        currentItems = loadShopItems(filename, character.level);
        // Prepare a list of option strings for each item.
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

void leaderboardsMenu (json &leaderboards_data, RenderWindow &window, Font textBoxFont, 
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
    mainBoxText.setString("Here are the Leaderboards!\nHere you can see the best players of the game!\nChoose an option from the ones on your right -->\n");
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
