
/*
Active.と表示された後、プレイする事ができます。
最初にselectコマンドで自分の石を指定する必要があります。
例)白色で選択する場合。
$ select
$ w

石を指定したらputコマンドで石を打つ事が出来ます。
-----コマンド-----
-quit
-exit
>>ゲームプログラムを終了します。

-mode [ai,pair]
>>対戦モードを設定します。[]の部分には[ai,pair]のどれかを指定する事が可能。
ai...相手がコンピュータの試合
pair..交代で石を打つ試合。

-count
>>石の数を表示します。

-echo [word]
>>[word]に入力された文字列を出力します。

-help
コマンド一覧を表示します。

-table
>>盤面を出力します。

-player
>>プレイヤーの石の色を出力します。

-com
>>コンピュータ(ボット)の石の色を出力します。

-other
>>相手の石の色を出力します。

-stone
>>石の色名と文字の凡例を出力します。

-darkmode
>>ダークモードを設定します。（白黒反転）

-darkmode off
>>ダークモードを解除します。


-select [w or b]
>>プレイヤーが打つ石の色を決定します。

-findput
>>プレイヤーが配置可能なマス目一覧を表示します。

-put
>>石を打ちます。wsadキーでカーソル移動(複数回押す必要あり)、Enterで配置
qキーでエスケープ

-stats
>>それぞれの石の数を表示します。
*/
#include <iostream>
#include <string>
#include <cmath>
#include <random>
#include <vector>
#include <utility>
#include <algorithm>
#include <functional>
#include <numeric>
#include "./conio.h"

using namespace std;

enum STONE
{
    NONE,
    BLACK,
    WHITE
};

enum WHO
{
    NUL,
    OTHER,
    PLAYER
};


/*

● ● ● ● ● ● ● ●
● ● ○ ● ○ ● ● ●
● ● ● ○ ● ○ ● ●
● ● ● ● ● ● ● ●
● ○ ○ ○ ○ ○ ● ○
○ ○ ● ○ ○ ● ○ ○
○ ○ ○ ○ ○ ○ ○ ○
○ ○ ○ ● ● ○ ● ●

    {0,0,0,1,0,0,0,2},
    {0,0,0,2,0,0,1,0},
    {0,0,0,2,0,1,0,0},
    {0,0,0,2,0,1,0,0},
    {0,0,0,0,1,1,0,0},
    {0,0,2,0,1,1,0,0},
    {0,2,0,0,1,2,0,0},
    {1,0,0,0,2,0,0,0}

    {1,1,1,1,1,1,1,1},
    {1,1,2,1,2,1,1,1},
    {1,2,1,2,1,1,1,1},
    {1,2,1,1,1,1,0,1},
    {1,2,2,1,1,1,1,0},
    {2,2,1,1,1,1,1,2},
    {2,2,2,1,1,2,0,2},
    {2,2,2,2,2,0,2,2}
*/

