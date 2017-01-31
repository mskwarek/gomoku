#include <ui.h>

  /* rozmiar planszy */


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
   {
         return Nought;
   }
   else
   {
         return Cross;
   }
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
