@echo off
g++ main.cpp -o game.exe -O3 -s -std=c++17 -march=native ^
 -fno-exceptions -lglfw3 -lopengl32 -lglu32 -lgdi32