const char *UTF_8 = "chcp 65001";
const char *SHIFT_JIS = "chcp 932";
const char *ENCODE = UTF_8;
const string USR = ">> ";
const char SPRT = ' ';
const int SIZE = 8;                         //盤面のサイズ(N × Nマス)
const char *CMD_CLEARCONSOLE = "clear";
const string C_EXIT = "exit";
const string S_NONE = "- ";
const string S_BLACK = "● ";
const string S_WHITE = "○ ";
const string S_CURSOR = "■ ";               //配置する場所を示すカーソル
const string S_SELECT_BLACK = "b";
const string S_SELECT_WHITE = "w";
const string S_SELECT_NONE = "e";
const string CMD_DARKMODE_DISABLE = "off";  //ダークモードを解除するコマンド引数
const string MSG_ON_ACTIVATE = "Activate."; //起動後に表示されるメッセージ
const string MSG_PLAYER = "Player";
const string MSG_COM = "Com";
const string MSG_OTHER = "Other";
const string MSG_PLAYER_DETAIL = "Player: ";//プレイヤーの詳細が表示される際のメッセージ
const string MSG_COM_DETAIL = "Com: ";      //コンピュータ(ボット)の詳細が表示される際のメッセージ
const string MSG_OTHER_DETAIL = "Other: ";  //相手の詳細が表示される際のメッセージ
const string MSG_DEFAULT = "Hello";         //適当なメッセージ
const string MSG_ON_BEGIN_HELP = "----------Commands----------";                    //コマンド一覧の先頭に表示
const string MSG_ON_END_LIST = "----------------------------";                      //一覧の末尾に表示
const string MSG_ON_ERROR = "Error.";                                               //エラーの際に表示されるメッセージ
const string MSG_ON_SELECT = "Select user's color. (b:black,w:white)";              //ユーザの色選択の際に表示するメッセージ
const string MSG_ON_FINISH_SELECTED = "Player selected: ";                          //プレイヤーの色が選択完了した際に表示されるメッセージ。
const string MSG_ON_NOT_ALLOWED = "This command is not allowed.";                   //許可されていない状態でコマンドを実行しようとした際に表示されるメッセージ。
const string MSG_ON_PLAYERSTONE_ISNT_SELECTED = "Player's stone is not selected.";  //プレイヤーの石が設定されていない状態で参照しようとした際に表示されるメッセージ。
const string MSG_ON_COMSTONE_ISNT_SELECTED = "Com's stone is not selected.";        //ボットの石が設定されていない状態で参照しようとした際に表示されるメッセージ
const string MSG_ON_OTHERSTONE_ISNT_SELECTED = "Other's stone is not selected.";    //相手の石が設定されていない状態で参照しようとした際に表示されるメッセージ
const string MSG_ON_NOT_PUTTABLE = "Can't to put here.";                            //カーソルの位置に石が置けない場所である際に表示されるメッセージ。
const string MSG_ON_SET_DARKMODE = "Set DarkMode.";                                 //ダークモードに設定した際のメッセージ
const string MSG_ON_RESET_DARKMODE = "Clear DarkMode.";                             //ダークモードを解除した際のメッセージ
const string MSG_NAME_NONE = "None: ";                                              //石の名前を表示する際のメッセージ。
const string MSG_NAME_BLACK = "Black: ";                                            //石の名前を表示する際のメッセージ。
const string MSG_NAME_WHITE = "White: ";                                            //石の名前を表示する際のメッセージ。
const string MSG_ON_START = "Start.";                                               //ゲームが開始された際に表示されるメッセージ
const string MSG_ON_STATS_START = "----------Stats----------";                      //統計を表示される際のメッセージ。
const string MSG_ON_SKIP = "Skip.";                                                 //スキップする際のメッセージ。
const string MSG_ON_BEFORE = "Before";                                              //打つ前の盤面
const string MSG_ON_AFTER = "After";                                                //打って返した後の盤面
const string MSG_ON_GAMEENDED = "----------Ended----------";                        //ゲームが終了した際のメッセージ
const string MSG_ON_WINNER = "Winner: ";                                            //勝者を表示する前のメッセージ。
const string MSG_ON_MODE = "Mode:";                                                 //対戦モードを表示する際のメッセージ
const string MSG_ON_PAIR = "Pair";                                                  //プレイヤー双方が交互に打つ試合
const string MSG_ON_AI = "AI";                                                      //片方はコンピュータによる試合
typedef vector<STONE> LINE;                 //盤面の行
typedef vector<LINE> TABLE;                 //盤面
typedef vector<TABLE> HISTORY;              //過去の盤面を格納するデータ型
typedef pair<int,int> AT;                   //マスの位置を表すデータ型(~ at)
typedef pair<int,int> LOCATION;             //左からx番目、上からy番目を表すデータ型
constexpr int UP = 1;                       //上
constexpr int UPRIGHT = 2;                  //右上
constexpr int RIGHT = 4;                    //右
constexpr int DOWNRIGHT = 7;                //右下
constexpr int DOWN = 6;                     //下
constexpr int DOWNLEFT = 5;                 //左下
constexpr int LEFT = 3;                     //左
constexpr int UPLEFT = 0;                   //左上
constexpr AT DIRECTIONS[] =                 //方向に対する位置の偏差を表す定数式
{
    {-1,-1},{ 0,-1},{ 1,-1},
    {-1, 0},        { 1, 0},
    {-1, 1},{ 0, 1},{ 1, 1}
};

//void Add(AT &a,AT b);
//STONE Get(AT at,TABLE &tb);
//bool Eq(AT a,AT b);
//LOCATION ToLocation(AT at);
//bool IsValid(AT at);
//void FindTurn(const STONE &st,AT at,AT direction,TABLE &tb);

//コンソール上でのコマンド一覧
const vector<string> COMMANDS
{
    "quit",
    "exit",
    "table",
    "help",
    "echo",
    "select",
    "player",
    "com",
    "other",
    "put",
    "clear",
    "test",
    "stone",
    "darkmode",
    "start",
    "count",
    "findput",
    "stats",
    "maxat",
    "mode"
};

//初期の盤面配置
const vector<vector<int>> TABLE_INIT =
{
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,1,2,0,0,0},
    {0,0,0,2,1,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}
};

