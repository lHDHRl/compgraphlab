// Задание 8.2А: Коррекция цвета — яркость/контраст в RGB
#include "raylib.h"
#include <math.h>

static unsigned char clamp(float v) { return v < 0 ? 0 : v > 255 ? 255 : (unsigned char)v; }

int main(void) {
    InitWindow(800, 600, "8.2А Яркость/Контраст");
    SetTargetFPS(60);
    Image src = GenImageGradientLinear(380, 560, 45, RED, BLUE);
    float bright = 0, contrast = 1.0f;
    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_UP)) bright += 1;
        if (IsKeyDown(KEY_DOWN)) bright -= 1;
        if (IsKeyDown(KEY_RIGHT)) contrast += 0.01f;
        if (IsKeyDown(KEY_LEFT)) contrast -= 0.01f;
        Image dst = ImageCopy(src);
        for (int y = 0; y < dst.height; y++)
            for (int x = 0; x < dst.width; x++) {
                Color c = GetImageColor(dst, x, y);
                ImageDrawPixel(&dst, x, y, (Color){
                    clamp((c.r - 128) * contrast + 128 + bright),
                    clamp((c.g - 128) * contrast + 128 + bright),
                    clamp((c.b - 128) * contrast + 128 + bright), 255});
            }
        Texture2D tsrc = LoadTextureFromImage(src);
        Texture2D tdst = LoadTextureFromImage(dst);
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(tsrc, 10, 20, WHITE);
        DrawTexture(tdst, 410, 20, WHITE);
        DrawText(TextFormat("Brightness: %.0f  Contrast: %.2f", bright, contrast), 10, 2, 18, WHITE);
        EndDrawing();
        UnloadTexture(tsrc);
        UnloadTexture(tdst);
        UnloadImage(dst);
    }
    UnloadImage(src);
    CloseWindow();
}
