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
    UserProgress currentUser;
    string progressFile;

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

    bool checkForExit() {
        if (cin.peek() == 'q' || cin.peek() == 'Q') {
            cin.ignore();
            cout << "\nSaving progress and exiting to menu...\n";
            saveUserProgress();
            return true;
        }
        return false;
    }

public:
    QuizGame() {
        loadQuestionsFromFiles();
    }

    void loadQuestionsFromFiles() {
        questions.clear();

        for (auto& pair : questionFiles) {
            string category = pair.first;
            string qFile = pair.second;
            string oFile = optionsFiles[category];
            string aFile = answersFiles[category];

            cout << "Loading category: " << category << "\n";

            ifstream qStream(qFile);
            ifstream oStream(oFile);
            ifstream aStream(aFile);

            if (!qStream.is_open()) {
                cout << "Error opening question file: " << qFile << "\n";
                continue;
            }
            if (!oStream.is_open()) {
                cout << "Error opening options file: " << oFile << "\n";
                continue;
            }
            if (!aStream.is_open()) {
                cout << "Error opening answers file: " << aFile << "\n";
                continue;
            }

            string qLine, oLine, aLine;
            int questionCount = 0;

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
                questionCount++;
            }

            cout << "Loaded " << questionCount << " questions for " << category << "\n";

            qStream.close();
            oStream.close();
            aStream.close();
        }

        cout << "Total questions loaded: " << questions.size() << "\n";
    }

    void setProgressFile(const string& username) {
        progressFile = username + "_progress.txt";
    }

    void loadUserProgress() {
        ifstream file(progressFile);
        if (file.is_open()) {
            file >> currentUser.username;
            file >> currentUser.totalScore;
            file >> currentUser.highestScore;

            string category;
            int progress;
            while (file >> category >> progress) {
                currentUser.categoryProgress[category] = progress;
            }
            file.close();
        }
    }

    void saveUserProgress() {
        ofstream file(progressFile);
        if (file.is_open()) {
            file << currentUser.username << "\n";
            file << currentUser.totalScore << "\n";
            file << currentUser.highestScore << "\n";
            for (auto& pair : currentUser.categoryProgress) {
                file << pair.first << " " << pair.second << "\n";
            }
            file.close();
        }
    }

    void displayMenu() {
        cout << "\n========== QUIZ GAME MENU ==========\n";
        cout << "1. Start New Game\n";
        cout << "2. Continue Previous Game\n";
        cout << "3. View Highest Score\n";
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
        cout << "Enter your choice (1-5) or 'q' to exit: ";
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
        cout << "\nEnter your username: ";
        cin >> currentUser.username;
        setProgressFile(currentUser.username);
        currentUser.totalScore = 0;
        currentUser.highestScore = 0;
        currentUser.categoryProgress.clear();
        for(auto& pair : questionFiles) {
            currentUser.categoryProgress[pair.first] = 0;
        }
        cout << "\nWelcome, " << currentUser.username << "!\n";
        cout << "Press 'q' at any time to exit to main menu.\n";
        playGame();
    }

    void continueGame() {
        string username;
        cout << "\nEnter your username: ";
        cin >> username;
        currentUser.username = username;
        setProgressFile(currentUser.username);
        loadUserProgress();
        if(currentUser.username.empty()) {
            cout << "\nNo saved game found. Starting new game...\n";
            startNewGame();
        } else {
            cout << "\nWelcome back, " << currentUser.username << "!\n";
            cout << "Your current score: " << currentUser.totalScore << "\n";
            cout << "Press 'q' at any time to exit to main menu.\n";
            playGame();
        }
    }

    void playGame() {
        int choice;
        do {
            displayCategories();
            cin >> choice;
            if (checkForExit()) return;
            
            if(choice >= 1 && choice <= 4) {
                string category = getCategoryName(choice);
                playCategory(category);
            } else if(choice != 5) {
                cout << "Invalid choice! Please try again.\n";
            }
        } while(choice != 5);
        saveUserProgress();
    }

    void playCategory(const string& category) {
        int currentProgress = currentUser.categoryProgress[category];
        vector<Question> categoryQuestions;

        for(auto& q : questions) {
            if(q.category == category) {
                categoryQuestions.push_back(q);
            }
        }

        if(categoryQuestions.empty()) {
            cout << "\nNo questions available for " << category << " category!\n";
            return;
        }

        if(currentProgress >= categoryQuestions.size()) {
            cout << "\n🎉 Congratulations! You've completed all questions in " << category << "!\n";
            return;
        }

        cout << "\nStarting " << category << " quiz...\n";
        cout << "Current progress: " << currentProgress << "/" << categoryQuestions.size() << " questions\n\n";

        int questionsToPlay = min(5, (int)categoryQuestions.size() - currentProgress);

        for(int i = 0; i < questionsToPlay; i++) {
            Question q = categoryQuestions[currentProgress + i];
            displayQuestion(q, i + 1);

            char answer;
            cout << "Your answer (A/B/C/D) or 'q' to exit: ";
            cin >> answer;
            answer = toupper(answer);

            if (answer == 'Q') {
                cout << "\nSaving progress and exiting...\n";
                saveUserProgress();
                return;
            }

            if(answer == q.correctAnswer) {
                cout << "✅ Correct! +10 points\n";
                currentUser.totalScore += 10;
                if(currentUser.totalScore > currentUser.highestScore) {
                    currentUser.highestScore = currentUser.totalScore;
                }
            } else {
                cout << "❌ Wrong! The correct answer was " << q.correctAnswer << "\n";
            }
            cout << "Current Score: " << currentUser.totalScore << " | Highest Score: " << currentUser.highestScore << "\n\n";

            currentUser.categoryProgress[category] = currentProgress + i + 1;
            saveUserProgress();
        }

        cout << "Category progress updated: " << currentUser.categoryProgress[category] << "/" << categoryQuestions.size() << "\n";
    }

    void displayQuestion(const Question& q, int number) {
        cout << "Question " << number << ":\n";
        cout << q.question << "\n";
        for(size_t i = 0; i < q.options.size(); i++) {
            cout << char('A' + i) << ". " << q.options[i] << "\n";
        }
    }

    void viewHighestScore() {
        string username;
        cout << "\nEnter your username: ";
        cin >> username;
        currentUser.username = username;
        setProgressFile(currentUser.username);
        loadUserProgress();
        if(currentUser.username.empty()) {
            cout << "\nNo game data found. Play a game first!\n";
        } else {
            cout << "\n========== HIGHEST SCORE ==========\n";
            cout << "Player: " << currentUser.username << "\n";
            cout << "Highest Score: " << currentUser.highestScore << "\n";
            cout << "===================================\n";
        }
    }

    void viewCategoryProgress() {
        string username;
        cout << "\nEnter your username: ";
        cin >> username;
        currentUser.username = username;
        setProgressFile(currentUser.username);
        loadUserProgress();
        if(currentUser.username.empty()) {
            cout << "\nNo game data found. Play a game first!\n";
            return;
        }

        cout << "\n====== CATEGORY PROGRESS ======\n";
        cout << "Player: " << currentUser.username << "\n";
        cout << "Total Score: " << currentUser.totalScore << "\n";
        cout << "Highest Score: " << currentUser.highestScore << "\n\n";

        for(auto& pair : questionFiles) {
            string category = pair.first;
            int progress = currentUser.categoryProgress[category];
            int total = 0;
            for(auto& q : questions) {
                if(q.category == category) total++;
            }
            cout << category << ": " << progress << "/" << total << " questions completed\n";
        }
        cout << "===============================\n";
    }

    void run() {
        cout << "Total questions loaded: " << questions.size() << "\n";

        int choice;
        do {
            displayMenu();
            cin >> choice;
            if (checkForExit()) break;
            
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
                    break;
                default:
                    cout << "Invalid choice! Please try again.\n";
            }
        } while(choice != 5);
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
