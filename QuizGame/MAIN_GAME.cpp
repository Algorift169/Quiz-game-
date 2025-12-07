#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <iomanip>
#include <limits>
using namespace std;

struct Question {
    string question;
    vector<string> options;
    char correctAnswer;
    string category;
};

struct UserProgress {
    string username;
    map<string, int> categoryProgress;
    map<string, int> categoryScore;
    map<string, int> categoryPenalty;
    int totalScore;
    int highestScore;
    int totalPenalty;
};

class QuizGame {
private:
    vector<Question> questions;
    map<string, UserProgress> users;
    UserProgress currentUser;
    string lastUser;
    string progressFile = "user_progress.txt";
    string userRegFile = "userRegistration.txt";
    string passwordFile = "passwords.txt";
    bool isLoggedIn = false;

    map<string, string> questionFiles = {
        {"Programming", "PROGRAMMING_questions.txt"},
        {"History", "HISTORY_questions.txt"},
        {"Math", "MATH_questions.txt"},
        {"General", "GENERAL_KNOWLEDGE_questions.txt"}
    };
    map<string, string> optionsFiles = {
        {"Programming", "PROGRAMMING_QUESTIONS_options.txt"},
        {"History", "HISTORY_QUESTIONS_options.txt"},
        {"Math", "MATH_QUESTIONS_options.txt"},
        {"General", "GENERAL_KNOWLEDGE_QUESTIONS_options.txt"}
    };
    map<string, string> answersFiles = {
        {"Programming", "PROGRAMMING_ANSWERS.txt"},
        {"History", "HISTORY_ANSWERS.txt"},
        {"Math", "MATH_ANSWERS.txt"},
        {"General", "GENERAL_KNOWLEDGE_ANSWERS.txt"}
    };

    string trim(const string& str) {
        size_t start = str.find_first_not_of(" \t\n\r");
        size_t end = str.find_last_not_of(" \t\n\r");
        if (start == string::npos) return "";
        return str.substr(start, end - start + 1);
    }

    char findCorrectAnswerLetter(const vector<string>& options, const string& correctAnswer) {
        for (size_t i = 0; i < options.size(); i++) {
            if (trim(options[i]) == trim(correctAnswer)) {
                return 'A' + i;
            }
        }
        return 'A';
    }

    bool isValidInput(const string& input, int min, int max) {
        if (input.empty()) return false;

        if (input.size() == 1 && (input[0] == 'q' || input[0] == 'Q')) {
            return true;
        }

        for (char c : input) {
            if (!isdigit(c)) return false;
        }

        int num = stoi(input);
        return num >= min && num <= max;
    }

