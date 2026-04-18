// Задание 8.2Б: Коррекция цвета — RGB + тон/насыщенность (HSV)
#include "raylib.h"
#include <math.h>

static unsigned char clamp(float v) { return v < 0 ? 0 : v > 255 ? 255 : (unsigned char)v; }

int main(void) {
    InitWindow(800, 600, "8.2Б RGB + HSV");
    SetTargetFPS(60);
    Image src = GenImageGradientLinear(380, 560, 45, RED, GREEN);
    for (int y = 0; y < src.height; y++)
        for (int x = 0; x < src.width; x++) {
            Color c = {(unsigned char)(x * 255 / src.width), (unsigned char)(y * 255 / src.height),
                       (unsigned char)(255 - x * 255 / src.width), 255};
            ImageDrawPixel(&src, x, y, c);
        }
    float bright = 0, contrast = 1.0f, hueShift = 0, satMul = 1.0f;
    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_UP)) bright += 1;
        if (IsKeyDown(KEY_DOWN)) bright -= 1;
        if (IsKeyDown(KEY_RIGHT)) contrast += 0.01f;
        if (IsKeyDown(KEY_LEFT)) contrast -= 0.01f;
        if (IsKeyDown(KEY_H)) hueShift += 1;
        if (IsKeyDown(KEY_J)) hueShift -= 1;
        if (IsKeyDown(KEY_K)) satMul += 0.01f;
        if (IsKeyDown(KEY_L)) satMul -= 0.01f;
        if (satMul < 0) satMul = 0;
        Image dst = ImageCopy(src);
        for (int y = 0; y < dst.height; y++)
            for (int x = 0; x < dst.width; x++) {
                Color c = GetImageColor(dst, x, y);
                Vector3 hsv = ColorToHSV(c);
                hsv.x = fmodf(hsv.x + hueShift + 360, 360);
                hsv.y *= satMul;
                if (hsv.y > 1) hsv.y = 1;
                c = ColorFromHSV(hsv.x, hsv.y, hsv.z);
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
        DrawText(TextFormat("Bright:%.0f Contr:%.2f Hue:%.0f Sat:%.2f", bright, contrast, hueShift, satMul), 10, 2, 16, WHITE);
        EndDrawing();
        UnloadTexture(tsrc);
        UnloadTexture(tdst);
        UnloadImage(dst);
    }
    UnloadImage(src);
    CloseWindow();
}
