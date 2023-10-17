#include <immintrin.h>
#include <stdint.h>
#include <stdio.h>

#define INPUT_WIDTH 1200
#define INPUT_HEIGHT 1200
#define OUTPUT_WIDTH 600
#define OUTPUT_HEIGHT 600
#define BMP_HEADER_SIZE 54
#define BITS_PER_PIXEL 8  // Assuming 8-bit grayscale

void resizeImage(const uint8_t* input, uint8_t* output) {
    for (int y = 0; y < OUTPUT_HEIGHT; y++) {
        for (int x = 0; x < OUTPUT_WIDTH; x++) {
            __m256i sum = _mm256_setzero_si256();

            // Average downsample 2x2 pixels
            for (int dy = 0; dy < 2; dy++) {
                for (int dx = 0; dx < 2; dx++) {
                    int inputX = x * 2 + dx;
                    int inputY = y * 2 + dy;

                    // Load 8-bit pixel values into AVX2 register
                    __m256i pixel = _mm256_set1_epi8(input[inputY * INPUT_WIDTH + inputX]);

                    // Add pixel values to the sum
                    sum = _mm256_add_epi8(sum, pixel);
                }
            }

            // Average the sum
            sum = _mm256_srli_epi16(sum, 2);

            // Store the result in the output image
            output[y * OUTPUT_WIDTH + x] = _mm256_extract_epi8(sum, 0);
        }
    }
}
uint8_t* loadBMP(const char* filename, int* width, int* height) {
    FILE* file = fopen(filename, "");

    // Read BMP header
    uint8_t bmpHeader[BMP_HEADER_SIZE];
    if (fread(bmpHeader, 1, BMP_HEADER_SIZE, file) != BMP_HEADER_SIZE) {
        fclose(file);
        return NULL;
    }

    *width = *(int*)&bmpHeader[18];
    *height = *(int*)&bmpHeader[22];
    int imageSize = *width * *height;
    
    // Verify BMP format (8-bit grayscale)
    if (*(uint16_t*)&bmpHeader[28] != BITS_PER_PIXEL) {
        fclose(file);
        return NULL;
    }

    // Allocate memory for the grayscale image
    uint8_t* imageData = (uint8_t*)malloc(imageSize);
    
    // Read image data
    if (fread(imageData, 1, imageSize, file) != imageSize) {
        free(imageData);
        fclose(file);
        return NULL;
    }

    fclose(file);
    return imageData;
}

int main() {
    // Load the input image (not shown)
    // Allocate memory for the output image

    // Assuming you have loaded the input image data into 'input' and
    // allocated memory for the 'output' image.

    resizeImage(input, output);
    stbi_write_bmp("output.bmp", OUTPUT_WIDTH, OUTPUT_HEIGHT, 1, output);

    // Save the output image (not shown)

    return 0;
}
