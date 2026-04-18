// Задание 8.2В: Кривые и цветовые фильтры (сепия, негатив)
#include "raylib.h"
#include <math.h>

static unsigned char clamp(float v) { return v < 0 ? 0 : v > 255 ? 255 : (unsigned char)v; }

int main(void) {
    InitWindow(800, 600, "8.2В Фильтры и кривые");
    SetTargetFPS(60);
    Image src = GenImageGradientLinear(380, 560, 45, RED, GREEN);
    for (int y = 0; y < src.height; y++)
        for (int x = 0; x < src.width; x++) {
            Color c = {(unsigned char)(x * 255 / src.width), (unsigned char)(y * 255 / src.height),
                       (unsigned char)(255 - x * 255 / src.width), 255};
            ImageDrawPixel(&src, x, y, c);
        }
    int mode = 1;
    float gamma = 1.0f;
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ONE)) mode = 1;
        if (IsKeyPressed(KEY_TWO)) mode = 2;
        if (IsKeyPressed(KEY_THREE)) mode = 3;
        if (IsKeyPressed(KEY_FOUR)) mode = 4;
        if (IsKeyDown(KEY_UP)) gamma += 0.01f;
        if (IsKeyDown(KEY_DOWN)) gamma -= 0.01f;
        if (gamma < 0.1f) gamma = 0.1f;
        Image dst = ImageCopy(src);
        for (int y = 0; y < dst.height; y++)
            for (int x = 0; x < dst.width; x++) {
                Color c = GetImageColor(dst, x, y);
                Color o = c;
                if (mode == 2) {
                    o.r = clamp(0.393f*c.r + 0.769f*c.g + 0.189f*c.b);
                    o.g = clamp(0.349f*c.r + 0.686f*c.g + 0.168f*c.b);
                    o.b = clamp(0.272f*c.r + 0.534f*c.g + 0.131f*c.b);
                } else if (mode == 3) {
                    o = (Color){255 - c.r, 255 - c.g, 255 - c.b, 255};
                } else if (mode == 4) {
                    o.r = clamp(255 * powf(c.r / 255.0f, gamma));
                    o.g = clamp(255 * powf(c.g / 255.0f, gamma));
                    o.b = clamp(255 * powf(c.b / 255.0f, gamma));
                }
                o.a = 255;
                ImageDrawPixel(&dst, x, y, o);
            }
        Texture2D tsrc = LoadTextureFromImage(src);
        Texture2D tdst = LoadTextureFromImage(dst);
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(tsrc, 10, 20, WHITE);
        DrawTexture(tdst, 410, 20, WHITE);
        const char *names[] = {"", "Normal", "Sepia", "Negative", "Gamma"};
        DrawText(TextFormat("Mode[1-4]: %s  Gamma: %.2f", names[mode], gamma), 10, 2, 18, WHITE);
        EndDrawing();
        UnloadTexture(tsrc);
        UnloadTexture(tdst);
        UnloadImage(dst);
    }
    UnloadImage(src);
    CloseWindow();
}
