#include <sys/types.h>
#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

  /* rozmiar planszy */
#define SIZE 20

  /* Value of having 0, 1,2,3,4 or 5 pieces in line */
int Weight[9] = {0, 0, 4, 20, 100, 500, 2500, 12500, 0};

#define Null 0
#define Horiz 1
#define DownLeft 2
#define DownRight 3
#define Vert 4

  //DWÓCH GRACZY
#define Empty 0
#define Cross 1
#define Nought 2

char PieceChar[Nought + 1] = {' ', 'X','o'};

int Board[SIZE + 1][SIZE + 1];  /* plansza */
int Player;                     /* gracz który rusza sie następny */
int TotalLines;                 /* linijki które pozostały wolne */
int GameWon;                    /* zmienna ustawiana gdy gra się kończy */
int condition;                  /* warunek kończący grę */
int WinningLine;

int Line[4][SIZE + 1][SIZE + 1][Nought + 1];

  /* użyteczność pola dla konkretnego gracza */
int Value[SIZE + 1][SIZE + 1][Nought + 1];

int X, Y;                       /* Move coordinates */
char Command;                   /* Polecenie z klawiatury */
int AutoPlay = FALSE;           /* program vs program */

  /*prototypy funkcji użytych w programie*/
void Initialize (void);
int Abort (char *s);
void WriteLetters (void);
void WriteLine (int j, int *s);
void WriteBoard (int N, int *Top, int *Middle, int *Bottom);
void SetUpScreen (void);
void GotoSquare (int x, int y);
void PrintMove (int Piece, int X, int Y);
void ClearMove (void);
void PrintMsg (char *Str);
void ClearMsg (void);
void WriteCommand (char *S);
void ResetGame ();
int OpponentColor (int Player);
void BlinkRow (int X, int Y, int Dx, int Dy, int Piece);
void BlinkWinner (int Piece, int X, int Y, int WinningLine);
int Random (int x);
void Add (int *Num);
void Update (int Lin[], int Valu[], int Opponent);
void MakeMove (int X, int Y);
int GameOver (void);
void FindMove (int *X, int *Y);
char GetChar (void);
void ReadCommand (int X, int Y, char *Command);
void InterpretCommand (char Command);
void PlayerMove (void);
void ProgramMove (void);
void WriteFile();
void ReadFile();
int main (void);

void Initialize()
{
    //srand(getpid() + 13);
    initscr();

    raw();
    refresh();
    noecho();
    setterm(0);
    clear();
}

  //-------------------CZYSZCZENIE I WYJSCIE Z PROGRAMU------------------
int Abort(char *s)
{
    move(LINES - 1, 0);
    refresh();
    endwin();
    exit(0);
}

 //-----------------LITERKI DO KOLUMN-----------------------------------
void WriteLetters()
{
   int i;

   addch(' ');
   addch(' ');
   for (i = 1; i <= SIZE; i++) printw(" %c", 'A' + i - 1);
   addch('\n');
}

//--------------ZAPISANIE JEDNEJ LINIJKI PLANSZY-------------------------
void WriteLine(int j, int *s)
{
   int i;
   printw("%2d ", j);
   addch(s[0]);
   for (i = 2; i <= SIZE - 1; i++) {
         addch(s[1]);
         addch(s[2]);
   }
   addch(s[1]);
   addch(s[3]);
   printw(" %-2d\n", j);
}

//--------------------PUSTA PLANSZA------------------------------
void WriteBoard(int N, int *Top, int *Middle, int *Bottom)
{
   int j;

   move(1, 0);
   WriteLetters();
   WriteLine(N, Top);
   for (j = N - 1; j >= 2; j--) WriteLine(j, Middle);
   WriteLine(1, Bottom);
   WriteLetters();
}

 //------------------------USTAWIENIE EKRANU----------------------
void SetUpScreen()
{
   int top[4], middle[4], bottom[4];

   top[0] = ACS_ULCORNER;
   top[1] = ACS_HLINE;
   top[2] = ACS_TTEE;
   top[3] = ACS_URCORNER;

   middle[0] = ACS_LTEE;
   middle[1] = ACS_HLINE;
   middle[2] = ACS_PLUS;
   middle[3] = ACS_RTEE;

   bottom[0] = ACS_LLCORNER;
   bottom[1] = ACS_HLINE;
   bottom[2] = ACS_BTEE;
   bottom[3] = ACS_LRCORNER;

   WriteBoard(SIZE, top, middle, bottom);
}

//-------------------------POKAZYWANIE RUCHÓW--------------------------

