#ifndef READ_DEM_H_
#define READ_DEM_H_

#define FILE_SIZE_HEADER 4194304    //1G的数据

#include<gdal.h>			//gdal头文件
#include<ogrsf_frmts.h>		//gdal头文件
#include<gdal_priv.h>		//gdal头文件
#include <windows.h>

#include <vector>
#include <amp_graphics.h>
#include "config.h"
using namespace std;
using namespace concurrency;
using namespace concurrency::graphics;

#define DIFF_OF_LONGITUDE 111085	//定义经差之间的距离（粗略）
#define DIFF_OF_LATITUDE 111240		//定义纬差之间的距离（粗略）


	//------------------------------------------------------
	//坐标系
	enum CROODINATE
	{
		BEIJING54 = 0,			//北京54坐标系
		XIAN80,					//西安80坐标系
		CHINA2000,				//中国2000坐标系
		WGS1984,				//美国国家标准地理坐标系，横轴莫卡托投影
	};

	//------------------------------------------------------
	//单位
	enum UNIT
	{
		MM = 0,		//毫米
		CM,			//厘米
		DM,			//分米
		M,			//米
		KM,			//千米

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
			vacuate = 100;				 // 默认抽稀比例为100
			reward = 100;				 //默认增强比例为100
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
		void Createbuf();//创建缓冲区
		inline void* Getbuf_ptr(){ return buf; }						//获取缓冲区数据指针
		void *ReadGdalData(char *_filepathname);						//读取高程数据
		inline long GetnumVertex()  { return size_x*size_y; }			//获取顶点数
		inline unsigned int Getvacuate(){ return vacuate; }				//获取抽稀比例
		inline long *GetSize_x_y(){ long *size = new long[2]; size[0] = size_x, size[1] = size_y; return size; }//返回边界大小
		//--
		void SetDEMinfo();				//设置影像信息
		void SetCoord();				//获取坐标信息
		void SetBound();				//设置影像范围

		//设置抽稀比例
		inline void SetVacuate(unsigned int _vacuate)
		{
			vacuate = _vacuate;
		}
		//设置增强比例
		inline void SetReward(float _reward)
		{
			reward = _reward;
		}
		//--
		void Alter_GData(); //将经纬度转换成地面距离
		vector<float_3> CreateCoordsandAddtoVec(); //创建世界坐标顶点并添加如向量
		vector<float_3>& GetALLCoords();			   //得到所有的坐标点
		//********************
	private:

		static ReadDEM *GReadDEMData;			 //全局唯一实例

		struct
		{
			long size_x;				//x

			long size_y;				//y

			char *filepathname;			//全路径文件名

			void *buf;					//缓冲区指针

			short poband;				//波段

			GDALDataType dataType;		//高程数据类型

			GDALDataset *poDataset;		//GDAL数据集

			GDALRasterBand *poBand1;	//获取图像波段
		};

		struct
		{
			vector<float_3> coords;  //用一个大向量存放生成后的地形节点

			/**********************************************************************************
			*    [0] = X,[1] = Y,[2] = Z(dem),[3] = index_x,[4] = index_y
			**********************************************************************************/
			float array_coords[5];

			unsigned int vacuate;	//抽稀

			float reward;			//增强
		};

		struct
		{
			long width;				//高度

			long length;			//宽度

			double begin_bound_coordinates[2];			//起始经纬度

			double end_bound_coordinates[2];			//结束经纬度

			CROODINATE croodinate;						//所用坐标系

			UNIT unit;									//高程数据单位

			GDALDataType GdalDataType;					//波段数据类型

			short band;									//所取波段

			int  vertices;								//最高点

			int  nadir;									//最低点

			long vertex_sum;							//顶点数量

			double trans[6];							//该坐标系的坐标变换参数
		};

	};


	//获取全局唯一实例
inline ReadDEM *GetReadDataHandler()
{
	return ReadDEM::GetReadDEMHandler();
}

#endif