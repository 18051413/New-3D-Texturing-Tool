#include "TextureWrapper.h"
#include "Includes.h"


void TextureWrapper::calculatePlanerCoords(Mesh* Mesh)
{

	

	XSCALE = 100;
	YSCALE = 100;
	ZSCALE = 100;
	XOFFSET = 0.5;
	YOFFSET = 0.5;
	ZOFFSET = 0.5;
	//planerAxis = -1; //0 = xy |1 = xz|2 = yz
	cout << "class variable = " << planerAxis << endl;

	int numvert = Mesh->vertices.size();
	for (int i = 0; i < numvert; i++)
	{
		if (planerAxis == 0)
		{
			Mesh->vertices[i].TexCoords.x = (Mesh->vertices[i].Position.x / XSCALE) + XOFFSET;
			Mesh->vertices[i].TexCoords.y = (Mesh->vertices[i].Position.y / YSCALE) + YOFFSET;
		}
		else if (planerAxis == 1)
		{
			Mesh->vertices[i].TexCoords.x = (Mesh->vertices[i].Position.x / XSCALE) + XOFFSET;
			Mesh->vertices[i].TexCoords.y = (Mesh->vertices[i].Position.z / ZSCALE) + ZOFFSET;
		}
		else
		{
			Mesh->vertices[i].TexCoords.x = (Mesh->vertices[i].Position.z / ZSCALE) + ZOFFSET;
			Mesh->vertices[i].TexCoords.y = (Mesh->vertices[i].Position.y / YSCALE) + YOFFSET;
		}
		//std::cout << "is here" << std::endl;
	}
	//std::cout << "numvert = " << numvert << std::endl;
	//sizeof(vertices)
	Mesh->setupMesh();
}

void TextureWrapper::calculateBoxCoords(Mesh* Mesh)
{
    int numvert = Mesh->vertices.size();



    //find object center
    glm::vec3 center = glm::vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < numvert; i++)
    {
        center.x += Mesh->vertices[i].Position.x;
        center.y += Mesh->vertices[i].Position.y;
        center.z += Mesh->vertices[i].Position.z;
    }
    center.x /= numvert;
    center.y /= numvert;
    center.z /= numvert;

    for (int i = 0; i < numvert; i++)
    {



        glm::vec3 Norm = glm::normalize(Mesh->vertices[i].Normal);
        glm::vec3 Pos = Mesh->vertices[i].Position;
        Pos.x -= center.x;
        Pos.y -= center.y;
        Pos.z -= center.z;
        //cout << Mesh->vertices[i].Normal.x << " " << Mesh->vertices[i].Normal.y << " " << Mesh->vertices[i].Normal.z << endl;
        biggest = -1;
        if ((fabs(Norm.x) >= fabs(Norm.y)) && (fabs(Norm.x) >= fabs(Norm.z)))
        {
            //if x biggest
            biggest = 1;
        }
        else if ((fabs(Norm.y) >= fabs(Norm.x)) && (fabs(Norm.y) >= fabs(Norm.z)))
        {
            //if y biggest
            biggest = 2;
        }
        else if ((fabs(Norm.z) >= fabs(Norm.y)) && (fabs(Norm.z) >= fabs(Norm.x)))
        {
            //if z biggest
            biggest = 3;
        }
        if (biggest == 1)
        {
            Mesh->vertices[i].TexCoords.x = Pos.y / Pos.x;
            Mesh->vertices[i].TexCoords.y = Pos.z / Pos.x;
            //cout << "X" << endl;
        }
        if (biggest == 2)
        {
            Mesh->vertices[i].TexCoords.x = Pos.x / Pos.y;
            Mesh->vertices[i].TexCoords.y = Pos.z / Pos.y;
            //cout << "Y" << endl;
        }
        if (biggest == 3)
        {
            Mesh->vertices[i].TexCoords.x = Pos.x / Pos.z;
            Mesh->vertices[i].TexCoords.y = Pos.y / Pos.z;
            //cout << "Z" << endl;
        }



        //cout << biggest << endl;
    }
    Mesh->setupMesh();



}

//void TextureWrapper::calculateBoxCoords(Mesh* Mesh)
//{
//	int numvert = Mesh->vertices.size();
//	for (int i = 0; i < numvert; i++)
//	{
//		//cout << "x pos before: " << Mesh->vertices[i].Position.x << endl;
//		//cout << "y pos before: " << Mesh->vertices[i].Position.y << endl;
//		//cout << "z pos before: " << Mesh->vertices[i].Position.z << endl;
//		//Mesh->vertices[i].Position = glm::normalize(Mesh->vertices[i].Position);
//		//cout << "x pos: " << Mesh->vertices[i].Position.x << endl;
//		//cout << "y pos: " << Mesh->vertices[i].Position.y << endl;
//		//cout << "z pos: " << Mesh->vertices[i].Position.z << endl;
//
//		glm::vec3 Pos = glm::normalize(Mesh->vertices[i].Position);
//
//		if (fabs(Pos.x) > fabs(Pos.y) & fabs(Pos.x) > fabs(Pos.z))
//		{
//			//if x biggest
//			biggest = 1;
//		}
//		if (fabs(Pos.y) > fabs(Pos.x) & fabs(Pos.y) > fabs(Pos.z))
//		{
//			//if y biggest
//			biggest = 2;
//		}
//		if (fabs(Pos.z) > fabs(Pos.y) & fabs(Pos.z) > fabs(Pos.x))
//		{
//			//if z biggest
//			biggest = 3;
//		}
//		if (biggest == 1);
//		{
//			Mesh->vertices[i].TexCoords.x = Pos.y / Pos.x;
//			Mesh->vertices[i].TexCoords.y = Pos.z / Pos.x;
//		}
//		if (biggest == 2);
//		{
//			Mesh->vertices[i].TexCoords.x = Pos.x / Pos.y;
//			Mesh->vertices[i].TexCoords.y = Pos.z / Pos.y;
//		}
//		if (biggest == 3);
//		{
//			Mesh->vertices[i].TexCoords.x = Pos.x / Pos.z;
//			Mesh->vertices[i].TexCoords.y = Pos.y / Pos.z;
//		}
//	}
//	Mesh->setupMesh();
//	/*
//	For each vertex
//	{
//	Normalize(pos)
//	Find biggest dimension X, Y or Z
//	If Y is biggest
//	U=X/Y
//	V=Z/Y
//	Else if X is biggest
//	U =Y/X
//	V= Z/X
//
//	Else //Z is biggest
//	U =X/Z
//	V= Y/Z
//	}
//	U/V == X/Y just for reference
//
//	*/
//		//std::cout << "is here" << std::endl;
//	//std::cout << "numvert = " << numvert << std::endl;
//	//sizeof(vertices)

//}
