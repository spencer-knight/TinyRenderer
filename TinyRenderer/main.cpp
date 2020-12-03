#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include <cmath>
#include <vector>
#include <iostream>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green = TGAColor(0, 255, 0, 255);
Model* model = NULL;
const int width = 300;
const int height = 300;

void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {

	bool steep = false;
	if (std::abs(y1 - y0) > std::abs(x1 - x0)) {

		std::swap(x0, y0);
		std::swap(x1, y1);//not copied just learned
		steep = true;
	}
	if (x0 > x1) {
		int temp = x0;
		x0 = x1;
		x1 = temp;

		temp = y0;
		y0 = y1;
		y1 = temp;
	}

	int deltaY = y1 - y0;
	int deltaX = x1 - x0;
	float slope = (float)deltaY / deltaX;

	for (int x = x0; x <= x1; x++) {

		int y = slope * (x - x0) + y0;
		if (steep)
			image.set(y, x, color);
		else
			image.set(x, y, color);
	}

}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color) {
	// for testing
	line(t0.x, t0.y, t1.x, t1.y, image, color);
	line(t1.x, t1.y, t2.x, t2.y, image, color);
	line(t2.x, t2.y, t0.x, t0.y, image, color);
	if (t0.y < t1.y) {
		std::swap(t0, t1);
	}
	if (t1.y < t2.y) {
		std::swap(t1, t2);
	}
	if (t0.y < t1.y) {
		std::swap(t0, t1);
	}

	//line(t1.x, t1.y, t2.x, t2.y, image, red);
	//Boundary a x 
	//float newX;
	//newX = (float)(t0.x - t2.x) / (t0.y - t2.y) * (t1.y - t2.y) + t2.x;
	//line(t2.x, t2.y, newX, t1.y, image, green);
	float alphaSlope = (float)(t0.x - t2.x)/(t0.y - t2.y + 1);
	float betaSlope = (float)(t1.x - t2.x) / (t1.y - t2.y + 1);
	int newx = 0;
	for (int y = t2.y; y <= t1.y; y++) {

		int deltaY = (y - t2.y);
		int alphaX = alphaSlope * deltaY + t2.x;
		int betaX = betaSlope * deltaY + t2.x;
		newx = alphaX;
		image.set(alphaX, y, color);
		image.set(betaX, y, color);
		if (betaX > alphaX)
			std::swap(betaX, alphaX);
		for (int x = betaX; x < alphaX; x++) {
			
			image.set(x, y, color);
		}
	}
	betaSlope = (float)(t0.x - t1.x) / (t0.y - t1.y + 1);
	for (int y = t1.y; y <= t0.y; y++) {

		int deltaY = (y - t1.y);
		int alphaX = alphaSlope * deltaY + newx;
		int betaX = betaSlope * deltaY + t1.x;
		image.set(alphaX, y, color);
		image.set(betaX, y, color);
		if (betaX > alphaX)
			std::swap(betaX, alphaX);
		for (int x = betaX; x <= alphaX; x++) {

			image.set(x, y, color);
		}
	}
}

int main(int argc, char** argv) {
	if (2 == argc) {
		model = new Model(argv[1]);
	}
	else {
		model = new Model("obj/african_head.obj");
	}

	TGAImage image(width, height, TGAImage::RGB);
	//#######################################################################

	for (int i = 0; i < model->nfaces(); i++) {
		std::vector<int> face = model->face(i);
		Vec2i screen_coords[3];
		for (int j = 0; j < 3; j++) {
			Vec3f world_coords = model->vert(face[j]);
			screen_coords[j] = Vec2i((world_coords.x + 1.) * width / 2., (world_coords.y + 1.) * height / 2.);
		}
		triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(rand() % 255, rand() % 255, rand() % 255, 255));
	}





	//########################################################################
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	delete model;
	return 0;
}

