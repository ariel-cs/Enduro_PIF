#include "raylib.h"

int main(void) {
  InitWindow(800, 600, "Enduro");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("Enduro", 340, 280, 40, WHITE);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
