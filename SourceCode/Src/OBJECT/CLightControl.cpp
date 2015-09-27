//-#include "DXUT.h"
#include "LightControl.h"

LightControl *LightControl::GLightConHandler = NULL;

	void LightControl::SetLightColor()
	{
		//**��Ӱɫ����
		float_3 v3_1(-1.9316f, -1.9849f, -2.0077f);
		float_3 v3_2(198.45f, 198.58f, 198.45f);
		//����̫���߶ȽǼ���RGB��ֵ
		color._shadowcolor = (((v3_1*h_angle) + v3_2));

		//�����ɫֵ������һ��С���㣬�򽫹����ɫ��Ϊ��ɫ����ҹ��
		if (color._shadowcolor.r < 0 || color._shadowcolor.g < 0 || color._shadowcolor.b < 0)
		{
			color._shadowcolor.r = 0.0f;
			color._shadowcolor.g = 0.0f;
			color._shadowcolor.b = 0.0f;

			//��ɫ��Ϣͳ��
			/*OSG_NOTICE << "��Ӱ��ɫֵ��" << std::endl;
			OSG_NOTICE << "|R:" << v3_rgb[0] << std::endl;
			OSG_NOTICE << "|G:" << v3_rgb[1] << std::endl;
			OSG_NOTICE << "|B:" << v3_rgb[2] << std::endl;
			OSG_NOTICE << "|----------------------------------------" << std::endl;*/
		}



		if (time < 12 && time >= 0)
		{
			float_3 v3_1(-0.001f, -0.0236f, -0.0093f);
			float_3 v3_2(0.3644f, 3.573f, 3.4549f);
			float_3 v3_3(230.65f, 118.8f, 24.977f);
			//����̫����ɫ
			color._lightcolor = v3_1*h_angle*h_angle + v3_2*h_angle + v3_3;

			//�����ɫֵ������һ��С���㣬�򽫹����ɫ��Ϊ��ɫ����ҹ��
			if (color._lightcolor.r < 0 || color._lightcolor.g < 0 || color._lightcolor.b < 0)
			{
				color._lightcolor.r = 0.0f;
				color._lightcolor.g = 0.0f;
				color._lightcolor.b = 0.0f;
			}

				

				/*OSG_NOTICE << "����ɫֵ��" << std::endl;
				OSG_NOTICE << "|R:" << v3_rgb[0] << std::endl;
				OSG_NOTICE << "|G:" << v3_rgb[1] << std::endl;
				OSG_NOTICE << "|B:" << v3_rgb[2] << std::endl;
				OSG_NOTICE << "|----------------------------------------" << std::endl;
				Show_init();*/
			
		}
		else if (time >= 12)
		{
			
			float_3 v3_1(-0.0016f, -0.008f, -0.0013f);
			float_3 v3_2(0.4378f, 2.4326f, 2.646f);
			float_3 v3_3(228.98f, 104.39f, 34.681f);
			//����̫����ɫ
			color._lightcolor = v3_1*h_angle*h_angle + v3_2*h_angle + v3_3;

			//�����ɫֵ������һ��С���㣬�򽫹����ɫ��Ϊ��ɫ����ҹ��
			if (color._lightcolor.r < 0 || color._lightcolor.g < 0 || color._lightcolor.b < 0)
			{
				color._lightcolor.r = 0.0f;
				color._lightcolor.g = 0.0f;
				color._lightcolor.b = 0.0f;
			}
				
			//��ʾĬ����ֵ
			/*OSG_NOTICE << "����ɫֵ��" << std::endl;
			OSG_NOTICE << "|R:" << v3_rgb[0] << std::endl;
			OSG_NOTICE << "|G:" << v3_rgb[1] << std::endl;
			OSG_NOTICE << "|B:" << v3_rgb[2] << std::endl;
			OSG_NOTICE << "|----------------------------------------" << std::endl;
			Show_init();*/
			
			
		}
		else
		{
			//OSG_NOTICE << "error! ʱ���������Ϊ������" << std::endl;
			exit(1);
		}
	}
