// Задание 1Б: Поворот контура треугольника (Брезенхем, центр поворота мышью)
#include "raylib.h"
#include <math.h>
#include <stdlib.h>

void bresenham(int x0, int y0, int x1, int y1, Color c) {
    int dx = abs(x1 - x0), dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    while (1) {
        DrawPixel(x0, y0, c);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }
    }
}

int main(void) {
    InitWindow(800, 600, "Lab 1B - Поворот треугольника");
    SetTargetFPS(60);

    float tri[] = {400,200, 250,450, 550,450};
    float cx = 400, cy = 350, angle = 0;

    while (!WindowShouldClose()) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            cx = GetMouseX(); cy = GetMouseY();
        }
        if (IsKeyDown(KEY_LEFT))  angle -= 1 * DEG2RAD;
        if (IsKeyDown(KEY_RIGHT)) angle += 1 * DEG2RAD;

        BeginDrawing();
        ClearBackground(BLACK);

        float r[6];
        for (int i = 0; i < 3; i++) {
            float dx = tri[i*2] - cx, dy = tri[i*2+1] - cy;
            r[i*2]   = cx + dx*cosf(angle) - dy*sinf(angle);
            r[i*2+1] = cy + dx*sinf(angle) + dy*cosf(angle);
        }
        for (int i = 0; i < 3; i++) {
            int j = (i + 1) % 3;
            bresenham((int)r[i*2], (int)r[i*2+1], (int)r[j*2], (int)r[j*2+1], GREEN);
        }
        DrawCircle((int)cx, (int)cy, 4, RED);
        DrawText(TextFormat("Angle: %.1f°  Click to set center", angle*RAD2DEG), 10, 10, 16, YELLOW);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
