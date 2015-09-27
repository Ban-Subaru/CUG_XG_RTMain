#include <vector>
#include <amp_graphics.h>

using namespace std;
using namespace concurrency;
using namespace concurrency::graphics;
#pragma once

#ifndef LIGHT_CONTROL_H_
#define LIGHT_CONTROL_H_

#define rad 57.31f  //定义弧度和角度之间的换算
#define light_index 100  //定义光影模拟系统光照颜色参数与OSG光照参数的比例系数
#define shadow_index -0.0003f  //定义光影模拟系统阴影参数与OSG光照参数的比例系数

	class LightControl
	{
	public:
		//带默认值
		LightControl(){ init(); }  

        //作为类的内部赋值之用
		LightControl(        
			float _h_angle,
			float _time,
			float _latitude,
			float _sun_latitude,
			float _t_angle,
			float _radius,
			float _animationLength):
			h_angle(_h_angle),
			time(_time), 
			latitude(_latitude), 
			sun_latitude(_sun_latitude), 
			t_angle(_t_angle), 
			radius(_radius), 
			animationLength(_animationLength){}

		//采用数组的方式传入参数
		LightControl(float *_v)
		{
			LightControl(_v[0],_v[1],_v[2],_v[3],_v[4],_v[5],_v[6]);
		}

		static void Create()
		{
			GLightConHandler = new LightControl();
		}

		inline void init()
		{
			//默认设置时间为18点
			time = 18;
			//默认设置地理纬度
			latitude = 43.77f;
			//默认时间在二月,根据一年当中的时间，计算太阳赤纬
			int days = 36;
			sun_latitude = ((23.44 / rad)*sinf(((360.0f*(284 + days)) / 365) / rad));
			// 太阳光旋转半径
			radius = 40000000000000000.0f;
			//循环一次的时间,一天是86400秒，再乘以二:172800.0f
			animationLength = 10.0f;

			//**
			//根据其它参数计算得到太阳时角
			t_angle = (((time - 12)*(15.0f / rad)));

			//**
			//根据其它参数计算得到太阳高度角
			h_angle = rad*asinf(sinf(latitude / rad)*sinf(sun_latitude) + cosf(latitude / rad)*cosf(sun_latitude)*cos(t_angle));
		}

		//----------
		void SetLightColor();//设置渲染光色

		inline float_3 *GetLightColor()//得到渲染光色
		{
			lightcolor[0] = color._shadowcolor;
			lightcolor[1] = color._lightcolor;
			return lightcolor;
		}

		static LightControl *GetLightConHandler()
		{
			return GLightConHandler;
		}

	private:
		static LightControl *GLightConHandler;

		union
		{
			struct
			{
				float h_angle;//太阳高度角
				float time; //时刻
				float latitude; // 地理纬度
				float sun_latitude; //太阳赤纬
				float t_angle;//太阳时角
				float radius;//太阳旋转半径
				float animationLength;//太阳旋转速度
		
			};

			float lc_v[7];
		};

	
		struct lcolor
		{
			float_3 _shadowcolor;  //阴影颜色
			float_3 _lightcolor;   //光色
		} color;

		float_3 lightcolor[2];  //保存渲染色，lightcolor[0]为阴影色,lightcolor[1]为光色
	};


	inline LightControl *GetLigConHandler()
	{
		return LightControl::GetLightConHandler();
	}

#endif