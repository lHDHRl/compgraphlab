// Задание 6В: Проволочная модель тора с перспективной проекцией и удалением невидимых граней
#include "raylib.h"
#include <math.h>
#include <stdlib.h>

#define NS 20
#define NT 20

typedef struct { float x, y, z; } V3;

V3 rotX(V3 v, float a) {
    return (V3){v.x, v.y * cosf(a) - v.z * sinf(a), v.y * sinf(a) + v.z * cosf(a)};
}
V3 rotY(V3 v, float a) {
    return (V3){v.x * cosf(a) + v.z * sinf(a), v.y, -v.x * sinf(a) + v.z * cosf(a)};
}

V3 transform(V3 v, float ax, float ay) { return rotX(rotY(v, ay), ax); }

Vector2 proj(V3 v, float fov, float d) {
    float z = v.z + d;
    if (z < 0.1f) z = 0.1f;
    return (Vector2){400 + v.x * fov / z, 300 - v.y * fov / z};
}

V3 toruspt(int i, int j, float R, float r) {
    float u = 2 * PI * i / NS, v = 2 * PI * j / NT;
    return (V3){(R + r * cosf(v)) * cosf(u), r * sinf(v), (R + r * cosf(v)) * sinf(u)};
}

typedef struct { int i, j; float z; } Face;

int cmpface(const void *a, const void *b) {
    float za = ((Face *)a)->z, zb = ((Face *)b)->z;
    return (za < zb) ? -1 : (za > zb) ? 1 : 0;
}

int main(void) {
    InitWindow(800, 600, "Lab 06C — Тор (перспектива + z-сортировка)");
    SetTargetFPS(60);
    float R = 3, r = 1, ax = 0.5f, ay = 0, fov = 500, d = 10;
    Face faces[NS * NT];
    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_LEFT)) ay -= 0.03f;
        if (IsKeyDown(KEY_RIGHT)) ay += 0.03f;
        if (IsKeyDown(KEY_UP)) ax -= 0.03f;
        if (IsKeyDown(KEY_DOWN)) ax += 0.03f;
        if (IsKeyDown(KEY_EQUAL) || IsKeyDown(KEY_KP_ADD)) fov += 3;
        if (IsKeyDown(KEY_MINUS) || IsKeyDown(KEY_KP_SUBTRACT)) fov -= 3;
        if (fov < 50) fov = 50;
        if (fov > 2000) fov = 2000;

        int nf = 0;
        for (int i = 0; i < NS; i++)
            for (int j = 0; j < NT; j++) {
                V3 c = toruspt(i, j, R, r);
                V3 tc = transform(c, ax, ay);
                faces[nf++] = (Face){i, j, tc.z};
            }
        qsort(faces, nf, sizeof(Face), cmpface);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (int f = 0; f < nf; f++) {
            int i = faces[f].i, j = faces[f].j;
            int ni = (i + 1) % NS, nj = (j + 1) % NT;
            V3 p00 = transform(toruspt(i, j, R, r), ax, ay);
            V3 p10 = transform(toruspt(ni, j, R, r), ax, ay);
            V3 p11 = transform(toruspt(ni, nj, R, r), ax, ay);
            V3 p01 = transform(toruspt(i, nj, R, r), ax, ay);
            Vector2 s[4] = {proj(p00, fov, d), proj(p10, fov, d), proj(p11, fov, d), proj(p01, fov, d)};
            DrawTriangle(s[0], s[1], s[2], RAYWHITE);
            DrawTriangle(s[0], s[2], s[3], RAYWHITE);
            DrawLineV(s[0], s[1], DARKBLUE);
            DrawLineV(s[1], s[2], DARKBLUE);
            DrawLineV(s[2], s[3], DARKBLUE);
            DrawLineV(s[3], s[0], DARKBLUE);
        }
        DrawText(TextFormat("FOV: %.0f (+/-)", fov), 10, 10, 20, DARKGRAY);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
