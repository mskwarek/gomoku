#ifndef __UI_H__
#define __UI_H__

#include <sys/types.h>
#include <stdlib.h>
#include <ncurses.h>
#include <ctype.h>
#include <unistd.h>
#define SIZE 20
  //DWÓCH GRACZY
#define Empty 0
#define Cross 1
#define Nought 2



#define Null 0
#define Horiz 1
#define DownLeft 2
#define DownRight 3
#define Vert 4


char PieceChar[Nought + 1] = {' ', 'X','o'};
int Board[SIZE + 1][SIZE + 1];  /* plansza */


  /* Value of having 0, 1,2,3,4 or 5 pieces in line */
int Weight[9] = {0, 0, 4, 20, 100, 500, 2500, 12500, 0};




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

#endif
