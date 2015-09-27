#ifndef RELEASE_HELPER_H
#define RELEASE_HELPER_H

template<typename T>
void SafeRelease(T* release)
{
	if (release)
	{
		delete release;
		release = NULL;
	}
}


#endif //RELEASE_HELPER_H