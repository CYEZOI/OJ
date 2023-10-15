/**********************************************************************
OJ: An online judge server written with only C++ and MySQL.
Copyright (C) 2023  langningchen

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
**********************************************************************/

#include <iostream>
#include <fstream>
#include <map>
#include <random>
#include <algorithm>
#include <jpeglib.h>
using namespace std;
class CAPTCHA
{
private:
    const int WIDTH = 200;
    const int HEIGHT = 100;
    const int TEXT_LENGTH = 4;
    const int PIXEL_SIZE = 12;
    const int DELTA = 64;
    const int SPACE = 6;
    const int TABLE_SIZE = 30;
    const float LINES_BLACK = 0.2;
    const float LINES_INVERT = 0.1;
    const int WHITENESS = 64;
    unsigned char *ImageBuffer;
    std::string Text;
    map<char, std::vector<bool>> Numbers = {
        {'0', {1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1}},
        {'1', {0, 0, 1, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1}},
        {'2', {1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1}},
        {'3', {1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1}},
        {'4', {1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1}},
        {'5', {1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1}},
        {'6', {1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1}},
        {'7', {1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1}},
        {'8', {1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1}},
        {'9', {1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1}}};
    void Draw(int x, int y);
    void Invert(int x, int y);
    void DrawRadomBackground();
    void CreateRandomText();
    void DrawRandomText();
    void DrawRandomLines();
    void InvertRandomLines();

public:
    CAPTCHA();
    ~CAPTCHA();
    void Create();
};
