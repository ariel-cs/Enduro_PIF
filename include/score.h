#ifndef SCORE_H
#define SCORE_H

typedef struct{
    char name[10];
    int score;
}ScoreEntry;

void SaveScore(const char *name, int score);
int LoadTopScores(ScoreEntry *top_scores,int max_count);

#endif
