#pragma once

/*
* Methods related to reading the chess images and storing them.
* 
* Images are stores in BYTE vectors, and those are collected into arrays.
*/

#include <array>
#include <vector>

#define NUM_IMAGES 7 //6 pieces and an empty square

namespace tchess
{
	typedef unsigned char BYTE;

	//White pieces on light background
	extern std::array<std::vector<BYTE>, NUM_IMAGES> light_white_images;

	//Black pieces on light background.
	extern std::array<std::vector<BYTE>, NUM_IMAGES> light_black_images;

	//White pieces on dark background
	extern std::array<std::vector<BYTE>, NUM_IMAGES> dark_white_images;

	//White pieces on dark background
	extern std::array<std::vector<BYTE>, NUM_IMAGES> dark_black_images;

	//Loads chess related images into the vectors.
	void loadChessImages();
}