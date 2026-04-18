// Задание 6Б: Проволочная модель цилиндра, конуса, пирамиды — перспективная проекция
#include "raylib.h"
#include <math.h>

#define S 20

typedef struct { float x, y, z; } V3;

V3 rotX(V3 v, float a) {
    return (V3){v.x, v.y * cosf(a) - v.z * sinf(a), v.y * sinf(a) + v.z * cosf(a)};
}
V3 rotY(V3 v, float a) {
    return (V3){v.x * cosf(a) + v.z * sinf(a), v.y, -v.x * sinf(a) + v.z * cosf(a)};
}

Vector2 proj(V3 v, float fov, float d) {
    float z = v.z + d;
    if (z < 0.1f) z = 0.1f;
    return (Vector2){400 + v.x * fov / z, 300 - v.y * fov / z};
}

void line3d(V3 a, V3 b, float ax, float ay, float fov, float d, Color c) {
    V3 ra = rotX(rotY(a, ay), ax), rb = rotX(rotY(b, ay), ax);
    DrawLineV(proj(ra, fov, d), proj(rb, fov, d), c);
}

int main(void) {
    InitWindow(800, 600, "Lab 06B — Цилиндр, конус, пирамида (перспектива)");
    SetTargetFPS(60);
    float ax = 0.4f, ay = 0, fov = 400, d = 8;
    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_LEFT)) ay -= 0.03f;
        if (IsKeyDown(KEY_RIGHT)) ay += 0.03f;
        if (IsKeyDown(KEY_UP)) ax -= 0.03f;
        if (IsKeyDown(KEY_DOWN)) ax += 0.03f;
        BeginDrawing();
        ClearBackground(RAYWHITE);
        float ox;
        // Цилиндр
        ox = -4;
        for (int i = 0; i < S; i++) {
            float a1 = 2 * PI * i / S, a2 = 2 * PI * ((i + 1) % S) / S;
            V3 b1 = {ox + cosf(a1), -1, sinf(a1)}, b2 = {ox + cosf(a2), -1, sinf(a2)};
            V3 t1 = {ox + cosf(a1), 1, sinf(a1)}, t2 = {ox + cosf(a2), 1, sinf(a2)};
            line3d(b1, b2, ax, ay, fov, d, BLUE);
            line3d(t1, t2, ax, ay, fov, d, BLUE);
            line3d(b1, t1, ax, ay, fov, d, BLUE);
        }
        // Конус
        ox = 0;
        for (int i = 0; i < S; i++) {
            float a1 = 2 * PI * i / S, a2 = 2 * PI * ((i + 1) % S) / S;
            V3 b1 = {ox + cosf(a1), -1, sinf(a1)}, b2 = {ox + cosf(a2), -1, sinf(a2)};
            V3 top = {ox, 1, 0};
            line3d(b1, b2, ax, ay, fov, d, RED);
            line3d(b1, top, ax, ay, fov, d, RED);
        }
        // Пирамида
        ox = 4;
        V3 pb[4] = {{ox-1,-1,-1},{ox+1,-1,-1},{ox+1,-1,1},{ox-1,-1,1}};
        V3 pt = {ox, 1, 0};
        for (int i = 0; i < 4; i++) {
            line3d(pb[i], pb[(i + 1) % 4], ax, ay, fov, d, DARKGREEN);
            line3d(pb[i], pt, ax, ay, fov, d, DARKGREEN);
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
