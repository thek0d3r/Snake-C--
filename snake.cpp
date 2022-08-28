#include <iostream>
#include <conio.h>
#include <windows.h>
#include <algorithm>
#include <cctype>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

string normalScore, hardcoreScore; // Variables for the highscore

bool gameOver;
bool hardcore;
bool gameStarted;

const int width = 20;
const int height = 20;

int x, y, fruitX, fruitY;
int score, speed;
int tailX[100], tailY[100];
int nTail;
enum eDirection{ STOP = 0, LEFT, RIGHT, UP, DOWN };
eDirection dir;

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void ShowConsoleCursor(bool showFlag) {
    // Hides the powershell and cmd cursor
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag;
    SetConsoleCursorInfo(out, &cursorInfo);
}

void Setup() {
    // Setup for the game
    gameOver = false;
    gameStarted = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    fruitX = rand() % width;
    fruitY = rand() % height;
    score = 0;

    tailX[100] = {};
    tailY[100] = {};

    if(!hardcore) speed = 1;
    else speed = 2; 
}

void Draw() {
    // Draws every element of the game
    system("cls");
    for(int i = 0; i < width + 2; i++){
        cout<<"#";
    }
    cout<<endl;

    for(int i = 0; i < height; i++) {
        for(int j = 0; j < width; j++) {

            if(j == 0) cout<<"#";

            if(i == y && j == x) cout<<"0";
            else if(i == fruitY && j == fruitX) cout<<"F";
            else{
                bool print = false;
                for(int k = 0; k < nTail; k++){
                    if(tailX[k] == j && tailY[k] == i) {
                        cout<<"o"; 
                        print = true;
                    }
                }
                if(!print) cout<<" ";
            }

            if(j == width - 1) cout<<"#";

        }
        cout<<endl;
    }

    for(int i = 0; i < width + 2; i++) {
        cout<<"#";
    }
    cout<<endl;
    cout<<endl;
    cout<<"Score: "<<score<<endl;
}

void gameInput() {
    // Takes users input
    if(_kbhit()) {
        switch(_getch()) {
            case 'a':
                if(dir != RIGHT) dir = LEFT;
                break;
            case 'd':
                if(dir != LEFT) dir = RIGHT;
                break;
            case 's':
                if(dir != UP) dir = DOWN;
                break;
            case 'w':
                if(dir != DOWN) dir = UP;
                break;
            case 'p':
                gameOver = true;
                break;
        }
    }
}

