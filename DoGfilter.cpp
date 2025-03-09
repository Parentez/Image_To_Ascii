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

    Vector2u imageSize = i.getSize();
    int xSize = imageSize.x;
    int ySize = imageSize.y;

    Color color1;
    Color color2;
    int grayScale = 0;

    string asciiChars = " .:-=+*%#@";
    int nAscii = asciiChars.length();

    // max x value for the terminal 
    
    float fixedWidth = 300.0f;
    float iRatio = fixedWidth / xSize; 
    xSize = (int) fixedWidth;
    ySize = (int) ySize * iRatio;
   
    
    for (int y = 0; y < ySize; y++) {
        for (int x = 0; x < xSize ; x++) {
            int oX = (int)(x / iRatio);
            int oY = (int)(y / iRatio);
            color1 = i.getPixel(oX, oY);
            //color2 = dogImage.getPixel(oX, oY);

            grayScale = color1.r * 0.299 + color1.g * 0.587 + color1.b * 0.114;
            
            //dogImage.setPixel(x, y, color);
            
            int mappedIndex = (grayScale * (nAscii - 1) )/ 256.0 ;
            cout << asciiChars[mappedIndex];
        }
        cout << endl;
    }
    

    Texture texture;
    texture.loadFromImage(dogImage);
    Sprite sprite(texture);
    RenderWindow window(VideoMode(texture.getSize().x, texture.getSize().y), "Blurred image");

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