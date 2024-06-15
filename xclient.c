#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

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
            data[index] = x * 255 / width;     // B
            data[index + 1] = y * 255 / height; // G
            data[index + 2] = 0; // R
        }
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    int width = WIDTH;
    int height = HEIGHT;
    char *pixel_data = malloc(width * height * PIXEL_SIZE);

    if (pixel_data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    // サーバーアドレスとポートを設定
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

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
    draw_rectangle(pixel_data, width, height);

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
