#include <string>
#include <fstream>

#include "image_utils.h"

namespace tchess
{
	//White pieces on light background
	std::array<std::vector<BYTE>, NUM_IMAGES> light_white_images;

	//Black pieces on light background.
	std::array<std::vector<BYTE>, NUM_IMAGES> light_black_images;

	//White pieces on dark background
	std::array<std::vector<BYTE>, NUM_IMAGES> dark_white_images;

	//White pieces on dark background
	std::array<std::vector<BYTE>, NUM_IMAGES> dark_black_images;

	//matches index to name
	std::vector<std::string> pieceNames = {"empty", "pawn", "knight", "bishop", "rook", "king", "queen"};

	//Loads image byte array into vector from the path
	void fillVectorWithImage(const std::string& path, std::vector<BYTE>& buffer) {
		// open the file:
		std::ifstream file(path, std::ios::binary);
		file.unsetf(std::ios::skipws);
		// get its size:
		std::streampos fileSize;
		file.seekg(0, std::ios::end);
		fileSize = file.tellg();
		file.seekg(0, std::ios::beg);
		// reserve capacity
		buffer.reserve(fileSize);
		// read the data:
		buffer.insert(buffer.begin(),
			std::istream_iterator<BYTE>(file),
			std::istream_iterator<BYTE>());
	}

	//all images are in the res folder
	static const std::string res = ".\\res\\";

	//Loads one background color + piece color combination
	void fillArrayWithImages(const std::string& background, const std::string& pieceColor, std::array<std::vector<BYTE>, NUM_IMAGES>& images) {
		for (size_t i = 0; i < NUM_IMAGES; ++i) {
			std::vector<BYTE> imageData;
			if (i == 0) { //empty image
				fillVectorWithImage(res + background + "_" + pieceNames[0] + ".png", imageData);
			}
			else { //image containing a piece
				fillVectorWithImage(res + background + "_" + pieceColor + "_" + pieceNames[i] + ".png", imageData);
			}
			images[i] = imageData; //save data
		}
	}

	void loadChessImages()
	{
		//load empty + white pieces on light background
		fillArrayWithImages("light", "white", light_white_images);
		//load empty + black pieces on light background
		fillArrayWithImages("light", "black", light_black_images);
		//load empty + white pieces on dark background
		fillArrayWithImages("dark", "white", dark_white_images);
		//load empty + black pieces on dark background
		fillArrayWithImages("dark", "black", dark_black_images);
	}
}