class Othello
{
private:
    bool IsDarkMode = true;
    bool IsPlaying = false;
    bool IsAIMode = true;
    STONE Player = STONE::NONE;
    STONE Com = STONE::NONE;
    STONE Other = STONE::NONE;
    WHO Now = WHO::NUL;
    TABLE table;
    TABLE table_v;
    HISTORY history;
protected:
public:
    Othello();
    void Run();
    vector<string> DecodeCommand(const string raw);
    bool ExecuteCommand(const vector<string> &args);
    void ShowCommandHelp();
    void ShowCommandError();
    void Echo(string arg);
    void SelectUserColor();
    void ShowPlayerStone();
    void ShowComStone();
    void ShowOtherStone();
    void ShowStoneExample();
    void ShowStats();
    void ShowMode();
    void SetPlayerStone(STONE playerst);
    void SetDarkMode(bool newmode);
    void SetMode(bool newmode);
    STONE Get(AT at);
    string GetStoneName(STONE st);
    void Put();
    bool Put(AT at,STONE newst);
    bool IsEmptyAt(const AT location);
    bool IsAbletoPut(AT at,STONE st);
    void InitializeTable(const vector<vector<int>> &init);
    void SaveTable();
    TABLE GetTablePrev();
    void PrintTable();
    void PrintTableRaw();
    void PrintTableCursor(AT cursor);
    void PrintTablePrev();
    int CountEmpty();
    int CountBlack();
    int CountWhite();
    void ClearConsole();
    void Turn(const STONE &st,AT at,AT direction);
    void On_PutStone(const AT at,STONE st);
    void On_Comp(STONE st);
    void On_Player(STONE st);
    void On_Other(STONE st);
    vector<AT> FindPut(STONE st);
    AT FindMaxPoint(STONE st);
    int Simulate(STONE st,AT at);
    bool Exit();
    bool State();
    static bool Eq(AT a,AT b);
    static void Add(AT &a,AT b);
    static STONE Get(AT at,TABLE &tb);
    static void Set(AT at,STONE st,TABLE &tb);
    static LOCATION ToLocation(AT at);
    static void Put(AT at,STONE st,TABLE &tb);
    static bool IsBoundAtDirction(const AT &location,const AT &direction);
    static bool IsValid(AT at);
    static bool IsValid(TABLE &tb);
    static void ShowTable(TABLE &table,bool Isdarkmode=false);
    static int Count(const TABLE &tb,const STONE st);
    static void InitializeTable(TABLE &tb);
    static void InitializeTable(TABLE &tb,TABLE tb_init);
    static void FindTurn(const STONE &st,AT at,AT direction,TABLE &tb);
    static void Turn(const STONE &st,AT at,TABLE &tb);
    static void ShowLocation(AT at);
    static void SaveTable(TABLE &tb,HISTORY &history);
    static STONE State(TABLE &tb,STONE player);
};

Othello::Othello()
{

}

//オセロのプロセスを開始します。
void Othello::Run()
{
    string cmd;
    vector<string> args;
    bool frag = false;
    this->InitializeTable(TABLE_INIT);
    cout << MSG_ON_ACTIVATE << endl;
    while(true)
    {
        cout << USR;
        getline(cin,cmd);
        cout << endl;
        args = this->DecodeCommand(cmd);
        frag = this->ExecuteCommand(args);
        if(frag)
        {
            this->Exit();
            break;
        }
    }
}

//読み取った一行のコマンドを分解します。
vector<string> Othello::DecodeCommand(const string raw)
{
    vector<string> args;
    string arg = "";
    for(auto s : raw)
    {
        if(s == SPRT)
        {
            args.push_back(arg);
            arg = "";
        }
        else arg += s;
    }
    args.push_back(arg);
    return args;
}

//コマンドを実行します。
bool Othello::ExecuteCommand(const vector<string> &args)
{
    //"quit" or "exit"
    if(args[0] == COMMANDS[0] || args[0] == COMMANDS[1]) return true;
    //"table"
    if(args[0] == COMMANDS[2])
    {
        this->PrintTable();
    }//"help"
    else if(args[0] == COMMANDS[3])
    {
        this->ShowCommandHelp();
    }//echo x
    else if(args[0] == COMMANDS[4])
    {
        if(args.size() == 1)
        {
            cout << MSG_DEFAULT << endl;
        }
        else this->Echo(args[1]);
    }//select
    else if(args[0] == COMMANDS[5])
    {
        this->SelectUserColor();
    }//player
    else if(args[0] == COMMANDS[6])
    {
        this->ShowPlayerStone();
    }//com
    else if(args[0] == COMMANDS[7])
    {
        this->ShowComStone();
    }//other
    else if(args[0] == COMMANDS[8])
    {
        this->ShowOtherStone();
    }//put
    else if(args[0] == COMMANDS[9])
    {
        this->Put();
    }//clear
    else if(args[0] == COMMANDS[10])
    {
        this->ClearConsole();
    }//test
    else if(args[0] == COMMANDS[11])
    {
        this->PrintTableCursor({5,5});
    }//stone
    else if(args[0] == COMMANDS[12])
    {
        this->ShowStoneExample();
    }//darkmode [option]
    else if(args[0] == COMMANDS[13])
    {
        if(args.size() != 1)
        {
            if(args[1] == CMD_DARKMODE_DISABLE) this->SetDarkMode(false);
            else this->SetDarkMode(true);
        }
        else this->SetDarkMode(true);
    }//start
    else if(args[0] == COMMANDS[14])
    {
        if(this->Player == STONE::NONE)
        {
            cout << MSG_ON_PLAYERSTONE_ISNT_SELECTED << endl;
            return false;
        }
        this->IsPlaying = true;
        cout << MSG_ON_START << endl;
        this->PrintTable();
    }//count [stone]
    else if(args[0] == COMMANDS[15])
    {
        if(args.size() == 1) this->ShowCommandError();
        else
        {
            if(args[1] == S_SELECT_NONE) cout << MSG_NAME_NONE << this->CountEmpty() << endl;
            else if(args[1] == S_SELECT_BLACK) cout << MSG_NAME_BLACK << this->CountBlack() << endl;
            else if(args[1] == S_SELECT_WHITE) cout << MSG_NAME_WHITE << this->CountWhite() << endl;
        }
    }//findput
    else if(args[0] == COMMANDS[16])
    {
        for(auto d : this->FindPut(this->Player))
        {
            auto lc = ToLocation(d);
            cout << "(" << lc.first << "," << lc.second << ")" << endl;
        }
        cout << "count:" << this->FindPut(this->Player).size() << endl;
    }//stats
    else if(args[0] == COMMANDS[17])
    {
        this->ShowStats();
    }//maxat
    else if(args[0] == COMMANDS[18])
    {
        //auto at = this->FindMaxPoint(this->Player);
        //Othello::ShowLocation(at);
    }//mode [mode]
    else if(args[0] == COMMANDS[19])
    {
        if(args.size() == 1) this->ShowMode();
        else if(args[1] == "ai") this->SetMode(true);
        else if(args[1] == "pair") this->SetMode(false);
    }
    return false;
}

