// Задание 12А.А: Плавающий горизонт. Функция y = sin(x)*cos(z).
#include "raylib.h"
#include <math.h>

int main(void) {
    InitWindow(800, 600, "12А.А Плавающий горизонт sin(x)*cos(z)");
    SetTargetFPS(60);
    float angle = 0.0f;
    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_LEFT)) angle -= 0.02f;
        if (IsKeyDown(KEY_RIGHT)) angle += 0.02f;
        BeginDrawing();
        ClearBackground(BLACK);
        int ymin[800], ymax[800];
        for (int i = 0; i < 800; i++) { ymin[i] = 600; ymax[i] = 0; }
        float ca = cosf(angle), sa = sinf(angle);
        for (float z = 5.0f; z >= -5.0f; z -= 0.1f) {
            int prevSx = -1, prevSy = -1;
            for (float x = -5.0f; x <= 5.0f; x += 0.05f) {
                float y = sinf(x) * cosf(z);
                float rx = x * ca - z * sa;
                float rz = x * sa + z * ca;
                int sx = (int)(400 + rx * 60);
                int sy = (int)(300 - y * 100 + rz * 20);
                if (sx >= 0 && sx < 800) {
                    if (sy < ymin[sx]) {
                        if (prevSx >= 0) DrawLine(prevSx, prevSy, sx, sy, GREEN);
                        ymin[sx] = sy;
                    } else if (sy > ymax[sx]) {
                        if (prevSx >= 0) DrawLine(prevSx, prevSy, sx, sy, DARKGREEN);
                        ymax[sx] = sy;
                    }
                }
                prevSx = sx; prevSy = sy;
            }
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
