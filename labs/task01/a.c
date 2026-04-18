// Задание 1А: Поворот одного отрезка (Брезенхем, ввод координат, поворот вокруг точки)
#include "raylib.h"
#include <math.h>
#include <stdlib.h>

void bresenham(int x0, int y0, int x1, int y1, Color c) {
    int dx = abs(x1 - x0), dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    while (1) {
        DrawPixel(x0, y0, c);
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }
    }
}

void rotate(float cx, float cy, float *x, float *y, float a) {
    float dx = *x - cx, dy = *y - cy;
    *x = cx + dx * cosf(a) - dy * sinf(a);
    *y = cy + dx * sinf(a) + dy * cosf(a);
}

int main(void) {
    InitWindow(800, 600, "Lab 1A - Поворот отрезка");
    SetTargetFPS(60);

    float pts[] = {200, 300, 600, 300};
    float cx = 400, cy = 300, angle = 0;
    char buf[6][16] = {"200","300","600","300","400","300"};
    int edit = -1;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_LEFT))  angle -= 5 * DEG2RAD;
        if (IsKeyPressed(KEY_RIGHT)) angle += 5 * DEG2RAD;

        if (edit >= 0) {
            int k = GetCharPressed();
            int len = TextLength(buf[edit]);
            while (k > 0) {
                if ((k >= '0' && k <= '9') || k == '-') {
                    if (len < 15) { buf[edit][len] = k; buf[edit][len+1] = 0; }
                }
                k = GetCharPressed();
            }
            if (IsKeyPressed(KEY_BACKSPACE) && len > 0) buf[edit][len-1] = 0;
            if (IsKeyPressed(KEY_ENTER)) {
                float v = atof(buf[edit]);
                if (edit < 4) pts[edit] = v;
                else if (edit == 4) cx = v;
                else cy = v;
                edit = -1;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        const char *labels[] = {"X0","Y0","X1","Y1","CX","CY"};
        for (int i = 0; i < 6; i++) {
            int bx = 10 + (i % 2) * 120, by = 10 + (i / 2) * 35;
            DrawText(labels[i], bx, by, 14, GRAY);
            Rectangle r = {bx + 25, by - 2, 80, 20};
            DrawRectangleRec(r, edit == i ? DARKBLUE : DARKGRAY);
            DrawText(buf[i], bx + 28, by, 14, WHITE);
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(GetMousePosition(), r))
                edit = i;
        }

        float x0 = pts[0], y0 = pts[1], x1 = pts[2], y1 = pts[3];
        rotate(cx, cy, &x0, &y0, angle);
        rotate(cx, cy, &x1, &y1, angle);
        bresenham((int)x0, (int)y0, (int)x1, (int)y1, GREEN);
        DrawCircle((int)cx, (int)cy, 4, RED);
        DrawText(TextFormat("Angle: %.1f°", angle * RAD2DEG), 10, 120, 16, YELLOW);

        EndDrawing();
    }
    CloseWindow();
    return 0;
}
