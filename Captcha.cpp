#include "Captcha.hpp"
#include <string.h>
CAPTCHA::CAPTCHA()
{
    srand(time(NULL));
    ImageBuffer = new unsigned char[WIDTH * HEIGHT * 3];
}
CAPTCHA::~CAPTCHA()
{
    delete[] ImageBuffer;
}
void CAPTCHA::Draw(int x, int y)
{
    int Index = x * WIDTH + y;
    ImageBuffer[Index * 3] -= DELTA;
    ImageBuffer[Index * 3 + 1] -= DELTA;
    ImageBuffer[Index * 3 + 2] -= DELTA;
}
void CAPTCHA::Invert(int x, int y)
{
    int Index = x * WIDTH + y;
    ImageBuffer[Index * 3] = 255 - ImageBuffer[Index * 3];
    ImageBuffer[Index * 3 + 1] = 255 - ImageBuffer[Index * 3 + 1];
    ImageBuffer[Index * 3 + 2] = 255 - ImageBuffer[Index * 3 + 2];
}
void CAPTCHA::DrawRadomBackground()
{
    for (int i = 0; i < WIDTH * HEIGHT * 3; i++)
        ImageBuffer[i] = rand() % (256 - DELTA - WHITENESS) + DELTA + WHITENESS;
}
void CAPTCHA::CreateRandomText()
{
    for (auto i : Numbers)
        Text.push_back(i.first);
    std::random_shuffle(Text.begin(), Text.end());
    Text = Text.substr(0, TEXT_LENGTH);
}
void CAPTCHA::DrawRandomText()
{
    for (int i = 0; i < Text.size(); i++)
        for (int x = 0; x < 5; x++)
            for (int y = 0; y < 3; y++)
                if (Numbers[Text[i]][x * 3 + y])
                    for (int h = 0; h < PIXEL_SIZE; h++)
                        for (int w = 0; w < PIXEL_SIZE; w++)
                            Draw((HEIGHT - PIXEL_SIZE * 5) / 2 + x * PIXEL_SIZE + h,
                                 (WIDTH - TEXT_LENGTH * 3 * PIXEL_SIZE - (TEXT_LENGTH - 1) * SPACE) / 2 + i * (3 * PIXEL_SIZE + SPACE) + y * PIXEL_SIZE + w);
}
void CAPTCHA::DrawRandomLines()
{
    for (int x = 0; x < HEIGHT; x++)
        if (rand() % HEIGHT <= LINES_BLACK * HEIGHT)
            for (int i = 0; i < WIDTH; i++)
                Draw(x, i);
    for (int y = 0; y < WIDTH; y++)
        if (rand() % WIDTH <= LINES_BLACK * WIDTH)
            for (int i = 0; i < HEIGHT; i++)
                Draw(i, y);
}
void CAPTCHA::InvertRandomLines()
{
    for (int x = 0; x < HEIGHT; x++)
        if (rand() % HEIGHT <= LINES_INVERT * HEIGHT)
            for (int i = 0; i < WIDTH; i++)
                Invert(x, i);
    for (int y = 0; y < WIDTH; y++)
        if (rand() % WIDTH <= LINES_INVERT * WIDTH)
            for (int i = 0; i < HEIGHT; i++)
                Invert(i, y);
}
void CAPTCHA::Create()
{
    DrawRadomBackground();
    CreateRandomText();
    DrawRandomText();
    DrawRandomLines();
    InvertRandomLines();

    FILE *file = fopen("captcha.jpg", "wb");
    if (file == NULL)
    {
        std::cerr << "Failed to open file\n";
        return;
    }

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, file);
    // unsigned char *outbuffer = NULL;
    // size_t outsize;
    // jpeg_mem_dest(&cinfo, &outbuffer, &outsize);
    // std::cout << outsize << std::endl;
    // std::cout << outbuffer << std::endl;
    // fwrite(outbuffer, 1, outsize, file);

    cinfo.image_width = WIDTH;
    cinfo.image_height = HEIGHT;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);

    jpeg_start_compress(&cinfo, TRUE);

    JSAMPROW row_pointer[1];
    while (cinfo.next_scanline < cinfo.image_height)
    {
        row_pointer[0] = &ImageBuffer[cinfo.next_scanline * WIDTH * 3];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    fclose(file);
}

int main()
{
    CAPTCHA Cap;
    Cap.Create();
    return 0;
}
