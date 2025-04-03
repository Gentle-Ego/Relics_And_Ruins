#include "../lib/MainFrame.h"
#include "../lib/SfmlFun.h"
using namespace std;
using namespace sf;

// Funzione per eseguire un file .exe (serve per runnare resize-images.exe)
void runExeFile(const string& filePath) {
    // Costruisci il comando per eseguire il file .exe.
    // Nota: ".\\" serve per indicare la directory corrente in ambiente Windows.
    string runCommand = ".\\" + filePath;
    
    // Esegui il file .exe tramite il comando system.
    int result = system(runCommand.c_str());
    
    // Verifica se l'esecuzione ha avuto successo
    if (result != 0) {
        cerr << "Execution failed with error code " << result << "!" << endl;
    }
}


int main ()
{
    //runExeFile("resize-images.exe");
    // Creazione della finestra
    RenderWindow window = RenderWindow(VideoMode::getDesktopMode(), "Relics & Ruins", Style::Default);
    window.setPosition(Vector2i(0, 0)); // Posiziona la finestra nell'angolo in alto a sinistra
    window.setFramerateLimit(60);

    const auto cursorHand = Cursor::createFromSystem(Cursor::Type::Hand).value();
    const auto cursorArrow = Cursor::createFromSystem(Cursor::Type::Arrow).value();
    Vector2i mousePosition;

    string filename="";
    int shopChoice = 0;
    vector<int> leadChoice = {0, 0, 0, 0};

    Texture textBoxTexture;
    if (!textBoxTexture.loadFromFile("../assets/Textures/Backgrounds/TextBoxBackground.png")) 
        return -1;
    Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("../assets/Textures/Backgrounds/Valoria/CapitalLobby.jpg")) 
        return -1;
    Texture lineTexture;
    if (!lineTexture.loadFromFile("../assets/FantasyMinimal/UI/Line.png")) 
        return -1;
    Texture shopTexture;
    if (!shopTexture.loadFromFile("../assets/Textures/Backgrounds/Valoria/out-shops.jpg")){
        return -1;
    }  

// TextBox e posizione relativa alla finestra
/*

- |__| = 78% width, 22% height
- | |
- |_| = 22% width, 78% height
- |_| = 22% width, 22% height

*/
    Font textBoxFont;
    if (!textBoxFont.openFromFile("../assets/Fonts/TextBox.ttf"))
        return -1; 
//------------------------------------------------------------------------------------------ 
    RectangleShape mainBox;
    mainBox.setFillColor(Color(153,136,124));
    const Vector2f mainRectSize(
        window.getSize().x*80/100,
        window.getSize().y *20/100
    );
    mainBox.setSize(mainRectSize);
    mainBox.setPosition(Vector2f(0, window.getSize().y - mainRectSize.y));

    Text mainBoxText(textBoxFont, "Hello World", 100);
    mainBoxText.setFont(textBoxFont);
    mainBoxText.setCharacterSize(24); // Dimensione del testo
    mainBoxText.setFillColor(Color::Black);
//--------------------------------------------------------------------
    RectangleShape lowerBox;
    lowerBox.setFillColor(Color(87,72,82));
    const Vector2f lowerRectSize(
        window.getSize().x*20/100,
        window.getSize().y *2/5
    );
    lowerBox.setSize(lowerRectSize);
    lowerBox.setPosition(Vector2f(window.getSize().x - lowerRectSize.x,
                                window.getSize().y - lowerRectSize.y));
//--------------------------------------------------------------------
    RectangleShape upperBox;
    upperBox.setFillColor(Color(87,72,82));
    //upperBox.setTexture(&textBoxTexture);
    const Vector2f upperRectSize(
        window.getSize().x*20/100,
        window.getSize().y *3/5-window.getSize().y *1/30
    );
    upperBox.setSize(upperRectSize);
    upperBox.setPosition(Vector2f(window.getSize().x - upperRectSize.x,
                                    window.getSize().y - upperRectSize.y - lowerRectSize.y));

    Text upperBoxText(textBoxFont, "Hello World", 100);
    upperBoxText.setFont(textBoxFont);
    upperBoxText.setCharacterSize(24); // Dimensione del testo
    upperBoxText.setFillColor(Color::Black);
