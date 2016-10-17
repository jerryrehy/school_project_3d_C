#ifndef _Model_H_
#define _Model_H_

#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\PostProcess.h"
#include "IL\config.h"
#include "IL\il.h"
#include "stdafx.h"
#include <iostream>
#include <time.h>
#include <stdlib.h>
#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <math.h>
#include "glut.h"
#include <string>
#include "stdio.h"
#include <windows.h>
#include <fstream>

class Model {
private:
	GLuint displaylist;
	// images / texture
	// map image filenames to textureIds
	// pointer to texture Array
	std::map<std::string, GLuint> textureIdMap;
	std::map<std::string, GLuint>::iterator iterat;
	GLuint* textureIds;
	int numTextures;
	Assimp::Importer modelImporter;
	GLuint *textures;
public:
	int LoadGLTextures(const aiScene* scene);
	// Represent a single model loaded by pyassimp.
	Model(const char *filename);
	void draw(void);
	GLdouble lp_hd_pz[6];//leva-prava, horni-dolni, predni-zadni
};

#endif