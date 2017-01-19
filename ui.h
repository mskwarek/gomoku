#include <sys/types.h>
#include <stdlib.h>

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