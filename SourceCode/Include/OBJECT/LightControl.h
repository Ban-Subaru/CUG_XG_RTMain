#include <vector>
#include <amp_graphics.h>

using namespace std;
using namespace concurrency;
using namespace concurrency::graphics;
#pragma once

#ifndef LIGHT_CONTROL_H_
#define LIGHT_CONTROL_H_

#define rad 57.31f  //���廡�ȺͽǶ�֮��Ļ���
#define light_index 100  //�����Ӱģ��ϵͳ������ɫ������OSG���ղ����ı���ϵ��
#define shadow_index -0.0003f  //�����Ӱģ��ϵͳ��Ӱ������OSG���ղ����ı���ϵ��

	class LightControl
	{
	public:
		//��Ĭ��ֵ
		LightControl(){ init(); }  

        //��Ϊ����ڲ���ֵ֮��
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

		//��������ķ�ʽ�������
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
			//Ĭ������ʱ��Ϊ18��
			time = 18;
			//Ĭ�����õ���γ��
			latitude = 43.77f;
			//Ĭ��ʱ���ڶ���,����һ�굱�е�ʱ�䣬����̫����γ
			int days = 36;
			sun_latitude = ((23.44 / rad)*sinf(((360.0f*(284 + days)) / 365) / rad));
			// ̫������ת�뾶
			radius = 40000000000000000.0f;
			//ѭ��һ�ε�ʱ��,һ����86400�룬�ٳ��Զ�:172800.0f
			animationLength = 10.0f;

			//**
			//����������������õ�̫��ʱ��
			t_angle = (((time - 12)*(15.0f / rad)));

			//**
			//����������������õ�̫���߶Ƚ�
			h_angle = rad*asinf(sinf(latitude / rad)*sinf(sun_latitude) + cosf(latitude / rad)*cosf(sun_latitude)*cos(t_angle));
		}

		//----------
		void SetLightColor();//������Ⱦ��ɫ

		inline float_3 *GetLightColor()//�õ���Ⱦ��ɫ
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
				float h_angle;//̫���߶Ƚ�
				float time; //ʱ��
				float latitude; // ����γ��
				float sun_latitude; //̫����γ
				float t_angle;//̫��ʱ��
				float radius;//̫����ת�뾶
				float animationLength;//̫����ת�ٶ�
		
			};

			float lc_v[7];
		};

	
		struct lcolor
		{
			float_3 _shadowcolor;  //��Ӱ��ɫ
			float_3 _lightcolor;   //��ɫ
		} color;

		float_3 lightcolor[2];  //������Ⱦɫ��lightcolor[0]Ϊ��Ӱɫ,lightcolor[1]Ϊ��ɫ
	};


	inline LightControl *GetLigConHandler()
	{
		return LightControl::GetLightConHandler();
	}

#endif