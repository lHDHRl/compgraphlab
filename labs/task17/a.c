// Задание 17А: Система частиц — фейерверк.
#include "raylib.h"
#include <math.h>

#define MAX 4000

typedef struct { float x, y, vx, vy, life; } P;
P ps[MAX];
int cnt = 0;

void burst(float bx, float by) {
    for (int i = 0; i < 200 && cnt < MAX; i++, cnt++) {
        float a = GetRandomValue(0, 3600) / 10.0f * DEG2RAD;
        float s = GetRandomValue(50, 300) / 100.0f;
        ps[cnt] = (P){bx, by, cosf(a)*s, sinf(a)*s, 1.0f + GetRandomValue(0, 100)/100.0f};
    }
}

int main(void) {
    InitWindow(800, 600, "17А Фейерверк");
    SetTargetFPS(60);
    burst(400, 300);
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) burst(GetMouseX(), GetMouseY());
        int alive = 0;
        for (int i = 0; i < cnt; i++) {
            ps[i].x += ps[i].vx; ps[i].y += ps[i].vy;
            ps[i].vy += 0.05f;
            ps[i].life -= dt;
            if (ps[i].life > 0) ps[alive++] = ps[i];
        }
        cnt = alive;
        BeginDrawing();
        ClearBackground(BLACK);
        for (int i = 0; i < cnt; i++) {
            float t = ps[i].life;
            Color c = ColorFromHSV(t * 60.0f, 1.0f, t > 0 ? 1.0f : 0.0f);
            c.a = (unsigned char)(t * 255 > 255 ? 255 : t * 255);
            DrawCircle((int)ps[i].x, (int)ps[i].y, 2, c);
        }
        DrawText("Click for firework", 10, 10, 20, GRAY);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
