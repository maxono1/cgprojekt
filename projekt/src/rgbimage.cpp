#include "rgbimage.h"
#include "color.h"
#include "assert.h"
#include "stdio.h"
#include "Matrix.h"
#include <algorithm>


RGBImage::RGBImage(unsigned int Width, unsigned Height)
{
	m_Width = Width;
	m_Height = Height;

	m_Image = new Color[Width * Height];
}

RGBImage::~RGBImage() {
	delete m_Image;
}

void RGBImage::setPixelColor(unsigned int x, unsigned int y, const Color& c) {
	if (x < 0 || x > width()) {
		if (y < 0 || y > height()) {
			throw std::invalid_argument("Pixelkoordinaten ausserhalb des gueltigen Bereichs.");
		}
	}
	m_Image[x + y * m_Width] = c;

}

const Color& RGBImage::getPixelColor(unsigned int x, unsigned int y) const {
	return m_Image[x + y * m_Width];
}

unsigned int RGBImage::width() const {
	return m_Width;
}
unsigned int RGBImage::height() const {
	return m_Height;
}

unsigned char RGBImage::convertColorChannel(float v) {
	/*
	char catchRound = std::max(0.0f, std::min(1.0f, v));
	// * 256 um fehler zu vermeiden
	if (catchRound == 1.0) {
		return 255;
	} else {
		return catchRound * 256;
	}*/

	if (v < 0.0f)
		v = 0.0f;
	if (v > 1.0f)
		v = 1.0f;

	return v * 255;
}

//https://www.cplusplus.com/reference/cstdio/fopen/
//quelle: https://stackoverflow.com/questions/2654480/writing-bmp-image-in-pure-c-c-without-other-libraries

bool RGBImage::saveToDisk(const char* Filename) {
	//
	FILE* f;
	unsigned char* img = NULL;
	int filesize = 54 + 3 * m_Width * m_Height;  //w is your image width, h is image height, both int

	img = (unsigned char*)malloc(3 * m_Width * m_Height);
	memset(img, 0, 3 * m_Width * m_Height);

	//in dieser schleife werden die floats in rbg umgerechnet
	for (int i = 0; i < m_Width; i++) {
		for (int j = 0; j < m_Height; j++) {
			int x = i; int y = (m_Height - 1) - j;
			unsigned char r = convertColorChannel(m_Image[i + j * m_Width].R);
			unsigned char g = convertColorChannel(m_Image[i + j * m_Width].G);
			unsigned char b = convertColorChannel(m_Image[i + j * m_Width].B);
			if (r > 255) {
				r = 255;
			}
			if (g > 255) {
				g = 255;
			}
			if (b > 255) {
				b = 255;
			}
			img[(x + y * m_Width) * 3 + 2] = (unsigned char)(r);
			img[(x + y * m_Width) * 3 + 1] = (unsigned char)(g);
			img[(x + y * m_Width) * 3 + 0] = (unsigned char)(b);
		}
	}

	unsigned char bmpfileheader[14] = { 'B', 'M', 0, 0, 0, 0, 0, 0, 0, 0, 54, 0, 0, 0 };
	unsigned char bmpinfoheader[40] = { 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 24, 0 };
	unsigned char bmppad[3] = { 0, 0, 0 };

	bmpfileheader[2] = (unsigned char)(filesize);
	bmpfileheader[3] = (unsigned char)(filesize >> 8);
	bmpfileheader[4] = (unsigned char)(filesize >> 16);
	bmpfileheader[5] = (unsigned char)(filesize >> 24);

	bmpinfoheader[4] = (unsigned char)(m_Width);
	bmpinfoheader[5] = (unsigned char)(m_Width >> 8);
	bmpinfoheader[6] = (unsigned char)(m_Width >> 16);
	bmpinfoheader[7] = (unsigned char)(m_Width >> 24);
	//*-1 weil wir eine topdown bitmap haben
	bmpinfoheader[8] = (unsigned char)(m_Height * -1);
	bmpinfoheader[9] = (unsigned char)(m_Height * -1 >> 8);
	bmpinfoheader[10] = (unsigned char)(m_Height * -1 >> 16);
	bmpinfoheader[11] = (unsigned char)(m_Height * -1 >> 24);

	f = fopen(Filename, "wb");
	fwrite(bmpfileheader, 1, 14, f);
	fwrite(bmpinfoheader, 1, 40, f);
	for (int i = 0; i < m_Height; i++) {
		fwrite(img + (m_Width * (m_Height - i - 1) * 3), 3, m_Width, f);
		fwrite(bmppad, 1, (4 - (m_Width * 3) % 4) % 4, f);
	}

	free(img);
	fclose(f);

	return true;
}

