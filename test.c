#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define WIDTH 400
#define HEIGHT 400
#define PIXEL_SIZE 4 // RGBA

void draw_image(Display *dpy, Window win, GC gc, unsigned char *pixel_data, int width, int height) {
    for (int i = 0; i < width * height; i++) {
        // printf("%d,%d,%d,%d ",pixel_data[i * PIXEL_SIZE],pixel_data[i * PIXEL_SIZE + 1],pixel_data[i * PIXEL_SIZE + 2], pixel_data[i * PIXEL_SIZE + 3]);
        printf("%d ",pixel_data[i * PIXEL_SIZE]);
    }
    int screen = DefaultScreen(dpy);
    XImage *image = XCreateImage(dpy, DefaultVisual(dpy, screen), DefaultDepth(dpy, screen), ZPixmap, 0, (char *)pixel_data, width, height, 32, width * PIXEL_SIZE);

    if (!image) {
        fprintf(stderr, "Failed to create XImage\n");
        return;
    }

    XPutImage(dpy, win, gc, image, 0, 0, 0, 0, width, height);
    XFlush(dpy);
    XDestroyImage(image);
}

void draw_rectangle(unsigned char *data, int width, int height) {
    // 背景を黒に設定
    for (int i = 0; i < width * height; i++) {
        data[i * PIXEL_SIZE] = 0;   // Red
        data[i * PIXEL_SIZE + 1] = 0; // Green
        data[i * PIXEL_SIZE + 2] = 0; // Blue
        data[i * PIXEL_SIZE + 3] = 0; // Alpha (opaque)
    }

    // 四角形の左上と右下の座標を設定
    int rect_left = width / 4;
    int rect_top = height / 4;
    int rect_right = width * 3 / 4;
    int rect_bottom = height * 3 / 4;

    // 四角形を描画
    for (int y = rect_top; y < rect_bottom; y++) {
        for (int x = rect_left; x < rect_right; x++) {
            int index = (y * width + x) * PIXEL_SIZE;
            data[index] = 255;     // Red
            data[index + 1] = 255; // Green
            data[index + 2] = 0; // Blue
            data[index + 3] = 0; // Alpha (opaque)
        }
    }
}

int main() {
    Display *dpy;
    Window win;
    GC gc;
    unsigned long white_pixel, black_pixel;
    int screen;
    unsigned char *pixel_data = malloc(WIDTH * HEIGHT * PIXEL_SIZE);

    if (pixel_data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    // X11の初期化
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        fprintf(stderr, "Unable to open X display\n");
        return 1;
    }

    screen = DefaultScreen(dpy);
    gc = DefaultGC(dpy, screen);
    white_pixel = WhitePixel(dpy, screen);
    black_pixel = BlackPixel(dpy, screen);

    // ピクセルデータを生成 (ここでは例として固定の四角形を描画する)
    draw_rectangle(pixel_data, WIDTH, HEIGHT);

    // ウィンドウを作成
    win = XCreateSimpleWindow(dpy, RootWindow(dpy, screen), 0, 0, WIDTH, HEIGHT, 1, black_pixel, white_pixel);
    XMapWindow(dpy, win);
    XFlush(dpy);

    // 描画
    draw_image(dpy, win, gc, pixel_data, WIDTH, HEIGHT);

    // イベントループ
    XEvent event;
    while (1) {
        XNextEvent(dpy, &event);
        // 何らかのイベント処理が必要な場合に記述
    }

    // クリーンアップ
    XFreeGC(dpy, gc);
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
    free(pixel_data);

    return 0;
}
