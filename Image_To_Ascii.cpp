#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace std;
using namespace sf;

vector<vector<float>> GenerateGaussianKernel(int size, float sigma);
Image GaussianBlur(Image& image, int kernelsize, float sigma);
Uint8 toGrayscale(const Color& color);
Image GaussianFilterDOG(Image& image, int kernelSize, float sigma1, float sigma2, bool binary, int threshold);
Image ResizeImage(Image& image, int resizeFactor);

int main() {
     
    Image i;

    if (i.loadFromFile("C:\\Users\\utente\\OneDrive\\Desktop\\Superliminal\\nswitchds_superliminal_01.jpg")) {
        cout << "Image loaded successfully." << endl;
    }
    else {
        cout << "Error: couldn't open the image" << endl;
        return 1;
    }


    int kernelSize = 5;
    float sigma1 = 1.0f;
    float sigma2 = 2.0;
    bool binary = true;
    int threshold = 126;

    Image dogImage = GaussianFilterDOG(i, kernelSize, sigma1, sigma2, binary, threshold);
    //Image blurredI = GaussianBlur(i, 5, 250.0f);

    Vector2u imageSize = i.getSize();
    int xSize = imageSize.x;
    int ySize = imageSize.y;

    Color color1;
    Color color2;
    int grayScale = 0;

    string asciiChars = " .:-=+*%#@";
    int nAscii = asciiChars.length();

    Image resizedImage = ResizeImage(i, 8);

    // max x value for the terminal 
    float fixedWidth = 300.0f;
    float iRatio = fixedWidth / xSize; 
    xSize = (int) fixedWidth;
    ySize = (int) ySize * iRatio;
   
    vector<vector<float>> xKernel = { {1.0f, 0.0f, -1.0f}, {2.0f, 0.0f, -2.0f}, {1.0f, 0.0f, -1.0f} };
    vector<vector<float>> yKernel = { {1.0f, 2.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {-1.0f, -2.0f, -1.0f} };

    Image edgedI;
    edgedI.create(imageSize.x, imageSize.y, Color::Black);

    for (int y = 1; y < ySize - 1; y++) {
        for (int x = 1; x < xSize - 1; x++) {
            int oX = (int)(x / iRatio);
            int oY = (int)(y /  iRatio);
            color1 = resizedImage.getPixel(oX, oY);
                       
            int pixel_x = 0, pixel_y = 0;

            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    Color pixel = resizedImage.getPixel((int)(x + i) / iRatio,(int) (y + j) / iRatio);
                    grayScale = pixel.r * 0.299 + pixel.g * 0.587 + pixel.b * 0.114;
                    pixel_x += xKernel[i + 1][j + 1] * grayScale;
                    pixel_y += yKernel[i + 1][j + 1] * grayScale;
                }
            }

            int magn = ceil(sqrt(pixel_x * pixel_x + pixel_y * pixel_y));
            magn = min(255, max(0, magn));

            edgedI.setPixel(oX , oY, Color(magn, magn, magn));

            if (magn < 50) {
                int mappedIndex = (grayScale * (nAscii - 1)) / 256.0;
                cout << asciiChars[mappedIndex];
                continue;
            }

            float angle = atan2(pixel_x, pixel_y) * 180.0 / M_PI;
            if (angle < 0) angle += 360;

            char asciiChar;
            if ((angle >= 0 && angle < 22.5) || (angle >= 157.5 && angle < 202.5) || (angle >= 337.5 && angle <= 360)) {
                asciiChar = '|';
            }
            else if ((angle >= 67.5 && angle < 112.5) || (angle >= 247.5 && angle < 292.5)) {
                asciiChar = '_';
            }
            else if ((angle >= 22.5 && angle < 67.5) || (angle >= 202.5 && angle < 247.5)) {
                asciiChar = '/';
            }
            else {
                asciiChar = '\\';
            }

            cout << asciiChar;
        }
        cout << endl;
    }

  
    Texture texture;
    texture.loadFromImage(edgedI);
    //texture.loadFromImage(blurredI);
    Sprite sprite(texture);
    RenderWindow window(VideoMode(edgedI.getSize().x, edgedI.getSize().y), "Image");

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
        }
        window.clear();
        window.draw(sprite);
        window.display();
    }
    return 0;
}

