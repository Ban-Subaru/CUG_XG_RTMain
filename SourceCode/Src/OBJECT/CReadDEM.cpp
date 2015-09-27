//-#include "DXUT.h"
#include "ReadDEM.h"
#define range 1   //适当按比例调整地形的范围
#define _range 1

ReadDEM *ReadDEM::GReadDEMData = NULL;

	void eclipse(float &data)  //当地形值小于某一数值时，就设置为0，防止在转换过程中生成科学计数法小数，使数据错误
	{
		if (fabs(data) < 0.0000000001)
			data = 0;
	}

	void ReadDEM::Alter_GData()
	{
#ifdef _NO_ECLIPSE
		coords.push_back(float_3((
			((trans[1] * array_coords[0] - trans[2] * array_coords[1])\
			+ trans[0])*DIFF_OF_LONGITUDE - array_coords[3])/range,/*这里减去array_coords[3]是为了减去一个起始位置的距离偏移*/
			(
			((trans[4] * array_coords[0] - trans[5] * array_coords[1])\
			+ trans[3])*DIFF_OF_LATITUDE - array_coords[4])/range,/*这里减去array_coords[4]是为了减去一个起始位置的距离偏移*/
			(array_coords[2])/_range));//将（size_x,size_y,dem_z）转化成终点经纬度,存入高程值
#else _USE_ECLIPSE

			float v1 = (((trans[1] * array_coords[0] - trans[2] * array_coords[1])\
				+ trans[0])*DIFF_OF_LONGITUDE - array_coords[3]) / range;/*这里减去array_coords[3]是为了减去一个起始位置的距离偏移*/
			eclipse(v1);
			float v2 = (((trans[4] * array_coords[0] - trans[5] * array_coords[1])\
				+ trans[3])*DIFF_OF_LATITUDE - array_coords[4]) / range;/*这里减去array_coords[4]是为了减去一个起始位置的距离偏移*/
			eclipse(v2);
			float v3 = (array_coords[2]) / _range;//将（size_x,size_y,dem_z）转化成终点经纬度,存入高程值
			eclipse(v3);
			coords.push_back(float_3(v1, v2, v3));

#endif
	}

	void ReadDEM::Createbuf()
	{
		if (buf == NULL)
		{
			long temp = size_x*size_y;//temp的值可能导致程序读取数据时出错
			if (temp != 0)
			{
				buf = new short[temp];
				memset(buf, 0, temp);
			}
			else
			{
				return;
			}
		}
	}

	void *ReadDEM::ReadGdalData(char *_filepathname)
	{
		if (_filepathname != NULL)
		{
			filepathname = _filepathname;
			poDataset = (GDALDataset *)GDALOpen(filepathname, GA_ReadOnly); //打开这个tif图像，然后将数据集传给poDataset中
			poBand1 = poDataset->GetRasterBand(poband);//获取栅格数据的一个波段，每个波段包含元数据，块大小，颜色表以及其他信息
			dataType = poBand1->GetRasterDataType();  //获取高程数据类型

			//获取图像的长X和宽Y
			size_x = poDataset->GetRasterXSize();
			size_y = poDataset->GetRasterYSize();

			Createbuf();
			if (buf != NULL)
			{
				poBand1->RasterIO(
					GF_Read, 0, 0, size_x, size_y,
					buf, size_x, size_y,
					GDALDataType(poBand1->GetRasterDataType()),
					0, 0); //读取所有高程值

				SetDEMinfo();  //读取影像之后获取相关的信息

				return buf;
			}

			else
			{
				return NULL;
			}

		}
		else
		{
			return buf = NULL;
		}
	}

	void ReadDEM::SetCoord()
	{
		char *Crood = (char *)poDataset->GetProjectionRef();   //获取坐标系信息
		char newCrood[7];
		int i = 0, a = 8, b = 13;
		for (; a <= b; i++, a++)
			newCrood[i] = Crood[a];        //得到坐标系名称
		newCrood[6] = '\0';
		if (strcmp(newCrood, "WGS 84") == 0)   croodinate = WGS1984;      //判断是否为WGS1984坐标系

	}

	void ReadDEM::SetBound()
	{
		begin_bound_coordinates[0] = trans[0];//将（0,0）转化成起始点经度
		begin_bound_coordinates[1] = trans[3];//将（0,0）转化成起始点纬度

		end_bound_coordinates[0] = ((trans[1] * size_x - trans[2] * size_y) + trans[0]);//将（size_x,size_y）转化成终点经度
		end_bound_coordinates[1] = ((trans[4] * size_x - trans[5] * size_y) + trans[3]);//将（size_x,size_y）转化成终点纬度
	}


	void ReadDEM::SetDEMinfo()
	{
		width = size_x;

		length = size_y;

		SetCoord();

		unit = M;//高程值单位

		GdalDataType = dataType;//获得波段的数据类型

		band = poband;//暂时只知道取第一波段

		//--------------------------------------
		double lim[2] = { 0.0, 8192.0 };
		poBand1->ComputeRasterMinMax(false, lim);
		vertices = (long)lim[1];     //最高点
		nadir = (long)lim[0];//最低点
		//--------------------------------------

		vertex_sum = (int)size_x*size_y;//顶点数量

		poDataset->GetGeoTransform(trans); //获取坐标系变换参数到数组trans[]

		SetBound();//设置范围
	}

	vector<float_3>& ReadDEM::GetALLCoords()
	{
		return coords;
	}


	vector<float_3> ReadDEM::CreateCoordsandAddtoVec()
	{
		if (buf != NULL)
		{
			if (&coords != NULL)
			{
				int array_sum = vertex_sum;//获取顶点数量
				short *DataArray = (short*)(buf);//设置为short类型,在以后的优化中要能动态的修改
				int temp_x = 0, temp_y = 0, i = 0;
				int offset = vacuate;
				int _offset = offset;

				array_coords[3] = begin_bound_coordinates[0] * DIFF_OF_LONGITUDE;//定义起始距离差，在上面的函数中减去
				array_coords[4] = begin_bound_coordinates[1] * DIFF_OF_LATITUDE;

				for (; i < array_sum; i = i + _offset)
				{
					_offset = offset;
					if (temp_x > size_x)//行遍历像素矩阵
					{
						temp_x = temp_x%size_x;
						temp_y = temp_y + _offset;
						_offset = _offset*size_x;
					}
					if (temp_y > size_y)
					{
						break;
					}
					array_coords[0] = temp_x;//X
					array_coords[1] = temp_y;//Y
					array_coords[2] = DataArray[i] * reward;//Z
					Alter_GData();//调用函数
					temp_x = temp_x + offset;
				}

			}

			return coords;
		}
		else
		{
			coords.clear();
			return coords;
		}
	}

