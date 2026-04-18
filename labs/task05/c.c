// Задание 5В: Триангуляция полигона с отверстием (мостик + ear clipping)
#include "raylib.h"
#include <math.h>

#define NO 13
#define NI 4
#define NM (NO + NI + 3)

typedef struct { Vector2 a, b, c; Color col; } Tri;

static float cross2(Vector2 o, Vector2 a, Vector2 b) {
    return (a.x-o.x)*(b.y-o.y) - (a.y-o.y)*(b.x-o.x);
}
static int pt_in_tri(Vector2 p, Vector2 a, Vector2 b, Vector2 c) {
    float d1=cross2(p,a,b), d2=cross2(p,b,c), d3=cross2(p,c,a);
    return !((d1<0||d2<0||d3<0) && (d1>0||d2>0||d3>0));
}

int main(void) {
    InitWindow(800, 600, "Задание 5В — Полигон с отверстием");
    SetTargetFPS(60);
    float cx=400, cy=300;

    // Outer: CW on screen (Y down) = standard for screen coords
    Vector2 outer[NO];
    for (int i=0; i<NO; i++) {
        float a = 2.0f*PI*i/NO;
        outer[i] = (Vector2){cx + 250*cosf(a), cy + 180*sinf(a)};
    }
    // Inner: CCW on screen (opposite to outer = hole)
    float hs = 50;
    Vector2 inner[NI] = {
        {cx+hs, cy-hs}, {cx+hs, cy+hs}, {cx-hs, cy+hs}, {cx-hs, cy-hs}
    };

    // Rightmost inner vertex -> bridge to closest outer
    int oi=0;
    for (int i=1; i<NI; i++) if (inner[i].x > inner[oi].x) oi=i;
    int ci=0; float best=1e9f;
    for (int i=0; i<NO; i++) {
        float dx=outer[i].x-inner[oi].x, dy=outer[i].y-inner[oi].y, d=dx*dx+dy*dy;
        if (d<best) { best=d; ci=i; }
    }

    // Merged: outer[ci]..outer[ci], inner[oi]..inner[oi], outer[ci]
    Vector2 merged[NM];
    int mn=0;
    for (int i=0; i<=NO; i++) merged[mn++] = outer[(ci+i)%NO];
    for (int i=0; i<=NI; i++) merged[mn++] = inner[(oi+i)%NI];
    merged[mn++] = outer[ci];

    // Determine winding of merged polygon (signed area)
    float area=0;
    for (int i=0; i<mn; i++) {
        int j=(i+1)%mn;
        area += merged[i].x*merged[j].y - merged[j].x*merged[i].y;
    }
    float sign = (area > 0) ? 1.0f : -1.0f; // ear test: cross*sign > 0

    // Ear clipping
    int idx[NM], n=mn;
    for (int i=0; i<mn; i++) idx[i]=i;
    Tri tris[NM]; int nt=0;
    while (n > 3) {
        int found=0;
        for (int i=0; i<n; i++) {
            int p=(i+n-1)%n, nx=(i+1)%n;
            Vector2 A=merged[idx[p]], B=merged[idx[i]], C=merged[idx[nx]];
            if (cross2(A,B,C)*sign <= 0) continue;
            int ear=1;
            for (int j=0; j<n; j++) {
                if (j==p||j==i||j==nx) continue;
                if (pt_in_tri(merged[idx[j]], A, B, C)) { ear=0; break; }
            }
            if (ear) {
                Color col = {(nt*73+80)%256, (nt*137+60)%256, (nt*197+100)%256, 220};
                tris[nt++] = (Tri){A, B, C, col};
                for (int j=i; j<n-1; j++) idx[j]=idx[j+1];
                n--; found=1; break;
            }
        }
        if (!found) break;
    }
    if (n==3) {
        Color col = {(nt*73+80)%256, (nt*137+60)%256, (nt*197+100)%256, 220};
        tris[nt++] = (Tri){merged[idx[0]], merged[idx[1]], merged[idx[2]], col};
    }

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        for (int i=0; i<nt; i++) {
            DrawTriangle(tris[i].a, tris[i].c, tris[i].b, tris[i].col);
            DrawTriangleLines(tris[i].a, tris[i].c, tris[i].b, WHITE);
        }
        for (int i=0; i<NO; i++) DrawLineV(outer[i], outer[(i+1)%NO], YELLOW);
        for (int i=0; i<NI; i++) DrawLineV(inner[i], inner[(i+1)%NI], RED);
        DrawText(TextFormat("Triangles: %d", nt), 10, 10, 20, WHITE);
        EndDrawing();
    }
    CloseWindow();
}