    bool isYesResponse(const string& input) {
        string lowerInput = input;
        transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);
        return (lowerInput == "y" || lowerInput == "yes");
    }

    void loadAllUsers() {
        users.clear();
        lastUser = "";
        ifstream file(progressFile);
        if (!file.is_open()) return;

        string line;
        UserProgress temp;
        bool inUser = false;

        while (getline(file, line)) {
            string s = trim(line);
            if (s.size() >= 5 && s.substr(0,5) == "LAST ") {
                lastUser = trim(s.substr(5));
                continue;
            }
            if (s.size() >= 5 && s.substr(0,5) == "USER ") {
                inUser = true;
                temp = UserProgress();
                temp.username = trim(s.substr(5));
                temp.totalScore = 0;
                temp.highestScore = 0;
                temp.totalPenalty = 0;
                temp.categoryProgress.clear();
                temp.categoryScore.clear();
                temp.categoryPenalty.clear();
                continue;
            }
            if (s == "ENDUSER") {
                inUser = false;
                users[temp.username] = temp;
                continue;
            }
            if (inUser) {
                if (s.size() >= 6 && s.substr(0,6) == "TOTAL ") {
                    temp.totalScore = stoi(trim(s.substr(6)));
                } else if (s.size() >= 5 && s.substr(0,5) == "HIGH ") {
                    temp.highestScore = stoi(trim(s.substr(5)));
                } else if (s.size() >= 5 && s.substr(0,5) == "PENAL ") {
                    temp.totalPenalty = stoi(trim(s.substr(5)));
                } else if (s.size() >= 5 && s.substr(0,5) == "PROG ") {
                    string rest = trim(s.substr(5));
                    stringstream ss(rest);
                    string cat;
                    int prog;
                    if (ss >> cat >> prog) {
                        temp.categoryProgress[cat] = prog;
                    }
                } else if (s.size() >= 5 && s.substr(0,5) == "CSCR ") {
                    string rest = trim(s.substr(5));
                    stringstream ss(rest);
                    string cat;
                    int score;
                    if (ss >> cat >> score) {
                        temp.categoryScore[cat] = score;
                    }
                } else if (s.size() >= 5 && s.substr(0,5) == "CPEN ") {
                    string rest = trim(s.substr(5));
                    stringstream ss(rest);
                    string cat;
                    int penalty;
                    if (ss >> cat >> penalty) {
                        temp.categoryPenalty[cat] = penalty;
                    }
                }
            }
        }
        file.close();

        if (!lastUser.empty() && users.find(lastUser) != users.end()) {
            currentUser = users[lastUser];
        } else {
            currentUser = UserProgress();
            currentUser.username = "";
        }
    }

    void saveAllUsers() {
        ofstream file(progressFile);
        if (!file.is_open()) return;

        if (!currentUser.username.empty() && isLoggedIn) {
            file << "LAST " << currentUser.username << "\n";
        }

        for (auto& pair : users) {
            UserProgress u = pair.second;
            file << "USER " << u.username << "\n";
            file << "TOTAL " << u.totalScore << "\n";
            file << "HIGH " << u.highestScore << "\n";
            file << "PENAL " << u.totalPenalty << "\n";

            for (auto& p : u.categoryProgress) {
                file << "PROG " << p.first << " " << p.second << "\n";
            }
            for (auto& p : u.categoryScore) {
                file << "CSCR " << p.first << " " << p.second << "\n";
            }
            for (auto& p : u.categoryPenalty) {
                file << "CPEN " << p.first << " " << p.second << "\n";
            }
            file << "ENDUSER\n";
        }
        file.close();
    }

    bool checkUserExists(const string& username) {
        ifstream file(userRegFile);
        if (!file.is_open()) return false;

        string line;
        while (getline(file, line)) {
            if (trim(line) == username) {
                file.close();
                return true;
            }
        }
        file.close();
        return false;
    }

    bool verifyPassword(const string& username, const string& password) {
        ifstream userFile(userRegFile);
        ifstream passFile(passwordFile);
        if (!userFile.is_open() || !passFile.is_open()) return false;

        string userLine, passLine;
        while (getline(userFile, userLine) && getline(passFile, passLine)) {
            if (trim(userLine) == username && trim(passLine) == password) {
                userFile.close();
                passFile.close();
                return true;
            }
        }
        userFile.close();
        passFile.close();
        return false;
    }

    bool registerUser() {
        cout << "\n====== REGISTER ======\n";

        string username, password;

        // Get username
        while (true) {
            cout << "Enter username (or 'q' to cancel): ";
            getline(cin, username);
            username = trim(username);

            if (username == "q" || username == "Q") {
                cout << "Registration cancelled.\n";
                return false;
            }

            if (username.empty()) {
                cout << "Username cannot be empty!\n";
                continue;
            }

            if (checkUserExists(username)) {
                cout << "Username already exists! Try another.\n";
                continue;
            }

            break;
        }

        // Get password
        while (true) {
            cout << "Enter password: ";
            getline(cin, password);
            password = trim(password);

            if (password.empty()) {
                cout << "Password cannot be empty!\n";
                continue;
            }
            break;
        }

        // Save to files
        ofstream userFile(userRegFile, ios::app);
        ofstream passFile(passwordFile, ios::app);

        if (!userFile.is_open() || !passFile.is_open()) {
            cout << "Error: Cannot save user data!\n";
            return false;
        }

        userFile << username << "\n";
        passFile << password << "\n";

        userFile.close();
        passFile.close();

        cout << "\n✅ Registration successful!\n";

        // Ask if user wants to login immediately
        cout << "\nDo you want to login now? (y/n): ";
        string choice;
        getline(cin, choice);

        if (isYesResponse(choice)) {
            cout << "\n====== AUTO-LOGIN ======\n";
            cout << "Logging in as " << username << "...\n";

            isLoggedIn = true;
            UserProgress newUser;
            newUser.username = username;
            newUser.totalScore = 0;
            newUser.highestScore = 0;
            newUser.totalPenalty = 0;
            for (auto& p : questionFiles) {
                newUser.categoryProgress[p.first] = 0;
                newUser.categoryScore[p.first] = 0;
                newUser.categoryPenalty[p.first] = 0;
            }
            currentUser = newUser;
            users[username] = newUser;
            lastUser = username;
            saveAllUsers();

            cout << "\n✅ Auto-login successful! Welcome, " << username << "!\n";
            return true;
        }

        return false;
    }

    bool loginUser() {
        cout << "\n====== LOGIN ======\n";

        string username, password;

        // Get username
        while (true) {
            cout << "Enter username (or 'q' to cancel): ";
            getline(cin, username);
            username = trim(username);

            if (username == "q" || username == "Q") {
                cout << "Login cancelled.\n";
                return false;
            }

            if (username.empty()) {
                cout << "Username cannot be empty!\n";
                continue;
            }

            if (!checkUserExists(username)) {
                cout << "❌ No such user found!\n";
                cout << "Press 'R' to register or any other key to try again: ";
                string choice;
                getline(cin, choice);

                if (choice == "R" || choice == "r") {
                    bool loggedIn = registerUser();
                    if (loggedIn) {
                        return true;
                    }
                    cout << "\n====== LOGIN ======\n";
                    continue;
                }
                cout << "\n====== LOGIN ======\n";
                continue;
            }
            break;
        }

        // Get password with limited attempts
        int attempts = 3;
        while (attempts > 0) {
            cout << "Enter password (attempts left: " << attempts << "): ";
            getline(cin, password);
            password = trim(password);

            if (verifyPassword(username, password)) {
                cout << "\n✅ Login successful! Welcome back, " << username << "!\n";
                isLoggedIn = true;

                loadAllUsers();
                if (users.find(username) == users.end()) {
                    UserProgress newUser;
                    newUser.username = username;
                    newUser.totalScore = 0;
                    newUser.highestScore = 0;
                    newUser.totalPenalty = 0;
                    for (auto& p : questionFiles) {
                        newUser.categoryProgress[p.first] = 0;
                        newUser.categoryScore[p.first] = 0;
                        newUser.categoryPenalty[p.first] = 0;
                    }
                    users[username] = newUser;
                }
                currentUser = users[username];
                lastUser = username;
                saveAllUsers();
                return true;
            }

            attempts--;
            if (attempts > 0) {
                cout << "❌ Incorrect password!\n";
            } else {
                cout << "❌ Too many failed attempts! Returning to main menu.\n";
                return false;
            }
        }

        return false;
    }

    void logoutUser() {
        if (isLoggedIn) {
            cout << "\nLogging out " << currentUser.username << "...\n";
            saveAllUsers();
            isLoggedIn = false;
            currentUser = UserProgress();
            currentUser.username = "";
        }
    }

    void displayUserInfo() {
        if (!isLoggedIn) {
            cout << "\nNo user logged in.\n";
            return;
        }

        cout << "\n====== USER INFORMATION ======\n";
        cout << "Username: " << currentUser.username << "\n";
        cout << "Total Score: " << currentUser.totalScore << "\n";
        cout << "Highest Score: " << currentUser.highestScore << "\n";
        cout << "Total Penalty: " << currentUser.totalPenalty << "\n";

        if (!currentUser.categoryProgress.empty()) {
            cout << "\nCategory Details:\n";
            cout << setw(15) << left << "Category"
                 << setw(10) << "Progress"
                 << setw(10) << "Score"
                 << setw(10) << "Penalty" << "\n";
            cout << string(45, '-') << "\n";

            for (auto& pair : questionFiles) {
                string category = pair.first;
                int progress = currentUser.categoryProgress[category];
                int score = currentUser.categoryScore[category];
                int penalty = currentUser.categoryPenalty[category];
                int total = 0;
                for (auto& q : questions) {
                    if (q.category == category) total++;
                }

                cout << setw(15) << left << category
                     << setw(10) << to_string(progress) + "/" + to_string(total)
                     << setw(10) << score
                     << setw(10) << penalty << "\n";
            }
        }
        cout << "===============================\n";
    }

    void displayRankingScoreboard() {
        loadAllUsers();
        if (users.empty()) {
            cout << "\nNo game data found. Play a game first!\n";
            return;
        }

        vector<pair<UserProgress, int>> rankings;
        for (auto& pair : users) {
            UserProgress u = pair.second;
            int adjustedScore = u.totalScore - u.totalPenalty;
            rankings.push_back({u, adjustedScore});
        }

        sort(rankings.begin(), rankings.end(),
             [](const pair<UserProgress, int>& a, const pair<UserProgress, int>& b) {
                if (a.second != b.second) return a.second > b.second;
                return a.first.totalPenalty < b.first.totalPenalty;
             });

        cout << "\n========== RANKING SCOREBOARD ==========\n";
        cout << setw(5) << "Rank"
             << setw(15) << "Player"
             << setw(10) << "Score"
             << setw(10) << "Penalty"
             << setw(15) << "Adjusted"
             << setw(10) << "Highest" << "\n";
        cout << string(65, '-') << "\n";

        for (size_t i = 0; i < rankings.size(); i++) {
            const UserProgress& u = rankings[i].first;
            int adjustedScore = rankings[i].second;

            cout << setw(5) << i+1
                 << setw(15) << u.username
                 << setw(10) << u.totalScore
                 << setw(10) << u.totalPenalty
                 << setw(15) << adjustedScore
                 << setw(10) << u.highestScore << "\n";
        }
        cout << "==========================================\n";
    }

    void viewHighestScore() {
        loadAllUsers();
        if (users.empty()) {
            cout << "\nNo game data found. Play a game first!\n";
            return;
        }

        vector<UserProgress> list;
        for (auto& p : users) list.push_back(p.second);

        sort(list.begin(), list.end(), [](const UserProgress& a, const UserProgress& b){
            return a.highestScore > b.highestScore;
        });

        cout << "\n========== LEADERBOARD (Highest Scores) ==========\n";
        cout << setw(5) << "Rank"
             << setw(15) << "Player"
             << setw(15) << "Highest Score" << "\n";
        cout << string(35, '-') << "\n";

        for (size_t i = 0; i < list.size(); i++) {
            cout << setw(5) << i+1
                 << setw(15) << list[i].username
                 << setw(15) << list[i].highestScore << "\n";
        }
        cout << "=================================================\n";
    }

    void loadQuestionsFromFiles() {
        questions.clear();
        for (auto& pair : questionFiles) {
            string category = pair.first;
            string qFile = pair.second;
            string oFile = optionsFiles[category];
            string aFile = answersFiles[category];
            ifstream qStream(qFile);
            ifstream oStream(oFile);
            ifstream aStream(aFile);
            if (!qStream.is_open() || !oStream.is_open() || !aStream.is_open()) {
                if (qStream.is_open()) qStream.close();
                if (oStream.is_open()) oStream.close();
                if (aStream.is_open()) aStream.close();
                continue;
            }
            string qLine, oLine, aLine;
            while (getline(qStream, qLine) && getline(oStream, oLine) && getline(aStream, aLine)) {
                Question q;
                q.category = category;
                q.question = trim(qLine);
                stringstream ss(oLine);
                string opt;
                while (getline(ss, opt, '|')) {
                    q.options.push_back(trim(opt));
                }
                string correctAnswerText = trim(aLine);
                q.correctAnswer = findCorrectAnswerLetter(q.options, correctAnswerText);
                questions.push_back(q);
            }
            qStream.close();
            oStream.close();
            aStream.close();
        }
    }

    void displayMainMenu() {
        cout << "\n========== QUIZ GAME MAIN MENU ==========\n";
        if (isLoggedIn) {
            cout << "Logged in as: " << currentUser.username << "\n";
            cout << "1. Play Game\n";
            cout << "2. View My Information\n";
            cout << "3. View Leaderboard\n";
            cout << "4. View Ranking Scoreboard\n";
            cout << "5. Logout\n";
            cout << "6. Exit\n";
        } else {
            cout << "1. Login\n";
            cout << "2. Register\n";
            cout << "3. View Leaderboard\n";
            cout << "4. View Ranking Scoreboard\n";
            cout << "5. Exit\n";
        }
        cout << "=========================================\n";
        if (isLoggedIn) {
            cout << "Enter your choice (1-6) or 'q' to exit: ";
        } else {
            cout << "Enter your choice (1-5) or 'q' to exit: ";
        }
    }

    void displayCategories() {
        cout << "\n====== SELECT CATEGORY ======\n";
        cout << "1. Programming Knowledge\n";
        cout << "2. History\n";
        cout << "3. Math\n";
        cout << "4. General Knowledge\n";
        cout << "5. Back to Main Menu\n";
        cout << "=============================\n";
        cout << "Enter your choice (1-5) or 'q' to return: ";
    }

    string getCategoryName(int choice) {
        switch(choice) {
            case 1: return "Programming";
            case 2: return "History";
            case 3: return "Math";
            case 4: return "General";
            default: return "";
        }
    }

    void playGame() {
        if (!isLoggedIn) {
            cout << "\nPlease login first!\n";
            return;
        }

        string inp;
        do {
            displayCategories();
            getline(cin, inp);

            if (inp.empty()) {
                cout << "Invalid choice! Please try again.\n";
                continue;
            }

            if (inp[0] == 'q' || inp[0] == 'Q') {
                break;
            }

            if (!isValidInput(inp, 1, 5)) {
                cout << "Invalid choice! Please enter a number between 1-5.\n";
                continue;
            }

            int choice = stoi(inp);
            if (choice == 5) {
                break;
            }

            if (choice >= 1 && choice <= 4) {
                string category = getCategoryName(choice);
                playCategory(category);
            } else {
                cout << "Invalid choice! Please try again.\n";
            }
        } while(true);

        if (!currentUser.username.empty()) {
            users[currentUser.username] = currentUser;
            saveAllUsers();
        }
    }

    void playCategory(const string& category) {
        int currentProgress = 0;
        if (currentUser.categoryProgress.find(category) != currentUser.categoryProgress.end())
            currentProgress = currentUser.categoryProgress[category];

        vector<Question> categoryQuestions;
        for (auto& q : questions) {
            if (q.category == category) categoryQuestions.push_back(q);
        }

        if (categoryQuestions.empty()) {
            cout << "\nNo questions available for " << category << " category!\n";
            return;
        }

        if (currentProgress >= (int)categoryQuestions.size()) {
            cout << "\n🎉 Congratulations! You've completed all questions in " << category << "!\n";
            return;
        }

        cout << "\nStarting " << category << " quiz...\n";
        cout << "Current progress: " << currentProgress << "/" << categoryQuestions.size() << " questions\n\n";

        int questionsToPlay = min(5, (int)categoryQuestions.size() - currentProgress);
        int correctInSession = 0;
        int wrongInSession = 0;

        for (int i = 0; i < questionsToPlay; i++) {
            Question q = categoryQuestions[currentProgress + i];

            cout << "\nQuestion " << currentProgress + i + 1 << ":\n";
            cout << q.question << "\n";
            for (size_t j = 0; j < q.options.size(); j++) {
                cout << char('A' + j) << ". " << q.options[j] << "\n";
            }

            string answer;
            cout << "\nYour answer (A/B/C/D) or 'q' to stop and return: ";
            getline(cin, answer);

            if (answer.empty()) {
                cout << "Invalid choice! Please try again.\n";
                i--;
                continue;
            }

            if (answer[0] == 'q' || answer[0] == 'Q') {
                cout << "Exiting to category menu...\n";
                break;
            }

            if (answer.size() != 1) {
                cout << "Invalid choice! Please try again.\n";
                i--;
                continue;
            }

            char ch = toupper(answer[0]);
            if (ch < 'A' || ch > 'D') {
                cout << "Invalid choice! Please try again.\n";
                i--;
                continue;
            }

            if (ch == q.correctAnswer) {
                cout << "\n✅ Correct! +10 points\n";
                currentUser.totalScore += 10;
                currentUser.categoryScore[category] += 10;
                correctInSession++;
                if (currentUser.totalScore > currentUser.highestScore) {
                    currentUser.highestScore = currentUser.totalScore;
                }
            } else {
                cout << "\n❌ Wrong! The correct answer was " << q.correctAnswer << "\n";
                currentUser.totalPenalty += 1;
                currentUser.categoryPenalty[category] += 1;
                wrongInSession++;
            }

            cout << "Current Score: " << currentUser.totalScore
                 << " | Highest Score: " << currentUser.highestScore
                 << " | Total Penalty: " << currentUser.totalPenalty << "\n";

            currentUser.categoryProgress[category] = currentProgress + i + 1;
            users[currentUser.username] = currentUser;
            saveAllUsers();
        }

        cout << "\nSession Summary for " << category << ":\n";
        cout << "Correct answers: " << correctInSession << " (+" << correctInSession * 10 << " points)\n";
        cout << "Wrong answers: " << wrongInSession << " (+" << wrongInSession << " penalty)\n";
        cout << "Category progress updated: " << currentUser.categoryProgress[category]
             << "/" << categoryQuestions.size() << "\n";
    }

