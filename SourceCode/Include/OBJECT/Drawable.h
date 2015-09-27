#include "WriteObject.h"

#ifndef DRAWABLE_H
#define DRAWABLE_H



class Drawable
{
public:
	Drawable(vector< GLuint > *vec_tris, vector< float_3 > *vertexs)
	{
		_vec_tris = vec_tris;
		_vertexs = vertexs;
		CreateCube();
	}
	void CreateCube();

private:
	vector< GLuint >		    *_vec_tris;  //�������������ε�����
	vector< float_3 >            *_vertexs;  //��������
	
};







#endif