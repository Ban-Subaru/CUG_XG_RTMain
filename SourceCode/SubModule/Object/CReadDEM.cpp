//-#include "DXUT.h"
#include "ReadDEM.h"
#define range 1   //�ʵ��������������εķ�Χ
#define _range 1

ReadDEM *ReadDEM::GReadDEMData = NULL;

	void eclipse(float &data)  //������ֵС��ĳһ��ֵʱ��������Ϊ0����ֹ��ת�����������ɿ�ѧ������С����ʹ���ݴ���
	{
		if (fabs(data) < 0.0000000001)
			data = 0;
	}

	void ReadDEM::Alter_GData()
	{
#ifdef _NO_ECLIPSE
		coords.push_back(float_3((
			((trans[1] * array_coords[0] - trans[2] * array_coords[1])\
			+ trans[0])*DIFF_OF_LONGITUDE - array_coords[3])/range,/*�����ȥarray_coords[3]��Ϊ�˼�ȥһ����ʼλ�õľ���ƫ��*/
			(
			((trans[4] * array_coords[0] - trans[5] * array_coords[1])\
			+ trans[3])*DIFF_OF_LATITUDE - array_coords[4])/range,/*�����ȥarray_coords[4]��Ϊ�˼�ȥһ����ʼλ�õľ���ƫ��*/
			(array_coords[2])/_range));//����size_x,size_y,dem_z��ת�����յ㾭γ��,����߳�ֵ
#else _USE_ECLIPSE

			float v1 = (((trans[1] * array_coords[0] - trans[2] * array_coords[1])\
				+ trans[0])*DIFF_OF_LONGITUDE - array_coords[3]) / range;/*�����ȥarray_coords[3]��Ϊ�˼�ȥһ����ʼλ�õľ���ƫ��*/
			eclipse(v1);
			float v2 = (((trans[4] * array_coords[0] - trans[5] * array_coords[1])\
				+ trans[3])*DIFF_OF_LATITUDE - array_coords[4]) / range;/*�����ȥarray_coords[4]��Ϊ�˼�ȥһ����ʼλ�õľ���ƫ��*/
			eclipse(v2);
			float v3 = (array_coords[2]) / _range;//����size_x,size_y,dem_z��ת�����յ㾭γ��,����߳�ֵ
			eclipse(v3);
			coords.push_back(float_3(v1, v2, v3));

#endif
	}

	void ReadDEM::Createbuf()
	{
		if (buf == NULL)
		{
			long temp = size_x*size_y;//temp��ֵ���ܵ��³����ȡ����ʱ����
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
			poDataset = (GDALDataset *)GDALOpen(filepathname, GA_ReadOnly); //�����tifͼ��Ȼ�����ݼ�����poDataset��
			poBand1 = poDataset->GetRasterBand(poband);//��ȡդ�����ݵ�һ�����Σ�ÿ�����ΰ���Ԫ���ݣ����С����ɫ���Լ�������Ϣ
			dataType = poBand1->GetRasterDataType();  //��ȡ�߳���������

			//��ȡͼ��ĳ�X�Ϳ�Y
			size_x = poDataset->GetRasterXSize();
			size_y = poDataset->GetRasterYSize();

			Createbuf();
			if (buf != NULL)
			{
				poBand1->RasterIO(
					GF_Read, 0, 0, size_x, size_y,
					buf, size_x, size_y,
					GDALDataType(poBand1->GetRasterDataType()),
					0, 0); //��ȡ���и߳�ֵ

				SetDEMinfo();  //��ȡӰ��֮���ȡ��ص���Ϣ

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
		char *Crood = (char *)poDataset->GetProjectionRef();   //��ȡ����ϵ��Ϣ
		char newCrood[7];
		int i = 0, a = 8, b = 13;
		for (; a <= b; i++, a++)
			newCrood[i] = Crood[a];        //�õ�����ϵ����
		newCrood[6] = '\0';
		if (strcmp(newCrood, "WGS 84") == 0)   croodinate = WGS1984;      //�ж��Ƿ�ΪWGS1984����ϵ

	}

	void ReadDEM::SetBound()
	{
		begin_bound_coordinates[0] = trans[0];//����0,0��ת������ʼ�㾭��
		begin_bound_coordinates[1] = trans[3];//����0,0��ת������ʼ��γ��

		end_bound_coordinates[0] = ((trans[1] * size_x - trans[2] * size_y) + trans[0]);//����size_x,size_y��ת�����յ㾭��
		end_bound_coordinates[1] = ((trans[4] * size_x - trans[5] * size_y) + trans[3]);//����size_x,size_y��ת�����յ�γ��
	}


	void ReadDEM::SetDEMinfo()
	{
		width = size_x;

		length = size_y;

		SetCoord();

		unit = M;//�߳�ֵ��λ

		GdalDataType = dataType;//��ò��ε���������

		band = poband;//��ʱֻ֪��ȡ��һ����

		//--------------------------------------
		double lim[2] = { 0.0, 8192.0 };
		poBand1->ComputeRasterMinMax(false, lim);
		vertices = (long)lim[1];     //��ߵ�
		nadir = (long)lim[0];//��͵�
		//--------------------------------------

		vertex_sum = (int)size_x*size_y;//��������

		poDataset->GetGeoTransform(trans); //��ȡ����ϵ�任����������trans[]

		SetBound();//���÷�Χ
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
				int array_sum = vertex_sum;//��ȡ��������
				short *DataArray = (short*)(buf);//����Ϊshort����,���Ժ���Ż���Ҫ�ܶ�̬���޸�
				int temp_x = 0, temp_y = 0, i = 0;
				int offset = vacuate;
				int _offset = offset;

				array_coords[3] = begin_bound_coordinates[0] * DIFF_OF_LONGITUDE;//������ʼ����������ĺ����м�ȥ
				array_coords[4] = begin_bound_coordinates[1] * DIFF_OF_LATITUDE;

				for (; i < array_sum; i = i + _offset)
				{
					_offset = offset;
					if (temp_x > size_x)//�б������ؾ���
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
					Alter_GData();//���ú���
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

