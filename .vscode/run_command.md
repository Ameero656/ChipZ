    gcc -I./util -I./chips/arithmetic -I./chips/memory -I./chips/routing -I./logic main.c -I./language logic/*.c util/vector.c chips/arithmetic/*.c chips/memory/*.c chips/routing/*.c language/*.c -o main

    ./main.exe

    