public:
    QuizGame() {
        loadQuestionsFromFiles();
        loadAllUsers();
    }

    void run() {
        cout << "====================================\n";
        cout << "     WELCOME TO THE QUIZ GAME\n";
        cout << "====================================\n";

        string inp;
        while (true) {
            displayMainMenu();
            getline(cin, inp);

            if (inp.empty()) {
                cout << "Invalid choice! Please try again.\n";
                continue;
            }

            if (inp[0] == 'q' || inp[0] == 'Q') {
                if (isLoggedIn) {
                    logoutUser();
                }
                cout << "\nThank you for playing! Goodbye!\n";
                break;
            }

            if (isLoggedIn) {
                if (!isValidInput(inp, 1, 6)) {
                    cout << "Invalid choice! Please enter a number between 1-6.\n";
                    continue;
                }

                int choice = stoi(inp);
                switch(choice) {
                    case 1:
                        playGame();
                        break;
                    case 2:
                        displayUserInfo();
                        break;
                    case 3:
                        viewHighestScore();
                        break;
                    case 4:
                        displayRankingScoreboard();
                        break;
                    case 5:
                        logoutUser();
                        break;
                    case 6:
                        logoutUser();
                        cout << "\nThank you for playing! Goodbye!\n";
                        saveAllUsers();
                        return;
                    default:
                        cout << "Invalid choice! Please try again.\n";
                }
            } else {
                if (!isValidInput(inp, 1, 5)) {
                    cout << "Invalid choice! Please enter a number between 1-5.\n";
                    continue;
                }

                int choice = stoi(inp);
                switch(choice) {
                    case 1:
                        if (loginUser()) {
                            // User logged in successfully, continue to next iteration
                        }
                        break;
                    case 2:
                        if (registerUser()) {
                            // User registered and logged in directly, continue to next iteration
                        }
                        break;
                    case 3:
                        viewHighestScore();
                        break;
                    case 4:
                        displayRankingScoreboard();
                        break;
                    case 5:
                        cout << "\nThank you for playing! Goodbye!\n";
                        return;
                    default:
                        cout << "Invalid choice! Please try again.\n";
                }
            }
        }
    }
};

int main() {
    QuizGame game;
    game.run();
    return 0;
}
