#include "WriteObject.h"

WriteObject *WriteObject::GWriteObjHandler = NULL;


WriteObject::~WriteObject()
{
	if (_fp != NULL)
		{
		cFile *p = _fp;
		delete p;
	}
	_fp = NULL;

	_vec_tris->clear();
	_vertexs->clear();
}

	void WriteObject::init()
	{
		h_i = new TCHAR[13];//使用未定义大小的临时宽字符串变量
		memset(h_i, '0', 13);
		h_i[13] = '\0';

	    h_i_c = new char[13];//使用未定义大小的临时宽字符串变量
		memset(h_i_c, '0', 13);
		h_i_c[13] = '\0';

		h_v = new char[13];
		memset(h_v, '0', 13);
		h_v[13] = '\0';
	    block = ' ';
    }

	void WriteObject::SetObjectName(wstring objectname)
	{
		wchar_t buf[64];
		memset(buf, 0, sizeof(buf));
		wcscpy(buf,objectname.c_str());
		wcscat(buf, L".o3d");
		//mbstowcs(Tbuf,buf,sizeof(buf) + 1);
		_m_name = buf;
	}

	const wchar_t *WriteObject::Set_o3d_Name(wstring name)
	{
		/*wchar_t *_name = name;
		size_t size = wcslen(_name);
		wchar_t * format = L".o3d";
		wchar_t *filepathname = (wchar_t *)wcscat_s(L"..\\", size+=4, _name);
		wchar_t *filepathnameformat = (wchar_t *)wcscat_s(filepathname, size+=5, format);
		return filepathnameformat;*/
		const wchar_t *_name = name.c_str();
		return _name;
	}


    //****写入对象其他信息
	bool WriteObject::Write_other_info()
	{
		if (_m_name[0] == ' ')
			return false;
		if (_info == NULL)
		{
			_fp->Create(_m_name.c_str());
			return false;
		}
		if (_fp == NULL)
			return false;

		
		_fp->Create(_m_name.c_str());
		TCHAR con= '#';
		_fp->WriteBuff(&con, sizeof(TCHAR));
		_fp->WriteBuff((void*)_info,wcslen(_info)*2);
		_fp->Write_n();
		return true;
	}

	bool WriteObject::WriteObjectName()
	{
		if (_m_name[0] == NULL) return false;
		wstring filename = _m_name;
		int strPos = -10;
		while (strPos != -1)
		{
			strPos = filename.find(L"\\");
			filename = filename.substr(strPos + 1, filename.length() - strPos - 1);
		}
		const TCHAR *str = filename.c_str();
		_fp->WriteBuff((void *)str,wcslen(str)<<1);
		_fp->WriteBuff(&block, sizeof(TCHAR));
		return true;
		
	}
	
	size_t Set_0(TCHAR *h_i)   //将_gcvt转换的不符合要求的字符串，标准化
	{
		int i = 0;
		while (h_i[i] != '.')
		{
			i++;
		}
		if (i <= 6)
		{
			int j = i;
			while (j < i + 7)
			{
				j++;
				if (h_i[j] == '\0')
				{
					h_i[j] = '0';
					h_i[j + 1] = '\0';
					return j + 1;
				}
			}
			return j;
		}
		else
		{
			while (i < 13)
			{
				i++;
				if (h_i[i] == '\0')
				{
					h_i[i] = '0';
					h_i[i + 1] = '\0';
					break;
				}

			}
			return i + 1;
		}
	}

	bool WriteObject::Set_str(char *h, int dec, int sign)
	{
		if (h == NULL) return false;

		if (dec == 0)
		{
			if (sign == 0)
			{
				strcpy(h_v,"0.");
				strcat(h_v,h);
				return true;
			}
			else
			{
				strcpy(h_v, "-0.");
				strcat(h_v, h);
				return true;
			}
		}

		else if (dec < 0)
		{
			if (sign == 0)
			{
				strcpy(h_v, "0.");
			}
			else
			{
				strcpy(h_v, "-0.");
			}
			int _dec = -dec;
				
			for (int i = _dec; i != 0; i--)
			{
				strcat(h_v,"0");
			}
			strcat(h_v, h);
			return true;
		}

		else
		{
			int len = strlen(h);
			if (sign == 0)
			{
				for (int i = 0; i < len; i++)
				{
					if (i != dec)
						h_v[i] = h[i];
					else
					{
						h_v[i] = '.';
						for (int j = i; h[j] != '\0'&&j < 12; j++)
						{
							h_v[j + 1] = h[j];
						}
						return true;
					}
					
				}
				return true;
			}
			else
			{
				strcpy(h_v, "-");
				for (int i = 0; i < len; i++)
				{
					if (i != dec)
						h_v[i+1] = h[i];
					else
						h_v[i+1] = '.';
					for (int j = i; h[j] != '\0'&&j < 11; j++)
					{
						h_v[j + 2] = h[j];
					}
					return true;
				}
				return true;
			}

		}

	}

	bool WriteObject::WriteHeader()//写入文件头
	{
		int i;
		
		if (_header._header_info == UNKNOWN) return false;
		//***
		if (_header._header_info != UNKNOWN)
		{
			_itow(_header._header_info, h_i, 10);
			i = wcslen(h_i) << 1;   //此处移位运算表示*2
			_fp->WriteBuff(h_i, i);
			_fp->WriteBuff(&block, sizeof(TCHAR));
	    }
		//***
		_itow(_header._offset, h_i, 10);
		i = wcslen(h_i) << 1;
		_fp->WriteBuff(h_i, i);
		_fp->WriteBuff(&block, sizeof(TCHAR));
		//***
		_itow(_header._vertexs, h_i, 10);
		i = wcslen(h_i) << 1;
		_fp->WriteBuff(h_i, i);
		_fp->WriteBuff(&block, sizeof(TCHAR));
		//***
		_itow(_header._tris, h_i, 10);
		i = wcslen(h_i) << 1;
		_fp->WriteBuff(h_i, i);

		_fp->Write_n();

		return true;
		
	}

	void WriteObject::WriteLine(GLuint data_x,GLuint data_y,GLuint data_z)
	{
		int i;
		_itow(data_x, h_i, 10);
		i = wcslen(h_i) << 1;
		_fp->WriteBuff(h_i,i);
		_fp->WriteBuff(&block, sizeof(TCHAR));

		_itow(data_y, h_i, 10);
		i = wcslen(h_i) << 1;
		_fp->WriteBuff(h_i, i);
		_fp->WriteBuff(&block, sizeof(TCHAR));

		_itow(data_z, h_i, 10);
		i = wcslen(h_i) << 1;
		_fp->WriteBuff(h_i, i);
		_fp->Write_n();
	}
	void WriteObject::WriteLine(float data_x, float data_y, float data_z)
	{
		

		int i,dec,sign;
		h_i_c = _fcvt(data_x, 6, &dec,&sign);
		Set_str(h_i_c,dec,sign);
		mbstowcs(h_i, h_v, strlen(h_v) + 1);
		i = wcslen(h_i)<<1;
		_fp->WriteBuff(h_i, i);
		_fp->WriteBuff(&block, sizeof(TCHAR));

		h_i_c = _fcvt(data_y, 6, &dec, &sign);
		Set_str(h_i_c, dec, sign);
		mbstowcs(h_i, h_v, strlen(h_v) + 1);
		i = wcslen(h_i) << 1;
		_fp->WriteBuff(h_i, i);
		_fp->WriteBuff(&block, sizeof(TCHAR));

		h_i_c = _fcvt(data_z, 6, &dec, &sign);
		Set_str(h_i_c, dec, sign);
		mbstowcs(h_i, h_v, strlen(h_v) + 1);
		i = wcslen(h_i) << 1;
		_fp->WriteBuff(h_i, i);
		_fp->Write_n();
	}

	bool WriteObject::WriteVertex()
	{
		if (_header._header_info == UNKNOWN) return false;
		vector<float_3>::iterator ve;
		for (ve = _vertexs->begin(); ve != _vertexs->end();ve++)
		{
			WriteLine(ve->r, ve->g, ve->b);
		}
		
		return true;
	}

	bool WriteObject::WriteTris()
	{
		if (_header._header_info == UNKNOWN) 
			return false;
		if ((_vec_tris->size() % 3) != 0) 
			return false;

		vector< GLuint >::iterator tri;
		GLuint j;
		for (tri = _vec_tris->begin(),j = 0; j <_vec_tris->size()-1;j++)
		{
			WriteLine(tri[j], tri[j++], tri[j++]);
		}
		return true;

	}

	bool WriteObject::bDoWriteObject()
	{
		Write_other_info();  //其它信息不是必要信息
		if (WriteObjectName() == true)
		{
			if (WriteHeader() == true)
			{
				if (WriteVertex() == true)
				{
					if (WriteTris() == true)
					{
						_fp->Close();
						return true;
						
					}
					return false;
				}
				return false;
			}
			return false;
		}
		return false;
		
	}
