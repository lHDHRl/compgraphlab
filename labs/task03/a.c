// Задание 3А: Классическое дерево Пифагора (прямоугольники, ±45°, 1/√2)
#include "raylib.h"
#include <math.h>

void pythtree(float x0, float y0, float x1, float y1, int depth) {
    if (depth <= 0) return;
    float dx = x1-x0, dy = y1-y0;
    float x2 = x1-dy, y2 = y1+dx;
    float x3 = x0-dy, y3 = y0+dx;
    float mx = (x3+x2)*0.5f, my = (y3+y2)*0.5f;
    float px = mx + (x3-x2)*0.5f, py = my + (y3-y2)*0.5f;

    Color c = ColorFromHSV(120.0f - depth*10.0f, 0.8f, 0.8f);
    DrawLine(x0,y0,x1,y1,c); DrawLine(x1,y1,x2,y2,c);
    DrawLine(x2,y2,x3,y3,c); DrawLine(x3,y3,x0,y0,c);

    pythtree(x3, y3, px, py, depth-1);
    pythtree(px, py, x2, y2, depth-1);
}

int main(void) {
    InitWindow(800, 600, "Lab 3A - Дерево Пифагора");
    SetTargetFPS(60);
    int depth = 6;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_UP) && depth < 12) depth++;
        if (IsKeyPressed(KEY_DOWN) && depth > 1) depth--;

        BeginDrawing();
        ClearBackground(BLACK);
        pythtree(350, 550, 450, 550, depth);
        DrawText(TextFormat("Depth: %d  UP/DOWN", depth), 10, 10, 16, YELLOW);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
