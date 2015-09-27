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
*by——Subaru
*/
//int main()
//{
//
//RegisterObject::Create();
//RegisterObject::GetRegisterObjectHandler()->RegisterObjCom();
////设置影像抽稀和增强比例,及读取文件路径
//RegisterObject::GetRegisterObjectHandler()->SetDataGrid(100, 100, "..\\..\\OutPut\\Data\\MEDIA\\srtm_59_04.tif");
////将坐标数据添加到向量
//vector<float_3>& vecFloat = RegisterObject::GetRegisterObjectHandler()->pGReadDEMData->CreateCoordsandAddtoVec();
////转换数据并保存到组件类
//Vec3Array& vecArray = RegisterObject::GetRegisterObjectHandler()->AlterFloat3ToVec3Array(vecFloat);
//
////创建Delaunay三角网生成器
//DelaunayTriangulator *dt = new DelaunayTriangulator(&vecArray);
//
////生成索引三角网并存入向量,保存顶点信息到WriteObject
//RegisterObject::GetRegisterObjectHandler()->pGWriObjData->SetTris(&dt->triangulate(RegisterObject::GetRegisterObjectHandler()->GVecArray));
////转换数据并保存到组件类
//RegisterObject::GetRegisterObjectHandler()->AlterVec3ArrayToFloat3(*RegisterObject::GetRegisterObjectHandler()->GVecArray);
//RegisterObject::GetRegisterObjectHandler()->pGWriObjData->SetVertexs(RegisterObject::GetRegisterObjectHandler()->GVecFloat);
////设置保存路径及文件名
//RegisterObject::GetRegisterObjectHandler()->pGWriObjData->SetObjectName(L"..\\..\\OutPut\\Data\\MEDIA\\atestterrain");
//RegisterObject::GetRegisterObjectHandler()->ToWriteComplete();
//
//}