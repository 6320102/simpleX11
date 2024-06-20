#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <arpa/inet.h>

#define WIDTH 400
#define HEIGHT 400
#define PIXEL_SIZE 4 // BGRX

void draw_rectangle(char *data, int width, int height) {
    // 四角形の左上と右下の座標を設定
    int rect_left = width / 8;
    int rect_top = height / 8;
    int rect_right = width * 7 / 8;
    int rect_bottom = height * 7 / 8;

    // 四角形を描画
    for (int y = rect_top; y < rect_bottom; y++) {
        for (int x = rect_left; x < rect_right; x++) {
            int index = (y * width + x) * PIXEL_SIZE;
            data[index] = 0;     // B
            data[index + 1] = 255; // G
            data[index + 2] = 0; // R
        }
    }
}

void draw_eight(char *data, int width, int height) {
    // 背景を白に設定
    for (int i = 0; i < width * height * PIXEL_SIZE; i++) {
        data[i] = 255;
    }

    int cx = width / 2;
    int cy = height / 2;
    int radius = (width < height ? width : height) / 4;
    double angle_step = M_PI / 5;

    // 星の頂点を計算
    for (int i = 0; i < 10; i++) {
        double angle = i * angle_step;
        int x1 = (int)(cx + radius * cos(angle));
        int y1 = (int)(cy + radius * sin(angle));
        int x2 = (int)(cx + radius * cos(angle + angle_step));
        int y2 = (int)(cy + radius * sin(angle + angle_step));

        // 線を描画 (Bresenham's line algorithm)
        int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
        int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
        int err = dx + dy, e2;

        while (1) {
            if (x1 >= 0 && x1 < width && y1 >= 0 && y1 < height) {
                int index = (y1 * width + x1) * PIXEL_SIZE;
                data[index] = 0;     // Red
                data[index + 1] = 0; // Green
                data[index + 2] = 0; // Blue
            }
            if (x1 == x2 && y1 == y2) break;
            e2 = 2 * err;
            if (e2 >= dy) { err += dy; x1 += sx; }
            if (e2 <= dx) { err += dx; y1 += sy; }
        }
    }
}

void draw_color(char *data, int width, int height) {
    // 四角形の左上と右下の座標を設定
    int rect_left = width / 4;
    int rect_top = height / 4;
    int rect_right = width * 3 / 4;
    int rect_bottom = height * 3 / 4;

    // 四角形を描画
    for (int y = rect_top; y < rect_bottom; y++) {
        for (int x = rect_left; x < rect_right; x++) {
            int index = (y * width + x) * PIXEL_SIZE;
            data[index] = x * 255 / width;     // B
            data[index + 1] = y * 255 / height; // G
            data[index + 2] = 0; // R
        }
    }
}

int main(int argc, char const *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr;
    int width = WIDTH;
    int height = HEIGHT;
    char *pixel_data = malloc(width * height * PIXEL_SIZE);

    if (pixel_data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    if(argc != 2) {
        printf("引数はIP ポート番号\n");
    }
    // サーバーアドレスとポートを設定
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(atoi(argv[2]));

    // ソケット作成
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        free(pixel_data);
        exit(EXIT_FAILURE);
    }

    // サーバーに接続
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to server failed");
        close(sockfd);
        free(pixel_data);
        exit(EXIT_FAILURE);
    }

    // ピクセルデータを生成
    //draw_rectangle(pixel_data, width, height);
    //draw_star(pixel_data, width, height);
    draw_color(pixel_data, width, height);

    // データを送信
    if (write(sockfd, &width, sizeof(int)) < 0 ||
        write(sockfd, &height, sizeof(int)) < 0 ||
        write(sockfd, pixel_data, width * height * PIXEL_SIZE) < 0) {
        perror("Failed to send data");
    }
    
    // 接続を閉じる
    close(sockfd);
    free(pixel_data);

    return 0;
}
