#include <SFML/Graphics.hpp>
#include <optional>
#include <iostream>
#include <vector>

using namespace sf;
using namespace std;

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

bool isMouseHovering(const RectangleShape& rect, const RenderWindow& window, const Vector2i& mousePosition) {
    FloatRect bounds = rect.getGlobalBounds();
    return bounds.contains(static_cast<Vector2f>(mousePosition));
}

Text selectCharacter(Text &characterNamesList, string &selection) 
{
    json characters;
    ifstream char_file("include/characters.json");
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
        "it, still warm from \nthe print. It feels real, solid—an achievement."
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
        "\nweapon and maybe utility.\nSince we are in the capital Eràn you'll be "
        "able to find many shops and stores of any kind, \nyou may chose your "
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
        "tour around Eràn, and then we'll head down \nto the association "
        "basement where the dungeons are located.\""
        "\n\n Click To Continue";
    
    string text5 = "You follow Rosie out of The Golden Pigeon and into the heart of "
        "Eràn, the capital city. \nThe streets are bustling with people of "
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
        if (checkForMouseClick(tutBoxSelectionYes, window, mousePosition)){
            selection = "YES";
            window.setMouseCursor(cursorArrow);
        }else if (checkForMouseClick(tutBoxSelectionNo, window, mousePosition)){
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
        if(fullText.empty() && checkForMouseClick(textBox, window, mousePosition))
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

    // Define padding as a small fraction of the container height.
    float padding = containerSize.y * 0.02f; // 2% of container height
    // Compute option height based on available vertical space.
    // We subtract padding at top and bottom, plus space between options.
    float totalPadding = padding * (optionTexts.size() + 1);
    float optionHeight = (containerSize.y - totalPadding) / optionTexts.size();

    // Option width is nearly the full width, minus horizontal padding on both sides.
    float optionWidth = containerSize.x - 2 * padding;

    // Starting position for the first option: add a top padding offset.
    float currentY = containerPos.y + padding;

    for (const auto& optionText : optionTexts) {
    // Create an Option with the computed width and height.
    Option opt(font, optionWidth, optionHeight);

    // Set the size and position of the option box.
    opt.box.setSize({ optionWidth, optionHeight });
    // Position it relative to the container's position plus the horizontal padding.
    opt.box.setPosition({ containerPos.x + padding, currentY });

    // Setup text properties:
    opt.text.setFont(font);
    opt.text.setString(optionText);

    // Set the character size proportional to the option's height.
    // Adjust the factor as needed (e.g., 50% of option height).
    unsigned int charSize = static_cast<unsigned int>(optionHeight * 0.5f);
    opt.text.setCharacterSize(charSize);

    // Position text within the box. Here we add some horizontal offset (padding)
    // and vertically center the text within the option box.
    // Note: Depending on the font, vertical centering might need tweaking.
    opt.text.setPosition({
    containerPos.x + 2 * padding,
    currentY + (optionHeight - charSize) / 2
    });

    // Move the current Y position for the next option (current option height plus padding).
    currentY += optionHeight + padding;
    options.push_back(opt);
    }

    return options;
}


// Funzione per gestire il click del mouse
void handleOptionMouseClick(RenderWindow& window, vector<Option>& options) {
    Vector2i mousePos = Mouse::getPosition(window);
    for (auto& opt : options) {
        if (checkForMouseClick(opt.box, window, mousePos)) {
            cout << "Opzione selezionata: " << opt.text.getString().toAnsiString() << endl;
        }
    }
}

void drawOptions(RenderWindow& window, std::vector<Option>& options) {
    for (auto& option : options) {
        option.draw(window);
    }
}

void shops(Clock &clock, Character &character, RenderWindow &window, Font textBoxFont, 
           RectangleShape &background, float &elapsedTime,
           RectangleShape &upperBox, Text &upperBoxText, RectangleShape &upperTitleBox, Text &upperTitleBoxText,
           RectangleShape &lowerBox, RectangleShape &mainBox, Text &mainBoxText)
{
    character.current_dungeon = -2;
    character.write_character_to_json(character);
    
    mainBoxText.setString("Welcome to the shops area! Choose a shop to visit:\n");
    vector<string> shopOptions = {"DragonForge Armory", "The Weapons of Valoria", "The Alchemist's Kiss",
                                  "Feast & Famine", "Relics & Rarities", "The Rusty Nail", "Exit the Shop"
                                , "Extra Opzione 1", "Extra Opzione 2", "Extra Opzione 3", "Extra Opzione 4"};
    vector<Option> options = createOptions(shopOptions, textBoxFont, lowerBox.getPosition(), lowerBox.getSize());
    if (Mouse::isButtonPressed(Mouse::Button::Left)) {
        handleOptionMouseClick(window, options);
    }

    window.clear();
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

    /*
    string filename, option;
  shop:
    mainBoxText.setString("Welcome to the shops area! Choose a shop to visit:\n");
    vector<string> shopOptions = {"DragonForge Armory", "The Weapons of Valoria", "The Alchemist's Kiss",
                                  "Feast & Famine", "Relics & Rarities", "The Rusty Nail", "Exit the Shop"};
    slowCout(
        "1. DragonForge Armory\n2. The Weapons of Valoria\n3. The Alchemist's "
        "Kiss\n4. Feast & Famine\n5. Relics & Rarities\n6. The Rusty Nail\n7. Exit the Shop\n");
    int choice;
    do{
      cout << "\nSelect a number > ";
      cin >> choice;
    }while(choice>7 || choice<1);
    clearScreen();
  
    switch (choice) {
    case 1:
      filename = "armors.json";
      slowCout("Welcome to Dragon Forge\n");
      break;
    case 2:
      filename = "weapons.json";
      slowCout("Welcome to The Weapons of Valoria\n");
      break;
    case 3:
      filename = "potions.json";
      slowCout("Welcome to The Alchemist's Kiss\n");
      break;
    case 4:
      filename = "foods.json";
      slowCout("Welcome to Feast & Famine\n");
      break;
    case 5:
      filename = "usables.json";
      slowCout("Welcome to Relics & Rarities\n");
      break;
    case 6:
      filename = "utilities.json";
      slowCout("Welcome to The Rusty Nail\n");
      break;
    default:
      cout << "Exiting shop.\n";
      this_thread::sleep_for(chrono::seconds(4));
      ;
      character.current_dungeon = 0;
      main_menu(character);
      return;
    }
  
    do {
      slowCout("Would you like to Buy or Sell?\n");
      cin >> option;
      option = stringToLower(option);
    } while (option != "buy" && option != "sell");
  shopx:
    vector<json> items;
  
    if (option == "buy") {
      items = loadShopItems(filename, character.level);
  
      // Visualizza gli oggetti disponibili da comprare
      cout << "Available items at your current level:\n";
      for (size_t i = 0; i < items.size(); ++i) {
        cout << i + 1 << ". " << items[i]["name"] << " - " << items[i]["value"]
             << " coins\n";
      }
    } else {
      items = character.findItemsType(filename);
  
      // Visualizza gli oggetti disponibili da vendere
      cout << "Available items in your inventory, that can be sold in this "
              "shop:\n";
      for (size_t i = 0; i < items.size(); ++i) {
        cout << i + 1 << ". " << items[i]["name"] << " - " << items[i]["value"]
             << " coins, selling price: " << double(items[i]["value"]) * 0.75
             << "\n"; // Oggetto vendibile al .75 del valore
      }
    }
    // Selezione e acquisto
    int itemChoice;
    cout << "Your current coins are: " << character.coins << endl;
    cout << "Enter the number of the item to " << stringToLower(option) << " (or 0 to exit): ";
    cin >> itemChoice;
  
    if (itemChoice > 0 && itemChoice <= items.size()) {
      json selectedItem = items[itemChoice - 1];
      if (option == "buy") {
        if (character.coins >= selectedItem["value"]) {
          character.coins -= int(selectedItem["value"]);
          character.addItem(selectedItem, character);
          clearScreen();
          cout << "Purchased " << selectedItem["name"] << " for "
               << selectedItem["value"] << " coins!\n";
          goto shopx;
        } else {
          clearScreen();
          cout << "Not enough coins to buy " << selectedItem["name"] << ".\n";
          goto shopx;
        }
      } else {
        character.coins += int(int(selectedItem["value"]) * 0.75);
        character.removeItem(selectedItem["name"], character);
        clearScreen();
        cout << "Sold " << selectedItem["name"] << " for "
             << int(double(selectedItem["value"]) * 0.75) << " coins!\n";
        goto shopx;
      }
    } else {
      cout << "Exiting shop.\n";
      goto shop;
    }*/
