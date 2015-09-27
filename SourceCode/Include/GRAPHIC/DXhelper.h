/*释放相应的DX资源*/
#ifndef _DXHELPER_H
#define _DXHELPER_H
#include "config.h"
template <class T>
struct sAutoZero : public T
{
	sAutoZero()
	{
		memset(this,0,sizeof(T));
	}
};


template <class T>
inline void SafeRelease( T& release)
{
	if (release)
	{
		release->Release();
		release = NULL;
	}
}
#endif //_DXHELPER_H