// Задание 17Б: Система частиц — дождь/снегопад. TAB переключает.
#include "raylib.h"
#include <math.h>

#define MAX 2000

typedef struct { float x, y, vy, size; } P;
P ps[MAX];
int cnt = 0;
int snow = 0;

void emit(void) {
    if (cnt >= MAX) return;
    float sz = snow ? GetRandomValue(2, 5) : 1;
    float sp = snow ? GetRandomValue(50, 150) / 100.0f : GetRandomValue(300, 600) / 100.0f;
    ps[cnt++] = (P){(float)GetRandomValue(0, 800), -5, sp, sz};
}

int main(void) {
    InitWindow(800, 600, "17Б Дождь/Снег");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_TAB)) snow = !snow;
        for (int i = 0; i < 5; i++) emit();
        int alive = 0;
        for (int i = 0; i < cnt; i++) {
            ps[i].y += ps[i].vy;
            ps[i].vy += 0.02f;
            if (ps[i].y < 600) ps[alive++] = ps[i];
        }
        cnt = alive;
        BeginDrawing();
        ClearBackground(snow ? (Color){20, 20, 40, 255} : (Color){30, 30, 50, 255});
        for (int i = 0; i < cnt; i++) {
            if (snow) DrawCircle((int)ps[i].x, (int)ps[i].y, ps[i].size, WHITE);
            else DrawLine((int)ps[i].x, (int)ps[i].y, (int)ps[i].x, (int)(ps[i].y + 8), (Color){150, 180, 255, 200});
        }
        DrawText(snow ? "SNOW (TAB to switch)" : "RAIN (TAB to switch)", 10, 10, 20, GRAY);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
