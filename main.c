#include <sys/types.h>
#include <ncurses.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#include <ui.h>


  /*prototypy funkcji użytych w programie*/

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
