# Image Processing (BMP)

Small C console program that loads 8-bit and 24-bit BMP images, applies common filters, and performs histogram equalization for contrast correction. Everything runs via interactive prompts in the terminal.

## Features
- 8-bit BMP workflow: open/save, print image metadata, filters (negative, brightness, threshold to B/W, box blur, Gaussian blur, sharpen, outline, emboss), histogram equalization.
- 24-bit BMP workflow: open/save, filters (negative, grayscale, brightness, box blur, Gaussian blur, sharpen, outline, emboss), histogram equalization on luminance (YUV) to keep colors consistent.
- Uses simple convolution kernels for blur/sharpen/edge/emboss effects.
- Sample assets included: `barbara_gray.bmp` (8-bit) and `flowers_color.bmp` (24-bit).

## Build
Requires a C compiler and CMake (tested with C11).

```bash
mkdir -p build
cd build
cmake ..
cmake --build .
./CProject
```

The `CProject` executable is defined in `CMakeLists.txt` and links `main.c`, `bmp8/`, `bmp24/`, and `histogram/`.

## Usage
The executable runs in an interactive loop:
1. Choose image format: `1` for 8-bit grayscale BMP, `2` for 24-bit color BMP.
2. Use menu options to open an image (provide a BMP path), apply filters, run histogram equalization, and save the result to a new path.
3. 8-bit only: you can also print basic image info from the header.

Filters:
- 8-bit: negative, brightness (integer delta), threshold to B/W, box blur, Gaussian blur, sharpen, outline, emboss.
- 24-bit: negative, grayscale, brightness, box blur, Gaussian blur, sharpen, outline, emboss.
- Histogram equalization: option `5` in the 8-bit menu, option `4` in the 24-bit menu.

## Notes and limitations
- Only uncompressed BMP files are supported (8-bit indexed or 24-bit true color).
- Paths are read from stdin; ensure they exist before loading and that the output path is writable.
- Image buffers are kept in memory; large images may increase memory use.

