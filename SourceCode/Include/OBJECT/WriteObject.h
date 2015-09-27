#ifndef WRITE_OBJECT_H_ 
#define WRITE_OBJECT_H_

#include "ReadDEM.h"
#include "LightControl.h"
#include "Array.h"
#include "File.h"
#include <stdlib.h>
#include <vector>
#include <string>
//--#include "tri.h"
//--#include "mathD3D.h"

using namespace std;
using namespace osg;


//--typedef unsigned short      WORD;

//对象文件头信息
	enum h_info
	{
		UNKNOWN = 0,
		COORDS3_ONLY =3,
		COORDS3_TEXTURE2 = 5,
		COORDS3_VECTOR3 = 6,
		COORDS3_VECTOR3_TEXTURE2 = 8
	};

	class WriteObject
	{
	public:
		//默认构造函数内部初始化变量
		WriteObject()
		{
			_fp = new cFile;
			_vec_tris = NULL;
			_m_name = ' ';
			//_info = NULL;
			_info = L"by Subaru";
			_vertexs = NULL;

			_header._header_info = UNKNOWN;
			_header._offset = 0;
			_header._vertexs = 0;
			_header._tris = 0;

			init();
		}
		//外部初始化变量
		WriteObject(vector< GLuint > *vec_tris, vector< float_3 > *vertexs)
		{
			_fp = new cFile;
			_vec_tris = vec_tris;
			_vertexs = vertexs;
			_info = L"by Subaru";

			init();
		}

		~WriteObject();

		void init();

		static WriteObject *GetWriteObjHandler()
		{
			return GWriteObjHandler;
		}

		static void Create()
		{
			GWriteObjHandler = new WriteObject();
		}

		//****外部接口，按顺序写入对象文件
		bool bDoWriteObject();

		//****外部传入顶点向量
		inline void SetVertexs(vector< float_3 > *vertexs){ _vertexs = vertexs; }

		//****外部传入三角形顶点索引向量
		inline void SetTris(vector< GLuint > *vec_tris){ _vec_tris = vec_tris; }

		//****外部传入对象名称
		void SetObjectName(wstring objectname);

		//****外部传入其它信息字符串指针
		inline void Set_other_info(TCHAR *info){ _info = info; }

		//****外部设置文件头信息
		inline void SetHeader(h_info header_info, int offset)
		{
			_header._header_info = header_info;
			_header._offset = offset;
			if (_vertexs)
			{
				_header._vertexs = _vertexs->size();
			}
			if (_vec_tris)
			{
				_header._tris = _vec_tris->size()/3;
			}
			
		}

	private:

		static WriteObject *GWriteObjHandler;

		//****写入对象其他信息
		bool Write_other_info();

		//****写入对象名称
		bool WriteObjectName();

		//****写入对象文件头
		bool WriteHeader();

		//****写入对象顶点
		bool WriteVertex();

		//****写入对象三角网索引
		bool WriteTris();

		//--辅助函数
		//按照坐标、行的形式写入文件：x y z \n
		void WriteLine(GLuint data_x, GLuint data_y, GLuint data_z);
		void WriteLine(float data_x, float data_y, float data_z);
		//标准化文件名
		const wchar_t* Set_o3d_Name(wstring name);
		//_fcvt函数返回的浮点字符串不符合要求，规范化字符串浮点数
		bool Set_str(char *h,int dec,int sign);

	private:

		cFile                             *_fp;  //创建文件对象

		vector< GLuint >		    *_vec_tris;  //保存索引三角形的向量

		wstring     		           _m_name;  //模型对象名称

		TCHAR                          * _info;  //文件的其他信息

		vector< float_3 >            *_vertexs;  //顶点数据



		TCHAR                             *h_i;//保存更改后的宽字符串
		char                            *h_i_c;//保存更改前的窄字符串
		char                              *h_v;  //保存更改后的窄字符串
		TCHAR                            block; //空格

		struct header
		{
			h_info    _header_info;  //顶点信息
			int            _offset;  //偏移量
			int           _vertexs;  //顶点数
			int              _tris;  //三角形数
			  
		}_header;
	};
	//获取全局唯一实例
	inline WriteObject *GetWriObjHandler()
	{
		return WriteObject::GetWriteObjHandler();
	}


#endif