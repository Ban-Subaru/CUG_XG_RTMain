#include "RegisterCom.h"

RegisterObject* RegisterObject::GRegisterObjHandler = NULL;

RegisterObject::~RegisterObject()
{
	SafeRelease<RegisterObject>(GRegisterObjHandler);
	GVecArray->clear();
	GVecFloat->clear();
}


void RegisterObject::RegisterObjCom()
{
	//检测对象是否存在，若存在先释放
	if (ReadDEM::GetReadDEMHandler())
	{
		SafeRelease<ReadDEM>(ReadDEM::GetReadDEMHandler());
	}

	if (WriteObject::GetWriteObjHandler())
	{
		SafeRelease<WriteObject>(WriteObject::GetWriteObjHandler());
	}

	if (LightControl::GetLightConHandler())
	{
		SafeRelease<LightControl>(LightControl::GetLightConHandler());
	}

	if (GVecArray && GVecArray->size() > 0)
	{
		GVecArray->clear();
	}

	if (GVecFloat && GVecFloat->size() > 0)
	{
		GVecFloat->clear();
	}

	ReadDEM::Create();
	WriteObject::Create();
	LightControl::Create();
	pGReadDEMData = ReadDEM::GetReadDEMHandler();
	pGWriObjData = WriteObject::GetWriteObjHandler();
	pGLigCondata = LightControl::GetLightConHandler();
}


Vec3Array& RegisterObject::AlterFloat3ToVec3Array(vector<float_3>& vecFloat)
{
	vector<float_3>::iterator iter;
	for ( iter = vecFloat.begin(); iter != vecFloat.end(); iter++ )
	{
		GVecArray->push_back(Vec3(iter->r, iter->g, iter->b));
	}
	return *GVecArray;
}

vector<float_3>& RegisterObject::AlterVec3ArrayToFloat3(Vec3Array& vecArray)
{
	for (int i = 0; i < vecArray.size(); i++)
	{
		GVecFloat->push_back(float_3(vecArray[i].x(), vecArray[i].y(),vecArray[i].z()));
	}
	return *GVecFloat;
}

void RegisterObject::bRegisterObject()
{
	if (!pGReadDEMData)
	{
		assert("ReadDEM's object haven't been created!");
	}
	if (!pGWriObjData)
	{
		assert("WriteObject's object haven't been created!");
	}
	if (!pGLigCondata)
	{
		assert("LightControl's object haven't been created!");
	}
}

void RegisterObject::SetDataGrid(int reward, int vacuate, char *filepathname)
{
	pGReadDEMData->SetReward(reward);
	pGReadDEMData->SetVacuate(vacuate);
	pGReadDEMData->ReadGdalData(filepathname);//读取影像数据
	if (!pGReadDEMData->Getbuf_ptr())
	{
		assert("read file failed!");
		exit(1);
	}
}