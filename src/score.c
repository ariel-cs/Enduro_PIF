#include <stdio.h>
#include <string.h>
#include "score.h"

#ifdef _WIN32
#include <direct.h> // _mkdir no Windows (.clang ta chorando mas é falso positivo)
#define MakeDir(path) _mkdir(path)
#else
#include <sys/stat.h> // mkdir no Unix/Linux/Mac
#define MakeDir(path) mkdir(path, 0755)
#endif

#define SCORES_DIR "data"
#define SCORES_PATH "data/scores.txt"

static void EnsureDataDir(void) {
    MakeDir(SCORES_DIR); // cria a pasta data se nao existir
}

void SaveScore(const char *name, int score) {
    EnsureDataDir(); // garante que a pasta data exista antes de escrever
    FILE *file = fopen(SCORES_PATH, "a"); //ponteiro do tipo FILE abri o score o "a" serve pra appendar cria ou edita (cria se nao existir)
    if (file != NULL){
        fprintf(file, "%s %d\n",name,score); // print pra tipo file de nome e int
        fclose(file); // fecha arquivo que nem em python ele tem que fechar e abrir para evitar erros
    }
}

int LoadTopScores(ScoreEntry *top_scores, int max_count) {
    FILE *file = fopen(SCORES_PATH, "r"); // abri o file so que dessa vez no modo leitura
    if (file == NULL) {
        return 0;
    }

    ScoreEntry temp_scores[100]; //lista de struct temporaria para guardar 100 scores
    int count = 0;

    while (fscanf(file, "%9s %d",temp_scores[count].name, &temp_scores[count].score) == 2) { //lendo o score.txt por linha e == 2 limita a quantidade de tipo de variaveis por linha
        count++;
        if (count >= 100) break;
    }
    fclose(file); //fecha novamente a file

    for (int i = 0; i < count - 1; i++) { //bubble sort ordena os scores
        for (int j = i + 1; j < count; j++) {
            if(temp_scores[j].score > temp_scores[i].score){
                ScoreEntry temp = temp_scores[i];
                temp_scores[i] = temp_scores[j];
                temp_scores[j] = temp;
            }
        }
    }

    int return_count = (count < max_count) ? count : max_count; //operador ternario if count < max ele vai ser = count else = max count
    for (int i = 0; i < return_count; i++) {
        top_scores[i] = temp_scores[i]; // passa o temporario para o definitivo
    }
    return return_count;
}
