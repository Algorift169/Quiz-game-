#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <cctype>
#include <algorithm>
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
    int totalScore;
    int highestScore;
};

class QuizGame {
private:
    vector<Question> questions;
    map<string, UserProgress> users;
    UserProgress currentUser;
    string lastUser;
    string progressFile = "user_progress.txt";

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
                temp.categoryProgress.clear();
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
                } else if (s.size() >= 5 && s.substr(0,5) == "PROG ") {
                    string rest = trim(s.substr(5));
                    stringstream ss(rest);
                    string cat;
                    int prog;
                    ss >> cat >> prog;
                    temp.categoryProgress[cat] = prog;
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
        if (!currentUser.username.empty()) file << "LAST " << currentUser.username << "\n";
        for (auto& pair : users) {
            UserProgress u = pair.second;
            file << "USER " << u.username << "\n";
            file << "TOTAL " << u.totalScore << "\n";
            file << "HIGH " << u.highestScore << "\n";
            for (auto& p : u.categoryProgress) {
                file << "PROG " << p.first << " " << p.second << "\n";
            }
            file << "ENDUSER\n";
        }
        file.close();
    }

    void setCurrentUser(const string& username, bool resetProgress) {
        if (users.find(username) == users.end()) {
            UserProgress u;
            u.username = username;
            u.totalScore = 0;
            u.highestScore = 0;
            u.categoryProgress.clear();
            for (auto& p : questionFiles) u.categoryProgress[p.first] = 0;
            users[username] = u;
        } else if (resetProgress) {
            users[username].totalScore = 0;
            for (auto& p : questionFiles) users[username].categoryProgress[p.first] = 0;
        }
        currentUser = users[username];
        lastUser = username;
        users[username] = currentUser;
        saveAllUsers();
    }

