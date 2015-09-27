//-#include "DXUT.h"
#include "LightControl.h"

LightControl *LightControl::GLightConHandler = NULL;

	void LightControl::SetLightColor()
	{
		//**阴影色计算
		float_3 v3_1(-1.9316f, -1.9849f, -2.0077f);
		float_3 v3_2(198.45f, 198.58f, 198.45f);
		//根据太阳高度角计算RGB的值
		color._shadowcolor = (((v3_1*h_angle) + v3_2));

		//如果颜色值有任意一个小于零，则将光的颜色置为黑色（黑夜）
		if (color._shadowcolor.r < 0 || color._shadowcolor.g < 0 || color._shadowcolor.b < 0)
		{
			color._shadowcolor.r = 0.0f;
			color._shadowcolor.g = 0.0f;
			color._shadowcolor.b = 0.0f;

			//颜色信息统计
			/*OSG_NOTICE << "阴影颜色值：" << std::endl;
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
			//计算太阳光色
			color._lightcolor = v3_1*h_angle*h_angle + v3_2*h_angle + v3_3;

			//如果颜色值有任意一个小于零，则将光的颜色置为黑色（黑夜）
			if (color._lightcolor.r < 0 || color._lightcolor.g < 0 || color._lightcolor.b < 0)
			{
				color._lightcolor.r = 0.0f;
				color._lightcolor.g = 0.0f;
				color._lightcolor.b = 0.0f;
			}

				

				/*OSG_NOTICE << "光颜色值：" << std::endl;
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
			//计算太阳光色
			color._lightcolor = v3_1*h_angle*h_angle + v3_2*h_angle + v3_3;

			//如果颜色值有任意一个小于零，则将光的颜色置为黑色（黑夜）
			if (color._lightcolor.r < 0 || color._lightcolor.g < 0 || color._lightcolor.b < 0)
			{
				color._lightcolor.r = 0.0f;
				color._lightcolor.g = 0.0f;
				color._lightcolor.b = 0.0f;
			}
				
			//显示默认数值
			/*OSG_NOTICE << "光颜色值：" << std::endl;
			OSG_NOTICE << "|R:" << v3_rgb[0] << std::endl;
			OSG_NOTICE << "|G:" << v3_rgb[1] << std::endl;
			OSG_NOTICE << "|B:" << v3_rgb[2] << std::endl;
			OSG_NOTICE << "|----------------------------------------" << std::endl;
			Show_init();*/
			
			
		}
		else
		{
			//OSG_NOTICE << "error! 时间参数不能为负数！" << std::endl;
			exit(1);
		}
	}
