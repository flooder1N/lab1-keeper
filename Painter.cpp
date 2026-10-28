#include "Painter.h"
#include <iostream>
#include <cmath>
#include <algorithm>

namespace {

    const int MAX_COLS = 40;
    const int MAX_ROWS = 14;

    void scaleToConsole(double width, double height, int& cols, int& rows) {
        double scale = std::min(MAX_COLS / width, MAX_ROWS * 2.0 / height);
        cols = std::max(2, static_cast<int>(std::lround(width * scale)));
        rows = std::max(1, static_cast<int>(std::lround(height * scale / 2.0)));
    }

}

namespace Painter {

    void drawCircle() {
        const int R = 6;
        for (int y = -R; y <= R; ++y) {
            std::cout << "  ";
            for (int x = -2 * R; x <= 2 * R; ++x) {
                double dx = x / 2.0;
                std::cout << ((dx * dx + y * y <= R * R + 0.5) ? '*' : ' ');
            }
            std::cout << '\n';
        }
    }

    void drawRectangle(double width, double height) {
        int cols, rows;
        scaleToConsole(width, height, cols, rows);
        for (int r = 0; r < rows; ++r) {
            std::cout << "  ";
            for (int c = 0; c < cols; ++c) {
                bool border = (r == 0 || r == rows - 1 || c == 0 || c == cols - 1);
                std::cout << (border ? '*' : ' ');
            }
            std::cout << '\n';
        }
    }

    void drawTriangle(double base, double height) {
        int cols, rows;
        scaleToConsole(base, height, cols, rows);
        if (cols % 2 == 0) ++cols;
        if (cols < 3) cols = 3;
        if (rows < 2) rows = 2;
        int half = cols / 2;
        for (int r = 0; r < rows; ++r) {
            int w = 1 + 2 * static_cast<int>(std::lround(static_cast<double>(half) * r / (rows - 1)));
            int pad = (cols - w) / 2;
            std::cout << "  ";
            for (int i = 0; i < pad; ++i) std::cout << ' ';
            for (int i = 0; i < w; ++i) std::cout << '*';
            std::cout << '\n';
        }
    }

}
