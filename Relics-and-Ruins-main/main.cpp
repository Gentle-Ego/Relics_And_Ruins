#include "json.hpp"
#include <chrono>
#include <cstdlib> // For system()
#include <fstream>
#include <iostream>
#include <algorithm>
#include <string>
#include <thread>
#include <vector>


using json = nlohmann::json;
using namespace std;


// Link Utili:
/*
https://tomeko.net/online_tools/cpp_text_escape.php?lang=en     Da Mega Text a
stringa
https://patorjk.com/software/taag/#p=display&f=Big&t=WELCOME%20TO%20%0A%0ARELICS%20%26%20RUINS
Per creare Mega Text https://github.com/Gentle-Ego/Relics-and-Ruins      GitHub
per Update
*/

const vector<string> RACES = {"Human",    "Elf",      "Dwarf", "Orc",
                              "Halfling", "Tiefling", "Gnome", "Goblin",
                              "Kobold",   "Hobbit"};
auto racesFirst = RACES.begin();
auto racesLast = RACES.end();

const vector<string> DIFFICULTIES = {"Easy", "Normal", "Hard", "Extreme"};
auto difficultiesFirst = DIFFICULTIES.begin();
auto difficultiesLast = DIFFICULTIES.end();

const int START_COINS = 100;
const string SEX[2] = {"M", "F"};

// Funzione per pulire la schermata
void clearScreen() {
#ifdef _WIN32
  system("cls"); // Windows
#else
  system("clear"); // Linux/macOS
#endif
}

void slowCout(const string &text, int delay_ms = 40) {
  for (char c : text) {
    cout << c << flush; // Print each character and flush to display immediately
    this_thread::sleep_for(
        chrono::milliseconds(delay_ms)); // Delay between characters
  }
  // cout << endl; // Print a newline after the text is done
}

string stringToLower(const string &str) {
  string result = "";

  for (char ch : str) {
    // Convert each character to lowercase using tolower
    result += tolower(ch);
  }

  return result;
}

string stringToUpper(const string &str) {
  string result = "";

  for (char ch : str) {
    // Convert each character to uppercase using toupper
    result += toupper(ch);
  }

  return result;
}

string findPosition(int x) {
  switch (x) {
  case -3:
    return "Monster Hunter Association";
  case -2:
    return "Shops Plaza";
  case -1:
    return "Tutorial";
  case 0:
    return "Capital";
  case 1:
    return "First Dungeon";
  case 2:
    return "Second Dungeon";
  case 3:
    return "Third Dungeon";
  case 4:
    return "Fourth Dungeon";
  case 5:
    return "Fifth Dungeon";
  case 6:
    return "Sixth Dungeon";
  case 7:
    return "Seventh Dungeon";
  case 8:
    return "Eighth Dungeon";
  case 9:
    return "Ninth Dungeon";
  case 10:
    return "Tenth Dungeon";
  }
  return "";
}

static int calculateRequiredExp(int level) {
  // Formula esponenziale base
  // Esempio: 100 * (level^1.5)
  return static_cast<int>(100 * pow(level, 1.5));
}