void GotoSquare(int x, int y)
{
   move(SIZE + 2 - y, 1 + x * 2);
}

void PrintMove(int Piece, int X, int Y)
{
   move(SIZE + 3, SIZE + 30);
   printw("%c %c %d", PieceChar[Piece], 'A' + X - 1, Y);
   clrtoeol();
   GotoSquare(X, Y);
   addch(PieceChar[Piece]);
   GotoSquare(X, Y);
   refresh();
}

//--------------------CZYSZCZENIE POKAZYWACZA-----------------------------
void ClearMove()
{
   move(SIZE + 3, SIZE + 30);
   clrtoeol();
}

//-------------------POMOCNICZE KOMUNIKATY------------------------
void PrintMsg(char *Str)
{
   mvprintw(SIZE + 3 , 1, "%s", Str);
}

void ClearMsg()
{
   move(SIZE + 3, 1);
    clrtoeol();
}

//--------------------PODSWIETLANIE LITERKI------------------------
void WriteCommand(char *S)
{
   standout();
   addch(*S);
   standend();
   printw("%s", S + 1);
}


 //---------------------------TABLICOWE-----------------------------

 //--------------------------RESET EKRANU--------------------------
void ResetGame()
{
   int I, J;
   int C, D;

   SetUpScreen();

         ClearMsg();
         ClearMove();


   for (I = 1; I <= SIZE; I++) for (J = 1; J <= SIZE; J++) {
                 Board[I][J] = Empty;
                 for (C = Cross; C <= Nought; C++) {
                         Value[I][J][C] = 0;
                         for (D = 0; D <= 3; D++) Line[D][I][J][C] = 0;
                 }
         }

   Player = Cross;
   TotalLines = 2 * 2 * (SIZE * (SIZE - condition -1) + (SIZE- condition -1 ) * (SIZE - condition -1));
   GameWon = FALSE;
}

int OpponentColor(int Player)
{
   if (Player == Cross)
         return Nought;
   else
         return Cross;
}

 //------------------PODŚWIETLANIE LINII WYGRANEGO--------------------
void BlinkRow(int X, int Y, int Dx, int Dy, int Piece)
{
   int I;

   attron(A_BLINK);
   for (I = 1; I <= condition; I++) {
         GotoSquare(X, Y);
         addch(PieceChar[Piece]);
         X = X - Dx;
         Y = Y - Dy;
   }
   attroff(A_BLINK);
}


void BlinkWinner(int Piece, int X, int Y, int WinningLine)
{
   /* uzywane do "przytrzymania" wygrywajacego ruchu */
   int XHold, YHold;
   /*zmiana w x i w y (delta)*/
   int Dx, Dy;


   PrintMove(Piece, X, Y);

   XHold = X;
   YHold = Y;
   switch (WinningLine) {
       case Horiz:
         {
                 Dx = 1;
                 Dy = 0;
                 break;
         }

       case DownLeft:
         {
                 Dx = 1;
                 Dy = 1;
                 break;
         }

       case Vert:
         {
                 Dx = 0;
                 Dy = 1;
                 break;
         }

       case DownRight:
         {
                 Dx = -1;
                 Dy = 1;
                 break;
         }
   }

   /* Go to topmost, leftmost */
   while (Board[X + Dx][Y + Dy] != Empty && Board[X + Dx][Y + Dy] == Piece) {
         X = X + Dx;
         Y = Y + Dy;
   }
   BlinkRow(X, Y, Dx, Dy, Piece);
   X = XHold;
   Y = YHold;
   GotoSquare(X, Y);
}

 //------------------------GRANIE----------------------------

int Random(int x)
{
   return((rand() / SIZE) % x);
}

 //---------------------ZLICZANIE ZNAKÓW W LINII-------------
void Add(int *Num)
{
   /* Adds one to the number.     */
   *Num = *Num + 1;
   /* If it is the first piece in the line, then the opponent cannot use
    * it any more.  */
   if (*Num == 1) TotalLines = TotalLines - 1;
   if (*Num == condition) GameWon = TRUE;
}
 /* Updates the value of a square for each player, taking into
    account that player has placed an extra piece in the square.
    The value of a square in a usable line is Weight[Lin[Player]+1]
    where Lin[Player] is the number of pieces already placed
 in the line */
