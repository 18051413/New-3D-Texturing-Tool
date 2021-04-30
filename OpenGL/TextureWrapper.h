#pragma once
#include "Mesh.h"

class TextureWrapper
{
public:

	double XSCALE;
	double YSCALE;
	double ZSCALE;
	double XOFFSET;
	double YOFFSET;
	double ZOFFSET;

	

	int planerAxis;
	int biggest;
	TextureWrapper() {}
	void calculatePlanerCoords(Mesh* Mesh);
	void calculateBoxCoords(Mesh* Mesh);
private:
	
};