public:
    QuizGame() {
        loadQuestionsFromFiles();
        loadAllUsers();
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

    void displayMenu() {
        cout << "\n========== QUIZ GAME MENU ==========\n";
        cout << "1. Start New Game\n";
        cout << "2. Continue Previous Game\n";
        cout << "3. View Highest Score (Leaderboard)\n";
        cout << "4. View Category Progress\n";
        cout << "5. Exit\n";
        cout << "====================================\n";
        cout << "Enter your choice (1-5) or 'q' to exit: ";
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

    void startNewGame() {
        cout << "\nEnter your username (or 'q' to cancel): ";
        string uname;
        cin >> uname;
        if (uname == "q" || uname == "Q") {
            cout << "Cancelled. Returning to main menu.\n";
            return;
        }
        setCurrentUser(uname, true);
        cout << "\nWelcome, " << currentUser.username << "!\n";
        playGame();
    }

    void continueGame() {
        loadAllUsers();
        if (!currentUser.username.empty()) {
            cout << "\nWelcome back, " << currentUser.username << "!\n";
            cout << "Your current score: " << currentUser.totalScore << "\n";
            playGame();
            return;
        }
        if (users.empty()) {
            cout << "\nNo saved game found. Starting new game...\n";
            startNewGame();
            return;
        }
        cout << "\nNo last user found. Enter username to continue (or 'q' to cancel): ";
        string uname;
        cin >> uname;
        if (uname == "q" || uname == "Q") {
            cout << "Cancelled. Returning to main menu.\n";
            return;
        }
        if (users.find(uname) == users.end()) {
            cout << "User not found. Starting new user with that name.\n";
            setCurrentUser(uname, false);
        } else {
            currentUser = users[uname];
            lastUser = uname;
        }
        cout << "\nWelcome back, " << currentUser.username << "!\n";
        cout << "Your current score: " << currentUser.totalScore << "\n";
        playGame();
    }

    void playGame() {
        int choice;
        string inp;
        do {
            displayCategories();
            cin >> inp;
            if (inp == "q" || inp == "Q") break;
            bool validNum = true;
            for (char c : inp) if (!isdigit(c)) validNum = false;
            if (!validNum) {
                cout << "Invalid choice! Please try again.\n";
                continue;
            }
            choice = stoi(inp);
            if (choice >= 1 && choice <= 4) {
                string category = getCategoryName(choice);
                playCategory(category);
            } else if (choice == 5) {
                break;
            } else {
                cout << "Invalid choice! Please try again.\n";
            }
        } while(true);
        users[currentUser.username] = currentUser;
        saveAllUsers();
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
        for (int i = 0; i < questionsToPlay; i++) {
            Question q = categoryQuestions[currentProgress + i];
            displayQuestion(q, currentProgress + i + 1);
            string answer;
            cout << "Your answer (A/B/C/D) or 'q' to stop and return: ";
            cin >> answer;
            if (answer == "q" || answer == "Q") {
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
                cout << "✅ Correct! +10 points\n";
                currentUser.totalScore += 10;
                if (currentUser.totalScore > currentUser.highestScore) currentUser.highestScore = currentUser.totalScore;
            } else {
                cout << "❌ Wrong! The correct answer was " << q.correctAnswer << "\n";
            }
            cout << "Current Score: " << currentUser.totalScore << " | Highest Score: " << currentUser.highestScore << "\n\n";
            currentUser.categoryProgress[category] = currentProgress + i + 1;
            users[currentUser.username] = currentUser;
            saveAllUsers();
        }
        cout << "Category progress updated: " << currentUser.categoryProgress[category] << "/" << categoryQuestions.size() << "\n";
    }

    void displayQuestion(const Question& q, int number) {
        cout << "Question " << number << ":\n";
        cout << q.question << "\n";
        for (size_t i = 0; i < q.options.size(); i++) {
            cout << char('A' + i) << ". " << q.options[i] << "\n";
        }
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
        cout << "\n========== LEADERBOARD ==========\n";
        cout << "Rank | Player | Highest Score\n";
        cout << "-------------------------------\n";
        for (size_t i = 0; i < list.size(); i++) {
            cout << i+1 << " | " << list[i].username << " | " << list[i].highestScore << "\n";
        }
        cout << "=================================\n";
    }

    void viewCategoryProgress() {
        loadAllUsers();
        if (users.empty()) {
            cout << "\nNo game data found. Play a game first!\n";
            return;
        }
        cout << "\nEnter username to view progress (or 'q' to cancel): ";
        string uname;
        cin >> uname;
        if (uname == "q" || uname == "Q") {
            cout << "Cancelled. Returning to main menu.\n";
            return;
        }
        if (users.find(uname) == users.end()) {
            cout << "User not found.\n";
            return;
        }
        UserProgress u = users[uname];
        cout << "\n====== CATEGORY PROGRESS ======\n";
        cout << "Player: " << u.username << "\n";
        cout << "Total Score: " << u.totalScore << "\n";
        cout << "Highest Score: " << u.highestScore << "\n\n";
        for (auto& pair : questionFiles) {
            string category = pair.first;
            int progress = 0;
            if (u.categoryProgress.find(category) != u.categoryProgress.end()) progress = u.categoryProgress[category];
            int total = 0;
            for (auto& q : questions) if (q.category == category) total++;
            cout << category << ": " << progress << "/" << total << " questions completed\n";
        }
        cout << "===============================\n";
    }

    void run() {
        loadAllUsers();
        cout << "Total questions loaded: " << questions.size() << "\n";
        string inp;
        do {
            displayMenu();
            cin >> inp;
            if (inp == "q" || inp == "Q") {
                cout << "\nThank you for playing! Goodbye!\n";
                break;
            }
            bool validNum = true;
            for (char c : inp) if (!isdigit(c)) validNum = false;
            if (!validNum) {
                cout << "Invalid choice! Please try again.\n";
                continue;
            }
            int choice = stoi(inp);
            switch(choice) {
                case 1:
                    startNewGame();
                    break;
                case 2:
                    continueGame();
                    break;
                case 3:
                    viewHighestScore();
                    break;
                case 4:
                    viewCategoryProgress();
                    break;
                case 5:
                    cout << "\nThank you for playing! Goodbye!\n";
                    saveAllUsers();
                    return;
                default:
                    cout << "Invalid choice! Please try again.\n";
            }
        } while(true);
    }
};

int main() {
    cout << "====================================\n";
    cout << "     WELCOME TO THE QUIZ GAME\n";
    cout << "====================================\n";
    QuizGame game;
    game.run();
    return 0;
}
