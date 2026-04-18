// Задание 3В: Фрактальное дерево на L-системах (куст/папоротник), листва кружками
#include "raylib.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define MAXSTR 200000

char str[MAXSTR], tmp[MAXSTR];

typedef struct { const char *axiom; const char *ruleF; float angle; float scale; } LSystem;

LSystem systems[] = {
    {"F", "FF+[+F-F-F]-[-F+F+F]", 22.5f, 4.0f},
    {"X", "F+[[X]-X]-F[-FX]+X", 25.0f, 4.5f},
};
const char *names[] = {"Bush", "Fern"};

void generate(int sys, int iter) {
    strcpy(str, systems[sys].axiom);
    const char *rF = systems[sys].ruleF;
    for (int i = 0; i < iter; i++) {
        int p = 0;
        for (int j = 0; str[j] && p < MAXSTR-200; j++) {
            if (str[j] == 'F') {
                int rl = strlen(rF);
                if (p + rl < MAXSTR) { memcpy(tmp+p, rF, rl); p += rl; }
            } else if (str[j] == 'X' && sys == 1) {
                const char *rX = "F+[[X]-X]-F[-FX]+X";
                int rl = strlen(rX);
                if (p + rl < MAXSTR) { memcpy(tmp+p, rX, rl); p += rl; }
            } else {
                tmp[p++] = str[j];
            }
        }
        tmp[p] = 0;
        memcpy(str, tmp, p+1);
    }
}

int main(void) {
    InitWindow(800, 600, "Lab 3C - L-система (куст/папоротник)");
    SetTargetFPS(60);
    int sys = 0, iter = 4, dirty = 1;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ONE))  { sys=0; dirty=1; }
        if (IsKeyPressed(KEY_TWO))  { sys=1; dirty=1; }
        if (IsKeyPressed(KEY_UP) && iter < 7)   { iter++; dirty=1; }
        if (IsKeyPressed(KEY_DOWN) && iter > 1)  { iter--; dirty=1; }
        if (dirty) { generate(sys, iter); dirty=0; }

        BeginDrawing();
        ClearBackground(BLACK);

        float x=400, y=580, a=-90*DEG2RAD;
        float len = 300.0f;
        for (int i = 0; i < iter; i++) len /= systems[sys].scale;
        float stx[256], sty[256], sta[256]; int sp=0;
        Color branch = (Color){139,90,43,255};

        for (int i = 0; str[i]; i++) {
            char c = str[i];
            if (c == 'F') {
                float nx = x + len*cosf(a), ny = y + len*sinf(a);
                DrawLine(x,y,nx,ny, branch);
                x=nx; y=ny;
            } else if (c == '+') {
                a += systems[sys].angle * DEG2RAD;
            } else if (c == '-') {
                a -= systems[sys].angle * DEG2RAD;
            } else if (c == '[') {
                if (sp<256) { stx[sp]=x; sty[sp]=y; sta[sp]=a; sp++; }
            } else if (c == ']') {
                if (sp>0) {
                    int isLeaf = (i+1 >= (int)strlen(str) || str[i+1]==']' || str[i+1]=='-' || str[i+1]=='+');
                    if (isLeaf) DrawCircle(x, y, 2, GREEN);
                    sp--; x=stx[sp]; y=sty[sp]; a=sta[sp];
                }
            }
        }

        DrawText(TextFormat("%s  iter:%d  1/2=type UP/DOWN=iter", names[sys], iter), 10, 10, 16, YELLOW);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
