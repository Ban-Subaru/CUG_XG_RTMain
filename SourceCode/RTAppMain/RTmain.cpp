#include "LightControl.h"
#include "Drawable.h"
#include "ReadDEM.h"
#include "DelaunayTriangulator.h"
#include "WriteObject.h"
#include "RegisterCom.h"
#include <iostream>
using namespace osg;
using namespace std;
/*
*by����Subaru
*/
//int main()
//{
//
//RegisterObject::Create();
//RegisterObject::GetRegisterObjectHandler()->RegisterObjCom();
////����Ӱ���ϡ����ǿ����,����ȡ�ļ�·��
//RegisterObject::GetRegisterObjectHandler()->SetDataGrid(100, 100, "..\\..\\OutPut\\Data\\MEDIA\\srtm_59_04.tif");
////������������ӵ�����
//vector<float_3>& vecFloat = RegisterObject::GetRegisterObjectHandler()->pGReadDEMData->CreateCoordsandAddtoVec();
////ת�����ݲ����浽�����
//Vec3Array& vecArray = RegisterObject::GetRegisterObjectHandler()->AlterFloat3ToVec3Array(vecFloat);
//
////����Delaunay������������
//DelaunayTriangulator *dt = new DelaunayTriangulator(&vecArray);
//
////������������������������,���涥����Ϣ��WriteObject
//RegisterObject::GetRegisterObjectHandler()->pGWriObjData->SetTris(&dt->triangulate(RegisterObject::GetRegisterObjectHandler()->GVecArray));
////ת�����ݲ����浽�����
//RegisterObject::GetRegisterObjectHandler()->AlterVec3ArrayToFloat3(*RegisterObject::GetRegisterObjectHandler()->GVecArray);
//RegisterObject::GetRegisterObjectHandler()->pGWriObjData->SetVertexs(RegisterObject::GetRegisterObjectHandler()->GVecFloat);
////���ñ���·�����ļ���
//RegisterObject::GetRegisterObjectHandler()->pGWriObjData->SetObjectName(L"..\\..\\OutPut\\Data\\MEDIA\\atestterrain");
//RegisterObject::GetRegisterObjectHandler()->ToWriteComplete();
//
//}