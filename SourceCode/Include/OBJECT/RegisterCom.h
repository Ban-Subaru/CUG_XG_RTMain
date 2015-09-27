#include "LightControl.h"
#include "Drawable.h"
#include "ReadDEM.h"
#include "DelaunayTriangulator.h"
#include "WriteObject.h"
#include "release_Helper.h"
#include "assert.h"
#include <iostream>
using namespace osg;
using namespace std;

class RegisterObject
{
public:
	RegisterObject(){}
	~RegisterObject();
	//register object
	void RegisterObjCom();
	//float_3 to Vec3Array
	Vec3Array& AlterFloat3ToVec3Array(vector<float_3>& vecFloat);
	//Vec3Array to float_3
	vector<float_3>& AlterVec3ArrayToFloat3(Vec3Array& vecArray);

	static void Create()
	{
		if (GRegisterObjHandler)
		{
			SafeRelease<RegisterObject>(GRegisterObjHandler);
		}
		GRegisterObjHandler = new RegisterObject();
		GRegisterObjHandler->GVecArray = new Vec3Array();
		GRegisterObjHandler->GVecFloat = new vector<float_3>;
	}

	static RegisterObject *GetRegisterObjectHandler()
	{
		GRegisterObjHandler->bRegisterObject();
		return GRegisterObjHandler;
	}
	void bRegisterObject();
	
	ReadDEM			*pGReadDEMData;
	WriteObject		 *pGWriObjData;
	LightControl     *pGLigCondata;

	Vec3Array         *GVecArray;
	vector<float_3>   *GVecFloat;
	//-----------------------
	//影像数据操作
	void SetDataGrid(int reward, int vacuate, char *filepathname);
	void ToWriteComplete()
	{
		//设置文件信息属性
		pGWriObjData->SetHeader(COORDS3_ONLY, 0);

		if (pGWriObjData->bDoWriteObject() == true)
		{
			cout << "success!" << endl;
		}
		else
		{
			cout << "failed!" << endl;
		}
		exit(1);
	}
private:
	static RegisterObject *GRegisterObjHandler;
};


inline static RegisterObject *GetGRegisterObjHandler()
{
	return RegisterObject::GetRegisterObjectHandler();
}




