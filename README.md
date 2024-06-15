# simpleX11

## インストール

Dockerでイメージをビルド
```
docker build . -t x11
```

## コンパイル
実行可能ファイル生成:
```
make
```

実行可能ファイル削除:
```
make clean
```

個別に実行可能ファイルを作る場合:
```
gcc -I/usr/include/X11 -L/usr/lib/x86_64-linux-gnu -o <exe_file_name> <c_file_name> -lX11 -lm
```

## TODO
- コンテンツに依らないサーバの構築