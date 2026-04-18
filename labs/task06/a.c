// Задание 6А: Проволочная модель куба и октаэдра, ортографическая проекция
#include "raylib.h"
#include <math.h>

typedef struct { float x, y, z; } V3;

V3 rotY(V3 v, float a) {
    return (V3){v.x * cosf(a) + v.z * sinf(a), v.y, -v.x * sinf(a) + v.z * cosf(a)};
}

Vector2 proj(V3 v, float ox, float oy, float s) {
    return (Vector2){ox + v.x * s, oy - v.y * s};
}

void drawEdge(V3 *verts, int a, int b, float angle, float ox, float oy, float s) {
    V3 ra = rotY(verts[a], angle), rb = rotY(verts[b], angle);
    DrawLineV(proj(ra, ox, oy, s), proj(rb, ox, oy, s), BLACK);
}

int main(void) {
    InitWindow(800, 600, "Lab 06A — Куб и октаэдр (ортографическая)");
    SetTargetFPS(60);
    V3 cube[8] = {
        {-1,-1,-1},{1,-1,-1},{1,1,-1},{-1,1,-1},
        {-1,-1,1},{1,-1,1},{1,1,1},{-1,1,1}
    };
    int ce[][2] = {{0,1},{1,2},{2,3},{3,0},{4,5},{5,6},{6,7},{7,4},{0,4},{1,5},{2,6},{3,7}};
    V3 oct[6] = {{1,0,0},{-1,0,0},{0,1,0},{0,-1,0},{0,0,1},{0,0,-1}};
    int oe[][2] = {{0,2},{0,3},{0,4},{0,5},{1,2},{1,3},{1,4},{1,5},{2,4},{2,5},{3,4},{3,5}};
    float angle = 0;
    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_LEFT)) angle -= 0.03f;
        if (IsKeyDown(KEY_RIGHT)) angle += 0.03f;
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Куб", 150, 20, 20, DARKGRAY);
        for (int i = 0; i < 12; i++) drawEdge(cube, ce[i][0], ce[i][1], angle, 200, 300, 100);
        DrawText("Октаэдр", 500, 20, 20, DARKGRAY);
        for (int i = 0; i < 12; i++) drawEdge(oct, oe[i][0], oe[i][1], angle, 600, 300, 120);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