void Update(int Lin[], int Valu[], int Opponent)
{
   /* If the opponent has no pieces in the line, then simply update the
    * value for player */
   if (Lin[Opponent] == 0)
         Valu[Player] += Weight[Lin[Player] + 1] - Weight[Lin[Player]];
   else
         /* If it is the first piece in the line, then the line is
          * spoiled for the opponent */
   if (Lin[Player] == 1) Valu[Opponent] -= Weight[Lin[Opponent] + 1];
}

void MakeMove(int X, int Y)
{
   int Opponent;
   int X1, Y1;
   int K, L;

   WinningLine = Null;
   Opponent = OpponentColor(Player);
   GameWon = FALSE;

   /* Each square of the board is part of 20 different lines. The adds
    * one to the number of pieces in each of these lines. Then it
    * updates the value for each of the 5 squares in each of the 20
    * lines. Finally Board is updated, and the move is printed on the
    * screen. */

   /* Horizontal lines, from left to right */
   for (K = 0; K <= condition-1; K++) {
         X1 = X - K;             /* Calculate starting point */
         Y1 = Y;
         if ((1 <= X1) && (X1 <= SIZE - condition-1)) {    /* Check starting point */
                 Add(&Line[0][X1][Y1][Player]);  /* Add one to line */
                 if (GameWon && (WinningLine == Null))   /* Save winning line */
                         WinningLine = Horiz;
                 for (L = 0; L <= condition-1; L++)        /* Update value for the
                                                  * 5 squares in the line */
                         Update(Line[0][X1][Y1], Value[X1 + L][Y1], Opponent);
         }
   }

   for (K = 0; K <= condition-1; K++) {    /* Diagonal lines, from lower left to
                                  * upper right */
         X1 = X - K;
         Y1 = Y - K;
         if ((1 <= X1) && (X1 <= SIZE - condition-1) &&
             (1 <= Y1) && (Y1 <= SIZE - condition-1)) {
                 Add(&Line[1][X1][Y1][Player]);
                 if (GameWon && (WinningLine == Null))   /* Save winning line */
                         WinningLine = DownLeft;
                 for (L = 0; L <= condition-1; L++)
                         Update(Line[1][X1][Y1], Value[X1 + L][Y1 + L], Opponent);
         }
   }                             /* for */

   for (K = 0; K <= condition-1; K++) {    /* Diagonal lines, down right to upper left */
         X1 = X + K;
         Y1 = Y - K;
         if ((5 <= X1) && (X1 <= SIZE) &&
             (1 <= Y1) && (Y1 <= SIZE - condition-1)) {
                 Add(&Line[3][X1][Y1][Player]);
                 if (GameWon && (WinningLine == Null))   /* Save winning line */
                         WinningLine = DownRight;
                 for (L = 0; L <= condition-1; L++)
                         Update(Line[3][X1][Y1], Value[X1 - L][Y1 + L], Opponent);
         }
   }                             /* for */

   for (K = 0; K <= condition-1; K++) {    /* Vertical lines, from down to up */
         X1 = X;
         Y1 = Y - K;
         if ((1 <= Y1) && (Y1 <= SIZE - condition-1)) {
                 Add(&Line[2][X1][Y1][Player]);
                 if (GameWon && (WinningLine == Null))   /* Save winning line */
                         WinningLine = Vert;
                 for (L = 0; L <= condition-1; L++)
                         Update(Line[2][X1][Y1], Value[X1][Y1 + L], Opponent);
         }
   }

   Board[X][Y] = Player;         /* Place piece in board */
   if (GameWon)
         BlinkWinner(Player, X, Y, WinningLine);
   else
         PrintMove(Player, X, Y);/* Print move on screen */
   Player = Opponent;            /* The opponent is next to move */
}


int GameOver()
{
   return(GameWon || (TotalLines <= 0));
}

 /* Znajdowanie ruchu, czyli wybranie pola o największej użyteczności */
void FindMove(int *X, int *Y)
{
   int Opponent;
   int I, J;
   int Max, Valu;

   Opponent = OpponentColor(Player);
   Max = -1000;
   /* If no square has a high value then pick the one in the middle */
   *X = (SIZE + 1) / 2;
   *Y = (SIZE + 1) / 2;
   if (Board[*X][*Y] == Empty) Max = condition;
   /* The evaluation for a square is simply the value of the square for
    * the player (attack points) plus the value for the opponent
    * (defense points). Attack is more important than defense, since it
    * is better to get 5 in line yourself than to prevent the op- ponent
    * from getting it. */

   /* For all Empty squares */
   for (I = 1; I <= SIZE; I++) for (J = 1; J <= SIZE; J++)
                 if (Board[I][J] == Empty) {
                         /* Calculate evaluation */
                         Valu = Value[I][J][Player] * (16 + condition-1) / 16 + Value[I][J][Opponent] + Random(condition-1);
                         /* Pick move with highest value */
                         if (Valu > Max) {
                                 *X = I;
                                 *Y = J;
                                 Max = Valu;
                         }
                 }
}

