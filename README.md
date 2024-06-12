# simpleX11

## Installation

X11の開発用ライブラリをインストール
```
sudo apt-get install libx11-dev
```
## To Compile
```
gcc -I/usr/include/X11 -L/usr/lib/x86_64-linux-gnu -o <name> xeyes.c -lX11 -lm
```
