#ifndef UI_H
#define UI_H

#include "raylib.h"
#include "game.h"
#include "score.h"

// Modulo de renderizacao de UI/telas. Concentra o desenho de cada tela do jogo.

// STATE_LOGO: logo da equipe centralizada sobre fundo preto.
void DrawLogoScreen(Texture2D logo);

// STATE_TITLE: tela de titulo com a arte de fundo e o "PRESSIONE ENTER".
void DrawTitleScreen(Texture2D title);

// STATE_MENU: menu principal (iniciar / volume / sair).
void DrawMenu(Texture2D menuImg, int selectedOption, float masterVolume);

// HUD da corrida (DIST, DIA, FALTAM).
void DrawHUD(const GameState *game);

// STATE_GAME_OVER: pontuacao final e entrada do nome do jogador.
void DrawGameOverScreen(const GameState *game);

// STATE_TOP_SCORES: ranking dos melhores scores.
void DrawTopScoresScreen(const ScoreEntry *scores, int count);

#endif