//helpコマンドが呼ばれた際にコマンド一覧を表示します。
void Othello::ShowCommandHelp()
{
    cout << MSG_ON_BEGIN_HELP << endl;
    for(auto c : COMMANDS)
    {
        cout << c << endl;
    }
    cout << MSG_ON_END_LIST << endl;
    cout << endl;
}

//間違ったコマンドや引数を指定した場合に呼ばれる関数
void Othello::ShowCommandError()
{
    cout << MSG_ON_ERROR << endl;
}

//引数に指定した文字列を表示させます。(デバッグ向け)
void Othello::Echo(string arg)
{
    cout << arg << endl;
}

//ユーザの石の色を決めさせます。
void Othello::SelectUserColor()
{
    if(this->IsPlaying)
    {
        cout << MSG_ON_NOT_ALLOWED << endl;
        return;
    }
    cout << MSG_ON_SELECT << endl;
    string cmd = "";
    while(true)
    {
        cout << USR;
        getline(cin,cmd);
        if(cmd == S_SELECT_BLACK) this->SetPlayerStone(STONE::BLACK);
        else if(cmd == S_SELECT_WHITE) this->SetPlayerStone(STONE::WHITE);
        else if(cmd == COMMANDS[0] || cmd == COMMANDS[1]) return;
        else 
        {
            cout << MSG_ON_ERROR << endl;
            continue;
        }
        cout << MSG_ON_FINISH_SELECTED << this->GetStoneName(this->Player) << endl;
        break;
    }
}

//プレイヤーの石の色を表示します。
void Othello::ShowPlayerStone()
{
    if(this->Player == STONE::NONE) cout << MSG_ON_PLAYERSTONE_ISNT_SELECTED << endl;
    else cout << MSG_PLAYER_DETAIL << this->GetStoneName(this->Player) << endl;
}

//ボットの石の色を表示します。
void Othello::ShowComStone()
{
    if(this->Com == STONE::NONE) cout << MSG_ON_COMSTONE_ISNT_SELECTED << endl;
    else cout << MSG_COM_DETAIL << this->GetStoneName(this->Com) << endl;
}

//相手の石の色を表示します。
void Othello::ShowOtherStone()
{
    if(this->Other == STONE::NONE) cout << MSG_ON_OTHERSTONE_ISNT_SELECTED << endl;
    else cout << MSG_OTHER_DETAIL << this->GetStoneName(this->Other) << endl;
}

//石の凡例を表示します。
void Othello::ShowStoneExample()
{
    string BLACK = S_BLACK;
    string WHITE = S_WHITE;
    if(this->IsDarkMode)
    {
        BLACK = S_WHITE;
        WHITE = S_BLACK;
    }
    cout << MSG_NAME_NONE << S_NONE << endl;
    cout << MSG_NAME_BLACK << BLACK << endl;
    cout << MSG_NAME_WHITE << WHITE << endl;
}

//各石の統計を表示します。
void Othello::ShowStats()
{
    cout << MSG_ON_STATS_START << endl;
    cout << MSG_NAME_NONE << this->CountEmpty() << endl;
    cout << MSG_NAME_BLACK << this->CountBlack() << endl;
    cout << MSG_NAME_WHITE << this->CountWhite() << endl;
}

//対戦モードを表示します。
void Othello::ShowMode()
{
    cout << MSG_ON_MODE;
    if(this->IsAIMode) cout << MSG_ON_AI;
    else cout << MSG_ON_PAIR;
    cout << endl;
}

