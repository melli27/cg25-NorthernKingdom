/*
* Copyright 2020 Vienna University of Technology.
* Institute of Computer Graphics and Algorithms.
* This file is part of the ECG Lab Framework and must not be redistributed.
*/
#pragma once


#include "INIReader.h"
#include <iostream>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <stb_image.h>


#define EXIT_WITH_ERROR(err) \
        { \
                glfwTerminate(); \
                std::cout << "ERROR: " << err << std::endl; \
                system("PAUSE"); \
                return EXIT_FAILURE; \
        }

/*!
 * A loaded '.dss' image
 */
class DDSImage {
public:
	unsigned char* data;
	unsigned int width;
	unsigned int height;
	unsigned int size;
	GLenum format;

	DDSImage() : data(nullptr), width(0), height(0), size(0), format(GL_NONE) {}
	DDSImage(const DDSImage& img) = delete;
	DDSImage(DDSImage&& img) : data(img.data), width(img.width), height(img.height), size(img.size), format(img.format) {
		img.data = nullptr;
	}
	DDSImage& operator=(const DDSImage& img) = delete;
	DDSImage& operator=(DDSImage&& img) {
		data = img.data;
		img.data = nullptr;
		width = img.width;
		height = img.height;
		size = img.size;
		format = img.format;
		return *this;
	};

	~DDSImage() { if (data != nullptr) { delete[] data; data = nullptr; } }
};


/* --------------------------------------------- */
// Framework functions
/* --------------------------------------------- */

///*!
// * Initializes the framework
// * Do not overwrite this function!
// */
//bool initFramework();
//
///*!
// * Draws a teapot
// */
//void drawTeapot();
//
///*!
// * Destroys the framework
// * Do not overwrite this function!
// */
//void destroyFramework();
//
///*!
// * Loads a '.dss' image from a file
// * @param file: the path to the image file
// * @return a loaded DSS image
// */
//DDSImage loadDDS(const char* file);
//
///*!
// * Saves the currently displayed frame as ppm image
// * @param filename		The filename to save to without .ppm format ending
// * @param width			The width of the swapchain image = window_width
// * @param height		The height of the swapchain image = window_height
// */
//void saveScreenshot(std::string filename, uint32_t width, uint32_t height);
