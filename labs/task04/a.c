// Задание 4А: Генерация выпуклого полигона N=13 вершин на эллипсе
#include "raylib.h"
#include <math.h>

#define N 13

int main(void) {
    InitWindow(800, 600, "Lab 04A — Выпуклый полигон на эллипсе");
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
        for (int i = 1; i < N - 1; i++)
            DrawTriangle(pts[0], pts[i], pts[i + 1], (Color){180, 210, 240, 255});
        for (int i = 0; i < N; i++)
            DrawLineV(pts[i], pts[(i + 1) % N], DARKBLUE);
        for (int i = 0; i < N; i++)
            DrawCircleV(pts[i], 5, RED);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