//プレイヤーの石の色を決定します。
void Othello::SetPlayerStone(STONE playerst)
{
    switch(playerst)
    {
        case STONE::BLACK:
            this->Player = playerst;
            this->Com = STONE::WHITE;
            this->Other = STONE::WHITE;
            break;
        case STONE::WHITE:
            this->Player = playerst;
            this->Com = STONE::BLACK;
            this->Other = STONE::BLACK;
            break;
        default:
            break;
    }
}

//ダークモードの設定をします。   
void Othello::SetDarkMode(bool newmode)
{
    this->IsDarkMode = newmode;
    if(this->IsDarkMode) cout << MSG_ON_SET_DARKMODE << endl;
    else cout << MSG_ON_RESET_DARKMODE << endl;
}

//対戦モードを選択します。
void Othello::SetMode(bool newmode)
{
    if(this->IsPlaying)
    {
        cout << MSG_ON_ERROR << endl;
        return;
    }
    this->IsAIMode = newmode;
}

//盤面から石の種類を取得します。
STONE Othello::Get(const AT at)
{
    return this->table[at.second][at.first];
}

//石の名称を返します。
string Othello::GetStoneName(STONE st)
{
    switch(st)
    {
        case STONE::NONE:   
            return "None";
            break;
        case STONE::WHITE:
            return "White";
            break;
        case STONE::BLACK:
            return "Black";
            break;
    }
    return "None";
}

//プレイヤーに石を置かせます
void Othello::Put()
{
    this->IsPlaying = true;
    if(this->Player == STONE::NONE)
    {
        cout << MSG_ON_PLAYERSTONE_ISNT_SELECTED << endl;
        return;
    }
    AT Cursor = {0,0};
    while(true)
    {
        bool IsEnter = false;
        bool IsNotPuttable = false;
        if(kbhit())
        {
            if(getch() == 'q') return;
            switch(getch())
            {
                case 'w':
                    if(IsBoundAtDirction(Cursor,DIRECTIONS[UP])) break;
                    Add(Cursor,DIRECTIONS[UP]);
                    break;
                case 's':
                    if(IsBoundAtDirction(Cursor,DIRECTIONS[DOWN])) break;
                    Add(Cursor,DIRECTIONS[DOWN]);
                    break;
                case 'a':
                    if(IsBoundAtDirction(Cursor,DIRECTIONS[LEFT])) break;
                    Add(Cursor,DIRECTIONS[LEFT]);
                    break;
                case 'd':
                    if(IsBoundAtDirction(Cursor,DIRECTIONS[RIGHT])) break;
                    Add(Cursor,DIRECTIONS[RIGHT]);
                    break;
                case 13://enter
                    if(this->IsAbletoPut(Cursor,this->Player)) IsEnter = true;
                    break;
            }
            this->ClearConsole();
            this->PrintTableCursor(Cursor);
            auto clc = ToLocation(Cursor);
            cout << clc.first << "," << clc.second << endl;
            if(this->IsAbletoPut(Cursor,this->Player)){}
            else cout << MSG_ON_NOT_PUTTABLE << endl;
            if(IsEnter) break;
        }
    }
    if(this->Put(Cursor,this->Player)) return;
    this->ClearConsole();
    cout << MSG_PLAYER << endl;
    this->PrintTable();
    if(this->IsAIMode)
    {
        this->On_Comp(this->Com);
    }
    else 
    {
        this->On_Other(this->Other);
    }
    this->PrintTable();
    this->ShowStats();
}

//盤面に石を置きます。
//戻り値:勝敗がついているかどうか
bool Othello::Put(const AT at,STONE newst)
{
    this->On_PutStone(at,newst);
    this->table[at.second][at.first] = newst;    
    return this->State();
}

//指定した位置から指定した方向が境界線上かどうかを返します。
bool Othello::IsBoundAtDirction(const AT &location,const AT &direction)
{
    if((location.first + direction.first) < 0 ||
    (location.first + direction.first) > SIZE-1) return true;
    if((location.second + direction.second) < 0 ||
    (location.second + direction.second) > SIZE-1) return true;
    return false;
}

//指定した盤面で、指定した石の数を返します。
int Othello::Count(const TABLE &tb,const STONE st)
{
    int cnt = 0;
    for(auto y : tb)
    {
        for(auto x : y)
        {
            if(x == st) cnt++;
        }
    }
    return cnt;
}

//盤面を全て空白で初期化します。
void Othello::InitializeTable(TABLE &tb)
{
    tb = TABLE(SIZE,LINE(SIZE,STONE::NONE));
}

//作成した盤面を別の盤面からインポートして初期化します。
void Othello::InitializeTable(TABLE &tb,TABLE tb_init)
{
    Othello::InitializeTable(tb);
    for(int y = 0;y < SIZE;y++)
    {
        for(int x = 0;x < SIZE;x++)
        {
            tb[y][x] = tb_init[y][x];
        }
    }
}


