#include "SqliteDataBase.h"
#include "Game.h"

// global mutex 
std::mutex lockDatabase;

// Opens the connection
bool SqliteDataBase::open()
{
    std::lock_guard<std::mutex> databaseLock(lockDatabase);

    // Open the database connection
    this->res = sqlite3_open(dbFileName.c_str(), &db);

    if (res != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    char* errMessage = nullptr;

    // SQL requests
    const char* createUSERS = "CREATE TABLE IF NOT EXISTS USERS (ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, USERNAME TEXT NOT NULL, PASSWORD TEXT NOT NULL, EMAIL TEXT NOT NULL);";
    const char* createQUESTIONS = "CREATE TABLE IF NOT EXISTS QUESTIONS ( ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, QUESTION TEXT NOT NULL UNIQUE, OPTION_1_CORRECT TEXT NOT NULL, OPTION_2 TEXT NOT NULL, OPTION_3 TEXT NOT NULL, OPTION_4 TEXT NOT NULL);";
    const char* createQUESTION1 = "INSERT OR IGNORE INTO QUESTIONS (QUESTION, OPTION_1_CORRECT, OPTION_2, OPTION_3, OPTION_4) VALUES ('What is the capital of France?', 'Paris', 'London', 'Berlin', 'Madrid');";
    const char* createQUESTION2 = "INSERT OR IGNORE INTO QUESTIONS (QUESTION, OPTION_1_CORRECT, OPTION_2, OPTION_3, OPTION_4) VALUES ('Who was the first President of the United States?', 'George Washington', 'Thomas Jefferson', 'Abraham Lincoln', 'John Adams');";
    const char* createQUESTION3 = "INSERT OR IGNORE INTO QUESTIONS (QUESTION, OPTION_1_CORRECT, OPTION_2, OPTION_3, OPTION_4) VALUES ('What is the main ingredient in guacamole?', 'Avocado', 'Tomato', 'Onion', 'Lemon');";
    const char* createQUESTION4 = "INSERT OR IGNORE INTO QUESTIONS (QUESTION, OPTION_1_CORRECT, OPTION_2, OPTION_3, OPTION_4) VALUES ('What is the chemical symbol for gold?', 'Au', 'Ag', 'Fe', 'Cu');";
    const char* createQUESTION5 = "INSERT OR IGNORE INTO QUESTIONS (QUESTION, OPTION_1_CORRECT, OPTION_2, OPTION_3, OPTION_4) VALUES ('Who wrote the play Romeo and Juliet?', 'William Shakespeare', 'Jane Austen', 'Charles Dickens', 'Mark Twain');";
    const char* createQUESTION6 = "INSERT OR IGNORE INTO QUESTIONS (QUESTION, OPTION_1_CORRECT, OPTION_2, OPTION_3, OPTION_4) VALUES ('Which country won the FIFA World Cup in 2018?', 'France', 'Germany', 'Brazil', 'Spain');";
    const char* createQUESTION7 = "INSERT OR IGNORE INTO QUESTIONS (QUESTION, OPTION_1_CORRECT, OPTION_2, OPTION_3, OPTION_4) VALUES ('What does HTML stand for?', 'Hypertext Markup Language', 'Hyperlinks and Text Markup Language', 'Highly Textured Multilayer Language', 'Home Tool Markup Language');";
    const char* createQUESTION8 = "INSERT OR IGNORE INTO QUESTIONS (QUESTION, OPTION_1_CORRECT, OPTION_2, OPTION_3, OPTION_4) VALUES ('What is the main ingredient in sushi?', 'Rice', 'Fish', 'Seaweed', 'Soy Sauce');";
    const char* createQUESTION9 = "INSERT OR IGNORE INTO QUESTIONS (QUESTION, OPTION_1_CORRECT, OPTION_2, OPTION_3, OPTION_4) VALUES ('Which animal is known as the King of the Jungle?', 'Lion', 'Tiger', 'Elephant', 'Gorilla');";
    const char* createQUESTION10 = "INSERT OR IGNORE INTO QUESTIONS (QUESTION, OPTION_1_CORRECT, OPTION_2, OPTION_3, OPTION_4) VALUES ('What is the longest river in the world?', 'Nile', 'Amazon', 'Yangtze', 'Mississippi');";
    const char* createSTATISTICS = "CREATE TABLE IF NOT EXISTS STATISTICS(ID INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, USERNAME TEXT NOT NULL UNIQUE, TOTAL_GAMES_PLAYED INTEGER NOT NULL, AVERAGE_ANSWER_TIME REAL NOT NULL, NUM_CORRECT_ANSWERS INTEGER NOT NULL, NUM_TOTAL_ANSWERS INTEGER NOT NULL, PLAYER_SCORE INTEGER NOT NULL);";

    // Execute SQL statements
    res = sqlite3_exec(db, createUSERS, nullptr, nullptr, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
        return false;
    }

    res = sqlite3_exec(db, createQUESTIONS, nullptr, nullptr, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
        return false;
    }

    res = sqlite3_exec(db, createQUESTION1, nullptr, nullptr, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
        return false;
    }

    res = sqlite3_exec(db, createQUESTION2, nullptr, nullptr, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
        return false;
    }

    res = sqlite3_exec(db, createQUESTION3, nullptr, nullptr, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
        return false;
    }

    res = sqlite3_exec(db, createQUESTION4, nullptr, nullptr, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
        return false;
    }

    res = sqlite3_exec(db, createQUESTION5, nullptr, nullptr, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
        return false;
    }

    res = sqlite3_exec(db, createQUESTION6, nullptr, nullptr, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
        return false;
    }

    res = sqlite3_exec(db, createQUESTION7, nullptr, nullptr, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
        return false;
    }

    res = sqlite3_exec(db, createQUESTION8, nullptr, nullptr, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
        return false;
    }

    res = sqlite3_exec(db, createQUESTION9, nullptr, nullptr, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
        return false;
    }

    res = sqlite3_exec(db, createQUESTION10, nullptr, nullptr, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
        return false;
    }

    res = sqlite3_exec(db, createSTATISTICS, nullptr, nullptr, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
        return false;
    }

    return true;
}

// Closes the connection
bool SqliteDataBase::close()
{
    std::lock_guard<std::mutex> databaseLock(lockDatabase);

    if (db)
    {
        sqlite3_close(db);
        db = nullptr;
        return true;
    }
    return false;
}

// Checks if user exists (used for signup and other things)
int SqliteDataBase::doesUserExist(const std::string& userName, const std::string& tableTosearchIn)
{
    // SQL request to find user
    std::string findUser = "SELECT * FROM " + tableTosearchIn + " WHERE USERNAME = '" + userName + "'";

    char* errMessage = nullptr;
    int doesUserExist = 0; // Initialize with 0

    std::lock_guard<std::mutex> databaseLock(lockDatabase);

    // Execute SQL statement
    int res = sqlite3_exec(db, findUser.c_str(), callbackDoesMatch, &doesUserExist, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error : " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
    }

    return doesUserExist;
}

// Check if the user's password matches (for login)
int SqliteDataBase::doesPasswordMatch(std::string userName, std::string givenPassword)
{
    // SQL request to find user
    std::string findUser = "SELECT * FROM USERS WHERE USERNAME = '" + userName + "' AND PASSWORD = '" + givenPassword + "';";

    char* errMessage = nullptr;
    int doesPasswordMatch = 0;

    std::lock_guard<std::mutex> databaseLock(lockDatabase);

    // Execute SQL statement
    int res = sqlite3_exec(db, findUser.c_str(), callbackDoesMatch, &doesPasswordMatch, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error : " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
    }

    return doesPasswordMatch;
}

int SqliteDataBase::callbackDoesMatch(void* data, int argc, char** argv, char** azColName)
{
    int* doesMatch = static_cast<int*>(data);
    if (argv[0] != nullptr) {
        *doesMatch = 1; // match exists
    }
    else {
        *doesMatch = 0; // match doesn't exist
    }

    return 0; // Success
}

// Adds new user to table (sign up)
int SqliteDataBase::addNewUser(std::string userName, std::string userPassword, std::string userEmail)
{
    // SQL request to insert a new user
    std::string addUser = "INSERT INTO USERS (USERNAME, PASSWORD, EMAIL) VALUES ('" + userName + "', '" + userPassword + "' , '" + userEmail + "' );";

    char* errMessage = nullptr;

    std::lock_guard<std::mutex> databaseLock(lockDatabase);

    // Execute SQL statements
    res = sqlite3_exec(db, addUser.c_str(), nullptr, nullptr, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
        return 1;
    }

    return 0;
}


// Adds new user to the statistcs table
int SqliteDataBase::addNewUserToStatistics(const std::string& userName)
{
    // SQL request to insert a new user
    std::string addUser = "INSERT INTO STATISTICS (USERNAME, TOTAL_GAMES_PLAYED, AVERAGE_ANSWER_TIME, NUM_CORRECT_ANSWERS, NUM_TOTAL_ANSWERS, PLAYER_SCORE) VALUES ('" + userName + "',0, 0, 0, 0, 0);";

    char* errMessage = nullptr;

    std::lock_guard<std::mutex> databaseLock(lockDatabase);

    // Execute SQL statements
    res = sqlite3_exec(db, addUser.c_str(), nullptr, nullptr, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
        return 1;
    }

    return 0;
}

// Gets the questions with given ID
std::list<Question> SqliteDataBase::getQuestions(const int& amount)
{
    std::list<Question> questionList;
    int questionId = 0;
    char* errMessage = nullptr;
    std::map<int, int> idOfSelectedQuestions; // map - for using the 'find' method
    int numOfQuestions = getTotalNumOfQuestion();
    std::srand(std::time(0));
    // Get the amount of questions in requests
    while (questionList.size() < amount)
    {
        // get random ID question
        questionId = std::rand() % numOfQuestions; // the formula take a random number (like 23741134), and modullu. it gives us a random number between 1 and the num of questions we have

        // if the question ID is 0, it means the random number could divided by the num of questions exactly. so - the number is the number of the questions we have
        if (questionId == ID_QUESTION_DEVIDE_EXACTLY)
        {
            questionId = numOfQuestions;
        }

        // check that the ID  didn't choosed yet
        if (idOfSelectedQuestions.find(questionId) != idOfSelectedQuestions.end())
        {
            // if the ID is not good - get other (in next iteration)
            continue;
        }

        // remember that this question ID choosed
        idOfSelectedQuestions.insert({ questionId, questionId });

        // get the question 
        Question questionInstance;

        std::string getQuestion = "SELECT * FROM QUESTIONS WHERE ID = '" + std::to_string(questionId) + "'";

        std::lock_guard<std::mutex> databaseLock(lockDatabase);

        // Execute SQL statements
        res = sqlite3_exec(db, getQuestion.c_str(), callbackGetQuestion, &questionInstance, &errMessage);
        if (res != SQLITE_OK) {
            std::cerr << "SQL error: " << errMessage << std::endl;
            sqlite3_free(errMessage); // Free error message
            sqlite3_close(db); // Close database
            return questionList; // Return empty list if error occurs
        }

        questionList.push_back(questionInstance);
    }

    return questionList;
}

// get the number of the questions in the database
int SqliteDataBase::getTotalNumOfQuestion()
{
    // set
    int numOfQuestions = 0;
    char* errMessage = nullptr;
    std::string getQuestion = "SELECT COUNT(ID) FROM QUESTIONS;";

    std::lock_guard<std::mutex> databaseLock(lockDatabase);

    // Execute SQL statements
    res = sqlite3_exec(db, getQuestion.c_str(), callbackGetInt, &numOfQuestions, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
        return 1;
    }

    return numOfQuestions;
}


// insert statistics of a game into the database
int SqliteDataBase::submitGameStatistics(GameData& gameData, const std::string& username)
{
    try
    {
        // if it has'nt - create a new row of statistics for the player
        if (!doesUserExist(username, TABLE_STATISTICS))
        {
            addNewUserToStatistics(username);
        }
        // update the DB
        setPlayerAverageAnswerTime((getPlayerAverageAnswerTime(username) * getNumOfPlayerGames(username) + gameData.averageAnswerTime) / (getNumOfPlayerGames(username) + ONE_GAME), username); // using formula to calculate the new avarage
        setNumOfCorrectAnswers(getNumOfCorrectAnswers(username) + gameData.correctAnswerCount, username);
        setNumOfPlayerGames(getNumOfPlayerGames(username) + ONE_GAME, username);
        setNumOfTotalAnswers(getNumOfTotalAnswers(username) + gameData.correctAnswerCount + gameData.wrongAnswerCount, username);
        setPlayerScore(getPlayerScore(username) + (gameData.correctAnswerCount * POINTS_OF_ANSWER) - (gameData.wrongAnswerCount * POINTS_OF_ANSWER), username);
    }
    catch (...)
    {
        return 1;
    }

    return 0;
}



int SqliteDataBase::callbackGetQuestion(void* data, int argc, char** argv, char** azColName)
{
    // Check if data is valid
    if (data == nullptr) {
        return 1; // Return non-zero to indicate error
    }

    // Cast data back to Question pointer
    Question* questionInstance = static_cast<Question*>(data);

    // Check if the number of columns is valid
    if (argc < 6) {
        return 1; // Return non-zero to indicate error
    }

    // Set question
    questionInstance->setQuestion(argv[1]);

    // Set possible answers
    std::vector<std::string> possibleAnswers;
    for (int i = 2; i < argc; i++) {
        possibleAnswers.push_back(argv[i]);
    }
    questionInstance->setPossibleAnswers(possibleAnswers);

    return 0;
}

float SqliteDataBase::getPlayerAverageAnswerTime(std::string userName)
{
    float averageAnswerTime = 0.0f; // Initialize to default value
    char* errMessage = nullptr;
    std::string getAATRequest = "SELECT AVERAGE_ANSWER_TIME FROM STATISTICS WHERE USERNAME = '" + userName + "';";

    std::lock_guard<std::mutex> databaseLock(lockDatabase);

    // Execute SQL statements
    res = sqlite3_exec(db, getAATRequest.c_str(), callbackGetFloat, &averageAnswerTime, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
        return 0; // Return empty list if error occurs
    }
    return averageAnswerTime;
}

int SqliteDataBase::callbackGetFloat(void* data, int argc, char** argv, char** azColName)
{
    if (argc > 0 && argv[0] != nullptr) {
        float* FloatPtr = static_cast<float*>(data);
        *FloatPtr = std::stof(argv[0]);
    }
    return 0;
}

int SqliteDataBase::getNumOfCorrectAnswers(std::string userName)
{
    int numOfCorrectAnswers = 0;
    char* errMessage = nullptr;
    std::string getNOCARequest = "SELECT NUM_CORRECT_ANSWERS FROM STATISTICS WHERE USERNAME = '" + userName + "';";

    std::lock_guard<std::mutex> databaseLock(lockDatabase);

    // Execute SQL statements
    res = sqlite3_exec(db, getNOCARequest.c_str(), callbackGetInt, &numOfCorrectAnswers, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
        return 0; // Return empty list if error occurs
    }
    return numOfCorrectAnswers;
}

int SqliteDataBase::getNumOfTotalAnswers(std::string userName)
{
    int numOfTotalAnswers = 0;
    char* errMessage = nullptr;
    std::string getNTARequest = "SELECT NUM_TOTAL_ANSWERS FROM STATISTICS WHERE USERNAME = '" + userName + "';";

    std::lock_guard<std::mutex> databaseLock(lockDatabase);

    // Execute SQL statements
    res = sqlite3_exec(db, getNTARequest.c_str(), callbackGetInt, &numOfTotalAnswers, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
        return 0; // Return empty list if error occurs
    }
    return numOfTotalAnswers;
}

int SqliteDataBase::getNumOfPlayerGames(std::string userName)
{
    int totalGamesPlayed = 0;
    char* errMessage = nullptr;
    std::string getTGPRequest = "SELECT TOTAL_GAMES_PLAYED FROM STATISTICS WHERE USERNAME = '" + userName + "';";

    std::lock_guard<std::mutex> databaseLock(lockDatabase);

    // Execute SQL statements
    res = sqlite3_exec(db, getTGPRequest.c_str(), callbackGetInt, &totalGamesPlayed, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
        return 0; // Return empty list if error occurs
    }
    return totalGamesPlayed;
}

int SqliteDataBase::getPlayerScore(std::string userName)
{
    int playerScore = 0;
    char* errMessage = nullptr;
    std::string getPlayerScoreRequest = "SELECT PLAYER_SCORE FROM STATISTICS WHERE USERNAME = '" + userName + "';";

    std::lock_guard<std::mutex> databaseLock(lockDatabase);

    // Execute SQL statements
    res = sqlite3_exec(db, getPlayerScoreRequest.c_str(), callbackGetInt, &playerScore, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
        return 0; // Return empty list if error occurs
    }
    return playerScore;
}

int SqliteDataBase::callbackGetInt(void* data, int argc, char** argv, char** azColName)
{
    if (argc > 0 && argv[0] != nullptr)
    {
        int* intPtr = static_cast<int*>(data);
        *intPtr = std::stoi(argv[0]);
    }
    return 0;
}

std::vector<int> SqliteDataBase::getHighScores()
{
    std::vector<int> playerScores;
    char* errMessage = nullptr;
    std::string getPlayerScoresQuery = "SELECT PLAYER_SCORE FROM STATISTICS;";

    std::lock_guard<std::mutex> databaseLock(lockDatabase);

    // Execute SQL statements
    int res = sqlite3_exec(db, getPlayerScoresQuery.c_str(), callbackGetHS, &playerScores, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
    }

    // keep only the 3 higest scores
    std::sort(playerScores.begin(), playerScores.end(), std::greater <>());
    std::vector<int> highScores(playerScores.begin(), playerScores.begin() + HIGEST_THREE_SCORES);

    return highScores;
}

int SqliteDataBase::callbackGetHS(void* data, int argc, char** argv, char** azColName)
{
    std::vector<int>* playerScores = static_cast<std::vector<int>*>(data);
    for (int i = 0; i < argc; i++) {
        playerScores->push_back(argv[i] ? atoi(argv[i]) : 0);
    }
    return 0;
}

// Setter function for updating player's average answer time
float SqliteDataBase::setPlayerAverageAnswerTime(float averageAnswerTime, std::string userName)
{
    char* errMessage = nullptr;
    std::string setAATRequest = "UPDATE STATISTICS SET AVERAGE_ANSWER_TIME = " + std::to_string(averageAnswerTime) + " WHERE USERNAME = '" + userName + "';";

    std::lock_guard<std::mutex> databaseLock(lockDatabase);

    // Execute SQL statements
    res = sqlite3_exec(db, setAATRequest.c_str(), nullptr, nullptr, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
        return 0.0f; // Return 0.0 if error occurs
    }
    return averageAnswerTime;
}

// Setter function for resetting the number of correct answers
void SqliteDataBase::setNumOfCorrectAnswers(int numOfCorrectAnswers, std::string userName)
{
    char* errMessage = nullptr;
    std::string setNOCARequest = "UPDATE STATISTICS SET NUM_CORRECT_ANSWERS = " + std::to_string(numOfCorrectAnswers) + " WHERE USERNAME = '" + userName + "';";

    std::lock_guard<std::mutex> databaseLock(lockDatabase);

    // Execute SQL statements
    res = sqlite3_exec(db, setNOCARequest.c_str(), nullptr, nullptr, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
    }
}

// Setter function for resetting the number of total answers
void SqliteDataBase::setNumOfTotalAnswers(int numOfTotalAnswers, std::string userName)
{
    char* errMessage = nullptr;
    std::string setNOTARequest = "UPDATE STATISTICS SET NUM_TOTAL_ANSWERS = " + std::to_string(numOfTotalAnswers) + " WHERE USERNAME = '" + userName + "';";

    std::lock_guard<std::mutex> databaseLock(lockDatabase);

    // Execute SQL statements
    res = sqlite3_exec(db, setNOTARequest.c_str(), nullptr, nullptr, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
    }
}

// Setter function for resetting the number of player games
void SqliteDataBase::setNumOfPlayerGames(int numOfPlayerGames, std::string userName)
{
    char* errMessage = nullptr;
    std::string setNOPGRequest = "UPDATE STATISTICS SET TOTAL_GAMES_PLAYED = " + std::to_string(numOfPlayerGames) + " WHERE USERNAME = '" + userName + "';";

    std::lock_guard<std::mutex> databaseLock(lockDatabase);

    // Execute SQL statements
    res = sqlite3_exec(db, setNOPGRequest.c_str(), nullptr, nullptr, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
    }
}

// Setter function for updating player's score
void SqliteDataBase::setPlayerScore(int playerScore, std::string userName)
{
    char* errMessage = nullptr;
    std::string setPSRequest = "UPDATE STATISTICS SET PLAYER_SCORE = " + std::to_string(playerScore) + " WHERE USERNAME = '" + userName + "';";

    std::lock_guard<std::mutex> databaseLock(lockDatabase);

    // Execute SQL statements
    res = sqlite3_exec(db, setPSRequest.c_str(), nullptr, nullptr, &errMessage);
    if (res != SQLITE_OK) {
        std::cerr << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage); // Free error message
        sqlite3_close(db); // Close database
    }
}