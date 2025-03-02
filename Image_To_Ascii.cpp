#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

int main() {
    Image i;

    if (i.loadFromFile("C:\\Users\\utente\\Downloads\\WhatsApp Image 2025-02-08 at 13.18.40.jpeg")) {
        cout << "Image loaded successfully." << endl;
    }
    else {
        cout << "Error: couldn't open the image" << endl;
        return 1;
    }

    Vector2u imageSize = i.getSize();
    int xSize = imageSize.x;
    int ySize = imageSize.y;

    int maxWidth = 300;
    float aspectRatio = static_cast<float>(ySize) / static_cast<float> (xSize);
    int newWidth = min(maxWidth, xSize);
    int newHeight = static_cast<int>(newWidth * aspectRatio);

    RenderTexture renderTexture;
    renderTexture.create(newWidth, newHeight);

    Texture texture;
    texture.loadFromImage(i);
    Sprite sprite(texture);

    sprite.setScale(static_cast<float>(newWidth) / xSize, static_cast<float>(newHeight) / ySize);
    renderTexture.draw(sprite);
    renderTexture.display();

    Image resizedImage = renderTexture.getTexture().copyToImage();
    xSize = resizedImage.getSize().x;
    ySize = resizedImage.getSize().y;

    Color color;
    int grayScale = 0;

    string asciiChars = " .:-=+*#%@";
    int nAscii = asciiChars.length();

    for (int y = 0; y < ySize; y++) {
        for (int x = 0; x < xSize; x++) {
            color = resizedImage.getPixel(x, y);
            grayScale = color.r * 0.299 + color.g * 0.587 + color.b * 0.114;

            int mappedIndex = (grayScale * (nAscii - 1) )/ 256.0 ;

            cout << asciiChars[mappedIndex];
        }
        cout << endl;
    }

    return 0;
}