//指定した場所に石が無いかどうかを返します。
bool Othello::IsEmptyAt(const AT location)
{
    if(Get(location) == STONE::NONE) return true;
    return false;
}

//指定した石が、指定した場所に配置可能かどうかを返します。
bool Othello::IsAbletoPut(AT at,STONE st)
{
    if(!this->IsEmptyAt(at)) return false;
    for(auto a : this->FindPut(st))
    {
        if(a == at) return true;
    }
    return false;
}

//盤面を初期の配置にします。
void Othello::InitializeTable(const vector<vector<int>> &init)
{
    this->table = TABLE(SIZE,LINE(SIZE,STONE::NONE));
    for(int y = 0;y < SIZE;y++)
    {
        for(int x = 0;x < SIZE;x++)
        {
            switch(init[y].at(x))
            {
                case STONE::NONE:
                    break;
                case STONE::BLACK:
                    this->table[y][x] = STONE::BLACK;
                    break;
                case STONE::WHITE:
                    this->table[y][x] = STONE::WHITE;
                    break;
            }
        }
    }
}

//現在の盤面を保存します。
void Othello::SaveTable()
{
    Othello::SaveTable(this->table,this->history);
}

//前回の盤面を取得します。
//存在しない場合には、空の盤面が返ります。
TABLE Othello::GetTablePrev()
{
    TABLE current;
    if(this->history.size() == 0) current;
    return this->history[this->history.size() - 1];
}

//盤面を出力します。
void Othello::PrintTable()
{
    Othello::ShowTable(this->table,this->IsDarkMode);
}

//盤面のデータを出力します。
void Othello::PrintTableRaw()
{
    for(auto y : this->table)
    {
        for(auto x : y)
        {
            cout << x << " ";
        }
        cout << endl;
    }
}

//配置する場所を示すカーソルを含む盤面を出力します。
void Othello::PrintTableCursor(AT cursor)
{
    string BLACK = S_BLACK;
    string WHITE = S_WHITE;
    if(this->IsDarkMode)
    {
        BLACK = S_WHITE;
        WHITE = S_BLACK;
    }
    for(int y = 0;y < SIZE;y++)
    {
        for(int x = 0;x < SIZE;x++)
        {
            if(cursor == make_pair(x,y))
            {
                cout << S_CURSOR;
                continue;
            }
            switch(this->table[y][x])
            {
                case STONE::NONE:
                    cout << S_NONE;
                    break;
                case STONE::BLACK:
                    cout << BLACK;
                    break;
                case STONE::WHITE:
                    cout << WHITE;
                    break;
            }
        }
        cout << endl;
    }
}

//前回の盤面を出力します。
void Othello::PrintTablePrev()
{
    TABLE current = this->GetTablePrev();
    if(!Othello::IsValid(current)) return;
    Othello::ShowTable(current);
}

//現在盤面上にある空白の数を返します。
int Othello::CountEmpty()
{
    int cnt = 0;
    for(auto y : this->table)
    {
        for(auto x : y)
        {
            if(x == STONE::NONE) cnt++;
        }
    }
    return cnt;
}

//現在盤面上にある黒石の数を返します。
int Othello::CountBlack()
{
    int cnt = 0;
    for(auto y : this->table)
    {
        for(auto x : y)
        {
            if(x == STONE::BLACK) cnt++;
        }
    }
    return cnt;
}

//現在盤面上にある白石の数を返します。
int Othello::CountWhite()
{
    int cnt = 0;
    for(auto y : this->table)
    {
        for(auto x : y)
        {
            if(x == STONE::WHITE) cnt++;
        }
    }
    return cnt;
}

//コンソールを消去します。
void Othello::ClearConsole()
{
    system(CMD_CLEARCONSOLE);
}

//指定した方向に石配列を走査し、置いた石と異なる石を見つけたら裏返します。
//空白の場合は走査を中止します。
void Othello::Turn(const STONE &st,AT at,AT direction)
{
    FindTurn(st,at,direction,this->table);
}

//石が置かれた時の処理
void Othello::On_PutStone(const AT at,STONE st)
{
    Othello::SaveTable();
    for(auto d : DIRECTIONS)
    {
        this->Turn(st,at,d);
    }
}

//ボット(Computer)が石を打つ時の処理
void Othello::On_Comp(STONE st)
{
    this->Now = WHO::OTHER;
    cout << MSG_COM << endl;
    auto mx = this->FindMaxPoint(st);
    if(!Othello::IsValid(mx))
    {
        cout << MSG_ON_SKIP << endl;
        return;
    }
    this->Put(mx,st);
    this->ShowLocation(mx);
    this->Now = WHO::PLAYER;
}

//プレイヤーが石を置いた時の処理
void Othello::On_Player(STONE st)
{
    this->Now = WHO::OTHER;
    this->On_Comp(this->Other);
    this->Now = WHO::PLAYER;
}

