// Задание 3Б: Дерево Пифагора с варьируемыми углами, только линии (скелет)
#include "raylib.h"
#include <math.h>
#include <stdlib.h>

#define MAXN 4096
float angles[MAXN];

void gen_angles(void) {
    for (int i = 0; i < MAXN; i++)
        angles[i] = (30.0f + (float)(rand()%31)) * DEG2RAD;
}

void tree(float x, float y, float a, float len, int depth, int *idx) {
    if (depth <= 0 || len < 1) return;
    float x1 = x + len*sinf(a), y1 = y - len*cosf(a);
    Color c = ColorFromHSV(90.0f + depth*12.0f, 0.7f, 0.8f);
    DrawLine(x,y,x1,y1,c);
    float la = angles[(*idx)++ % MAXN], ra = angles[(*idx)++ % MAXN];
    float s = 0.7071f;
    tree(x1, y1, a - la, len*s, depth-1, idx);
    tree(x1, y1, a + ra, len*s, depth-1, idx);
}

int main(void) {
    InitWindow(800, 600, "Lab 3B - Дерево Пифагора (случайные углы)");
    SetTargetFPS(60);
    srand(42); gen_angles();
    int depth = 8;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_UP) && depth < 12) depth++;
        if (IsKeyPressed(KEY_DOWN) && depth > 1) depth--;
        if (IsKeyPressed(KEY_R)) { srand(GetRandomValue(0,99999)); gen_angles(); }

        BeginDrawing();
        ClearBackground(BLACK);
        int idx = 0;
        tree(400, 580, 0, 120, depth, &idx);
        DrawText(TextFormat("Depth: %d  UP/DOWN  R=regen", depth), 10, 10, 16, YELLOW);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
