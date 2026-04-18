// Задание 5Б: Триангуляция ear clipping для звездчатого полигона
#include "raylib.h"
#include <math.h>

#define N 26
#define MAX_TRI 24

typedef struct { Vector2 a, b, c; Color col; } Tri;

float cross2(Vector2 o, Vector2 a, Vector2 b) {
    return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}

int pt_in_tri(Vector2 p, Vector2 a, Vector2 b, Vector2 c) {
    float d1 = cross2(p, a, b), d2 = cross2(p, b, c), d3 = cross2(p, c, a);
    int has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    int has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);
    return !(has_neg && has_pos);
}

Color tricolor(int i) {
    return (Color){(unsigned char)((i * 47 + 80) % 256), (unsigned char)((i * 97 + 60) % 256), (unsigned char)((i * 157 + 100) % 256), 255};
}

int main(void) {
    InitWindow(800, 600, "Lab 05B — Ear Clipping (звезда)");
    SetTargetFPS(60);
    float cx = 400, cy = 300, r1 = 250, r2 = 100;
    Vector2 poly[N];
    for (int i = 0; i < N; i++) {
        float a = 2.0f * PI * i / N;
        float r = (i % 2 == 0) ? r1 : r2;
        poly[i] = (Vector2){cx + r * cosf(a), cy + r * sinf(a)};
    }
    int idx[N], n = N;
    for (int i = 0; i < N; i++) idx[i] = i;
    Tri tris[MAX_TRI];
    int nt = 0;
    while (n > 2 && nt < MAX_TRI) {
        int found = 0;
        for (int i = 0; i < n; i++) {
            int p = (i + n - 1) % n, nx = (i + 1) % n;
            Vector2 A = poly[idx[p]], B = poly[idx[i]], C = poly[idx[nx]];
            if (cross2(A, B, C) < 0) continue;
            int ear = 1;
            for (int j = 0; j < n; j++) {
                if (j == p || j == i || j == nx) continue;
                if (pt_in_tri(poly[idx[j]], A, B, C)) { ear = 0; break; }
            }
            if (ear) {
                tris[nt] = (Tri){A, B, C, tricolor(nt)};
                nt++;
                for (int j = i; j < n - 1; j++) idx[j] = idx[j + 1];
                n--;
                found = 1;
                break;
            }
        }
        if (!found) break;
    }
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (int i = 0; i < nt; i++) {
            DrawTriangle(tris[i].a, tris[i].b, tris[i].c, tris[i].col);
            DrawTriangleLines(tris[i].a, tris[i].b, tris[i].c, BLACK);
        }
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
