#include "raylib.h"
#include "config.h"
#include "game.h"
#include <stdlib.h>

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Enduro");
  SetTargetFPS(60);

  Texture2D logotex = LoadTexture("LOGO.png");
  Image img = LoadImage("LOGO.png");
  ImageResize(&img, 294, 110);
  UnloadTexture(logotex);
  logotex = LoadTextureFromImage(img);
  SetTextureFilter(logotex, TEXTURE_FILTER_POINT);

  GameState *game = init_game();

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    if(game->current_state == STATE_TITLE && IsKeyPressed(KEY_ENTER)){
        change_state(game, STATE_MENU);
    }

    update_game(game, dt);

    BeginDrawing();
    ClearBackground(BLACK);
    if (game->current_state == STATE_LOGO){
        int logoX = (SCREEN_WIDTH / 2) - (logotex.width / 2);
        int logoY = (SCREEN_HEIGHT / 2) - (logotex.height / 2);
        DrawTexture(logotex, logoX, logoY, WHITE);
    }
    else if (game->current_state == STATE_TITLE) {
        DrawText("Enduro", 340, 280, 40, WHITE);
    }
    else if (game->current_state == STATE_MENU) {
        DrawText("MENU PLACEHOLDER", 280, 280, 20, GREEN);
    }
    EndDrawing();
  }

  free(game);
  CloseWindow();
  return 0;
}