RGBImage& RGBImage::SobelFilter(RGBImage& dst, const RGBImage& src, float factor = 1.0f) {
	assert((dst.height() == src.height()) && dst.width() == src.width());
	Matrix k = Matrix(1, 0, -1, 0,
		2, 0, -2, 0,
		1, 0, -1, 0,
		0, 0, 0, 1);
	Matrix kT = Matrix(k);
	kT.transpose();

	//const int kLength = 3;


	//Matrix u = k * src.;
	//new benutzen?
	RGBImage u = RGBImage(src.width(), src.height());
	RGBImage v = RGBImage(src.width(), src.height());
	for (size_t x = 0; x < src.width(); x++)
	{
		for (size_t y = 0; y < src.height(); y++)
		{
			u.setPixelColor(x, y, Color(0, 0, 0));
			v.setPixelColor(x, y, Color(0, 0, 0));
		}
	}
	for (int x = 0; x < src.width(); x++)
	{
		for (int y = 0; y < src.height(); y++)
		{
			for (int i = 0; i < 3; i++)
			{
				for (int j = 0; j < 3; j++)
				{
					if (((x - i - 1) >= 0) && ((y - j - 1) >= 0)) {
						Color beforeU = u.getPixelColor(x, y);
						Color beforeV = v.getPixelColor(x, y);
						u.setPixelColor(x, y, beforeU + src.getPixelColor(x - i - 1, y - j - 1) * k.m[i + j * 4] * factor);
						v.setPixelColor(x, y, beforeV + src.getPixelColor(x - i - 1, y - j - 1) * kT.m[i + j * 4] * factor);
					}
				}
			}
		}

	}
	for (size_t x = 0; x < src.width(); x++)
	{
		for (size_t y = 0; y < src.height(); y++)
		{
			//geht nur bei grayscale!!!
			float color = std::sqrt(u.getPixelColor(x, y).R * u.getPixelColor(x, y).R + v.getPixelColor(x, y).R * v.getPixelColor(x, y).R);
			dst.setPixelColor(x, y, Color(color, color, color));
		}
	}



	return dst;

	/*
	Matrix K(1, 0, -1, 0,
		2, 0, -2, 0,
		1, 0, -1, 0,
		0, 0, 0, 0);

	Matrix KT = K.transpose();


	for (int y = 3; y < dst.height(); ++y) {
		for (int x = 3; x < dst.width(); ++x) {

			float u = 0, v = 0;

			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {

					u = u + src.getPixelColor(x - i - 1, y - j - 1).R * K.m[i + j * 4] * factor;
					v = v + src.getPixelColor(x - i - 1, y - j - 1).R * KT.m[j + i * 4] * factor;
				}

			}
			float color = std::sqrt(u * u + v * v);
			dst.setPixelColor(x, y, Color(color, color, color));
		}
	}

	return dst;*/
}

/*
	auto k = new float[kLength][kLength];
	auto kT = new float[kLength][kLength];
	k[0][0] = 1;
	k[1][0] = 0;
	k[2][0] = -1;
	k[0][1] = 2;
	k[0][2] = 1;
	k[1][1] = 0;
	k[2][1] = -2;
	k[1][2] = 0;
	k[2][2] = -1;

	kT[0][0] = 1;
	kT[0][1] = 0;
	kT[0][2] = -1;
	kT[1][0] = 2;
	kT[1][1] = 0;
	kT[1][2] = -2;
	kT[2][0] = 1;
	kT[2][1] = 0;
	kT[2][2] = -1;

	for (size_t i = 0; i < kLength; i++)
	{
		delete[] k[i];
		delete[] kT[i];
	}
	delete[] k;
	delete[] kT;*/