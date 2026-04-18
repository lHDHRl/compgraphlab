// Задание 2Б: Повёрнутые прямоугольники, отсечение Лианга-Барски
#include "raylib.h"
#include <math.h>

static float xmin=250, ymin=150, xmax=550, ymax=450;

int liang_barsky(float *x0, float *y0, float *x1, float *y1) {
    float dx = *x1-*x0, dy = *y1-*y0;
    float p[] = {-dx, dx, -dy, dy};
    float q[] = {*x0-xmin, xmax-*x0, *y0-ymin, ymax-*y0};
    float u0 = 0, u1 = 1;
    for (int i = 0; i < 4; i++) {
        if (p[i] == 0) { if (q[i] < 0) return 0; }
        else {
            float r = q[i] / p[i];
            if (p[i] < 0) { if (r > u0) u0 = r; }
            else { if (r < u1) u1 = r; }
        }
    }
    if (u0 > u1) return 0;
    float ox=*x0, oy=*y0;
    *x1 = ox + u1*dx; *y1 = oy + u1*dy;
    *x0 = ox + u0*dx; *y0 = oy + u0*dy;
    return 1;
}

typedef struct { float x0,y0,x1,y1; } Seg;

int main(void) {
    InitWindow(800, 600, "Lab 2B - Лианг-Барски");
    SetTargetFPS(60);

    Seg segs[100]; int ns = 0;
    for (int i = 0; i < 25; i++) {
        float h = 20 + i * 20;
        float ox = 400 + i * 8 - 96, oy = 300;
        float ang = (float)(i * 137 % 360) * DEG2RAD;
        float cs = cosf(ang), sn = sinf(ang);
        float cx[] = {-h,h,h,-h}, cy[] = {-h,-h,h,h};
        float rx[4], ry[4];
        for (int j = 0; j < 4; j++) {
            rx[j] = ox + cx[j]*cs - cy[j]*sn;
            ry[j] = oy + cx[j]*sn + cy[j]*cs;
        }
        for (int j = 0; j < 4; j++) {
            int k = (j+1)%4;
            segs[ns++] = (Seg){rx[j],ry[j],rx[k],ry[k]};
        }
    }

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        for (int i = 0; i < ns; i++)
            DrawLine(segs[i].x0,segs[i].y0,segs[i].x1,segs[i].y1, DARKGRAY);
        for (int i = 0; i < ns; i++) {
            float a=segs[i].x0, b=segs[i].y0, c=segs[i].x1, d=segs[i].y1;
            if (liang_barsky(&a,&b,&c,&d))
                DrawLine(a,b,c,d, GREEN);
        }
        DrawRectangleLines(xmin,ymin,xmax-xmin,ymax-ymin, YELLOW);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
