#include "Images.h"

const uint8_t titleImageData[] = {
    // metadata
    36,   // frame width
    15,   // frame height
    1,    // frames (lower byte)
    0,    // frames (upper byte)
    0,    // frame loop
    0x0,  // transparent color
    1,    // indexed color mode

    // colormap
    0x55, 0x55, 0x00, 0x05, 0x55, 0x00, 0x05, 0x55, 0x00, 0x55, 0x05, 0x50, 0x05, 0x55, 0x05, 0x55, 0x55, 0x50,
    0x55, 0x05, 0x50, 0x55, 0x05, 0x50, 0x55, 0x05, 0x50, 0x55, 0x05, 0x50, 0x55, 0x55, 0x50, 0x55, 0x55, 0x50,
    0x55, 0x05, 0x50, 0x55, 0x05, 0x50, 0x55, 0x05, 0x50, 0x55, 0x05, 0x50, 0x55, 0x00, 0x00, 0x05, 0x50, 0x00,
    0x55, 0x05, 0x50, 0x55, 0x05, 0x50, 0x55, 0x00, 0x00, 0x55, 0x55, 0x00, 0x55, 0x55, 0x00, 0x05, 0x50, 0x00,
    0x55, 0x55, 0x00, 0x55, 0x05, 0x50, 0x55, 0x05, 0x50, 0x55, 0x05, 0x50, 0x55, 0x00, 0x00, 0x05, 0x50, 0x00,
    0x55, 0x00, 0x00, 0x55, 0x05, 0x50, 0x55, 0x05, 0x50, 0x55, 0x05, 0x50, 0x55, 0x55, 0x50, 0x05, 0x50, 0x00,
    0x55, 0x00, 0x00, 0x05, 0x55, 0x00, 0x05, 0x55, 0x00, 0x55, 0x05, 0x50, 0x05, 0x55, 0x00, 0x05, 0x50, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x05, 0x55, 0x00, 0x00, 0x50, 0x00, 0x55, 0x55, 0x00, 0x55, 0x55, 0x00, 0x05, 0x55, 0x00, 0x55, 0x55, 0x00,
    0x55, 0x00, 0x50, 0x05, 0x05, 0x00, 0x55, 0x05, 0x50, 0x55, 0x05, 0x50, 0x55, 0x55, 0x50, 0x55, 0x55, 0x50,
    0x55, 0x00, 0x00, 0x05, 0x05, 0x00, 0x55, 0x05, 0x50, 0x55, 0x05, 0x50, 0x55, 0x00, 0x00, 0x55, 0x05, 0x50,
    0x55, 0x05, 0x50, 0x55, 0x55, 0x50, 0x55, 0x55, 0x00, 0x55, 0x05, 0x50, 0x55, 0x55, 0x00, 0x55, 0x05, 0x50,
    0x55, 0x05, 0x50, 0x55, 0x05, 0x50, 0x55, 0x05, 0x50, 0x55, 0x05, 0x50, 0x55, 0x00, 0x00, 0x55, 0x05, 0x50,
    0x55, 0x00, 0x50, 0x55, 0x05, 0x50, 0x55, 0x05, 0x50, 0x55, 0x05, 0x50, 0x55, 0x55, 0x50, 0x55, 0x05, 0x50,
    0x05, 0x55, 0x00, 0x55, 0x05, 0x50, 0x55, 0x05, 0x50, 0x55, 0x55, 0x00, 0x05, 0x55, 0x00, 0x55, 0x05, 0x50
};

Image titleImage(titleImageData);

const uint8_t gameOverImageData[] = {

    // metadata

    25,   // frame width
    15,   // frame height
    0x01, // frames (lower byte)
    0x00, // frames (upper byte)
    0,    // frame loop
    0xff, // transparent color
    1,    // indexed color mode

    // colormap
    0x02, 0x22, 0x00, 0x00, 0x20, 0x00, 0x22, 0x22, 0x22, 0x00, 0x02, 0x22, 0x00,
    0x22, 0x00, 0x20, 0x02, 0x02, 0x00, 0x22, 0x02, 0x02, 0x20, 0x22, 0x22, 0x20,
    0x22, 0x00, 0x00, 0x02, 0x02, 0x00, 0x22, 0x02, 0x02, 0x20, 0x22, 0x00, 0x00,
    0x22, 0x02, 0x20, 0x22, 0x22, 0x20, 0x22, 0x02, 0x02, 0x20, 0x22, 0x22, 0x00,
    0x22, 0x02, 0x20, 0x22, 0x02, 0x20, 0x22, 0x02, 0x02, 0x20, 0x22, 0x00, 0x00,
    0x22, 0x00, 0x20, 0x22, 0x02, 0x20, 0x22, 0x02, 0x02, 0x20, 0x22, 0x22, 0x20,
    0x02, 0x22, 0x00, 0x22, 0x02, 0x20, 0x22, 0x02, 0x02, 0x20, 0x02, 0x22, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x22, 0x20, 0x02, 0x20, 0x22, 0x00, 0x22, 0x20, 0x02, 0x22, 0x20, 0x00,
    0x02, 0x20, 0x22, 0x02, 0x20, 0x22, 0x02, 0x22, 0x22, 0x02, 0x20, 0x22, 0x00,
    0x02, 0x20, 0x22, 0x02, 0x20, 0x22, 0x02, 0x20, 0x00, 0x02, 0x20, 0x22, 0x00,
    0x02, 0x20, 0x22, 0x02, 0x20, 0x22, 0x02, 0x22, 0x20, 0x02, 0x22, 0x20, 0x00,
    0x02, 0x20, 0x22, 0x00, 0x20, 0x20, 0x02, 0x20, 0x00, 0x02, 0x20, 0x22, 0x00,
    0x02, 0x20, 0x22, 0x00, 0x20, 0x20, 0x02, 0x22, 0x22, 0x02, 0x20, 0x22, 0x00,
    0x00, 0x22, 0x20, 0x00, 0x02, 0x00, 0x00, 0x22, 0x20, 0x02, 0x20, 0x22, 0x00
};

Image gameOverImage(gameOverImageData);