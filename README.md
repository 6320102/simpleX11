# simpleX11

## インストール

Dockerでイメージをビルドしてコンテナを起動
```
docker build . -t x11
docker run --rm -it x11 bash
```

## コンパイル
```
gcc -I/usr/include/X11 -L/usr/lib/x86_64-linux-gnu -o <name> xeyes.c -lX11 -lm
```
