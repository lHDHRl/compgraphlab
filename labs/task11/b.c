// Задание 11Б: Безье / B-Spline / Катмулл-Ром, TAB переключение, правый клик — добавить, D — удалить
#include "raylib.h"
#include <math.h>

#define W 800
#define H 600
#define MAXP 64
#define R 8
#define STEPS 200

static Vector2 pts[MAXP];
static int np = 4;
static int drag = -1;
static int mode = 0;
static const char *names[] = {"Bezier", "B-Spline", "Catmull-Rom"};

static Vector2 bezier_decasteljau(float t) {
    Vector2 tmp[MAXP];
    for (int i = 0; i < np; i++) tmp[i] = pts[i];
    for (int k = np-1; k > 0; k--)
        for (int i = 0; i < k; i++) {
            tmp[i].x = (1-t)*tmp[i].x + t*tmp[i+1].x;
            tmp[i].y = (1-t)*tmp[i].y + t*tmp[i+1].y;
        }
    return tmp[0];
}

static Vector2 bspline_seg(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3, float t) {
    float t2=t*t, t3=t2*t;
    float b0=(-t3+3*t2-3*t+1)/6, b1=(3*t3-6*t2+4)/6, b2=(-3*t3+3*t2+3*t+1)/6, b3=t3/6;
    return (Vector2){b0*p0.x+b1*p1.x+b2*p2.x+b3*p3.x, b0*p0.y+b1*p1.y+b2*p2.y+b3*p3.y};
}

static Vector2 catmull_seg(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p3, float t) {
    float t2=t*t, t3=t2*t;
    float b0=(-t3+2*t2-t)/2, b1=(3*t3-5*t2+2)/2, b2=(-3*t3+4*t2+t)/2, b3=(t3-t2)/2;
    return (Vector2){b0*p0.x+b1*p1.x+b2*p2.x+b3*p3.x, b0*p0.y+b1*p1.y+b2*p2.y+b3*p3.y};
}

static void draw_spline(int type) {
    if (np < 2) return;
    if (type == 0) {
        Vector2 prev = bezier_decasteljau(0);
        for (int i = 1; i <= STEPS; i++) {
            Vector2 cur = bezier_decasteljau((float)i/STEPS);
            DrawLineV(prev, cur, GREEN);
            prev = cur;
        }
    } else {
        if (np < 4) return;
        int segs = np - 3;
        for (int s = 0; s < segs; s++) {
            Vector2 prev = (type==1) ? bspline_seg(pts[s],pts[s+1],pts[s+2],pts[s+3],0) : catmull_seg(pts[s],pts[s+1],pts[s+2],pts[s+3],0);
            for (int i = 1; i <= STEPS; i++) {
                float t = (float)i/STEPS;
                Vector2 cur = (type==1) ? bspline_seg(pts[s],pts[s+1],pts[s+2],pts[s+3],t) : catmull_seg(pts[s],pts[s+1],pts[s+2],pts[s+3],t);
                DrawLineV(prev, cur, GREEN);
                prev = cur;
            }
        }
    }
}

int main(void) {
    InitWindow(W, H, "Lab 09B - Curves");
    SetTargetFPS(60);
    pts[0]=(Vector2){100,400}; pts[1]=(Vector2){250,150}; pts[2]=(Vector2){550,150}; pts[3]=(Vector2){700,400};
    while (!WindowShouldClose()) {
        Vector2 m = GetMousePosition();
        if (IsKeyPressed(KEY_TAB)) mode = (mode+1)%3;
        if (IsKeyPressed(KEY_D) && np > 2) np--;
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && np < MAXP) { pts[np++] = m; }
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            drag = -1;
            for (int i = 0; i < np; i++)
                if (fabsf(m.x-pts[i].x)<R && fabsf(m.y-pts[i].y)<R) { drag=i; break; }
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) drag = -1;
        if (drag >= 0) pts[drag] = m;
        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (int i = 0; i < np-1; i++) DrawLineV(pts[i], pts[i+1], DARKGRAY);
        draw_spline(mode);
        for (int i = 0; i < np; i++) DrawCircleV(pts[i], R, RED);
        DrawText(TextFormat("Mode: %s (TAB)", names[mode]), 10, 10, 20, DARKGRAY);
        DrawText("RClick=add  D=delete last", 10, 35, 16, GRAY);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
