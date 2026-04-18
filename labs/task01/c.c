// Задание 1В: Поворот звезды (10 вершин), антиалиасинг Ву, автовращение
#include "raylib.h"
#include <math.h>

void plot(int x, int y, float bright, Color c) {
    unsigned char a = (unsigned char)(bright * 255);
    DrawPixel(x, y, (Color){c.r, c.g, c.b, a});
}

void wu_line(float x0f, float y0f, float x1f, float y1f, Color c) {
    int steep = fabsf(y1f - y0f) > fabsf(x1f - x0f);
    if (steep) { float t=x0f; x0f=y0f; y0f=t; t=x1f; x1f=y1f; y1f=t; }
    if (x0f > x1f) { float t=x0f; x0f=x1f; x1f=t; t=y0f; y0f=y1f; y1f=t; }
    float dx = x1f - x0f, dy = y1f - y0f;
    float grad = dx == 0 ? 1 : dy / dx;
    int xpx1 = (int)(x0f + 0.5f), xpx2 = (int)(x1f + 0.5f);
    float intery = y0f + grad * (xpx1 - x0f);
    if (steep) { plot((int)intery, xpx1, 1, c); }
    else { plot(xpx1, (int)intery, 1, c); }
    for (int x = xpx1 + 1; x < xpx2; x++) {
        intery += grad;
        int iy = (int)intery;
        float f = intery - iy;
        if (steep) { plot(iy, x, 1-f, c); plot(iy+1, x, f, c); }
        else { plot(x, iy, 1-f, c); plot(x, iy+1, f, c); }
    }
    if (steep) { plot((int)(intery+grad), xpx2, 1, c); }
    else { plot(xpx2, (int)(intery+grad), 1, c); }
}

int main(void) {
    InitWindow(800, 600, "Lab 1C - Звезда (Ву, автовращение)");
    SetTargetFPS(60);

    float vx[10], vy[10];
    for (int i = 0; i < 10; i++) {
        float a = i * 2 * PI / 10 - PI / 2;
        float r = (i % 2 == 0) ? 200 : 80;
        vx[i] = 400 + r * cosf(a);
        vy[i] = 300 + r * sinf(a);
    }

    float angle = 0;
    int autorot = 1;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_SPACE)) autorot = !autorot;
        if (autorot) angle += 0.5f * DEG2RAD;

        BeginDrawing();
        ClearBackground(BLACK);

        float rx[10], ry[10];
        for (int i = 0; i < 10; i++) {
            float dx = vx[i] - 400, dy = vy[i] - 300;
            rx[i] = 400 + dx*cosf(angle) - dy*sinf(angle);
            ry[i] = 300 + dx*sinf(angle) + dy*cosf(angle);
        }
        for (int i = 0; i < 10; i++) {
            int j = (i + 1) % 10;
            wu_line(rx[i], ry[i], rx[j], ry[j], GREEN);
        }
        DrawText(TextFormat("Auto: %s  SPACE toggle", autorot ? "ON" : "OFF"), 10, 10, 16, YELLOW);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