char GetChar()
{
   int c;

   c = getch();
   if (c < 0) abort();
   if (c == '\033') {    /* arrow key */
         if ((c = getch()) == '[') {
                 c = getch();
                 switch (c) {
                 case 'A': c = 'U'; break;
                 case 'B': c = 'D'; break;
                 case 'C': c = 'R'; break;
                 case 'D': c = 'L'; break;
                 default:
                         c = '?';
                         break;
                 }
         }
         else
                 c = '?';
   }
   if (islower(c))
         return toupper(c);
   else
         return c;
}

//-----------------------STEROWANIE-------------------------------------------------
void ReadCommand(int X, int Y, char *Command)
{
   int ValidCommand;

   do {
         ValidCommand = TRUE;
         GotoSquare(X, Y);
         refresh();
         *Command = GetChar();
         switch (*Command) {
             case '\n':          /* '\n' or space means place a */
             case ' ':
                 *Command = 'E';
                 break;          /* stone at the cursor position  */

             case 'L':
             case 'R':
             case 'U':
             case 'D':
             case '7':
             case '9':
             case '1':
             case '3':
             case 'N':
             case 'Q':
             case 'A':
             case 'P':
             case 'H':
             case 'S':
             case 'W':
                 break;

             case '8':   *Command = 'U'; break;
             case '2':   *Command = 'D'; break;
             case '4':   *Command = 'L'; break;
             case '6':   *Command = 'R'; break;
             default:
                 {
                         if (GameOver())
                                 *Command = 'P';
                         else
                                 ValidCommand = FALSE;
                         break;
                 }
         }
   } while (!ValidCommand);
}

void InterpretCommand(char Command)
{
   int Temp;

   switch (Command)
   {
       case 'N':{                /* Start new game */
                 ResetGame();       /* ResetGame but only redraw
                                          * the board */
                 X = (SIZE + 1) / 2;
                 Y = X;
                 break;
         }
       case 'S':
                WriteFile();
                break;

       case 'W':
                ReadFile();
                break;
       case 'H':
                FindMove(&X, &Y);
                break;                  /* Give the user a hint */
       case 'L':
                X = (X + SIZE - 2) % SIZE + 1;
                break;                  /* Left  */
       case 'R':
                X = X % SIZE + 1;
                break;                  /* Right */
       case 'D':
                Y = (Y + SIZE - 2) % SIZE + 1;
                break;                  /* Down  */
       case 'U':
                Y = Y % SIZE + 1;
                break;                  /* Up    */
       case '7':
       {
                if ((X == 1) || (Y == SIZE))
                {                               /* Move diagonally    *//* t
                                                  * owards upper left */
                         Temp = X;
                         X = Y;
                         Y = Temp;
                }
                else
                {
                         X = X - 1;
                         Y = Y + 1;
                }
                break;
        }
       case '9':
       {                                        /* Move diagonally    */
                if (X == SIZE)
                {                               /* toward upper right */
                         X = (SIZE - Y) + 1;
                         Y = 1;
                }
                else if (Y == SIZE)
                {
                         Y = (SIZE - X) + 1;
                         X = 1;
                }
                else
                {
                         X = X + 1;
                         Y = Y + 1;
                }
                break;
        }
       case '1':
       {                                        /* Move diagonally   */
                if (Y == 1)
                {                               /* toward lower left */
                         Y = (SIZE - X) + 1;
                         X = SIZE;
                }
                else if (X == 1)
                {
                         X = (SIZE - Y) + 1;
                         Y = SIZE;
                }
                else
                {
                         X = X - 1;
                         Y = Y - 1;
                }
                break;
        }
       case '3':
       {                                            /* Move diagonally    */
                if ((X == SIZE) || (Y == 1))
                {                                   /* toward lower right */
                         Temp = X;
                         X = Y;
                         Y = Temp;
                }
                else
                {
                         X = X + 1;
                         Y = Y - 1;
                }
                break;
         }
       case 'A':
                AutoPlay = TRUE;
                break;                  /* Auto play mode */
   }                                    /* case */
}                                       /* InterpretCommand */

void PlayerMove()
{
   if (Board[X][Y] == Empty) {
         MakeMove(X, Y);
         if (GameWon) PrintMsg("WYGRALES!");
         Command = 'P';
   }
   refresh();
}                               /* PlayerMove */

