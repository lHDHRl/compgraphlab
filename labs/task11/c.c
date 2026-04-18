// Задание 11В: Рациональные кривые (NURBS) с редактированием весов
#include "raylib.h"
#include <math.h>

#define W 800
#define H 600
#define MAXP 64
#define R 8
#define STEPS 300

static Vector2 pts[MAXP];
static float wt[MAXP];
static int np = 4;
static int drag = -1;
static int sel = -1;

static Vector2 nurbs_eval(float t) {
    float wx = 0, wy = 0, wsum = 0;
    int n = np - 1;
    float *bc = MemAlloc((n+1) * sizeof(float));
    bc[0] = 1;
    for (int i = 1; i <= n; i++) bc[i] = bc[i-1] * (n-i+1) / i;
    for (int i = 0; i <= n; i++) {
        float b = bc[i] * powf(t, i) * powf(1-t, n-i) * wt[i];
        wx += b * pts[i].x;
        wy += b * pts[i].y;
        wsum += b;
    }
    MemFree(bc);
    return (Vector2){wx/wsum, wy/wsum};
}

int main(void) {
    InitWindow(W, H, "Lab 09C - NURBS");
    SetTargetFPS(60);
    pts[0]=(Vector2){100,450}; pts[1]=(Vector2){250,100}; pts[2]=(Vector2){550,100}; pts[3]=(Vector2){700,450};
    for (int i = 0; i < MAXP; i++) wt[i] = 1.0f;
    while (!WindowShouldClose()) {
        Vector2 m = GetMousePosition();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            drag = -1; sel = -1;
            for (int i = 0; i < np; i++)
                if (fabsf(m.x-pts[i].x)<R*2 && fabsf(m.y-pts[i].y)<R*2) { drag=i; sel=i; break; }
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) drag = -1;
        if (drag >= 0) pts[drag] = m;
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && np < MAXP) { pts[np] = m; wt[np] = 1.0f; np++; }
        if (IsKeyPressed(KEY_D) && np > 2) np--;
        float mw = GetMouseWheelMove();
        if (sel >= 0 && fabsf(mw) > 0) {
            wt[sel] += mw * 0.2f;
            if (wt[sel] < 0.1f) wt[sel] = 0.1f;
            if (wt[sel] > 10.0f) wt[sel] = 10.0f;
        }
        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (int i = 0; i < np-1; i++) DrawLineV(pts[i], pts[i+1], DARKGRAY);
        if (np >= 2) {
            Vector2 prev = nurbs_eval(0);
            for (int i = 1; i <= STEPS; i++) {
                Vector2 cur = nurbs_eval((float)i/STEPS);
                DrawLineV(prev, cur, GREEN);
                prev = cur;
            }
        }
        for (int i = 0; i < np; i++) {
            DrawCircleV(pts[i], R, (i==sel)?ORANGE:RED);
            DrawText(TextFormat("w=%.1f", wt[i]), (int)pts[i].x+12, (int)pts[i].y-8, 14, DARKGRAY);
        }
        DrawText("NURBS - click point + scroll to change weight", 10, 10, 20, DARKGRAY);
        DrawText("RClick=add  D=delete last", 10, 35, 16, GRAY);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