//相手が石を置く時の処理
void Othello::On_Other(STONE st)
{
    if(this->Other == STONE::NONE)
    {
        cout << MSG_ON_OTHERSTONE_ISNT_SELECTED << endl;
        return;
    }
    AT Cursor = {0,0};
    cout << MSG_OTHER << endl;
    while(true)
    {
        bool IsEnter = false;
        bool IsNotPuttable = false;
        if(kbhit())
        {
            if(getch() == 'q') return;
            switch(getch())
            {
                case 'w':
                    if(IsBoundAtDirction(Cursor,DIRECTIONS[UP])) break;
                    Add(Cursor,DIRECTIONS[UP]);
                    break;
                case 's':
                    if(IsBoundAtDirction(Cursor,DIRECTIONS[DOWN])) break;
                    Add(Cursor,DIRECTIONS[DOWN]);
                    break;
                case 'a':
                    if(IsBoundAtDirction(Cursor,DIRECTIONS[LEFT])) break;
                    Add(Cursor,DIRECTIONS[LEFT]);
                    break;
                case 'd':
                    if(IsBoundAtDirction(Cursor,DIRECTIONS[RIGHT])) break;
                    Add(Cursor,DIRECTIONS[RIGHT]);
                    break;
                case 13://enter
                    if(this->IsAbletoPut(Cursor,this->Other)) IsEnter = true;
                    break;
            }
            this->ClearConsole();
            this->PrintTableCursor(Cursor);
            auto clc = ToLocation(Cursor);
            cout << clc.first << "," << clc.second << endl;
            if(this->IsAbletoPut(Cursor,this->Other)){}
            else cout << MSG_ON_NOT_PUTTABLE << endl;
            if(IsEnter) break;
        }
    }
    if(this->Put(Cursor,this->Other)) return;
    this->ClearConsole();
    cout << MSG_OTHER << endl;
}

//指定した石が配置可能な場所を探索し、その位置情報を格納した配列を返します。
vector<AT> Othello::FindPut(STONE st)
{
    vector<AT> puttable;
    for(int y = 0;y < SIZE;y++)
    {
        for(int x = 0;x < SIZE;x++)
        {
            if(this->IsEmptyAt({x,y}))
            {
                bool frag = false;
                for(auto d : DIRECTIONS)
                {
                    
                    AT to = {x,y};
                    if(this->IsBoundAtDirction(to,d)) continue;
                    //Add(to,d);
                    int cnt = 0;
                    bool fragd = false;
                    do
                    {
                        Add(to,d);
                        cnt++;
                        if(d == DIRECTIONS[DOWN])
                        {
                            //cout << to.first << "," << to.second << ":" << this->GetStoneName(Get(to)) << endl;
                        }
                        if(!this->IsEmptyAt(to))
                        {
                            //cout << "this->Get(to) == st && !Eq({x,y},to):" << (this->Get(to) == st && !Eq({x,y},to)) << endl;
                            //cout << "this->Get(to) == st:" << (this->Get(to) == st) << endl;
                            if(cnt < 2 && this->Get(to) == st) break;
                            else if(this->Get(to) == st && !Eq({x,y},to))
                            {
                                puttable.push_back({x,y});
                                //cout << "Puttable:" << x << "," << y << endl;
                                frag = true;
                                break;
                            }
                        }
                        else break;
                        //Add(to,d);
                        //cnt++;
                    }
                    while(!this->IsBoundAtDirction(to,d));
                    /*
                    if(Eq({4,7},to))
                    {
                        cout << "Break:"  << this->GetStoneName(Get(to)) << endl;
                        cout << "x,y:" << x << "," << y << " to:" << to.first << "," << to.second << endl;
                    }
                    */
                    if(frag) break;
                }
                //if(frag) break;
            }
        }
    }
    return puttable;
}

//[ボット用] 指定した石を配置した際に得られる得点(=裏返した石の数)が最大になるような位置を探索し返します。
//配置不可の場合には(-1,-1)が返ります。
AT Othello::FindMaxPoint(STONE st)
{
    auto puts = this->FindPut(st);
    if(puts.size() == 0) return {-1,-1};
    vector<int> points;
    for(auto a : puts)
    {
        auto pt = this->Simulate(this->Com,a);
        points.push_back(pt);
        //Othello::ShowLocation(a);
    }
    int idx = max_element(points.begin(),points.end()) - points.begin();
    return puts[idx];
}

//現在の盤面から仮想の盤面を作成しそこに石を置き、返した後の石の総数を返します。
int Othello::Simulate(STONE st,AT at)
{
    Othello::InitializeTable(this->table_v,this->table);
    Othello::Put(at,st,this->table_v);
    Othello::Turn(st,at,this->table_v);
    //Set(at,st,this->table_v);
    return Count(this->table_v,st);
}

//"exit"または"quit"コマンドが呼ばれた際に呼びだします。
bool Othello::Exit()
{
    return true;
}

