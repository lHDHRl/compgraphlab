// Задание 12А.В: Плавающий горизонт. Две пересекающиеся поверхности.
#include "raylib.h"
#include <math.h>

typedef struct { int ymin[800]; int ymax[800]; } Horizon;

void resetH(Horizon *h) { for (int i = 0; i < 800; i++) { h->ymin[i] = 600; h->ymax[i] = 0; } }

void drawSurface(Horizon *h, float ax, float ay, int func, Color col) {
    float cax = cosf(ax), sax = sinf(ax), cay = cosf(ay), say = sinf(ay);
    for (float z = 5.0f; z >= -5.0f; z -= 0.1f) {
        int px = -1, py = -1;
        for (float x = -5.0f; x <= 5.0f; x += 0.05f) {
            float y = func == 0 ? sinf(x) * cosf(z) : 0.5f * cosf(x) * sinf(z);
            float rx = x * cay - z * say;
            float rz = x * say + z * cay;
            float ry = y * cax - rz * sax;
            float rz2 = y * sax + rz * cax;
            int sx = (int)(400 + rx * 60);
            int sy = (int)(300 - ry * 100 + rz2 * 20);
            if (sx >= 0 && sx < 800) {
                if (sy < h->ymin[sx]) { if (px >= 0) DrawLine(px, py, sx, sy, col); h->ymin[sx] = sy; }
                else if (sy > h->ymax[sx]) { if (px >= 0) DrawLine(px, py, sx, sy, col); h->ymax[sx] = sy; }
            }
            px = sx; py = sy;
        }
    }
}

int main(void) {
    InitWindow(800, 600, "12А.В Две поверхности");
    SetTargetFPS(60);
    float ax = 0.3f, ay = 0.0f;
    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_LEFT)) ay -= 0.02f;
        if (IsKeyDown(KEY_RIGHT)) ay += 0.02f;
        if (IsKeyDown(KEY_UP)) ax -= 0.02f;
        if (IsKeyDown(KEY_DOWN)) ax += 0.02f;
        BeginDrawing();
        ClearBackground(BLACK);
        Horizon h; resetH(&h);
        drawSurface(&h, ax, ay, 0, GREEN);
        drawSurface(&h, ax, ay, 1, ORANGE);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
