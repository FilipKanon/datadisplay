#include <iostream>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <cstdint>
#include <bitset>
#include <random>

#define ROW_SIZE 8
#define COLUMN_SIZE 8

struct fb_t {
    uint16_t pixel[ROW_SIZE][COLUMN_SIZE];
};

/*
* Clears the led matrix
*/
void clear_display(struct fb_t *fb){
    memset(fb, 0, 128);
}

uint16_t from_rgb_to_565(uint16_t r, uint16_t g, uint16_t b){
	/*std::bitset<8> xr(r);
	std::bitset<8> xg(g);
	std::bitset<8> xb(b);
	std::cout << "Red: \t" << xr << "\n";
	std::cout << "Green: \t" << xg << "\n";
	std::cout << "Blue: \t" << xb << "\n";*/
	uint16_t rgb = 0;
	
	rgb = ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3);

	//std::bitset<16> xrgb(rgb);
	//std::cout << "RGB: \t" << xrgb << "\n";
	
	return rgb;
}

/**
* Fills the display with a single color
*/
void fill(struct fb_t *fb, uint16_t color){
	for(int r = 0; r < ROW_SIZE; r++){
		for(int c = 0; c < COLUMN_SIZE; c++){
			fb->pixel[r][c] = color;
		}
	}
}

void set_random(struct fb_t *fb, uint16_t color){
	std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> row_dist(0,ROW_SIZE-1);
	std::uniform_int_distribution<std::mt19937::result_type> column_dist(0,COLUMN_SIZE-1);

	int r = row_dist(rng);
	int c = column_dist(rng);

	//std::cout << "(" << r << ", " << c << ")\n";

	// Set a random row and random column to the color.
	fb->pixel[r][c] = color;
}

void set_n_random(struct fb_t *fb, uint16_t color, uint16_t n){
	for(int i = 0; i < n; i++){
		set_random(fb, color);
	}
}

int main() {
    int fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd < 0) {
        perror("open");
        return 1;
    }

	// Create a framebuffer
    struct fb_t *fb = (fb_t*) mmap(0, 128, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);

    if (fb == MAP_FAILED) {
		perror("mmap");
		return 1;
	}

    // Clear screen
   	clear_display(fb);

	// blue 0x001F; 
	// red 0xF800;
	// green 0x07E0;
	// white 0xFFFF;

	uint16_t rgb = from_rgb_to_565(125, 125, 0);
	fill(fb, rgb);
	set_n_random(fb, from_rgb_to_565(125, 0, 0), 10);

    // Light some pixels (RGB565 format)
    //fb->pixel[0][0] = 0xF800; // red
    //fb->pixel[1][1] = 0x07E0 + 0xF800; // yellow
    //fb->pixel[2][2] = 0x001F; // blue

    //sleep(6);

    //clear_display(fb);

    munmap(fb, 128);
    close(fbfd);

    return 0;
}