//勝敗がついているかどうかを確認します。
//勝敗がついていた場合にはメッセージを表示しtrueを返します。
bool Othello::State()
{
    auto res = Othello::State(this->table,this->Player);
    if(res != STONE::NONE)
    {
        cout << MSG_ON_GAMEENDED << endl;
        cout << MSG_ON_WINNER << endl;        
        if(res == this->Player) cout << MSG_PLAYER << endl;
        else cout << MSG_OTHER << endl;
    }
    return false;
}

//指定した位置データの要素同士を加算します。(a += b)
void Othello::Add(AT &a,AT b)
{
    a.first += b.first;
    a.second += b.second;
}

//指定した位置データの要素同士が等しいかどうかを返します。(a == b)
bool Othello::Eq(AT a,AT b)
{
    if(a.first == b.first && a.second == b.second) return true;
    return false;
}

//指定した盤面、位置の石を取得します。
STONE Othello::Get(AT at,TABLE &tb)
{
    return tb[at.second][at.first];
}

//指定した盤面、位置に石を置きます。
//あるいは、取り除きます。
void Othello::Set(AT at,STONE st,TABLE &tb)
{
    tb[at.second][at.first] = st;
}

//ATデータ型(0~7,0~7)を左からx番目(1~8)、上からy番目(1~8)という形式に変換します。
LOCATION Othello::ToLocation(AT at)
{
    return {at.first+1,at.second+1};
}

void Othello::Put(AT at,STONE st,TABLE &tb)
{
    tb[at.second][at.second] = st;
}

//指定した位置データが有効かどうかを返します。
//(範囲外だったり、不正な数である場合はfalseが返ります。)
bool Othello::IsValid(AT at)
{
    if(at.first < 0 || at.first >= SIZE) return false;
    else if(at.second < 0 || at.second >= SIZE) return false;
    return true;
}

//指定した盤面が有効かどうかを返します。
//空の場合にはfalseが返ります。
bool Othello::IsValid(TABLE &tb)
{
    TABLE init;
    if(init == tb) return false;
    else if(tb.size() == 0) return false;
    return true;
}

//指定した盤面を出力します。
void Othello::ShowTable(TABLE &table,bool Isdarkmode)
{
    string BLACK = S_BLACK;
    string WHITE = S_WHITE;
    if(Isdarkmode)
    {
        BLACK = S_WHITE;
        WHITE = S_BLACK;
    }
    for(auto y : table)
    {
        for(auto x : y)
        {
            switch(x)
            {
                case STONE::NONE:
                    cout << S_NONE;
                    break;
                case STONE::BLACK:
                    cout << BLACK;
                    break;
                case STONE::WHITE:
                    cout << WHITE;
                    break;
            }
        }
        cout << endl;
    }
    cout << endl;
}

//指定した盤面、指定した方向に石配列を走査し、置いた石と異なる石を見つけたら裏返します。
//空白の場合は走査を中止します。
void Othello::FindTurn(const STONE &st,AT at,AT direction,TABLE &tb)
{
    if(Othello::IsBoundAtDirction(at,direction)) return;
    auto to = at;
    int cnt = 0;
    bool found = false;
    while(true)
    {
        if(Othello::IsBoundAtDirction(at,direction)) break;
        Add(at,direction);
        auto current = Get(at,tb);
        if((current == STONE::NONE)) return;
        if((st == current))
        {
            found = true;
            cnt++;
            break;
        }
        cnt++;
    }
    if(cnt <= 1 || !found) return;
    for(int i = 0;i < cnt;i++)
    {
        Add(to,direction);
        Set(to,st,tb);
    }
}

//指定した盤面で指定した場所から全ての方向を探索し、裏返せる石があれば裏返します。
void Othello::Turn(const STONE &st,AT at,TABLE &tb)
{
    for(auto d : DIRECTIONS)
    {
        Othello::FindTurn(st,at,d,tb);
    }
}

//指定した位置データをマス位置として表示します。
void Othello::ShowLocation(AT at)
{
    auto l = Othello::ToLocation(at);
    cout << "(" << l.first << "," << l.second << ")" << endl;
}

//指定した盤面を履歴データに保存します
void Othello::SaveTable(TABLE &tb,HISTORY &history)
{
    TABLE current;
    Othello::InitializeTable(current,tb);
    history.push_back(current);
}

//指定した盤面から勝敗を判定します。返り値は勝利した石を返します。
//勝負がついていない場合は空白が返されます。
//ドローの場合にはプレイヤーの勝利
STONE Othello::State(TABLE &tb,STONE player)
{
    if(Othello::Count(tb,STONE::NONE) != 0) return STONE::NONE;
    int b = Othello::Count(tb,STONE::BLACK);
    int w = Othello::Count(tb,STONE::WHITE);
    if(b == w) return player;
    else if(b > w) return STONE::BLACK;
    return STONE::WHITE;
}

int main()
{
    Othello othello;
    othello.Run();
    return 0;
}