//--------------------------------------------------------------------
    RectangleShape upperTitleBox;
    upperTitleBox.setFillColor(Color(87,72,82));
    //upperBox.setTexture(&textBoxTexture);
    const Vector2f upperTitleRectSize(
        window.getSize().x*20/100,
        window.getSize().y *1/30
    );
    upperTitleBox.setSize(upperTitleRectSize);
    upperTitleBox.setPosition(Vector2f(window.getSize().x - upperTitleRectSize.x,
                                0));

    Text upperTitleBoxText(textBoxFont, "Hello World", 100);
    upperTitleBoxText.setFont(textBoxFont);
    upperTitleBoxText.setCharacterSize(24); // Dimensione del testo
    upperTitleBoxText.setFillColor(Color::Black);
//--------------------------------------------------------------------------------------------------------------------//USARE SPRITE!!!
    // Create the sprite with the texture
    Sprite line1(lineTexture);
    rescaleSprite(line1, window.getSize(), 0.8f, 0.02f);
    // Position the sprite at the top-left corner (you can adjust this as needed)
    line1.setPosition(Vector2f(0, window.getSize().y - mainRectSize.y));
    mainBoxText.setPosition(Vector2f(mainBox.getPosition().x + 10, mainBox.getPosition().y + 10 + (line1.getTexture().getSize().y/2)*0.16f));
//--------------------------------------------------------------------------------------------------------------------//USARE SPRITE!!!
    // Create the sprite with the texture
    Sprite line2(lineTexture);
    line2.setRotation(degrees(90));
    rescaleSprite(line2, window.getSize(), 0.8f, 0.02f);
    // Position the sprite at the top-left corner (you can adjust this as needed)
    line2.setPosition(Vector2f(window.getSize().x - lowerRectSize.x, 0));
//-------------------------------------------------------------------------------------------------------------------//USARE SPRITE!!!
    // Create the sprite with the texture
    Sprite line3(lineTexture);
    rescaleSprite(line3, window.getSize(), 0.2f, 0.02f);
    // Position the sprite at the top-left corner (you can adjust this as needed)
    line3.setPosition(Vector2f(window.getSize().x - lowerRectSize.x, window.getSize().y - lowerRectSize.y));
    upperBoxText.setPosition(Vector2f(upperBox.getPosition().x + 10, upperBox.getPosition().y + 10 + (line3.getTexture().getSize().y/2)*0.16f));
//-------------------------------------------------------------------------------------------------------------------//USARE SPRITE!!!
    // Create the sprite with the texture
    Sprite line4(lineTexture);
    rescaleSprite(line4, window.getSize(), 0.2f, 0.02f);
    // Position the sprite at the top-left corner (you can adjust this as needed)
    line4.setPosition(Vector2f(window.getSize().x - lowerRectSize.x, window.getSize().y - upperRectSize.y - lowerRectSize.y));
    upperTitleBoxText.setPosition(Vector2f(upperTitleBox.getPosition().x + 10, upperTitleBox.getPosition().y + 10));
//------------------------------------------------------------------------------------------
    RectangleShape background;
    background.setTexture(&backgroundTexture);
    const Vector2f backgroundSize(
        window.getSize().x*80/100,
        window.getSize().y *80/100
    );
    background.setSize(backgroundSize);
    background.setPosition(Vector2f(0, 0));
