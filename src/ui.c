#include <raylib.h>
#include "ui.h"
#include "config.h"
#include "game.h"
#include "player.h" // IWYU pragma: keep  (necessario p/ player->speed/z; Player e opaco em game.h)

// Paleta do HUD, inspirada no painel do Enduro original (Atari 2600)
static const Color HUD_FRAME = (Color){ 150,  30,  22, 255 };
static const Color HUD_PANEL = (Color){ 216, 172,  92, 255 };
static const Color HUD_DIGIT = (Color){  20,  16,  10, 255 };

// Glifo do carrinho no contador
static void DrawCarGlyph(int x, int y, Color body) {
    DrawRectangle(x + 4, y + 1, 8, 16, body);
    DrawRectangle(x,     y + 3, 4, 4, body);
    DrawRectangle(x + 12, y + 3, 4, 4, body);
    DrawRectangle(x,     y + 11, 4, 4, body);
    DrawRectangle(x + 12, y + 11, 4, 4, body);
    DrawRectangle(x + 6, y + 4, 4, 3, HUD_PANEL);
}

// Texto alinhado à direita
static void DrawTextRight(const char *text, int x, int w, int y, int size, Color color) {
    int tw = MeasureText(text, size);
    DrawText(text, x + w - tw, y, size, color);
}

// Texto centralizado horizontalmente
static void DrawTextCenter(const char *text, int x, int w, int y, int size, Color color) {
    int tw = MeasureText(text, size);
    DrawText(text, x + (w - tw) / 2, y, size, color);
}

void DrawHUD(const GameState *game) {
    int faltam = game->cars_to_pass - game->cars_passed_today;
    if (faltam < 0) faltam = 0;

    // ---- Faixa do painel (banda preta na base, estilo Enduro) ----
    DrawRectangle(0, PLAY_HEIGHT, SCREEN_WIDTH, HUD_HEIGHT, BLACK);

    // ---- Painel principal (centralizado dentro da faixa) ----
    const int boxW = 340;
    const int boxH = 110;
    const int boxX = (SCREEN_WIDTH - boxW) / 2;
    const int boxY = PLAY_HEIGHT + (HUD_HEIGHT - boxH) / 2;
    const int pad  = 12;

    // Moldura com uma borda interna mais escura para dar profundidade.
    DrawRectangle(boxX - 3, boxY - 3, boxW + 6, boxH + 6, (Color){ 90, 16, 12, 255 });
    DrawRectangle(boxX, boxY, boxW, boxH, HUD_FRAME);

    // Linha de cima: DISTÂNCIA
    const int topX = boxX + pad;
    const int topY = boxY + pad;
    const int topW = boxW - 2 * pad;
    const int topH = 44;
    DrawRectangle(topX, topY, topW, topH, HUD_PANEL);
    DrawTextRight(TextFormat("%05.0f", game->player->z), topX + 8, topW - 16, topY + 6, 38, HUD_DIGIT);

    // Linha de baixo: DIA (esq.) | CARRO + ULTRAPASSAGENS (dir.)
    const int botY = topY + topH + 10;
    const int botH = boxH - topH - 2 * pad - 10;

    // Dia
    const int dayW = 70;
    DrawRectangle(topX, botY, dayW, botH, HUD_PANEL);
    DrawTextCenter(TextFormat("%d", game->day), topX, dayW, botY + 4, 30, HUD_DIGIT);

    // Carros que faltam ultrapassar no dia
    const int passX = topX + dayW + 12;
    const int passW = topW - dayW - 12;
    DrawRectangle(passX, botY, passW, botH, HUD_PANEL);
    DrawCarGlyph(passX + 10, botY + (botH - 18) / 2, HUD_DIGIT);
    DrawTextRight(TextFormat("%d", faltam), passX + 8, passW - 16, botY + 4, 30, HUD_DIGIT);
}
