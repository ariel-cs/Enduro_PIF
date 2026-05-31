#include <raylib.h>
#include "ui.h"
#include "config.h"
#include "game.h"
#include "score.h"
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

void DrawLogoScreen(Texture2D logo) {
    ClearBackground(BLACK);
    int logoX = (SCREEN_WIDTH / 2) - (logo.width / 2);
    int logoY = (SCREEN_HEIGHT / 2) - (logo.height / 2);
    DrawTexture(logo, logoX, logoY, WHITE);
    DrawText("APRESENTA", 330, 350, 24, GRAY);
}

void DrawTitleScreen(Texture2D title) {
    ClearBackground(BLACK);
    Rectangle dest = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    Rectangle src  = { 0, 0, title.width, title.height };
    DrawTexturePro(title, src, dest, (Vector2){0,0}, 0.0f, WHITE);

    DrawText("Enduro", 52, 82, 80, BLACK);
    DrawText("Enduro", 50, 80, 80, WHITE);
    DrawText("PRESSIONE ENTER", 532, 522, 22, BLACK);
    DrawText("PRESSIONE ENTER", 530, 520, 22, YELLOW);
}

void DrawMenu(Texture2D menuImg, int selectedOption, float masterVolume) {
    ClearBackground(BLACK);

    Rectangle dest = { 0, 0, SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT };
    Rectangle src  = { 0, 0, menuImg.width, menuImg.height };
    DrawTexturePro(menuImg, src, dest, (Vector2){0,0}, 0.0f, WHITE);

    DrawText("MENU PRINCIPAL", 450, 150, 30, RED);
    Color colorOpt0 = (selectedOption == 0) ? YELLOW : WHITE;
    Color colorOpt1 = (selectedOption == 1) ? YELLOW : WHITE;
    Color colorOpt2 = (selectedOption == 2) ? YELLOW : GRAY;
    DrawText(TextFormat("%s INICIAR CORRIDA", (selectedOption == 0) ? ">" : " "), 450, 280, 22, colorOpt0);
    DrawText(TextFormat("%s VOLUME  < %3d%% >", (selectedOption == 1) ? ">" : " ",
                        (int)(masterVolume * 100.0f)), 450, 340, 22, colorOpt1);
    DrawText(TextFormat("%s SAIR DO JOGO", (selectedOption == 2) ? ">" : " "), 450, 400, 22, colorOpt2);
}

void DrawGameOverScreen(const GameState *game) {
    ClearBackground(BLACK);
    DrawText("FIM DE JOGO", 280, 150, 40, RED);
    DrawText(TextFormat("PONTUAÇÃO FINAL: %d", game->score), 250, 220, 20, WHITE);
    DrawText("DIGITE SEU NOME (Max 9 letras):", 220, 280, 20, RED);
    DrawText(game->player_name, 350, 330, 30, YELLOW);

    if ((int)(GetTime() * 2) % 2 == 0 && game->name_length < 9) {
        DrawText("_", 350 + MeasureText(game->player_name, 30), 330, 30, YELLOW);
    }
    DrawText("PRESSIONE ENTER PARA SALVAR", 230, 420, 20, DARKGRAY);
}

void DrawTopScoresScreen(const ScoreEntry *scores, int count) {
    ClearBackground(BLACK);
    DrawText("RANKING ENDURO", 250, 80, 35, GOLD);

    if (count == 0) {
        DrawText("NENHUM SCORE REGISTRADO", 200, 200, 20, GRAY);
    }
    else {
        for (int i = 0; i < count; i++) {
            int posY = 180 + (i * 40);
            DrawText(TextFormat("%d", i + 1), 260, posY, 25, WHITE);
            DrawText(scores[i].name, 300, posY, 25, SKYBLUE);
            DrawText(TextFormat("%06d", scores[i].score), 480, posY, 25, YELLOW);
        }
    }
    DrawText("PRESSIONE ENTER PARA VOLTAR", 200, 500, 20, DARKGRAY);
}