//--------------------------------------------------------------------
    RectangleShape textBox;
    textBox.setTexture(&textBoxTexture);
    const float marginX = 20.0f;
    const float marginY = 10.0f;
    const Vector2f rectangleSize(
        window.getSize().x - 2 * marginX,
        window.getSize().y / 4 // Rettangolo alto un quarto della finestra
    );
    textBox.setSize(rectangleSize);
    textBox.setPosition(Vector2f(marginX, window.getSize().y - rectangleSize.y - marginY));

    Text textBoxText(textBoxFont, "Hello World", 100);
    textBoxText.setFont(textBoxFont);
    textBoxText.setCharacterSize(24); // Dimensione del testo
    textBoxText.setFillColor(Color::Black);
//--------------------------------------------------------------------

    string fullText = "";
    string currentText = "";
    string fullTextTutorial = "";
    string currentTextTutorial = "";
    Clock clock;
    float delay = 0.02f; // Delay in secondi tra una lettera e l'altra
    bool hasJustStarted = true; // Flag per l'inizializzazione
    bool isPrintingText = false; // Flag per gestire la stampa graduale del testo

    Text tutChoiceText(textBoxFont, "Do you want to start from scratch?", 40);
    tutChoiceText.setPosition(Vector2f(textBox.getPosition().x + 10, textBox.getPosition().y + 10));
    tutChoiceText.setFillColor(Color::Black);

    RectangleShape tutBoxSelectionYes;
    tutBoxSelectionYes.setFillColor(Color(0,0,0,0));
    tutBoxSelectionYes.setSize(Vector2f(100, 55));
    tutBoxSelectionYes.setPosition(Vector2f(textBox.getPosition().x + 900, textBox.getPosition().y + 10));
    Text tutSelectionYes(textBoxFont, "YES", 40);
    tutSelectionYes.setPosition(Vector2f(tutBoxSelectionYes.getPosition().x + 5, tutBoxSelectionYes.getPosition().y + 5));
    tutSelectionYes.setFillColor(Color::Black);
    
    RectangleShape tutBoxSelectionNo;
    tutBoxSelectionNo.setFillColor(Color(0,0,0,0));
    tutBoxSelectionNo.setSize(Vector2f(66, 55));
    tutBoxSelectionNo.setPosition(Vector2f(textBox.getPosition().x + 1040, textBox.getPosition().y + 10));
    Text tutSelectionNo(textBoxFont, "NO", 40);
    tutSelectionNo.setPosition(Vector2f(tutBoxSelectionNo.getPosition().x + 5, tutBoxSelectionNo.getPosition().y + 5));
    tutSelectionNo.setFillColor(Color::Black);


    string selection = "";
    Texture backroundShopTexture;
    bool inputBoxSelected = false;

    Text characterSelectionText(textBoxFont, "Select your character", 40);
    characterSelectionText.setPosition(Vector2f(textBox.getPosition().x + 10, textBox.getPosition().y - 550));
    characterSelectionText.setFillColor(Color::Black);

    RectangleShape inputBox;
    Texture inputBoxTexture;
    inputBox.setFillColor(Color(0,0,0,0));
    inputBox.setOutlineColor(Color::Black);
    inputBox.setOutlineThickness(2);
    inputBox.setSize(Vector2f(textBox.getSize().x-50, 70));
    inputBox.setPosition(Vector2f(characterSelectionText.getPosition().x, characterSelectionText.getPosition().y + 70));

    string playerInput;
    Text playerText (textBoxFont, "", 36);
    playerText.setFillColor(Color::Black);
    playerText.setPosition(Vector2f(inputBox.getPosition().x + 10, inputBox.getPosition().y + 10));

    string characterName = "";
    int characterCreationStep = 1;

    Text characterNamesList(textBoxFont, "No Characters Found", 40);
    characterNamesList.setFillColor(Color::Black);
    characterNamesList.setPosition(Vector2f(inputBox.getPosition().x, inputBox.getPosition().y + 150));

    Text newCharacterCreationText(textBoxFont, "Create Your Character\n\n\n Name:\n\n Race:\n\n Sex:\n\n Difficulty:", 40);
    newCharacterCreationText.setFillColor(Color::Black);
    newCharacterCreationText.setPosition(Vector2f(textBox.getPosition().x + 10, textBox.getPosition().y - 550));

    string newCharacterName = "";
    Text newCharacterNameText(textBoxFont, "", 40);
    newCharacterNameText.setFillColor(Color::Black);
    newCharacterNameText.setPosition(Vector2f(newCharacterCreationText.getPosition().x + 155, newCharacterCreationText.getPosition().y +160));

    string newCharacterRace = "";
    Text newCharacterRaceText(textBoxFont, "", 40);
    newCharacterRaceText.setFillColor(Color::Black);
    newCharacterRaceText.setPosition(Vector2f(newCharacterCreationText.getPosition().x + 150, newCharacterCreationText.getPosition().y + 265));

    string newCharacterSex = "";
    Text newCharacterSexText(textBoxFont, "", 40);
    newCharacterSexText.setFillColor(Color::Black);
    newCharacterSexText.setPosition(Vector2f(newCharacterCreationText.getPosition().x + 120, newCharacterCreationText.getPosition().y + 370));

    string newCharacterDifficulty = "";
    Text newCharacterDifficultyText(textBoxFont, "", 40);
    newCharacterDifficultyText.setFillColor(Color::Black);
    newCharacterDifficultyText.setPosition(Vector2f(newCharacterCreationText.getPosition().x + 270, newCharacterCreationText.getPosition().y + 475));

    bool characterCreationFinished = false;
    bool characterSelectedCorrectly = true;

    Text invalidInsertionTextRace(textBoxFont, "Invalid Insertion \n(human, elf, dwarf, orc, halfling, tiefling, gnome, goblin, kobold, hobbit)", 25);
    invalidInsertionTextRace.setFillColor(Color::Red);
    invalidInsertionTextRace.setPosition(Vector2f(inputBox.getPosition().x + 350, inputBox.getPosition().y + 100));

    Text invalidInsertionTextSex(textBoxFont, "Invalid Insertion \n(Male, Female)", 40);
    invalidInsertionTextSex.setFillColor(Color::Red);
    invalidInsertionTextSex.setPosition(Vector2f(inputBox.getPosition().x + 330, inputBox.getPosition().y + 100));

    Text invalidInsertionTextDiff(textBoxFont, "Invalid Insertion \n(Easy, Normal, Hard, Extreme)", 40);
    invalidInsertionTextDiff.setFillColor(Color::Red);
    invalidInsertionTextDiff.setPosition(Vector2f(inputBox.getPosition().x + 330, inputBox.getPosition().y + 100));

    Character playerCharacter;

    string selectionForStartGame = "";
    float elapsedTime = 0; 
    int tutorialTextStep = 1;

    // Ciclo principale del gioco
    while (window.isOpen())
    {
        bool leftMouseReleased = false;
        if (playerCharacter.name != "") {
            upperTitleBoxText.setString(string(playerCharacter.name) + " (Level " + to_string(playerCharacter.level) + "/" + string(playerCharacter.difficulty) + ")");
        }
        // Process events
        optional<Event> event;
        while ((event = window.pollEvent())) {
            if (event->is<Event::Closed>()) {
                window.close();
            }
            // Detect mouse button release
            else if (event->is<Event::MouseButtonReleased>()) {
                const auto* mouseEvent = event->getIf<Event::MouseButtonReleased>();
                if (mouseEvent && mouseEvent->button == Mouse::Button::Left) {
                    leftMouseReleased = true;
                }
            }
            else if (const auto* textEvent = event->getIf<Event::TextEntered>()) {
                if (textEvent->unicode == '\b') { // Handle Backspace
                    if (!playerInput.empty()) {
                        playerInput.pop_back();
                    }
                } else if (textEvent->unicode < 128 && inputBoxSelected) {
                    playerInput += static_cast<char>(textEvent->unicode);
                }
                playerText.setString(playerInput);
            }
        }
        isMouseReleased(leftMouseReleased);
        mousePosition = Mouse::getPosition(window);

        // Sfondo
        //resizeBackground(backgroundSprite, window);
        window.clear();
        // window.draw(backgroundSprite);
        //Tutorial
        if (hasJustStarted) 
        {
            hasJustStarted = false;
            isPrintingText = true;
            initializeTextBoxForTutorial(textBox, textBoxText, window, fullText, currentText);
        } 
        if (isPrintingText) 
        {
            isPrintingText = !handleTextPrintingForTutorial(clock, delay, fullText, currentText, textBoxText, window);
            window.draw(textBox);
            window.draw(textBoxText);
        } else if (selection == "YES")
        {
            string x = stringToLower(playerText.getString());
            x[0]-=32;
            playerText.setString(x);
            window.clear();
            // window.draw(backgroundSprite);
            window.draw(textBox);
            window.draw(newCharacterCreationText);
            window.draw(inputBox);
            window.draw(playerText);
            switch (characterCreationStep)
            {
                case 1:
                    newCharacterNameText.setString(playerText.getString());
                    break;
                case 2:
                    newCharacterRaceText.setString(playerText.getString());
                    break;
                case 3:
                    newCharacterSexText.setString(playerText.getString());
                    break;
                case 4:
                    newCharacterDifficultyText.setString(playerText.getString());
                    break;
                default:
                    break;
            }
            window.draw(newCharacterNameText);
            window.draw(newCharacterRaceText);
            window.draw(newCharacterSexText);
            window.draw(newCharacterDifficultyText);

            if(checkForMouseClick(inputBox, window, mousePosition, leftMouseReleased))
            {
                inputBoxSelected = true;
                inputBox.setOutlineColor(Color(20, 90, 200));
            } else if(Mouse::isButtonPressed(Mouse::Button::Left))
            {
                inputBoxSelected = false;
                inputBox.setOutlineColor(Color::Black);
            }

            if (inputBox.getOutlineColor() == Color(20, 90, 200) && Keyboard::isKeyPressed(Keyboard::Key::Enter))
            {
                string ph;
                switch (characterCreationStep)
                {
                    case 1:
                        newCharacterName = trim(newCharacterNameText.getString());
                        characterCreationStep ++;
                        break;
                    case 2:
                        ph = trim(stringToLower(newCharacterRaceText.getString()));
                        if (ph == "human" or ph == "elf" or ph == "dwarf" or 
                            ph == "orc" or ph == "halfling" or
                            ph == "tiefling" or ph == "gnome" or ph == "goblin" or
                            ph == "kobold" or ph == "hobbit")//find(racesFirst, racesLast, ph) != RACES.end())
                        {
                            // Se la razza è valida, salvala e passa allo step successivo
                            newCharacterRace = trim(newCharacterRaceText.getString());
                            newCharacterRace = stringToLower(newCharacterRace);
                            newCharacterRace[0] -= 32;
                            characterCreationStep++;
                        } else 
                        {
                            // Se la razza è invalida, mostra il messaggio di errore
                            invalidInsertionTextRace.setFillColor(sf::Color::Red);
                            window.draw(invalidInsertionTextRace);
                            window.display();
                            Clock clock;
                            while (clock.getElapsedTime().asSeconds() < 3.0f) 
                            {
                                // Lascio che il programma risponda agli eventi durante l'attesa
                                optional<Event> event;
                                while ((event = window.pollEvent())) {
                                    if (event->is<Event::Closed>()) {
                                        window.close();
                                    }
                                }
                            }
                            // Nascondi il messaggio di errore
                            invalidInsertionTextRace.setFillColor(sf::Color::Transparent);
                            window.draw(invalidInsertionTextRace);
                            window.display();
                        }
                        break;
                    case 3:
                        ph = trim(stringToLower(newCharacterSexText.getString()));
                        if (ph == "male" or ph == "female") 
                        {
                            // Se il sesso è valido, salvalo e passa allo step successivo
                            newCharacterSex = ph;
                            newCharacterSex = stringToLower(newCharacterSex);
                            newCharacterSex[0] -= 32;
                            characterCreationStep++;
                        } else 
                        {
                            // Se il sesso è invalido, mostra il messaggio di errore
                            invalidInsertionTextSex.setFillColor(sf::Color::Red);
                            window.draw(invalidInsertionTextSex);
                            window.display();
                            Clock clock;
                            while (clock.getElapsedTime().asSeconds() < 3.0f) 
                            {
                                // Lascio che il programma risponda agli eventi durante l'attesa
                                optional<Event> event;
                                while ((event = window.pollEvent())) {
                                    if (event->is<Event::Closed>()) {
                                        window.close();
                                    }
                                }
                            }
                            // Nascondi il messaggio di errore
                            invalidInsertionTextSex.setFillColor(Color::Transparent);
                            window.draw(invalidInsertionTextSex);
                            window.display();
                        }
                        break;
                    case 4:
                        ph = trim(stringToLower(newCharacterDifficultyText.getString()));
                        if (ph == "easy" or ph == "normal" or
                            ph == "hard" or ph == "extreme")//find(difficultiesFirst, difficultiesLast, ph) != DIFFICULTIES.end()) 
                        {
                            // Se la difficoltà è valida, salvala e passa allo step successivo
                            newCharacterDifficulty = trim(newCharacterDifficultyText.getString());
                            newCharacterDifficulty = stringToLower(newCharacterDifficulty);
                            newCharacterDifficulty[0] -= 32;
                            characterCreationStep++;
                            characterCreationFinished = true;
                        } else 
                        {
                            // Se la difficoltà è invalida, mostra il messaggio di errore
                            invalidInsertionTextDiff.setFillColor(sf::Color::Red);
                            window.draw(invalidInsertionTextDiff);
                            window.display();
                            Clock clock;
                            while (clock.getElapsedTime().asSeconds() < 3.0f) 
                            {
                                // Lascio che il programma risponda agli eventi durante l'attesa
                                optional<Event> event;
                                while ((event = window.pollEvent())) {
                                    if (event->is<Event::Closed>()) {
                                        window.close();
                                    }
                                }
                            }
                            // Nascondi il messaggio di errore
                            invalidInsertionTextDiff.setFillColor(Color::Transparent);
                            window.draw(invalidInsertionTextDiff);
                            window.display();
                        }
                        break;
                    default:
                        break;
                }
                inputBoxSelected = false;
                inputBox.setOutlineColor(Color::Black);
                playerInput.clear();
                playerText.setString(playerInput);
            
                //Character playerCharacter = fromJSONtoCharacter(characterName);
                //start_game(playerCharacter); 
            } else if (characterCreationFinished)
            {
                window.clear();
                textBoxText.setString("");
                textBoxText.setPosition(Vector2f(textBox.getPosition().x + 10, textBox.getPosition().y +10));
                textBoxText.setCharacterSize(24);
                selection = "GAMESTART";
            }
        } else if (selection == "NO")
        {
            window.clear();
            // window.draw(backgroundSprite);
            window.draw(textBox);
            window.draw(characterSelectionText);
            window.draw(inputBox);
            window.draw(playerText);

            if(checkForMouseClick(inputBox, window, mousePosition, leftMouseReleased))
            {
                inputBoxSelected = true;
                inputBox.setOutlineColor(Color(20, 90, 200));
            } else if(Mouse::isButtonPressed(Mouse::Button::Left))
            {
                inputBoxSelected = false;
                inputBox.setOutlineColor(Color::Black);
            }
            if (inputBox.getOutlineColor() == Color(20,90,200) && Keyboard::isKeyPressed(Keyboard::Key::Enter))
            {
                inputBoxSelected = false;
                inputBox.setOutlineColor(Color::Black);
                characterName = trim(playerText.getString().toAnsiString());
                playerInput.clear();
                playerText.setString(playerInput);

                json characters;
                ifstream char_file("../include/characters.json");
                if (char_file.is_open()) 
                {
                    char_file >> characters;
                    char_file.close();
                } else 
                {
                    cerr << "Error loading characters.json file." << endl;
                    exit(1);
                }
                for (const auto &character : characters["characters"]) 
                {   
                    if (character["name"] == characterName) 
                    {
                        playerCharacter = fromJSONtoCharacter(character);
                        playerCharacter.write_character_to_json(playerCharacter);
                        selection = "TUTORIAL";
                        textBoxText.setString("");
                        textBoxText.setPosition(Vector2f(textBox.getPosition().x+10, textBox.getPosition().y+10));
                        textBoxText.setCharacterSize(24);
                        break; // Esci una volta trovato il personaggio
                    }
                }
            }

            characterNamesList = selectCharacter(characterNamesList, selection);
            window.draw(characterNamesList);
        } else if (selection == "")
        {
            window.draw(textBox);
            window.draw(textBoxText);
            window.draw(tutChoiceText);
            window.draw(tutBoxSelectionYes);
            window.draw(tutSelectionYes);
            window.draw(tutBoxSelectionNo);
            window.draw(tutSelectionNo);
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
                window.setMouseCursor(cursorArrow);
            }
        } else if(selection == "GAMESTART")
        {   
            playerCharacter = Character(newCharacterName, newCharacterRace, newCharacterSex, newCharacterDifficulty);
            playerCharacter.current_dungeon = -5;
            playerCharacter.write_character_to_json(playerCharacter);
            selection = "TUTORIAL";
        } else if (playerCharacter.current_dungeon == -5) // Tutorial // Tributo a GAMESTART2 :{ (Baffi) // commento: il mio commento è più bello del tuo (Gianluca) (Baffi) [chi è Gianluca?] by Copilot (Baffi)
        {
            printTutorialText(clock, playerCharacter, textBoxText, window, textBoxFont, textBox, selectionForStartGame, fullTextTutorial, currentTextTutorial, elapsedTime, tutorialTextStep, selection);
            window.draw(textBoxText);
        } else if (playerCharacter.current_dungeon == -2) // Shop
        {
            shops(playerCharacter, window, textBoxFont, 
                background, shopTexture,
                upperBox, upperBoxText, upperTitleBox, upperTitleBoxText,
                lowerBox, mainBox, mainBoxText, filename, shopChoice, backroundShopTexture);
            window.draw(line1);
            window.draw(line2);
            window.draw(line3);
            window.draw(line4);
        }
        else if (playerCharacter.current_dungeon == 0) // Lobby
        {
            mainMenu(playerCharacter, window, textBoxFont, 
                background, shopTexture,
                upperBox, upperBoxText, upperTitleBox, upperTitleBoxText,
                lowerBox, mainBox, mainBoxText);
            window.draw(line1);
            window.draw(line2);
            window.draw(line3);
            window.draw(line4);
        }
        else if (playerCharacter.current_dungeon == -3) // MHA
        {
            if (leaderboardIn){
                leaderboardsMenu(leadChoice, leaderboards_data, window, textBoxFont, background, backgroundTexture, upperBox, upperBoxText, upperTitleBox, upperTitleBoxText, lowerBox, mainBox, mainBoxText);
            } else {
                mhaMenu(playerCharacter, window, textBoxFont, 
                    background, shopTexture,
                    upperBox, upperBoxText, upperTitleBox, upperTitleBoxText,
                    lowerBox, mainBox, mainBoxText);
                window.draw(line1);
                window.draw(line2);
                window.draw(line3);
                window.draw(line4);
            }
        }
        window.display();
    }
    return 0;
}
    