static int calculateLevel(int experience) {
  int level = 1;
  while (calculateRequiredExp(level) <= experience) {
    level++;
  }
  return level - 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Character {
public:
  // Stats base
  string name, race, sex, difficulty;
  int coins, level;

  int coins_spent, tot_kills, deaths, tot_money_acquired; // for leaderboards
  int dunKills, dunDeaths, dunTurns; // for each dungeon leaderboard

  int experience;
  int current_turn; // solo dentro dungeon
  // in caso di combattimenti si aggiornano solo a fine combattimento per
  // sicurezza di salvataggio
  int current_dungeon; // da 1 a (num dungeon) o 0 se capitale, -1 se si è
                       // all'inizio, -2 se Shop, -3 se MHA
  int pos_x;
  int pos_y;

  // Risorse
  int health, max_health;
  int current_food, max_food;
  int mana, max_mana;
  int mana_regeneration;

  // Stats combattimento
  int strength;
  int defense;
  int dexterity;
  double critical;

  // Inventario e equipaggiamento
  vector<json> inventory;
  vector<json> equipped;

  Character(string n, string r, string s, string d)
      : name(n), race(r), sex(s), difficulty(d), coins(START_COINS), level(1), experience(0),
        current_turn(0), current_dungeon(-1), pos_x(0), pos_y(0), health(100),
        max_health(100), current_food(100), max_food(100), mana(50),
        max_mana(50), mana_regeneration(1), strength(10), defense(10),
        dexterity(10), critical(0.1), coins_spent(0), tot_kills(0), deaths(0),
        tot_money_acquired(START_COINS), dunKills(0), dunDeaths(0), dunTurns(0) {}


  // Controlla se l'oggetto esiste
  bool hasItem(const string &itemName) const {
    return any_of(
        inventory.begin(), inventory.end(),
        [&itemName](const json &item) { return item["name"] == itemName; });
  }

  // Aggiungi un singolo oggetto
  void addItem(const json &item, Character &character) {
    if (!item.contains("name")) {
      //cout << "ERROR IN ADD ITEM";
      throw invalid_argument("Item must contain 'name' field");
    }
    cout << "1\n";

    auto it = find_if(inventory.begin(), inventory.end(),
                      [&item](const json &invItem) { return invItem["name"] == item["name"]; });
              // potevo benissimo usare hasItem, ma volevo testare una cosa eheh...

    if (it != inventory.end()) { // Se l'oggetto esiste già
      if (it->contains("count")) {
          (*it)["count"] = int((*it)["count"]) + 1;  // (*it)["count"].get<int>() it->at("count") è un altro modo al posto di (*it)["count"] non è un puntatore, ma iteratore
      } else {
          (*it)["count"] = 2;  // se "count" non esiste, usa 1 come default
      }   // cosa importante, (*it)["count"] crea "count" nel caso non esista, in questo caso non importa poichè è controllato per sicurezza
    } else { // Se l'oggetto non esiste, aggiungilo con il suo 'count'
      json newItem = item;
      if (!newItem.contains("count")) {
          newItem["count"] = 1; // Iniializza 'count' a 1 se non è presente
      }
      inventory.push_back(newItem);
    }

    write_character_to_json(character);
  }

  // Aggiungi molteplici oggetti
  void addItems(const vector<json> &items, Character &character) {
    for (const auto &item : items) {
      addItem(item, character);
    }
    write_character_to_json(character);
  }

  // Elimina un oggetto
  bool removeItem(const string &itemName, Character &character) {
    auto it = find_if(
        inventory.begin(), inventory.end(),
        [&itemName](const json &item) { return item["name"] == itemName; });

    if (it != inventory.end()) {
      inventory.erase(it);
      write_character_to_json(character);
      return true;
    }
    return false;
  }

  // Trova numero di stesso oggetto
  int getItemTypeCount(const string &itemType) const {
    return count_if(
        inventory.begin(), inventory.end(),
        [&itemType](const json &item) { return item["type"] == itemType; });
  }

  vector<json> findItemsType(const string &itemType) {
    vector<json> items;
    string selection = itemType.substr(
        0, itemType.length() -
               6); // Togliere il .json e la s di multiple armors.json -> armor
    for (const auto &item : inventory) {
      // Controlla se l'oggetto ha un level_required minore o uguale al livello
      // del giocatore
      if (item.contains("type") && item["type"] == "selection") {
        items.push_back(item);
      }
    }
    return items;
  }

  vector<json> findItemsName(const string &itemName) {
    vector<json> items;
    for (const auto &item : inventory) {
      // Controlla se l'oggetto ha un level_required minore o uguale al livello
      // del giocatore
      if (item.contains("name") && item["name"] == "itemName") {
        items.push_back(item);
      }
    }
    return items;
  }

  void write_character_to_json(Character &charac) {
    // Crea il JSON del personaggio con tutti i nuovi attributi
    json character = {
        // Stats base
        {"name", charac.name},
        {"race", charac.race},
        {"sex", charac.sex},
        {"difficulty", charac.difficulty},
        {"coins", charac.coins},
        {"level", charac.level},
        {"experience", charac.experience},

        // Posizione
        {"current_turn", charac.current_turn},
        {"current_dungeon", charac.current_dungeon},
        {"pos_x", charac.pos_x},
        {"pos_y", charac.pos_y},

        // Risorse
        {"health", charac.health},
        {"max_health", charac.max_health},
        {"current_food", charac.current_food},
        {"max_food", charac.max_food},
        {"mana", charac.mana},
        {"max_mana", charac.max_mana},
        {"mana_regeneration", charac.mana_regeneration},

        // Stats combattimento
        {"strength", charac.strength},
        {"defense", charac.defense},
        {"dexterity", charac.dexterity},
        {"critical", charac.critical},

        // Inventario e equipaggiamento
        {"inventory", charac.inventory},
        {"equipped", charac.equipped},
    };

    // Carica i personaggi esistenti se il file esiste
    json characters;
    ifstream input_file("characters.json");
    if (input_file.is_open()) {
      input_file >> characters;
      input_file.close();
    } else {
      characters["characters"] = json::array();
    }

    // Aggiungi o aggiorna il personaggio
    bool character_exists = false;
    for (auto &existing_char : characters["characters"]) {
      if (existing_char["name"] == charac.name) {
        existing_char = character;
        character_exists = true;
        break;
      }
    }

    if (!character_exists) {
      characters["characters"].push_back(character);
    }

    // Salva il file JSON aggiornato
    ofstream output_file("characters.json");
    if (output_file.is_open()) {
      output_file << characters.dump(4);
      output_file.close();
      cout << "Character saved to characters.json!" << endl;
    } else {
      cerr << "Error: Could not open file for writing!" << endl;
    }
  }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void main_menu(Character character);    // Dichiarazione funzione main_menu

// Funzione per convertire da JSON a oggetto Character
Character fromJSONtoCharacter(json ch) {
  // Crea un nuovo personaggio con i dati base
  Character c(ch["name"], ch["race"], ch["sex"], ch["difficulty"]);

  // Aggiorna tutti gli altri attributi
  // Stats base
  c.coins = ch["coins"];
  c.level = ch["level"];
  c.experience = ch["experience"];

  // Posizione
  c.current_turn = ch["current_turn"];
  c.current_dungeon = ch["current_dungeon"];
  c.pos_x = ch["pos_x"];
  c.pos_y = ch["pos_y"];

  // Risorse
  c.health = ch["health"];
  c.max_health = ch["max_health"];
  c.current_food = ch["current_food"];
  c.max_food = ch["max_food"];
  c.mana = ch["mana"];
  c.max_mana = ch["max_mana"];
  c.mana_regeneration = ch["mana_regeneration"];

  // Stats combattimento
  c.strength = ch["strength"];
  c.defense = ch["defense"];
  c.dexterity = ch["dexterity"];
  c.critical = ch["critical"];

  // Inventario e equipaggiamento
  c.inventory = ch["inventory"].get<vector<json>>();
  c.equipped = ch["equipped"].get<vector<json>>();

  return c;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

string selectDifficulty() {
  clearScreen();
  slowCout("This is the Hall of Fame, here you will find many leaderboards regarding various statistics of all of the saved characters."
           "\n\nPlease select the difficulty of the desired leaderboards series:\n");
  slowCout("1. Easy\n");
  slowCout("2. Normal\n");
  slowCout("3. Hard\n");
  slowCout("4. Extreme\n");
  int choice;
  do{
    cout << "\nSelect a number > ";
    cin >> choice;
  }while(choice>4 || choice<1);

  switch (choice) {
      case 1: return "Easy";
      case 2: return "Normal";
      case 3: return "Hard";
      case 4: return "Extreme";
  }
  return "Easy"; // Fallback in caso di errore.
}

string selectLeaderboardType() {
  clearScreen();
  slowCout("Please select leaderboard type:\n");
  slowCout("1. Total Game\n");
  slowCout("2. Dungeons\n");
  int choice;
  do{
    cout << "\nSelect a number > ";
    cin >> choice;
  }while(choice>2 || choice<1);

  return (choice == 1) ? "total_game" : "dungeons";
}

string selectTotalParam() {
  clearScreen();
  slowCout("Please select parameter:\n");
  slowCout("1. Turns\n");
  slowCout("2. Coins Spent\n");
  slowCout("3. Kill-Death Ratio\n");
  slowCout("4. Total Kills\n");
  slowCout("5. Total Money Acquired\n");
  slowCout("6. Level\n");
  int choice;
  do{
    cout << "\nSelect a number > ";
    cin >> choice;
  }while(choice>6 || choice<1);

  switch (choice) {
      case 1: return "turns";
      case 2: return "coins_spent";
      case 3: return "kill_death_ratio";
      case 4: return "tot_kills";
      case 5: return "tot_money_acquired";
      case 6: return "level";
  }
  return "turns"; // Fallback.
}

string selectDungeon() {
  clearScreen();
  slowCout("Please enter the name of the dungeon (e.g., Dungeon1, Dungeon2): ");
  slowCout("1. Dungeon 1\n");
  slowCout("2. Dungeon 2\n");
  slowCout("3. Dungeon 3\n");
  slowCout("4. Dungeon 4\n");
  slowCout("5. Dungeon 5\n");
  slowCout("6. Dungeon 6\n");
  slowCout("7. Dungeon 7\n");
  slowCout("8. Dungeon 8\n");
  slowCout("9. Dungeon 9\n");
  slowCout("10. Dungeon 10\n");
  int choice;
  do{
    cout << "\nSelect a number > ";
    cin >> choice;
  }while(choice>6 || choice<1);

  switch (choice) {
      case 1: return "Dungeon1";
      case 2: return "Dungeon2";
      case 3: return "Dunguon3";
      case 4: return "Dungeon4";
      case 5: return "Dungeon5";
      case 6: return "Dungeon6";
      case 7: return "Dungeon7";
      case 8: return "Dungeon8";
      case 9: return "Dungeon9";
      case 10: return "Dungeon10";
  }
  return "Dungeon1";    //Fallback
}

string selectDungeonParam() {
  clearScreen();
  slowCout("Please select the dungeon parameter:\n");
  slowCout("1. Turns to Complete\n");
  slowCout("2. Kill-Death Ratio\n");
  int choice;
  do{
    cout << "\nSelect a number > ";
    cin >> choice;
  }while(choice>2 || choice<1);

  return (choice == 1) ? "turns_to_complete" : "kill_death_ratio";
}

void displayTotalLeaderboard(const json& leaderboards_data, const string& difficulty, const string& param) {
  clearScreen();
  vector<pair<string, int>> leaderboard;

  // Estrai i dati dal JSON
  for (const auto& [character, character_data] : leaderboards_data["leaderboards"].items()) {
      if (character_data.contains(difficulty) && character_data[difficulty].contains("total_game")) {
          int value = character_data[difficulty]["total_game"][param].get<int>();
          leaderboard.push_back({character, value});
      }
  }

  // Ordina in ordine decrescente
  sort(leaderboard.begin(), leaderboard.end(),
            [](const auto& lhs, const auto& rhs) { return lhs.second > rhs.second; });

  // Mostra i risultati
  slowCout("\nTotal Leaderboard for parameter: " + param + " (Difficulty: " + difficulty + ")\n");
  for (const auto& [character, value] : leaderboard) {
      slowCout(character + ": " + to_string(value) + "\n");
  }
}

void displayDungeonLeaderboard(const json& leaderboards_data, const string& difficulty, const string& dungeon, const string& param) {
  clearScreen();
  vector<pair<string, double>> leaderboard;

  // Estrai i dati dal JSON
  for (const auto& [character, character_data] : leaderboards_data["leaderboards"].items()) {
      if (character_data.contains(difficulty) && character_data[difficulty].contains("dungeons") && character_data[difficulty]["dungeons"].contains(dungeon)) {
          double value = character_data[difficulty]["dungeons"][dungeon][param].get<double>();
          leaderboard.push_back({character, value});
      }
  }

  // Ordina in base alla metrica specificata (crescente o decrescente)
  bool descending = (param == "kill_death_ratio"); // Solo kill_death_ratio è in ordine decrescente
  sort(leaderboard.begin(), leaderboard.end(),
            [descending](const auto& lhs, const auto& rhs) {
                return descending ? lhs.second > rhs.second : lhs.second < rhs.second;
            });

  // Mostra i risultati
  slowCout("\nDungeon Leaderboard: " + dungeon + ", parameter: " + param + " (Difficulty: " + difficulty + ")\n");
  for (const auto& [character, value] : leaderboard) {
      slowCout(character + ": " + to_string(value) + "\n");
  }
}

void leaderboards_menu(const json& leaderboards_data) {
  clearScreen();
  // Step 1: Seleziona difficoltà
  string difficulty = selectDifficulty();

  // Step 2: Scegli il tipo di leaderboard (totale o per dungeon)
  string leaderboard_type = selectLeaderboardType();

  if (leaderboard_type == "total_game") {
      // Step 3a: Leaderboard Totale - seleziona tipo di parametro
      string total_param = selectTotalParam();

      // Mostra leaderboard totale basata sul parametro scelto
      displayTotalLeaderboard(leaderboards_data, difficulty, total_param);
  } else if (leaderboard_type == "dungeons") {
      // Step 3b: Leaderboard per Dungeon - selezione Dungeon
      string dungeon = selectDungeon();

      // Step 4: Seleziona parametro specifico del dungeon
      string dungeon_param = selectDungeonParam();

      // Mostra leaderboard per il dungeon selezionato e parametro
      displayDungeonLeaderboard(leaderboards_data, difficulty, dungeon, dungeon_param);
  }

  cout << "\n";
  system("pause");
  return;
}

/*json leaderboards_data = load_leaderboards_data("ideal_leads.json");
  leaderboards_menu(leaderboards_data);*/

json load_leaderboards_data(const string& file_path) {
  ifstream file(file_path);
  if (!file.is_open()) {
      cerr << "Error opening file: " << file_path << endl;
      exit(1);
  }
  json data;
  file >> data;
  return data;
}


// Funzione per leggere il file JSON del negozio e visualizzare gli oggetti con
// livello appropriato
vector<json> loadShopItems(const string &filename, int lvl) {
  vector<json> items;
  ifstream file(filename);
  if (file.is_open()) {
    json data;
    file >> data;
    string selection = filename.substr(0, filename.length() - 5);
    for (const auto &item : data[selection]) {
      // Controlla se l'oggetto ha un level_required minore o uguale al livello
      // del giocatore
      if (!item.contains("level_required") || item["level_required"] <= lvl) {
        items.push_back(item);
      }
    }
    file.close();
  } else {
    cerr << "Error loading shop file: " << filename << endl;
  }
  return items;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Funzione per il negozio
void shop(Character &character) {
  character.current_dungeon = -2;
  character.write_character_to_json(character);
  string filename, option;
shop:
  clearScreen();
  slowCout("Welcome to the shops area! Choose a shop to visit:\n");
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
  }
}

void showObject(json object, string inventoryType)
{
  clearScreen();
  cout << "╔═══════════════════════════════════════╗\n";
  slowCout("║ Showing info for ");
  slowCout(object["name"]);
  cout << "\n╠═══════════════════════════════════════╣\n";

  if(inventoryType == "armor")
  {
    slowCout("║ ");
    slowCout("Defence: ");
    cout << object["defense"];
    slowCout("\n║ ");
    slowCout("Info: ");
    cout << object["info"];
    slowCout("\n║ ");
    slowCout("Item level: ");
    cout << object["level_required"];
    slowCout("\n║ ");
    slowCout("Item Value: ");
    cout << object["value"];
    cout << "\n╚═══════════════════════════════════════╝\n\n";
  }

  if(inventoryType == "weapon")
  {
    slowCout("║ ");
    slowCout("Damage: ");
    cout << object["damage"];
    slowCout("\n║ ");
    slowCout("Info: ");
    cout << object["info"];
    slowCout("\n║ ");
    slowCout("Item level: ");
    cout << object["level_required"];
    slowCout("\n║ ");
    slowCout("Item Value: ");
    cout << object["value"];
    cout << "\n╚═══════════════════════════════════════╝\n\n";
  }

  if(inventoryType == "food")
  {
    slowCout("║ ");
    slowCout("Saturation: ");
    cout << object["hunger_recovery"];
    slowCout("\n║ ");
    slowCout("Info: ");
    cout << object["info"];
    slowCout("\n║ ");
    slowCout("Item level: ");
    cout << object["level_required"];
    slowCout("\n║ ");
    slowCout("Item Value: ");
    cout << object["value"];
    cout << "\n╚═══════════════════════════════════════╝\n\n";
  }

  if(inventoryType == "potion")
  {
    slowCout("║ ");
    slowCout("Healing: ");
    cout << object["health_recovery"];
    slowCout("\n║ ");
    slowCout("Info: ");
    cout << object["info"];
    slowCout("\n║ ");
    slowCout("Item level: ");
    cout << object["level_required"];
    slowCout("\n║ ");
    slowCout("Item Value: ");
    cout << object["value"];
    cout << "\n╚═══════════════════════════════════════╝\n\n";
  }

  if(inventoryType == "utilitie")
  {
    slowCout("║ ");
    slowCout("Effect: ");
    cout << object["effect"];
    slowCout("\n║ ");
    slowCout("Duration: ");
    cout << object["duration"];
    slowCout("\n║ ");
    slowCout("Info: ");
    cout << object["info"];
    slowCout("\n║ ");
    slowCout("Item level: ");
    cout << object["level_required"];
    slowCout("\n║ ");
    slowCout("Item Value: ");
    cout << object["value"];
    cout << "\n╚═══════════════════════════════════════╝\n\n";
  }
}

extern void showInventory(Character &character);

void showArmorInventory(Character &character)
{
  int i=0;
  clearScreen();
  cout << "╔═══════════════════════════════════════╗\n";
  slowCout("║ "+stringToUpper(character.name));
  slowCout("'S ARMORS\n");
  cout << "╠═══════════════════════════════════════╣\n";
  for(i=0; i<character.inventory.size(); i++)
  {
    if(character.inventory[i]["type"] == "armor")
    {
      slowCout("║ ");
      slowCout(to_string(i+1)+". ");
      slowCout(character.inventory[i]["name"]);
      slowCout(" X ");
      slowCout(to_string(character.inventory[i]["count"]));
      cout << endl;
    }
  }
  cout << "╚═══════════════════════════════════════╝\n";
  cout << "\n";

  int choice;
  do{
    cout << "\nSelect a number > ";
    cin >> choice;
  }while(choice>i+1 || choice<1);

  showObject(character.inventory[choice-1], "armor");

  slowCout("Press anything to return to Inventory\n> ");
  string temp;
  cin >> temp;
  showInventory(character);
  return;
}

void showWeaponInventory(Character &character)
{
  int i=0;
  clearScreen();
  cout << "╔═══════════════════════════════════════╗\n";
  slowCout("║ "+stringToUpper(character.name));
  slowCout("'S WEAPONS\n");
  cout << "╠═══════════════════════════════════════╣\n";
  for(i =0; i<character.inventory.size(); i++)
  {
    if(character.inventory[i]["type"] == "weapon")
    {
      slowCout("║ ");
      slowCout(to_string(i+1)+". ");
      slowCout(character.inventory[i]["name"]);
      slowCout(" X ");
      slowCout(to_string(character.inventory[i]["count"]));
      cout << endl;
    }
  }
  cout << "╚═══════════════════════════════════════╝\n";
  cout << "\n";

  int choice;
  do{
    cout << "\nSelect a number > ";
    cin >> choice;
  }while(choice>i+1 || choice<1);

  showObject(character.inventory[choice-1], "weapon");
  slowCout("Enter anything to return to Inventory\n> ");
  string temp;
  cin >> temp;
  showInventory(character);
  return;
}

void showFoodInventory(Character &character)
{
  clearScreen();
  int i=0;
  cout << "╔═══════════════════════════════════════╗\n";
  slowCout("║ "+stringToUpper(character.name));
  slowCout("'S FOOD\n");
  cout << "╠═══════════════════════════════════════╣\n";
  for(i =0; i<character.inventory.size(); i++)
  {
    if(character.inventory[i]["type"] == "food")
    {
      slowCout("║ ");
      slowCout(to_string(i+1)+". ");
      slowCout(character.inventory[i]["name"]);
      slowCout(" X ");
      slowCout(to_string(character.inventory[i]["count"]));
      cout << endl;
    }
  }
  cout << "╚═══════════════════════════════════════╝\n";
  cout << "\n";

  int choice;
  do{
    cout << "\nSelect a number > ";
    cin >> choice;
  }while(choice>i+1 || choice<1);

  showObject(character.inventory[choice-1], "food");

  slowCout("Enter anything to return to Inventory\n> ");
  string temp;
  cin >> temp;
  showInventory(character);
  return;
}

void showUsablesInventory(Character &character)
{
  clearScreen();
  int i=0;
  cout << "╔═══════════════════════════════════════╗\n";
  slowCout("║ "+stringToUpper(character.name));
  slowCout("'S USABLES\n");
  cout << "╠═══════════════════════════════════════╣\n";
    for(i =0; i<character.inventory.size(); i++)
  {
    if(character.inventory[i]["type"] == "usable")
    {
      slowCout("║ ");
      slowCout(to_string(i+1)+". ");
      slowCout(character.inventory[i]["name"]);
      slowCout(" X ");
      slowCout(to_string(character.inventory[i]["count"]));
      cout << endl;
    }
  }
  cout << "╚═══════════════════════════════════════╝\n";
  cout << "\n";

  int choice;
  do{
    cout << "\nSelect a number > ";
    cin >> choice;
  }while(choice>i+1 || choice<1);

  showObject(character.inventory[choice-1], "usable");

  slowCout("Enter anything to return to Inventory\n> ");
  string temp;
  cin >> temp;
  showInventory(character);
  return;
}

void showUtilitiesInventory(Character &character)
{
  clearScreen();
  int i=0;
  cout << "╔═══════════════════════════════════════╗\n";
  slowCout("║ "+stringToUpper(character.name));
  slowCout("'S UTILITIES\n");
  cout << "╠═══════════════════════════════════════╣\n";
  for(i =0; i<character.inventory.size(); i++)
  {
    if(character.inventory[i]["type"] == "utilitie")
    {
      slowCout("║ ");
      slowCout(to_string(i+1)+". ");
      slowCout(character.inventory[i]["name"]);
      slowCout(" X ");
      slowCout(to_string(character.inventory[i]["count"]));
      cout << endl;
    }
  }
  cout << "╚═══════════════════════════════════════╝\n";
  cout << "\n";

  int choice;
  do{
    cout << "\nSelect a number > ";
    cin >> choice;
  }while(choice>i+1 || choice<1);

  showObject(character.inventory[choice-1], "utilitie");

  slowCout("Enter anything to return to Inventory\n> ");
  string temp;
  cin >> temp;
  showInventory(character);
  return;
}

void showPotionsInventory(Character &character)
{
  clearScreen();
  int i=0;
  cout << "╔═══════════════════════════════════════╗\n";
  slowCout("║ "+stringToUpper(character.name));
  slowCout("'S POTIONS\n");
  cout << "╠═══════════════════════════════════════╣\n";
  for(i =0; i<character.inventory.size(); i++)
  {
    if(character.inventory[i]["type"] == "potion")
    {
      slowCout("║ ");
      slowCout(to_string(i+1)+". ");
      slowCout(character.inventory[i]["name"]);
      slowCout(" X ");
      slowCout(to_string(character.inventory[i]["count"]));
      cout << endl;
    }
  }
  cout << "╚═══════════════════════════════════════╝\n";
  cout << "\n";

  int choice;
  do{
    cout << "\nSelect a number > ";
    cin >> choice;
  }while(choice>i+1 || choice<1);

  showObject(character.inventory[choice-1], "potion");

  slowCout("Enter anything to return to Inventory\n> ");
  string temp;
  cin >> temp;
  showInventory(character);
  return;
}

void showInventory(Character &character)
{
  clearScreen();
  cout << "╔═══════════════════════════════════════════════════════╗\n";
  slowCout("║ "+stringToUpper(character.name));
  slowCout("'S INVENTORY\n");
  cout << "╠═══════════════════════════════════════════════════════╣\n";
  slowCout("║ What part of the inventory would you like to see");
  slowCout("\n║ 1. Armors");
  slowCout("\n║ 2. Weapons");
  slowCout("\n║ 3. Food");
  slowCout("\n║ 4. Usables");
  slowCout("\n║ 5. Utilities");
  slowCout("\n║ 6. Potions");
  slowCout("\n║ 7. Back to Profile");
  cout << "\n╚══════════════════════════════════════════════════════╝\n";
  
  int choice;
  do{
    cout << "\nSelect a number > ";
    cin >> choice;
  }while(choice>7 || choice<1);

  switch (choice) {
    case 1:
      showArmorInventory(character);
      break;;
    case 2:
      showWeaponInventory(character);
      break;
    case 3:
      showFoodInventory(character); 
      break;
    case 4:
      showUsablesInventory(character); 
      break;
    case 5:
      showUtilitiesInventory(character); 
      break;
    case 6:
      showPotionsInventory(character);
      break;
    case 7:
      return;
    default:
    return;
  }
  return;
}

void showAbilities(Character &character)
{
  clearScreen();
  cout << "╔═══════════════════════════════════════╗\n";
  slowCout("║ "+stringToUpper(character.name));
  slowCout("'S ABILITIES\n");
  cout << "╠═══════════════════════════════════════╣\n";
  cout << "╠═══════════════════════════════════════╣\n";
  cout << "║ DEFENSE: ";
  slowCout(to_string(character.defense));
  cout << "\n║ CRITICAL: ";
  slowCout(to_string(character.critical));
  cout << "\n║ DEXTERITY: ";
  slowCout(to_string(character.dexterity));
  cout << "\n║ STRENGHT: ";
  slowCout(to_string(character.strength));
  cout << "\n╚═══════════════════════════════════════╝\n";

  cout << "\n";
  slowCout("Press anything to return to Profile");
  string temp;
  cin >> temp;
  return;
}

void profile(Character &character)
{
  clearScreen();
  /*
    "╔═══════════════════════════════════════╗
     ║  + ["name"] + "'s PROFILE"            ║
     ╠═══════════════════════════════════════╣
    -║ NAME:                                 ║
    -║ RACE:                                 ║
    -║ SEX:                                  ║
    -╠═══════════════════════════════════════╣
    -║ COINS:                                ║
    -║ LEVEL: currexp/nextexp  lvl           ║
    -║ HEALTH: curr/max                      ║
    -║ MANA: curr/max                        ║
    -║ TURN:                                 ║
    -║ CURRENT LOCATION:                     ║
    -╠═══════════════════════════════════════╣
    -║ GAME MODE: Easy/Normal/Hard/Extreme   ║
    -║ KILLS: value (classifica globale)     ║
    -║ DEATHS: value (classifica globale)    ║
    -║ KILLS/DEATHS RATIO: value (class. gl.)║
    -╠═══════════════════════════════════════╣
    -║ INVENTARIO                            ║
    -║ ABILITA'                              ║
  */ 
  cout << "╔═══════════════════════════════════════╗\n";
  slowCout("║ "+stringToUpper(character.name));
  slowCout("'S PROFILE\n");
  cout << "╠═══════════════════════════════════════╣\n";

  cout << "╠═══════════════════════════════════════╣\n";
  cout << "║ NAME: ";
  slowCout(character.name);
  cout << "\n║ RACE: ";
  slowCout(character.race);
  cout << "\n║ SEX: ";
  slowCout(character.sex);
  cout << "\n╠═══════════════════════════════════════╣\n";
  cout << "║ COINS: ";
  slowCout(to_string(character.coins));
  cout << "\n║ LEVEL: ";
  slowCout(to_string(character.level));
  cout << "\n║ HEALTH: ";
  slowCout(to_string(character.health));
  slowCout(" / ");
  slowCout(to_string(character.max_health));
  cout << "\n║ MANA: ";
  slowCout(to_string(character.mana));
  slowCout(" / ");
  slowCout(to_string(character.max_mana));
  cout << "\n╠═══════════════════════════════════════╣\n";
  cout << "║ TURN: ";
  slowCout(to_string(character.current_turn));
  cout << "\n║ CURRRENT LOCATION: ";
  slowCout(findPosition(character.current_dungeon));
  cout << "\n║ GAME MODE: ";
  slowCout(character.difficulty);
  cout << "\n╠═══════════════════════════════════════╣\n";
  cout << "║ KILLS: ";
  slowCout(to_string(character.tot_kills));
  cout << "\n║ DEATHS: ";
  slowCout(to_string(character.deaths));
  cout << "\n║ KDR: ";
  //slowCout(to_string(character.tot_kills / character.deaths));
  cout << "\n╠═══════════════════════════════════════╣\n";
  cout << "║ For INVENTORY, type 1...";
  cout << "\n║ For ABILITIES, type 2...";
  cout << "\n╚═══════════════════════════════════════╝\n";
  cout << endl;

  cout << "What would you like to do?";
  cout << "\n1. Check my inventory\n2. Check my abilities\n3. Return to the Main Menu\n";

  int choice;
  do{
    cout << "\nSelect a number > ";
    cin >> choice;
  }while(choice>3 || choice<1);

  switch (choice) {
    case 1:
      showInventory(character);
      break;;
    case 2:
      showAbilities(character);
      break;
    case 3:
       return;
  }
  profile(character);
  return;
}

void mhaPub (Character character)
{
  clearScreen();
  int youWantToBeDrunk = 0;
  int timesPassedOut = 0;
  pu:
  slowCout("You are now in the Pub.\n");
  slowCout("What would you like to do?\n");
  slowCout("1. Buy ale (2 coins)\n");
  slowCout("2. Buy beer (4 coins)\n");
  slowCout("3. Buy whisky (20 coins)\n");
  slowCout("4. Back to the Association hall\n");

  slowCout("\nYou have ");
  slowCout(to_string(character.coins));
  slowCout(" coins");

  int choice;
  do{
    cout << "\nSelect a number > ";
    cin >> choice;
  }while(choice>4 || choice<1);

  switch (choice) {
    case 1:
      if (character.coins >= 2) {
        character.coins -= 2;
        clearScreen();
        cout << "Purchased Ale for 2 coins\n";
      } else {
        clearScreen();
        cout << "Not enough coins to buy Ale. You really are that poor\n";
      }
      youWantToBeDrunk +=2;
      if (youWantToBeDrunk >= 20)
      {
        slowCout("You drunk too much alchool and passed out, you lost 5HP\n");
        slowCout(". . . . . ", 400);
        slowCout("\nYou are now awake\n");
        character.health -= 5;
        timesPassedOut += 1;
        youWantToBeDrunk = 0;
      }
      if (timesPassedOut >= 3)
      {
        slowCout("You really love alchool, maybe too much, everything goes dark and you die\n\n");
        timesPassedOut = 0;
        character.health = 0;
      }
      break;
    case 2:
      if (character.coins >= 4) {
        character.coins -= 4;
        clearScreen();
        cout << "Purchased Beer for 4 coins\n";
      } else {
        clearScreen();
        cout << "Not enough coins to buy Beer, very sad :( " << ".\n";
      }
      youWantToBeDrunk +=4;
      if (youWantToBeDrunk >= 20)
      {
        slowCout("You drunk too much alchool and passed out, you lost 6HP\n");
        slowCout(". . . . . ", 400);
        slowCout("\nYou are now awake\n");
        character.health -= 6;
        timesPassedOut += 1;
        youWantToBeDrunk = 0;
      }
      if (timesPassedOut >= 3)
      {
        slowCout("You really love alchool, maybe too much, everything goes dark and you die\n\n");
        timesPassedOut = 0;
        character.health = 0;
      }
      break;
    case 3:
      if (character.coins >= 20) {
        character.coins -= 20;
        clearScreen();
        cout << "Purchased Whisky for 20 coins\n";
      } else {
        clearScreen();
        cout << "Not enough coins to buy Whisky, you are poor" << ".\n";
      }
      youWantToBeDrunk +=10;
      if (youWantToBeDrunk >= 20)
      {
        slowCout("You drunk too much alchool and passed out, you lost 10HP\n");
        slowCout(". . . . . ", 400);
        slowCout("\nYou are now awake\n");
        character.health -= 10;
        timesPassedOut += 1;
        youWantToBeDrunk = 0;
      }
      if (timesPassedOut >= 3)
      {
        slowCout("You really love alchool, maybe too much, everything goes dark and you die\n\n");
        timesPassedOut = 0;
        character.health = 0;
      }
      break;
    default:
      return;
  }
  goto pu;
}

void mha_menu(Character character) {
  clearScreen();
  character.current_dungeon = -3;
  character.write_character_to_json(character);
  slowCout("You are now in the Monster Hunter Association.\n");
  slowCout("What would you like to do?\n");
  slowCout("1. Go to the Dungeons\n");
  slowCout("2. Check the Hall of Fame\n");
  slowCout("3. Go to the pub\n");
  slowCout("4. Exit the Association\n");
  int choice;
  do{
    cout << "\nSelect a number > ";
    cin >> choice;
  }while(choice>4 || choice<1);

  switch (choice) {
    case 1:
      // dungeonsMenu(character);
      break;
    case 2:{
      json leaderboards_data = load_leaderboards_data("ideal_leads.json");
      leaderboards_menu(leaderboards_data);
    }
      break;
    case 3:
      mhaPub(character);
      break;
    default:
      return;
  }
  mha_menu(character);
}

void main_menu(Character character) {
  character.current_dungeon = 0;
  character.write_character_to_json(character);
  clearScreen();
  slowCout("You are now in the main part of the capital city of Valoria.\n");
  slowCout("What would you like to do?\n");
  slowCout("1. Go to the shop\n");
  slowCout("2. Go to the Monster Hunter Association\n");
  slowCout("3. Check your profile\n");
  slowCout("4. Quit game\n");
  int choice;
  do{
    cout << "\nSelect a number > ";
    cin >> choice;
  }while(choice>5 || choice<1);

  switch (choice) {
    case 1:
      shop(character);
      break;
    case 2:
      mha_menu(character);
      break;
    case 3:
      profile(character);
      break;
    default:
      return;
  }
  main_menu(character);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void start_game(Character &character) {
  string replacementForCinIgnore;
  string tutChoice;
  // Aspetta 5 secondi prima di iniziare il gioco
  this_thread::sleep_for(chrono::seconds(4));
  clearScreen();

  // mettere il giocatore nel posto ultimo salvato, o iniziare con introduzione
  // se current_dungeon == -1
  if (character.current_dungeon == -1) {
    slowCout(
        " __          ________ _      _____ ____  __  __ ______   _______ ____ "
        "                \n \\ \\        / /  ____| |    / ____/ __ \\|  \\/  "
        "|  ____| |__   __/ __ \\                \n  \\ \\  /\\  / /| |__  | | "
        "  | |   | |  | | \\  / | |__       | | | |  | |               \n   \\ "
        "\\/  \\/ / |  __| | |   | |   | |  | | |\\/| |  __|      | | | |  | | "
        "              \n    \\  /\\  /  | |____| |___| |___| |__| | |  | | "
        "|____     | | | |__| |               \n  ___\\/  "
        "\\/___|______|______\\_____\\____/|_|  |_|______|___ |_|  \\____/__ _ "
        "  _  _____ \n |  __ \\|  ____| |    |_   _/ ____|/ ____|   ___    |  "
        "__ \\| |  | |_   _| \\ | |/ ____|\n | |__) | |__  | |      | || |    "
        "| (___    ( _ )   | |__) | |  | | | | |  \\| | (___  \n |  _  /|  __| "
        "| |      | || |     \\___ \\   / _ \\/\\ |  _  /| |  | | | | | . ` "
        "|\\___ \\ \n | | \\ \\| |____| |____ _| || |____ ____) | | (_>  < | | "
        "\\ \\| |__| |_| |_| |\\  |____) |\n |_|  "
        "\\_\\______|______|_____\\_____|_____/   \\___/\\/ |_|  "
        "\\_\\\\____/|_____|_| \\_|_____/ \n                                   "
        "                                                   \n                 "
        "                                                                     ",
        5);
    slowCout("\nWould you like to skip the tutorial?\n> ");
    cin >> tutChoice;
    if (stringToLower(tutChoice) == "yes") {
      character.current_dungeon = 0;
      start_game(character);
      return;
    }

    slowCout(
        "\nIt's a brisk morning, and the first rays of sunlight begin to warm "
        "the chilly air as you make your way to the association. \nThe path is "
        "familiar, but today, every step feels heavier, charged with "
        "anticipation. After years of waiting, you're finally here, standing "
        "at the threshold, 18 and ready to join.\nThe building stands tall and "
        "welcoming, with the association's emblem proudly displayed by the "
        "entrance. \nYou take a deep breath and step inside, feeling a strange "
        "mix of nerves and excitement. \nThe reception area is bustling, with "
        "people chatting and moving about, each seemingly caught up in their "
        "own purpose. \nYou feel an odd sense of belonging, this is where you've "
        "always wanted to be, and today, it's happening.\nApproaching the "
        "front desk there is a red haired cute girl waiting, you hand over "
        "your ID with a subtle grin, savoring the moment. The receptionist "
        "smiles knowingly, having seen this scene many times before, and "
        "says,\n\n \"Happy birthday! Excited to finally join? My name's Rosie, "
        "and I'll be your guide through the new chapter of your life!\nFollow "
        "me, we have to finish some formal paper works, then I'll be honored "
        "to let you know in depth your job and how to do it well!\"\n\nA rush "
        "of pride washes over you as you nod, and she gestures toward a set of "
        "double doors at the end of the hall.\nYou walk through, and the room "
        "beyond has an almost ceremonial feel. You see walls lined with framed "
        "photos of previous members, a legacy of sorts, and you feel a "
        "connection to the history, as though your name, too, will someday "
        "join those ranks, becoming a DUNGEONS CLEARER!\nThe official "
        "enrollment process is straightforward but significant: signing your "
        "name in the registry, filling out some final paperwork, and "
        "confirming your dedication to the association's values. \nWhen you "
        "finish, Rosie hands you a membership badge with your name engraved on "
        "it, still warm from the print. It feels real, solid—an achievement.");
    if (!character.hasItem("Association Badge")) {
      character.addItem({{"type", "badge"}, {"name", "Association Badge"}},
                        character);
    }
    cout << "\n";
    cout << "Enter anything to contunue\n";
    cin >> replacementForCinIgnore;
    clearScreen();

    slowCout(
        "Rosie:\n\"Well, welcome to the Association " + character.name +
        "!\nNow let me introduce you to the association, as you may know, your "
        "objective is to clear the dungeons that are in the basement, do not "
        "worry I'll show you them as soon as we can. \nTo clear those dungeons "
        "you'll need to defeat the Dungeon's keeper, you may see it like a "
        "boss of the dungeon, they are very strong creatures able to destroy "
        "one's career in a matter of seconds.\nDo not worry, the other "
        "monsters of the dungeons are very easy to beat, as long as they not "
        "come in horde, because they are like insects, weak alone, strong in "
        "group.\n\nNow, you caneasily understand that killing those monsters "
        "is not an easy task without the proper equipment. For this reason we "
        "of the association will give you 100 Coins to buy your first armor, "
        "weapon and maybe utility.\nSince we are in the capital Eràn you'll be "
        "able to find many shops and stores of any kind, you may chose your "
        "equipment according to yourfighting style.\n\nOk now, please take a "
        "rest in our motel, The Golden Pidgeon, tomorrow I'll help you find "
        "the shops, and after the dungeons.\"");
    cout << "\n";
    cout << "Enter anything to contunue\n";
    cin >> replacementForCinIgnore;
    clearScreen();
    slowCout("Sleeping, at the Golden Pidgeon", 100);
    slowCout(". . . . . . . .", 1000);
    cout << "\n";
    cout << "Enter anything to contunue\n";
    cin >> replacementForCinIgnore;
    clearScreen();

    slowCout(
        "The soft morning light seeps through the small window of your room at "
        "The Golden Pigeon, casting a gentle glow over the simple but cozy "
        "space. Stretching, you feel the excitement and nerves of the previous "
        "day slowly start to return. Today's the day you'll begin your first "
        "steps as a Dungeon Clearer.\n\nAfter freshening up, you make your way "
        "downstairs, greeted by the sounds and smells of a bustling morning. "
        "You spot Rosie by the entrance, waiting with a welcoming smile.\n\n");
    slowCout("Rosie:\n\"Good morning! Did you sleep well? It's a big day "
             "today,\" she says with a grin. \"Now, let me take you on a short "
             "tour around Eràn, and then we'll head down to the association "
             "basement where the dungeons are located.\"\n\n");
    cout << "Enter anything to contunue\n";
    cin >> replacementForCinIgnore;
    clearScreen();

    slowCout("You follow Rosie out of The Golden Pigeon and into the heart of "
             "Eràn, the capital city. The streets are bustling with people of "
             "all kinds: merchants calling out their goods, adventurers "
             "gathered in small groups, and children running around, eyes wide "
             "with wonder. Rosie points out several important spots as you go, "
             "including the various guilds, a few inns, and, most importantly, "
             "the local armory and supply shops.\n\n");
    slowCout("Rosie:\n\"As a Monster Hunter, you'll want to invest wisely in "
             "your equipment. This is where you'll find everything from "
             "weapons to potions and magical items. Just remember, your first "
             "purchase should match your style, whether it's something more "
             "defensive or offensive.\"\n\n");
    cout << "Enter anything to contunue\n";
    cin >> replacementForCinIgnore;
    clearScreen();

    slowCout(
        "After the short tour, Rosie leads you back to the association "
        "building. Passing through the main hall, you reach a set of thick, "
        "reinforced doors marked with an emblem. Rosie pushes them open, "
        "revealing a dark staircase descending into the basement.\n\n");
    slowCout(
        "Rosie:\n\"Here's where your journey begins,\" she says with a note of "
        "pride. \"The dungeons lie below, and each dungeon has its own unique "
        "challenges. The first one isn't as tough as the others, but the "
        "Dungeon Keeper still poses a significant threat.\"\n\n");
    slowCout(
        "You feel the weight of your responsibility settling on your "
        "shoulders. The basement air is cool and smells faintly of stone and "
        "earth, as if centuries of battles and challenges have left their mark "
        "on the atmosphere. You take a deep breath, preparing yourself.\n\n");

    cout << "Enter anything to contunue\n";
    cin >> replacementForCinIgnore;
    clearScreen();
    slowCout(
        "Rosie:\n\"But first, you'll need the right equipment. The association "
        "has provided you with 100 Coins, so head over to the shops and make "
        "sure you're well prepared. Once you're ready, come back here, and "
        "I'll show you the way to the dungeon entrance.\"\n\n");

    cout << "Enter anything to contunue\n";
    cin >> replacementForCinIgnore;
    clearScreen();

    shop(character);

    main_menu(character);

  } else if (character.current_dungeon == 0) {
    main_menu(character);
  } else if (character.current_dungeon == -2) {
    shop(character);
  } else if (character.current_dungeon == -3) {
    mha_menu(character);
  } else {
  }
  main_menu(character);
  return;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void select_char() {
  string scelta = "";
  do {
    cout << "Do you want to start from scratch? (YES or NO)" << endl;
    cin >> scelta;
    if (stringToLower(scelta) == "no") {
      json characters;
      ifstream char_file("characters.json");
      if (char_file.is_open()) {
        char_file >> characters;
        char_file.close();
        string char_name;

        clearScreen();
        // Stampo i vari personaggi chiedendo fra quali scegliere
        cout << "___________________\nSELECT YOUR "
                "CHARACTER!\n___________________"
             << endl;
        for (const auto &character : characters["characters"]) {
          cout << "Name: " << character["name"] << endl;
          cout << "Race: " << character["race"] << endl;
          cout << "Sex: " << character["sex"] << endl;
          cout << "Coins: " << character["coins"] << endl;
          cout << "Level: " << character["level"] << endl;
          cout << "Position: " << findPosition(character["current_dungeon"])
               << endl;
          cout << "___________________" << endl;
        }

        bool x = true;
        do {
          cout << "\nWhat's the name of the choosen character?\n> ";
          cin >> char_name;
          for (const auto &character : characters["characters"]) {
            if (stringToLower(character["name"]) == stringToLower(char_name)) {
              Character chosen_char = fromJSONtoCharacter(character);
              clearScreen();
              cout << "You selected: " << chosen_char.name << " (Level "
                   << chosen_char.level << ")\n";
              x = false;
              start_game(chosen_char);
              return;
            }
          }
        } while (x);

      } else {
        clearScreen();
        cout << "No characters found. Please start a new game." << endl;
        select_char();
      }
    } else if (stringToLower(scelta) == "yes") {
      string n, r, s, d;

      cout << "Create your character:\nName: ";
      cin >> n;

      do {
        cout << "Race (select from Human, Elf, Dwarf, Orc, Halfling, Tiefling, "
                "Gnome, Goblin, Kobold, Hobbit): ";
        cin >> r;
      } while (find(racesFirst, racesLast, r) == RACES.end());

      do {
        cout << "Sex (insert M or F): ";
        cin >> s;
      } while (s != "M" && s != "F");

      do {
        cout << "Choose your difficulty:\n• Easy\n• Normal\n• Hard\n• Extreme: ";
        cin >> d;
      } while (find(difficultiesFirst, difficultiesLast, d) == DIFFICULTIES.end());

      Character chosen_char(n, r, s, d);

      clearScreen();
      cout << "You created: " << chosen_char.name << " (Level "
           << chosen_char.level << ")\n";
      chosen_char.write_character_to_json(chosen_char);
      start_game(chosen_char);
      return;
    }
  } while (scelta != "YES" && scelta != "NO");
}



int main() {
  clearScreen();
  select_char();
  return 0;
}
