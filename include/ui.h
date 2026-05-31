#ifndef UI_H
#define UI_H

#include "game.h"

// Modulo de renderizacao de UI/telas. Concentra o que antes estava inline no main.c.
// (Outras telas — game over, ranking — serao migradas para ca.)

// Renderiza o HUD durante a corrida (DIST, TEMPO, PONTOS, DIA, FALTAM).
void DrawHUD(const GameState *game);

#endif