void ProgramMove()
{
   do {
         if (GameOver()) {
                 AutoPlay = FALSE;
                 if ((Command != 'Q') && (!GameWon)) PrintMsg("REMIS!");
         } else {
                 FindMove(&X, &Y);
                 MakeMove(X, Y);
                 if (GameWon) PrintMsg("JA WYGRALEM!");
         }
         refresh();
   } while (AutoPlay);
 }

void WriteFile()
{

    int j, i, z, s;
    FILE *b = fopen("board", "w");
    FILE *v = fopen("value", "w");
    FILE *l = fopen("line", "w");

    /*wpisywanie do plików planszy (board) i użyteczności pół (value) i warunków zwycięstwa (line) */
    for(j=1; j<=SIZE ; j++)
    {
        for(i=1; i<=SIZE ; i++)
        {
            fprintf (b, "%d", Board[i][j]);     /* zapisz nasz łańcuch w pliku */
                for(z=0; z<=2; z++)
                    fprintf (v, "%d\n", Value[j][i][z]);
        }
        fprintf(b, "\n");

    }
    fprintf(b, "%d\n", condition);                /*wpisanie do pliku warunku wygranej (ile znaków w linii)*/
    fprintf(b, "%d", Player);

    for(s=0; s<=3; s++)
        for(j=1; j<=SIZE ; j++)
            for(i=1; i<=SIZE ; i++)
                for(z=0; z<=2; z++)
                    fprintf (l, "%d\n", Line[s][j][i][z]);
    fclose (b);
    fclose (v);
    fclose (l);
}

void ReadFile()
{
    int i, j=0, z, k;
    int e, f, g, h;
    char fileline[SIZE];
    int temptabline[SIZE*SIZE*12];
    int temptabvalue[SIZE*SIZE*3];

    FILE *b = fopen("board", "r");
    FILE *v = fopen("value", "r");
    FILE *l = fopen("line", "r");

    while( !feof(b) )
    {
            if(j<20)
            {
                fscanf(b, "%s", fileline);
                for(i=0; i<SIZE; i++)
                {

                    if(fileline[i]=='1')
                    {
                        Board[i+1][j+1]=1;
                        GotoSquare(i+1, j+1);
                        addch(PieceChar[1]);
                    }
                    else if(fileline[i]=='0')
                        Board[i+1][j+1]=0;

                    else if(fileline[i]=='2')
                    {
                        Board[i+1][j+1]=2;
                        GotoSquare(i+1, j+1);
                        addch(PieceChar[2]);
                    }

                }
            }

            else if(j==20)
                fscanf(b, "%d", &condition);
            else if(j==21)
                fscanf(b, "%d", &Player);

        j++;

    }


    k=0;
    while ( !feof(v) )
    {
        fscanf(v, "%d", &temptabvalue[k]);
        k++;
    }

    k=0;
    for(j=1; j<=SIZE ; j++)
        for(i=1; i<=SIZE ; i++)
            for(z=0; z<=2; z++)
                {
                Value[j][i][z]=temptabvalue[k];
                k++;
                }


    k=0;
    while ( !feof(l) )
    {
        fscanf (l, "%d", &temptabline[k]);
        k++;
    }
    k=0;
    for(e=0; e<=3; e++)
        for(f=1; f<=SIZE ; f++)
            for(g=1; g<=SIZE ; g++)
                for(h=0; h<=2; h++)
                    {
                    Line[e][f][g][h]=temptabline[k];
                    k++;
                    }



    fclose(b);
    fclose(v);
    fclose(l);
    refresh();
}

int main()
{

    //printf("POWIEKSZ TERMINAL\nSTEROWANIE:\nA - GRA AUTOMATYCZNA\nN - NOWA GRA\nS - ZAPISANIE STANU GRY\nQ - WYJSCIE\nPodaj warunek zakonczenia gry:\n>");
    scanf("%d", &condition);
    Initialize();
    ResetGame();
    refresh();

    X = (SIZE + 1) / 2;
    Y = X;
    do
    {
         ReadCommand(X, Y, &Command);
         if (GameOver())
                 if (Command != 'Q') Command = 'N';
         InterpretCommand(Command);
         if (Command == 'E') PlayerMove();
         if (Command == 'P' || Command == 'A') ProgramMove();
    } while (Command != 'Q');
    Abort("PAPA!");
    /*ReadFile();
    printf("%d\n", Player);
    printf("%d\n", condition);*/
    return(0);
}
