// Задание 11А: Кривая Безье по 4 точкам, перетаскивание мышью
#include "raylib.h"
#include <math.h>

#define W 800
#define H 600
#define NP 4
#define R 8

static Vector2 pts[NP] = {{150,400},{300,100},{500,100},{650,400}};
static int drag = -1;

int main(void) {
    InitWindow(W, H, "Lab 09A - Bezier Curve");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        Vector2 m = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            for (int i = 0; i < NP; i++)
                if (fabsf(m.x-pts[i].x)<R && fabsf(m.y-pts[i].y)<R) { drag=i; break; }
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) drag = -1;
        if (drag >= 0) pts[drag] = m;
        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (int i = 0; i < NP-1; i++)
            DrawLineV(pts[i], pts[i+1], DARKGRAY);
        Vector2 prev = pts[0];
        for (int i = 1; i <= 200; i++) {
            float t = (float)i / 200;
            float u = 1-t;
            float x = u*u*u*pts[0].x + 3*u*u*t*pts[1].x + 3*u*t*t*pts[2].x + t*t*t*pts[3].x;
            float y = u*u*u*pts[0].y + 3*u*u*t*pts[1].y + 3*u*t*t*pts[2].y + t*t*t*pts[3].y;
            Vector2 cur = {x, y};
            DrawLineV(prev, cur, GREEN);
            prev = cur;
        }
        for (int i = 0; i < NP; i++)
            DrawCircleV(pts[i], R, RED);
        DrawText("Drag control points", 10, 10, 20, DARKGRAY);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
