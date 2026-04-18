// Задание 2А: Отсечение Сазерленда-Коэна, 25 концентрических квадратов
#include "raylib.h"
#include <math.h>

enum { LEFT=1, RIGHT=2, BOTTOM=4, TOP=8 };
static float xmin, ymin, xmax, ymax;

int code(float x, float y) {
    int c = 0;
    if (x < xmin) c |= LEFT; else if (x > xmax) c |= RIGHT;
    if (y > ymax) c |= BOTTOM; else if (y < ymin) c |= TOP;
    return c;
}

int cohen_sutherland(float *x0, float *y0, float *x1, float *y1) {
    int c0 = code(*x0,*y0), c1 = code(*x1,*y1);
    while (1) {
        if (!(c0|c1)) return 1;
        if (c0&c1) return 0;
        int c = c0 ? c0 : c1;
        float x, y;
        if (c & TOP)    { x = *x0+(*x1-*x0)*(ymin-*y0)/(*y1-*y0); y = ymin; }
        else if (c & BOTTOM) { x = *x0+(*x1-*x0)*(ymax-*y0)/(*y1-*y0); y = ymax; }
        else if (c & RIGHT)  { y = *y0+(*y1-*y0)*(xmax-*x0)/(*x1-*x0); x = xmax; }
        else                  { y = *y0+(*y1-*y0)*(xmin-*x0)/(*x1-*x0); x = xmin; }
        if (c == c0) { *x0=x; *y0=y; c0=code(x,y); }
        else         { *x1=x; *y1=y; c1=code(x,y); }
    }
}

int main(void) {
    InitWindow(800, 600, "Lab 2A - Сазерленд-Коэн");
    SetTargetFPS(60);
    xmin=250; ymin=150; xmax=550; ymax=450;

    typedef struct { float x0,y0,x1,y1; } Seg;
    Seg segs[100]; int ns = 0;
    for (int i = 0; i < 25; i++) {
        float h = 20 + i * 20;
        float ox = 400 + i * 8 - 96, oy = 300;
        float l=ox-h, r=ox+h, t=oy-h, b=oy+h;
        segs[ns++] = (Seg){l,t,r,t};
        segs[ns++] = (Seg){r,t,r,b};
        segs[ns++] = (Seg){r,b,l,b};
        segs[ns++] = (Seg){l,b,l,t};
    }

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        for (int i = 0; i < ns; i++)
            DrawLine(segs[i].x0,segs[i].y0,segs[i].x1,segs[i].y1, DARKGRAY);
        for (int i = 0; i < ns; i++) {
            float a=segs[i].x0, b=segs[i].y0, c=segs[i].x1, d=segs[i].y1;
            if (cohen_sutherland(&a,&b,&c,&d))
                DrawLine(a,b,c,d, GREEN);
        }
        DrawRectangleLines(xmin,ymin,xmax-xmin,ymax-ymin, YELLOW);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
