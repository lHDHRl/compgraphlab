// Задание 7А: Закраска Гуро — низкополигональная сфера, точечный источник света
#include "raylib.h"
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define W 800
#define H 600
#define SEGS 16

typedef struct { float x, y, z; } Vec3;

static Vec3 v3(float x, float y, float z) { return (Vec3){x, y, z}; }
static Vec3 v3sub(Vec3 a, Vec3 b) { return v3(a.x-b.x, a.y-b.y, a.z-b.z); }
static Vec3 v3scale(Vec3 a, float s) { return v3(a.x*s, a.y*s, a.z*s); }
static float v3dot(Vec3 a, Vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
static Vec3 v3norm(Vec3 a) { float l = sqrtf(v3dot(a,a)); return l > 0 ? v3scale(a, 1.0f/l) : a; }

typedef struct { float x, y; } Vec2;

static Vec3 sphere_pos(int i, int j, float r) {
    float theta = (float)i / SEGS * M_PI;
    float phi = (float)j / SEGS * 2.0f * M_PI;
    return v3(r * sinf(theta) * cosf(phi), r * cosf(theta), r * sinf(theta) * sinf(phi));
}

static Vec2 project(Vec3 p) {
    float fov = 500.0f, cz = 4.0f;
    float z = p.z + cz;
    if (z < 0.1f) z = 0.1f;
    return (Vec2){ W/2.0f + p.x * fov / z, H/2.0f - p.y * fov / z };
}

static float shade_vertex(Vec3 pos, Vec3 normal, Vec3 light) {
    Vec3 L = v3norm(v3sub(light, pos));
    float diff = v3dot(normal, L);
    if (diff < 0) diff = 0;
    float ambient = 0.1f;
    float c = ambient + 0.9f * diff;
    return c > 1.0f ? 1.0f : c;
}

static float edge_func(Vec2 a, Vec2 b, Vec2 c) {
    return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

static void draw_tri(Image *img, Vec2 p0, Vec2 p1, Vec2 p2, float c0, float c1, float c2) {
    int minx = (int)fminf(fminf(p0.x, p1.x), p2.x);
    int maxx = (int)fmaxf(fmaxf(p0.x, p1.x), p2.x);
    int miny = (int)fminf(fminf(p0.y, p1.y), p2.y);
    int maxy = (int)fmaxf(fmaxf(p0.y, p1.y), p2.y);
    if (minx < 0) minx = 0;
    if (miny < 0) miny = 0;
    if (maxx >= W) maxx = W-1;
    if (maxy >= H) maxy = H-1;
    float area = edge_func(p0, p1, p2);
    if (fabsf(area) < 0.001f) return;
    for (int y = miny; y <= maxy; y++) {
        for (int x = minx; x <= maxx; x++) {
            Vec2 p = {x + 0.5f, y + 0.5f};
            float w0 = edge_func(p1, p2, p) / area;
            float w1 = edge_func(p2, p0, p) / area;
            float w2 = edge_func(p0, p1, p) / area;
            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                float c = w0 * c0 + w1 * c1 + w2 * c2;
                unsigned char v = (unsigned char)(c * 255);
                ImageDrawPixel(img, x, y, (Color){v, v, v, 255});
            }
        }
    }
}

int main(void) {
    InitWindow(W, H, "Lab 07A - Gouraud Shading");
    SetTargetFPS(60);
    float angle = 0;
    while (!WindowShouldClose()) {
        angle += 0.01f;
        Vec3 light = v3(3*cosf(angle), 2, 3*sinf(angle));
        Image img = GenImageColor(W, H, BLACK);
        for (int i = 0; i < SEGS; i++) {
            for (int j = 0; j < SEGS; j++) {
                Vec3 v00 = sphere_pos(i, j, 1.5f);
                Vec3 v10 = sphere_pos(i+1, j, 1.5f);
                Vec3 v11 = sphere_pos(i+1, j+1, 1.5f);
                Vec3 v01 = sphere_pos(i, j+1, 1.5f);
                Vec3 n00 = v3norm(v00), n10 = v3norm(v10), n11 = v3norm(v11), n01 = v3norm(v01);
                float c00 = shade_vertex(v00, n00, light);
                float c10 = shade_vertex(v10, n10, light);
                float c11 = shade_vertex(v11, n11, light);
                float c01 = shade_vertex(v01, n01, light);
                Vec2 s00 = project(v00), s10 = project(v10), s11 = project(v11), s01 = project(v01);
                draw_tri(&img, s00, s10, s11, c00, c10, c11);
                draw_tri(&img, s00, s11, s01, c00, c11, c01);
            }
        }
        Texture2D tex = LoadTextureFromImage(img);
        BeginDrawing();
        DrawTexture(tex, 0, 0, WHITE);
        DrawText("Gouraud Shading (vertex lighting)", 10, 10, 20, GREEN);
        EndDrawing();
        UnloadTexture(tex);
        UnloadImage(img);
    }
    CloseWindow();
    return 0;
}