void gameLogic() {
    //Main logic for the game
    int prevX = tailX[0];
    int prevY = tailY[0];
    int prev2X, prev2Y;

    tailX[0] = x;
    tailY[0] = y;

    for(int i = 1; i < nTail; i++)  {
        prev2X = tailX[i];
        prev2Y = tailY[i];
        tailX[i] = prevX;
        tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    switch (dir) { // If the game mode is set to hardcore, we invert the key binds ;)
        case LEFT:
            if(!hardcore) x -= speed;
            else x += speed;
            break;
        case RIGHT:
            if(!hardcore) x += speed;
            else x -= speed;
            break;
        
        case UP:
            if(!hardcore) y -= speed;
            else y += speed;
            break;
        
        case DOWN:
            if(!hardcore) y += speed;
            else y -= speed;
            break;

        default:
            break;
    }

    if(hardcore) { // If the game mode is set to hardcore, we add a new way to die
        if(x > width || x < 0 || y > height || y < 0) gameOver = true; // End the game if it hits the border
    } else {
        if(x >= width) x = 0; else if(x < 0) x = width - 1;
        if(y >= height) y = 0; else if(y < 0) y = height -1;
    }

    for(int i = 0; i < nTail; i++) {
        if(tailX[i] == x && tailY[i] == y) gameOver = true; // End the game if the player bites its tail
    }

    if(x == fruitX && y == fruitY) {
        score += 10;
        fruitX = rand() % width;
        fruitY = rand() % height;
        nTail++;
    }
}

void setGameMode() {
    // Sets the gmaemode to the users desired gamemode
    system("cls");

    string option;
    
    cout<<"Enter your desired gamemode (Normal or HARDCORE): "; cin>>option;

    for_each(option.begin(), option.end(), [](char & c) {
        c = tolower(c);
    });

    if(option == "normal") hardcore = false; // Sets the game mode to normal
    else if(option == "hardcore") hardcore = true; // Sets the game mode to hardcore
    else {
        system("cls");
        cout<<"Invalid input of '"<<option<<"'."<<endl<<"Try again!"<<endl;
        Sleep(1000);
        system("cls");
        setGameMode();
    }
}

string renderLogo(ifstream& File) {
    // Renders the logo
    string Lines = "";
    if(File){
        while(File.good()) {
            string TempLine;
            getline(File, TempLine);
            TempLine += "\n";
            Lines += TempLine;
        }
        return Lines;
    } else return "ERROR! File does not exist!";
}

void readHighScore() {
    // Gets the highscore for each gamemode
    ifstream Normal("highscoreNormal.txt"), Hardcore("highscoreHardcore.txt");
    if(!Normal.fail()) getline(Normal, normalScore);
    else normalScore = "0";
    Normal.close();
    if(!Hardcore.fail()) getline(Hardcore, hardcoreScore);
    else hardcoreScore = "0";
    Hardcore.close();
}

void retainHighScore(string retainedScore) {
    // Stores the highscore for each mode in separate files
    readHighScore();
    if(!hardcore) {
        fstream HighScoreCheck;
        HighScoreCheck.open("highscoreNormal.txt");
        if(HighScoreCheck.fail()) {
            ofstream HighScore;
            HighScore.open("highscoreNormal.txt");
            HighScore << retainedScore;
            HighScore.close();
        } else {
            char filename[] = "highscoreNormal.txt";
            remove(filename);
            ofstream HighScore;
            if(stoi(normalScore) < stoi(retainedScore)){
                HighScore.open("highscoreNormal.txt");
                HighScore << retainedScore;
                HighScore.close();
            }
        }
    } else {
        fstream HighScoreCheck;
        HighScoreCheck.open("highscoreHardcore.txt");
        if(HighScoreCheck.fail()) {
            ofstream HighScore;
            HighScore.open("highscoreHardcore.txt");
            HighScore << retainedScore;
            HighScore.close();
        } else {
            char filename[] = "highcoreNormal.txt";
            remove(filename);
            ofstream HighScore;
            if(stoi(hardcoreScore) < stoi(retainedScore)){
                HighScore.open("highscoreHardcore.txt");
                HighScore << retainedScore;
                HighScore.close();
            }
        }
    }
}

void startScreen() {
    // Render the Start Screen and its menus
    ShowConsoleCursor(true);
    system("cls");
    SetConsoleTextAttribute(hConsole, 2);
    ifstream Reader("art.txt");
    string Art = renderLogo(Reader);
    cout<<Art<<endl;
    Reader.close();
    SetConsoleTextAttribute(hConsole, 7);
    cout<<"1 -> Start"<<endl
        <<"2 -> Help"<<endl
        <<"3 -> Hall of Fame"<<endl
        <<"q -> Quit"<<endl
        <<endl;
    char option;
    cout<<": "; cin>>option;
    switch(option) {
        case '1':
            gameStarted = true;
            break;
        case '2':
            char quitHelpMenu;
            system("cls");
            SetConsoleTextAttribute(hConsole, 2);
            cout<<"Key Binds"<<endl
                <<"========="<<endl;
            SetConsoleTextAttribute(hConsole, 7);
            cout<<endl
                <<"w -> Move Forwards"<<endl
                <<"a -> Move Backwards"<<endl
                <<"s -> Move Right"<<endl
                <<"d -> Move Left"<<endl
                <<"q -> Quit"<<endl
                <<endl
                <<"Enter anything to go back"
                <<": "; cin>>quitHelpMenu;
                if(quitHelpMenu) startScreen();
            break;
        case '3':
            char quitHOFMenu;
            system("cls");
            readHighScore();
            SetConsoleTextAttribute(hConsole, 2);
            cout<<"Hall of Fame"<<endl
                <<"============"<<endl;
            SetConsoleTextAttribute(hConsole, 7);
            cout<<endl
                <<"Your highscore in 'normal' is: "<<normalScore<<endl
                <<"Your highscore in 'HARDCORE' is: "<<hardcoreScore<<endl
                <<endl
                <<"Enter anything to go back"
                <<": "; cin>>quitHOFMenu;
                if(quitHOFMenu) startScreen();
            break;
        case 'q':
            system("cls");
            gameStarted = false;
            break;
        default:
            system("cls");
            cout<<"Wrong input!";
            Sleep(1000);
            system("cls");
            startScreen();
    }

}

int main() {
    // Main function. Call anyt function from here
    startScreen();

    if(gameStarted) {
        setGameMode();
        ShowConsoleCursor(false);
        Setup();
        while(!gameOver) {
            Draw();
            gameInput();
            gameLogic();

        }
        if(gameOver) {
            string highscore;
            
            retainHighScore(to_string(score));

            system("cls");
            Sleep(500);
            cout<<"Game Over!";
            Sleep(2500);
            system("cls");
            cout<<"Your score is "<<score<<"."<<endl;
            Sleep(2500);
            system("cls");
        }
        main();
    }

    return 0;
}