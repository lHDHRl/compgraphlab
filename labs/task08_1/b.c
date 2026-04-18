// Задание 8.1Б: Алгоритм Флойда-Стейнберга
#include "raylib.h"
#include <math.h>

#define W 800
#define H 600
#define IW (W/2)

int main(void) {
    InitWindow(W, H, "Lab 08B - Floyd-Steinberg");
    SetTargetFPS(60);
    Image orig = GenImageColor(IW, H, BLACK);
    for (int y = 0; y < H; y++)
        for (int x = 0; x < IW; x++) {
            float fx = (float)x / IW, fy = (float)y / H;
            unsigned char v = (unsigned char)((0.5f + 0.5f * sinf(fx * 6.28f) * cosf(fy * 6.28f)) * 255);
            ImageDrawPixel(&orig, x, y, (Color){v, v, v, 255});
        }
    float *buf = MemAlloc(IW * H * sizeof(float));
    for (int y = 0; y < H; y++)
        for (int x = 0; x < IW; x++)
            buf[y * IW + x] = GetImageColor(orig, x, y).r / 255.0f;
    Image dith = GenImageColor(IW, H, BLACK);
    for (int y = 0; y < H; y++)
        for (int x = 0; x < IW; x++) {
            float old = buf[y * IW + x];
            float nw = old > 0.5f ? 1.0f : 0.0f;
            float err = old - nw;
            buf[y * IW + x] = nw;
            if (x + 1 < IW)          buf[y * IW + x + 1]       += err * 7.0f / 16;
            if (y + 1 < H) {
                if (x > 0)           buf[(y+1) * IW + x - 1]   += err * 3.0f / 16;
                buf[(y+1) * IW + x]       += err * 5.0f / 16;
                if (x + 1 < IW)      buf[(y+1) * IW + x + 1]   += err * 1.0f / 16;
            }
        }
    for (int y = 0; y < H; y++)
        for (int x = 0; x < IW; x++) {
            unsigned char v = buf[y * IW + x] > 0.5f ? 255 : 0;
            ImageDrawPixel(&dith, x, y, (Color){v, v, v, 255});
        }
    MemFree(buf);
    Texture2D texO = LoadTextureFromImage(orig);
    Texture2D texD = LoadTextureFromImage(dith);
    UnloadImage(orig); UnloadImage(dith);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(texO, 0, 0, WHITE);
        DrawTexture(texD, IW, 0, WHITE);
        DrawText("Original", 10, 10, 20, GREEN);
        DrawText("Floyd-Steinberg", IW + 10, 10, 20, GREEN);
        EndDrawing();
    }
    UnloadTexture(texO); UnloadTexture(texD);
    CloseWindow();
    return 0;
}