vector<vector<float>> GenerateGaussianKernel(int size, float sigma) {
    vector<vector<float>> kernel(size, vector<float>(size));
    float sum = 0.0;
    int halfSize = size / 2;

    for (int y = -halfSize; y <= halfSize; y++) {
        for (int x = -halfSize; x <= halfSize; x++) {
            float value = exp(-(x * x + y * y) / (2 * sigma * sigma)) / (2 * M_PI * sigma * sigma);
            kernel[y + halfSize][x + halfSize] = value;
            sum += value;
        }
    }

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            kernel[y][x] /= sum;
        }
    }

    return kernel;
}

Image GaussianBlur(Image& image, int kernelsize, float sigma) {
    Image blurredImage = image;
    vector<vector<float>> kernel = GenerateGaussianKernel(kernelsize, sigma);

    Vector2u size = image.getSize();
    Image tempImage = image;

    int halfSize = kernelsize / 2;

    for (unsigned int y = halfSize; y < size.y - halfSize; y++) {
        for (unsigned int x = halfSize; x < size.x - halfSize; x++) {
            float r = 0, g = 0, b = 0;
            float weightSum = 0;

            for (int ky = -halfSize; ky <= halfSize; ky++) {
                for (int kx = -halfSize; kx <= halfSize; kx++) {
                    Color pixel = tempImage.getPixel(x + kx, y + ky);
                    float weight = kernel[ky + halfSize][kx + halfSize];

                    r += pixel.r * weight;
                    g += pixel.g * weight;
                    b += pixel.b * weight;
                    weightSum += weight;
                }
            }

            blurredImage.setPixel(x, y, Color(static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b)));
        }
    }

    return blurredImage;
}

Uint8 toGrayscale(const Color& color) {
    return static_cast<Uint8>(0.299 * color.r + 0.587 * color.g + 0.114 * color.b);
}

Image GaussianFilterDOG(Image& image, int kernelSize, float sigma1, float sigma2, bool binary, int threshold) {
    Image blurred1 = GaussianBlur(image, kernelSize, sigma1);
    Image blurred2 = GaussianBlur(image, kernelSize, sigma2);

    Image result = image;
    Vector2u size = image.getSize();

    for (unsigned int y = 0; y < size.y; y++) {
        for (unsigned int x = 0; x < size.x; x++) {
            Uint8 gray1 = toGrayscale(blurred1.getPixel(x, y));
            Uint8 gray2 = toGrayscale(blurred2.getPixel(x, y));

            int diff = static_cast<int>(gray1) - static_cast<int>(gray2) + 129;

            diff = max(0, min(255, diff));

            if (binary) {
                diff = (diff > threshold) ? 255 : 0;
            }


            result.setPixel(x, y, Color(diff, diff, diff));
        }
    }

    return result;
}

Image ResizeImage(Image& image, int resizeFactor) {

    Vector2u originalsize = image.getSize();
    int xreSized = originalsize.x / resizeFactor;
    int yreSized = originalsize.y / resizeFactor;

    Texture texture;
    texture.loadFromImage(image);

    Sprite sprite(texture);
    sprite.setScale(static_cast<float>(xreSized) / originalsize.x, static_cast<float>(yreSized) / originalsize.y);
    
    RenderTexture resizeTex;
    resizeTex.create(xreSized, yreSized);
    resizeTex.clear(Color::Transparent);
    resizeTex.draw(sprite);
    resizeTex.display();

    Image resizedImage = resizeTex.getTexture().copyToImage();

    RenderTexture outputTex;
    outputTex.create(originalsize.x, originalsize.y);
    outputTex.clear(Color::Transparent);

    Texture finalTex;
    finalTex.loadFromImage(resizedImage);
    Sprite finalSprite(finalTex);
    finalSprite.setScale(static_cast<float>(originalsize.x) / xreSized, static_cast<float>(originalsize.y) / yreSized);

    outputTex.draw(finalSprite);
    outputTex.display();

    Image result = outputTex.getTexture().copyToImage();

    return result;
}
