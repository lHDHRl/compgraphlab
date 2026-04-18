// Задание 8.1А: Узорчатый дизеринг (матрица Bayer 4x4)
#include "raylib.h"
#include <math.h>

#define W 800
#define H 600

static const int bayer4[4][4] = {
    { 0, 8, 2,10},
    {12, 4,14, 6},
    { 3,11, 1, 9},
    {15, 7,13, 5}
};

int main(void) {
    InitWindow(W, H, "Lab 08A - Bayer Dithering");
    SetTargetFPS(60);
    Image orig = GenImageColor(W/2, H, BLACK);
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W/2; x++) {
            unsigned char v = (unsigned char)(x * 255.0f / (W/2 - 1));
            ImageDrawPixel(&orig, x, y, (Color){v, v, v, 255});
        }
    Image dith = ImageCopy(orig);
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W/2; x++) {
            Color c = GetImageColor(orig, x, y);
            float gray = c.r / 255.0f;
            float threshold = (bayer4[y % 4][x % 4] + 0.5f) / 16.0f;
            unsigned char out = gray > threshold ? 255 : 0;
            ImageDrawPixel(&dith, x, y, (Color){out, out, out, 255});
        }
    Texture2D texOrig = LoadTextureFromImage(orig);
    Texture2D texDith = LoadTextureFromImage(dith);
    UnloadImage(orig);
    UnloadImage(dith);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(texOrig, 0, 0, WHITE);
        DrawTexture(texDith, W/2, 0, WHITE);
        DrawText("Original", 10, 10, 20, GREEN);
        DrawText("Bayer 4x4", W/2 + 10, 10, 20, GREEN);
        EndDrawing();
    }
    UnloadTexture(texOrig);
    UnloadTexture(texDith);
    CloseWindow();
    return 0;
}
