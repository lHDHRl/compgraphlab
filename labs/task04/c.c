// Задание 4В: Генерация сложного самопересекающегося полигона — бабочка
#include "raylib.h"
#include <math.h>

#define N 13

int main(void) {
    InitWindow(800, 600, "Lab 04C — Самопересекающийся полигон (бабочка)");
    SetTargetFPS(60);
    float cx = 400, cy = 300, sc = 150;
    Vector2 pts[N];
    for (int i = 0; i < N; i++) {
        float t = 2.0f * PI * i / N;
        float r = sc * (sinf(2 * t) + 0.5f * sinf(3 * t) + 1.2f);
        pts[i] = (Vector2){cx + r * cosf(t), cy + r * sinf(t)};
    }
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (int i = 1; i < N - 1; i++)
            DrawTriangle(pts[0], pts[i], pts[i + 1], (Color){210, 240, 210, 255});
        for (int i = 0; i < N; i++)
            DrawLineV(pts[i], pts[(i + 1) % N], DARKGREEN);
        for (int i = 0; i < N; i++)
            DrawCircleV(pts[i], 4, MAROON);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
