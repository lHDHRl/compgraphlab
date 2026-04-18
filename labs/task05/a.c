// Задание 5А: Триангуляция выпуклого полигона веером от первой вершины
#include "raylib.h"
#include <math.h>

#define N 13

Color tricolor(int i) {
    unsigned char r = (i * 47 + 80) % 256;
    unsigned char g = (i * 97 + 60) % 256;
    unsigned char b = (i * 157 + 100) % 256;
    return (Color){r, g, b, 255};
}

int main(void) {
    InitWindow(800, 600, "Lab 05A — Триангуляция веером");
    SetTargetFPS(60);
    float cx = 400, cy = 300, rx = 250, ry = 180;
    Vector2 pts[N];
    for (int i = 0; i < N; i++) {
        float a = 2.0f * PI * i / N;
        pts[i] = (Vector2){cx + rx * cosf(a), cy + ry * sinf(a)};
    }
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (int i = 1; i < N - 1; i++) {
            DrawTriangle(pts[0], pts[i], pts[i + 1], tricolor(i));
            DrawTriangleLines(pts[0], pts[i], pts[i + 1], BLACK);
        }
        for (int i = 0; i < N; i++)
            DrawCircleV(pts[i], 4, RED);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
