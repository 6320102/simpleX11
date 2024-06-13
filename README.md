# simpleX11

## インストール

Dockerでイメージをビルド
```
docker build . -t x11
```

## コンパイル
```
gcc -I/usr/include/X11 -L/usr/lib/x86_64-linux-gnu -o <name> xeyes.c -lX11 -lm
```
