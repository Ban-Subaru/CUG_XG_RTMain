#ifndef READ_DEM_H_
#define READ_DEM_H_

#define FILE_SIZE_HEADER 4194304    //1G������

#include<gdal.h>			//gdalͷ�ļ�
#include<ogrsf_frmts.h>		//gdalͷ�ļ�
#include<gdal_priv.h>		//gdalͷ�ļ�
#include <windows.h>

#include <vector>
#include <amp_graphics.h>
#include "config.h"
using namespace std;
using namespace concurrency;
using namespace concurrency::graphics;

#define DIFF_OF_LONGITUDE 111085	//���徭��֮��ľ��루���ԣ�
#define DIFF_OF_LATITUDE 111240		//����γ��֮��ľ��루���ԣ�


	//------------------------------------------------------
	//����ϵ
	enum CROODINATE
	{
		BEIJING54 = 0,			//����54����ϵ
		XIAN80,					//����80����ϵ
		CHINA2000,				//�й�2000����ϵ
		WGS1984,				//�������ұ�׼��������ϵ������Ī����ͶӰ
	};

	//------------------------------------------------------
	//��λ
	enum UNIT
	{
		MM = 0,		//����
		CM,			//����
		DM,			//����
		M,			//��
		KM,			//ǧ��

	};

	class ReadDEM
	{
	public:
		ReadDEM()
		{
			size_x = 0;
			size_y = 0;
			filepathname = NULL;
			buf = NULL;
			poband = 1;
			dataType = GDT_Unknown;
			GDALAllRegister();
			poBand1 = NULL;
			//--
			memset(array_coords, 0, 5);
			vacuate = 100;				 // Ĭ�ϳ�ϡ����Ϊ100
			reward = 100;				 //Ĭ����ǿ����Ϊ100
		}
		~ReadDEM()
		{
			coords.clear();
		}
		static ReadDEM * GetReadDEMHandler()
		{
			return GReadDEMData;
		}
		static void Create()
		{
			GReadDEMData = new ReadDEM();
		}
		void Destroy()
		{
			if (poDataset)
			{
				delete poDataset;
			}
		}


		//********************
		//--
		void Createbuf();//����������
		inline void* Getbuf_ptr(){ return buf; }						//��ȡ����������ָ��
		void *ReadGdalData(char *_filepathname);						//��ȡ�߳�����
		inline long GetnumVertex()  { return size_x*size_y; }			//��ȡ������
		inline unsigned int Getvacuate(){ return vacuate; }				//��ȡ��ϡ����
		inline long *GetSize_x_y(){ long *size = new long[2]; size[0] = size_x, size[1] = size_y; return size; }//���ر߽��С
		//--
		void SetDEMinfo();				//����Ӱ����Ϣ
		void SetCoord();				//��ȡ������Ϣ
		void SetBound();				//����Ӱ��Χ

		//���ó�ϡ����
		inline void SetVacuate(unsigned int _vacuate)
		{
			vacuate = _vacuate;
		}
		//������ǿ����
		inline void SetReward(float _reward)
		{
			reward = _reward;
		}
		//--
		void Alter_GData(); //����γ��ת���ɵ������
		vector<float_3> CreateCoordsandAddtoVec(); //�����������궥�㲢���������
		vector<float_3>& GetALLCoords();			   //�õ����е������
		//********************
	private:

		static ReadDEM *GReadDEMData;			 //ȫ��Ψһʵ��

		struct
		{
			long size_x;				//x

			long size_y;				//y

			char *filepathname;			//ȫ·���ļ���

			void *buf;					//������ָ��

			short poband;				//����

			GDALDataType dataType;		//�߳���������

			GDALDataset *poDataset;		//GDAL���ݼ�

			GDALRasterBand *poBand1;	//��ȡͼ�񲨶�
		};

		struct
		{
			vector<float_3> coords;  //��һ��������������ɺ�ĵ��νڵ�

			/**********************************************************************************
			*    [0] = X,[1] = Y,[2] = Z(dem),[3] = index_x,[4] = index_y
			**********************************************************************************/
			float array_coords[5];

			unsigned int vacuate;	//��ϡ

			float reward;			//��ǿ
		};

		struct
		{
			long width;				//�߶�

			long length;			//���

			double begin_bound_coordinates[2];			//��ʼ��γ��

			double end_bound_coordinates[2];			//������γ��

			CROODINATE croodinate;						//��������ϵ

			UNIT unit;									//�߳����ݵ�λ

			GDALDataType GdalDataType;					//������������

			short band;									//��ȡ����

			int  vertices;								//��ߵ�

			int  nadir;									//��͵�

			long vertex_sum;							//��������

			double trans[6];							//������ϵ������任����
		};

	};


	//��ȡȫ��Ψһʵ��
inline ReadDEM *GetReadDataHandler()
{
	return ReadDEM::GetReadDEMHandler();
}

#endif