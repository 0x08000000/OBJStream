#ifndef OBJSTREAM_H
#define OBJSTREAM_H

#define OBJSTREAM_VERSION "1.0.0"

#include <stdlib.h>

//An Obj file can store:
// 1. vertices (v)
// 2. normals (vn)
// 3. texture coordinates (vt)
// 4. faces (f)
// 5. materials (mtllib)

//Material file .mtl can store:
// 1. material name (newmtl)
// 2. material properties (Ka, Kd, Ks, Ke, Ns, Ni, d, Tf, illum)
// 3. texture maps. (map_Ka, map_Kd, map_Ks, map_Ke, map_Ns, map_bump, map_d)

//Struct to store TGA FILE.
struct OBJMaterialTexture
{
	char *Name;

	char *PathFile;
};

struct OBJMaterial
{
	char *Name;

	float KA[3]; //Ambient
	float KD[3]; //Diffuse
	float KS[3]; //Specular
	float KE[3]; //Emissive Coefficient

	float NS; //Specular Exponent
	float NI; //Index of Refraction

	float TR; //Transparency = (1 - d) sometimes used instead of 'd'
	float D; //Dissolve

	float TF[3]; //Transmition Filter

	int   Illum;

	//tga files.
	struct OBJMaterialTexture Map_KA;
	struct OBJMaterialTexture Map_KD;
	struct OBJMaterialTexture Map_KS;
	struct OBJMaterialTexture Map_KE;
	struct OBJMaterialTexture Map_NS;
	struct OBJMaterialTexture Map_NI;
	struct OBJMaterialTexture Map_Bump;
	struct OBJMaterialTexture Map_D;
};

#endif