// Задание 17В: Система частиц — огонь. Аддитивное смешивание.
#include "raylib.h"
#include <math.h>

#define MAX 3000

typedef struct { float x, y, vx, vy, life, maxlife; } P;
P ps[MAX];
int cnt = 0;

void emit(void) {
    if (cnt >= MAX) return;
    float l = 0.5f + GetRandomValue(0, 100) / 100.0f;
    ps[cnt++] = (P){400 + GetRandomValue(-30, 30), 550, GetRandomValue(-50, 50) / 100.0f, -(1.5f + GetRandomValue(0, 200) / 100.0f), l, l};
}

int main(void) {
    InitWindow(800, 600, "17В Огонь");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        for (int i = 0; i < 15; i++) emit();
        int alive = 0;
        for (int i = 0; i < cnt; i++) {
            ps[i].x += ps[i].vx; ps[i].y += ps[i].vy;
            ps[i].vx += GetRandomValue(-10, 10) / 100.0f * dt;
            ps[i].life -= dt;
            if (ps[i].life > 0) ps[alive++] = ps[i];
        }
        cnt = alive;
        BeginDrawing();
        ClearBackground(BLACK);
        BeginBlendMode(BLEND_ADDITIVE);
        for (int i = 0; i < cnt; i++) {
            float t = ps[i].life / ps[i].maxlife;
            float r, g, b;
            if (t > 0.5f) { r = 1; g = (t - 0.5f) * 2; b = 0; }
            else { r = t * 2; g = 0; b = 0; }
            unsigned char a = (unsigned char)(t * 180);
            float sz = 4 + (1 - t) * 8;
            DrawCircle((int)ps[i].x, (int)ps[i].y, sz, (Color){(unsigned char)(r*255), (unsigned char)(g*255), (unsigned char)(b*255), a});
        }
        EndBlendMode();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
