#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define WIDTH 400
#define HEIGHT 400
#define PIXEL_SIZE 4 // BGRX

void draw_rectangle(char *data, int width, int height) {
    // 四角形の左上と右下の座標を設定
    int rect_left = width / 4;
    int rect_top = height / 4;
    int rect_right = width * 3 / 4;
    int rect_bottom = height * 3 / 4;

    // 四角形を描画
    for (int y = rect_top; y < rect_bottom; y++) {
        for (int x = rect_left; x < rect_right; x++) {
            int index = (y * width + x) * PIXEL_SIZE;
            data[index] = 255;     // B
            data[index + 1] = y * 255 / height; // G
            data[index + 2] = x * 255 / width; // R
        }
    }
}

int main() {
    Display *dpy;
    Window win;
    XEvent event;
    XImage *image;
    unsigned long white_pixel, black_pixel;
    int screen;

    char *pixel_data = malloc(WIDTH * HEIGHT * PIXEL_SIZE);
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
    white_pixel = WhitePixel(dpy, screen);
    black_pixel = BlackPixel(dpy, screen);

    // ウィンドウを作成
    win = XCreateSimpleWindow(dpy, RootWindow(dpy, screen), 10, 10, WIDTH, HEIGHT, 1, black_pixel, white_pixel);
    XSelectInput(dpy, win, ExposureMask | KeyPressMask);
    XMapWindow(dpy, win);
    XFlush(dpy);

    // ピクセルデータを生成 (ここでは例として固定の四角形を描画する)
    draw_rectangle(pixel_data, WIDTH, HEIGHT);

    // 描画
    image = XCreateImage(dpy, DefaultVisual(dpy, screen), DefaultDepth(dpy, screen), ZPixmap, 0, pixel_data, WIDTH, HEIGHT, 32, WIDTH * PIXEL_SIZE);

    if (!image) {
        fprintf(stderr, "Failed to create XImage\n");
        return 1;
    }

    // イベントループ
    while (1) {
        XNextEvent(dpy, &event);
        // 何らかのイベント処理が必要な場合に記述
        if (event.type == Expose) {
            // ウィンドウの露出時に画像を描画
            XPutImage(dpy, win, DefaultGC(dpy, screen), image, 0, 0, 0, 0, WIDTH, HEIGHT);
            XFlush(dpy);
        }

        if (event.type == KeyPress) {
            break;
        }
    }

    // クリーンアップ
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
    XDestroyImage(image);
    free(pixel_data);

    return 0;
}
