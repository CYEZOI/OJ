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
