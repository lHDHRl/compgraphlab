// Задание 4Б: Генерация звездчатого полигона N=13
#include "raylib.h"
#include <math.h>

#define N 13
#define V (N * 2)

int main(void) {
    InitWindow(800, 600, "Lab 04B — Звездчатый полигон");
    SetTargetFPS(60);
    float cx = 400, cy = 300, r1 = 250, r2 = 100;
    Vector2 pts[V];
    for (int i = 0; i < V; i++) {
        float a = 2.0f * PI * i / V;
        float r = (i % 2 == 0) ? r1 : r2;
        pts[i] = (Vector2){cx + r * cosf(a), cy + r * sinf(a)};
    }
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (int i = 1; i < V - 1; i++)
            DrawTriangle(pts[0], pts[i], pts[i + 1], (Color){240, 210, 180, 255});
        for (int i = 0; i < V; i++)
            DrawLineV(pts[i], pts[(i + 1) % V], DARKPURPLE);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
