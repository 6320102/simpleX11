#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define PORT 12345
#define PIXEL_SIZE 4 // RGBA

void draw_image(Display *dpy, Window win, GC gc, char *pixel_data, int width, int height) {
    printf("%d\n", (int)pixel_data[479596]);

    int screen = DefaultScreen(dpy);
    XImage *image = XCreateImage(dpy, DefaultVisual(dpy, screen), DefaultDepth(dpy, screen), ZPixmap, 0, pixel_data, width, height, 32, width * PIXEL_SIZE);

    if (!image) {
        fprintf(stderr, "Failed to create XImage\n");
        return;
    }

    // イベントループ
    XEvent event;
    while (1) {
        XNextEvent(dpy, &event);
        // 何らかのイベント処理が必要な場合に記述
        if (event.type == Expose) {
            // ウィンドウの露出時に画像を描画
            XPutImage(dpy, win, DefaultGC(dpy, screen), image, 0, 0, 0, 0, width, height);
            XFlush(dpy);
        }
    }

    XPutImage(dpy, win, gc, image, 0, 0, 0, 0, width, height);
    XFlush(dpy);
    XDestroyImage(image);
}

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int width, height;
    char *pixel_data = NULL;

    // ソケット作成
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // サーバーアドレスとポートを設定
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // ソケットバインド
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // ソケットをリスニングモードに設定
    if (listen(sockfd, 5) < 0) {
        perror("Listen failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // X11の初期化
    Display *dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        fprintf(stderr, "Unable to open X display\n");
        exit(EXIT_FAILURE);
    }

    int screen = DefaultScreen(dpy);
    GC gc = DefaultGC(dpy, screen);
    unsigned long white_pixel = WhitePixel(dpy, screen);
    unsigned long black_pixel = BlackPixel(dpy, screen);

    while (1) {
        // クライアントからの接続を受け入れる
        newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len);
        if (newsockfd < 0) {
            perror("Accept failed");
            continue;
        }

        // クライアントからデータを受信
        if (read(newsockfd, &width, sizeof(int)) <= 0 ||
            read(newsockfd, &height, sizeof(int)) <= 0) {
            perror("Failed to receive data");
            close(newsockfd);
            continue;
        }

        // メモリ割り当ての前に正しいデータサイズを確認
        size_t pixel_data_size = width * height * PIXEL_SIZE; // Each pixel is RGBX (4 bytes)
        pixel_data = malloc(pixel_data_size);
        if (pixel_data == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            close(newsockfd);
            continue;
        }

        if (read(newsockfd, pixel_data, pixel_data_size) <= 0) {
            perror("Failed to receive pixel data");
            free(pixel_data);
            close(newsockfd);
            continue;
        }

        // ウィンドウを作成
        Window win = XCreateSimpleWindow(dpy, RootWindow(dpy, screen), 0, 0, width, height, 1, black_pixel, white_pixel);
        XMapWindow(dpy, win);
        // どのイベントを受け付けるか決定
        XSelectInput(dpy, win, ExposureMask);
        XFlush(dpy);

        // 受け取ったデータに基づいてイメージを描画
        draw_image(dpy, win, gc, pixel_data, width, height);
        // クライアントからのデータをウィンドウマネージャへ送信後、ウィンドウを閉じる
        // free(pixel_data);
        close(newsockfd);
    }

    // クリーンアップ
    close(sockfd);
    XCloseDisplay(dpy);

    return 0;
}