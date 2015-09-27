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

//�����ļ�ͷ��Ϣ
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
		//Ĭ�Ϲ��캯���ڲ���ʼ������
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
		//�ⲿ��ʼ������
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

		//****�ⲿ�ӿڣ���˳��д������ļ�
		bool bDoWriteObject();

		//****�ⲿ���붥������
		inline void SetVertexs(vector< float_3 > *vertexs){ _vertexs = vertexs; }

		//****�ⲿ���������ζ�����������
		inline void SetTris(vector< GLuint > *vec_tris){ _vec_tris = vec_tris; }

		//****�ⲿ�����������
		void SetObjectName(wstring objectname);

		//****�ⲿ����������Ϣ�ַ���ָ��
		inline void Set_other_info(TCHAR *info){ _info = info; }

		//****�ⲿ�����ļ�ͷ��Ϣ
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

		//****д�����������Ϣ
		bool Write_other_info();

		//****д���������
		bool WriteObjectName();

		//****д������ļ�ͷ
		bool WriteHeader();

		//****д����󶥵�
		bool WriteVertex();

		//****д���������������
		bool WriteTris();

		//--��������
		//�������ꡢ�е���ʽд���ļ���x y z \n
		void WriteLine(GLuint data_x, GLuint data_y, GLuint data_z);
		void WriteLine(float data_x, float data_y, float data_z);
		//��׼���ļ���
		const wchar_t* Set_o3d_Name(wstring name);
		//_fcvt�������صĸ����ַ���������Ҫ�󣬹淶���ַ���������
		bool Set_str(char *h,int dec,int sign);

	private:

		cFile                             *_fp;  //�����ļ�����

		vector< GLuint >		    *_vec_tris;  //�������������ε�����

		wstring     		           _m_name;  //ģ�Ͷ�������

		TCHAR                          * _info;  //�ļ���������Ϣ

		vector< float_3 >            *_vertexs;  //��������



		TCHAR                             *h_i;//������ĺ�Ŀ��ַ���
		char                            *h_i_c;//�������ǰ��խ�ַ���
		char                              *h_v;  //������ĺ��խ�ַ���
		TCHAR                            block; //�ո�

		struct header
		{
			h_info    _header_info;  //������Ϣ
			int            _offset;  //ƫ����
			int           _vertexs;  //������
			int              _tris;  //��������
			  
		}_header;
	};
	//��ȡȫ��Ψһʵ��
	inline WriteObject *GetWriObjHandler()
	{
		return WriteObject::GetWriteObjHandler();
	}


#endif