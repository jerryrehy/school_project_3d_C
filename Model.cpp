#include "Model.h"

	int Model::LoadGLTextures(const aiScene* scene) {
		ILboolean success;
		/* initialization of DevIL */
		ilInit(); 
		/* scan scene's materials for textures */
		for (int m=0; m<scene->mNumMaterials; ++m) {
			int texIndex = 0;
			aiString path;  // filename
			aiReturn texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
			while (texFound == AI_SUCCESS) {
				//fill map with textures, OpenGL image ids set to 0
				textureIdMap[path.data] = 0; 
				// more textures?
				texIndex++;
				texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
			}
		}
		
		numTextures = textureIdMap.size();
		/* create and fill array with DevIL texture ids */
		ILuint* imageIds = new ILuint[numTextures];
		ilGenImages(numTextures, imageIds); 
		/* create and fill array with GL texture ids */
		textureIds = new GLuint[numTextures];
		glGenTextures(numTextures, textureIds); /* Texture name generation */

		/* get iterator */
		std::map<std::string, GLuint>::iterator itr = textureIdMap.begin();
		printf("TextureIDMap Begin %i\n", textureIdMap.begin());
		int i=0;
		for (; itr != textureIdMap.end(); ++i, ++itr) {
			//save IL image ID
			std::string filename = (*itr).first;  // get filename
			(*itr).second = textureIds[i];    // save texture id for filename in map
			printf("Texture loaded: %s\n",filename.c_str());
			printf("Texture ID Map End: %i\n",textureIdMap.end());
			ilBindImage(imageIds[i]); /* Binding of DevIL image name */
			ilEnable(IL_ORIGIN_SET);
			ilOriginFunc(IL_ORIGIN_LOWER_LEFT); 
			success = ilLoadImage((ILstring)filename.c_str());

			if (success) {
				/* Convert image to RGBA */
				ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE); 

				/* Create and load textures to OpenGL */
				glBindTexture(GL_TEXTURE_2D, textureIds[i]); 
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ilGetInteger(IL_IMAGE_WIDTH),
					ilGetInteger(IL_IMAGE_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE,
					ilGetData()); 
			}
			else 
				printf("Couldn't load Image: %s\n", filename.c_str());
		}
		/* Because we have already copied image data into texture data  we can release memory used by image. */
		ilDeleteImages(numTextures, imageIds); 
		//Cleanup
		delete [] imageIds;
		//return success;
		return true;
	}
	GLvoid* getPixels(aiTexel *pixels, GLuint numofpixs){
		GLubyte *vysledek=new GLubyte[numofpixs*4];//RGBA8
		for(unsigned int i=0;i<(numofpixs*4);i=4){
			vysledek[i]=pixels[numofpixs/4].r;
			vysledek[i+1]=pixels[numofpixs/4].g;
			vysledek[i+2]=pixels[numofpixs/4].b;
			vysledek[i+3]=pixels[numofpixs/4].a;
		}
		return (GLvoid *)vysledek;
	}
	// Represent a single model loaded by pyassimp.
  Model::Model(const char *filename) {
    // Load the model and create a display list for it.
	GLfloat color_grey[]={0.7, 0.7, 0.7, 1.0};
    const struct aiScene *scene;
	lp_hd_pz[0]=100;
	lp_hd_pz[1]=-100;
	lp_hd_pz[2]=-100;
	lp_hd_pz[3]=100;
	lp_hd_pz[4]=100;
	lp_hd_pz[5]=-100;
	aiString path;
    std::cout << "loading " << filename << std::endl;
    //scene = aiImportFile(filename,aiProcess_GenNormals);
	scene = modelImporter.ReadFile(filename,aiProcess_GenNormals| aiProcess_FlipUVs);

    displaylist = glGenLists(1);
    glNewList(displaylist,GL_COMPILE);
	//printf("disp id: %d\n", displaylist);
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//textures=new GLuint[scene->mNumTextures];
    //glGenTextures(scene->mNumTextures, textures);
	Model::LoadGLTextures(scene);
	glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color_grey );
	/*if(scene->HasTextures())printf("Num of textures:%d\n", scene->mNumTextures);
	else printf("No textures\n");
	if(scene->HasMaterials())printf("Num of mats:%d\n", scene->mNumMaterials);
	else printf("No materials\n");*/
	//for (int i=0;i<scene->mNumMaterials;i++)printf("Num of texs in mat %d: %d\n", i, scene->mMaterials[i]->GetTextureCount(aiTextureType_DIFFUSE));
	//scene->mMeshes[0]->mMaterialIndex;
	//printf("coords: %f %f", scene->mMeshes[0]->mTextureCoords[0]->x,scene->mMeshes[0]->mTextureCoords[0]->y);
    for (int meshnum=0; meshnum < scene->mNumMeshes; meshnum++) {
		aiMesh *mesh = scene->mMeshes[meshnum];
		if(scene->mMaterials[mesh->mMaterialIndex]->GetTextureCount(aiTextureType_DIFFUSE)!=0){
			scene->mMaterials[mesh->mMaterialIndex]->GetTexture(aiTextureType_DIFFUSE, 0, &path);
			iterat=textureIdMap.find(path.data);
			glBindTexture(GL_TEXTURE_2D, iterat->second);
			//printf("TextureCount: %d\n", scene->mMaterials[mesh->mMaterialIndex]->GetTextureCount(aiTextureType_DIFFUSE));
		}

      for (int facenum=0; facenum < mesh->mNumFaces; facenum++) {
		aiFace *face = &mesh->mFaces[facenum];
		//aiReturn texFound = scene->mMaterials[m]->GetTexture(aiTextureType_DIFFUSE, texIndex, &path);
		glBegin(GL_POLYGON);
		int vertex;
		for (int v=0; v < face->mNumIndices; v++) {
		  vertex = face->mIndices[v];
		  glNormal3f(mesh->mNormals[vertex].x,
				 mesh->mNormals[vertex].y,
				 mesh->mNormals[vertex].z);
		  if(mesh->HasTextureCoords(0))glTexCoord2f(mesh->mTextureCoords[0][vertex].x, 1-mesh->mTextureCoords[0][vertex].y);
		  else if(mesh->HasTextureCoords(1))glTexCoord2f(mesh->mTextureCoords[1][vertex].x, 1-mesh->mTextureCoords[1][vertex].y);
		  else if(mesh->HasTextureCoords(2))glTexCoord2f(mesh->mTextureCoords[2][vertex].x, 1-mesh->mTextureCoords[2][vertex].y);
		  else if(mesh->HasTextureCoords(3))glTexCoord2f(mesh->mTextureCoords[3][vertex].x, 1-mesh->mTextureCoords[3][vertex].y);
		  else if(mesh->HasTextureCoords(4))glTexCoord2f(mesh->mTextureCoords[4][vertex].x, 1-mesh->mTextureCoords[4][vertex].y);
		  else if(mesh->HasTextureCoords(5))glTexCoord2f(mesh->mTextureCoords[5][vertex].x, 1-mesh->mTextureCoords[5][vertex].y);
		  else if(mesh->HasTextureCoords(6))glTexCoord2f(mesh->mTextureCoords[6][vertex].x, 1-mesh->mTextureCoords[6][vertex].y);
		  else if(mesh->HasTextureCoords(7))glTexCoord2f(mesh->mTextureCoords[7][vertex].x, 1-mesh->mTextureCoords[7][vertex].y);
		  //if(mesh->HasTextureCoords(0))printf("ano\r");
		  glVertex3f(mesh->mVertices[vertex].x,
				 mesh->mVertices[vertex].y,
				 mesh->mVertices[vertex].z);
		  if(lp_hd_pz[0]>mesh->mVertices[vertex].x)lp_hd_pz[0]=mesh->mVertices[vertex].x;
		  else if(lp_hd_pz[1]<mesh->mVertices[vertex].x)lp_hd_pz[1]=mesh->mVertices[vertex].x;
		  if(lp_hd_pz[2]<mesh->mVertices[vertex].y)lp_hd_pz[2]=mesh->mVertices[vertex].y;
		  else if(lp_hd_pz[3]>mesh->mVertices[vertex].y)lp_hd_pz[3]=mesh->mVertices[vertex].y;
		  if(lp_hd_pz[4]>mesh->mVertices[vertex].z)lp_hd_pz[4]=mesh->mVertices[vertex].z;
		  else if(lp_hd_pz[5]<mesh->mVertices[vertex].z)lp_hd_pz[5]=mesh->mVertices[vertex].z;
		}
		glEnd();
      }
    }
	//printf("\n");
    glEndList();
	delete [] textureIds;
  }

  void Model::draw(void) {
    // Draw the model from the display list."""
	//printf("Drawing player (%d)\n", displaylist);
	glCallList(displaylist);
  }
