#include "Drawable.h"

void Drawable::CreateCube()
{
	float_3 _arrcube[6];
	_arrcube[0].r = 1.000000;
	_arrcube[0].g = 0.000000;
	_arrcube[0].b = 0.000000;
	_arrcube[1].r = -1.000000;
	_arrcube[1].g = 0.000000;
	_arrcube[1].b = 0.000000;
	_arrcube[2].r = 0.000000;
	_arrcube[2].g = 1.000000;
	_arrcube[2].b = 0.000000;
	_arrcube[3].r = 0.000000;
	_arrcube[3].g = -1.000000;
	_arrcube[3].b = 0.000000;
	_arrcube[4].r = 0.000000;
	_arrcube[4].g = 0.000000;
	_arrcube[4].b = 1.000000;
	_arrcube[5].r = 0.000000;
	_arrcube[5].g = 0.000000;
	_arrcube[5].b = -1.000000;

	for (int i = 0; i < 6; i++)
	{
		_vertexs->push_back(_arrcube[i]);
	}

	_vec_tris->push_back(3);
	_vec_tris->push_back(1);
	_vec_tris->push_back(4);

	_vec_tris->push_back(0);
	_vec_tris->push_back(3);
	_vec_tris->push_back(4);

	_vec_tris->push_back(2);
	_vec_tris->push_back(0);
	_vec_tris->push_back(4);

	_vec_tris->push_back(1);
	_vec_tris->push_back(2);
	_vec_tris->push_back(4);

	_vec_tris->push_back(3);
	_vec_tris->push_back(0);
	_vec_tris->push_back(5);

	_vec_tris->push_back(1);
	_vec_tris->push_back(3);
	_vec_tris->push_back(5);

	_vec_tris->push_back(2);
	_vec_tris->push_back(1);
	_vec_tris->push_back(5);

	_vec_tris->push_back(0);
	_vec_tris->push_back(2);
	_vec_tris->push_back(5);
}