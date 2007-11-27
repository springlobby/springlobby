/**
    This file is part of Settings++,
    Copyright (C) 2007
    Original work by Kloot
    cross-plattform/UI adaptation and currently maintained by koshi (Ren� Milk)
    visit http://spring.clan-sy.com/phpbb/viewtopic.php?t=12104
    for more info/help

    Settings++ is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Settings++ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Settings++.  If not, see <http://www.gnu.org/licenses/>.
**/


#ifndef PRESETS_H_
#define PRESETS_H_
#include <map>
#include <string>
/**** Presets for tab_simple go here
 * 
 * 
 */
template <typename T> class presetValues 
{
	std::map<wxString,T> values;
	std::string key;
	//T def;
public: 
	//levelLabels and values_arg MUST have same length
	presetValues (std::string key_arg,wxString* levelLabels,T* values_arg)
	{
		for(int i = 0; i< sizeof(levelLabels);++i)
		{
			values[levelLabels[i]]=values_arg[i];
		}
		key = key_arg;
		//def = def_arg;
	}
	
};
typedef std::map<std::string,int> intMap;

wxString low_To_High[3] = { wxT("low"),wxT("medium"),wxT("high")};
wxString vlow_To_vHigh[5] = { wxT("very low"),wxT("low"),wxT("medium"),wxT("high"),wxT("very high")};

/** RENDER_DETAIL ***************************************************************/
int vl_ShadowMapSize[3] = 		{ 2048,		4096,	8192 	}; //shadowmapsize
int vl_TreeRadius[3] = 			{ 600,		1800,	3000 	}; //tree view distance
int vl_GroundDetail[3] =		{ 20, 		70,		120		}; //terrain detail
int vl_UnitLodDist[3] =			{ 100,		350,	600		}; //unit detail
int vl_GrassDetail[3] = 		{ 0,		5,		10		}; //grass detail
int vl_GroundDecals[3] = 		{ 0,		50,		100		}; // ground decals
int vl_UnitIconDist[3] = 		{ 100,		550,	1000	}; // unit icon distance
int vl_MaxParticles[3] = 		{ 100,		9950,	20000	};
int vl_SM3MaxTextureStages[3] = { 1,		10,		20		};

presetValues<int> pr_ShadowMapSize = presetValues<int>("ShadowMapSize",low_To_High,vl_ShadowMapSize);
presetValues<int> pr_TreeRadius = presetValues<int>("TreeRadius",low_To_High,vl_TreeRadius);
presetValues<int> pr_GroundDetail = presetValues<int>("GroundDetail",low_To_High,vl_GroundDetail);
presetValues<int> pr_UnitLodDist = presetValues<int>("UnitLodDist",low_To_High,vl_UnitLodDist);
presetValues<int> pr_GrassDetail = presetValues<int>("GrassDetail",low_To_High,vl_GrassDetail);
presetValues<int> pr_GroundDecals = presetValues<int>("GroundDecals",low_To_High,vl_GroundDecals);
presetValues<int> pr_UnitIconDist = presetValues<int>("UnitIconDist",low_To_High,vl_UnitIconDist);
presetValues<int> pr_MaxParticles = presetValues<int>("MaxParticles",low_To_High,vl_MaxParticles);
presetValues<int> pr_SM3MaxTextureStages = presetValues<int>("SM3MaxTextureStages",low_To_High,vl_SM3MaxTextureStages);

presetValues<int> prVal_RenderDetail[9] = {pr_ShadowMapSize, pr_TreeRadius, pr_GroundDetail,
		pr_UnitLodDist, pr_GrassDetail, pr_GroundDecals,
		pr_UnitIconDist, pr_MaxParticles, pr_SM3MaxTextureStages};

/** RENDER QUALITY **********************************************************/
int vl_DepthBufferBits[5] = {16,16,16,24,24};
int vl_ReflectiveWater[5] = {0,0,1,3,2};
int vl_Shadows[5] = {0,0,0,0,1};
int vl_3DTrees[5] = {0,1,1,1,1};
int vl_AdvSky[5] = {0,0,1,1,1};//high-res clouds
int vl_DynamicSky[5] = {0,0,0,1,1};//dynamic clouds
int vl_SmoothPoints[5] = {0,1,1,1,1};
int vl_SmoothLines[5] = {0,0,1,1,1};
int vl_FSAALevel[5] = {0,1,2,8,16};//fullscreen aa samples
int vl_AdvUnitShading[5] = {0,0,0,1,1}; //refl. units
//int vl_[5] = {};
presetValues<int> pr_DepthBufferBits = presetValues<int>("DepthBufferBits",vlow_To_vHigh,vl_DepthBufferBits);
presetValues<int> pr_ReflectiveWater = presetValues<int>("ReflectiveWater",vlow_To_vHigh,vl_ReflectiveWater);
presetValues<int> pr_Shadows = presetValues<int>("Shadows",vlow_To_vHigh,vl_Shadows);
presetValues<int> pr_3DTrees = presetValues<int>("3DTrees",vlow_To_vHigh,vl_3DTrees);
presetValues<int> pr_AdvSky = presetValues<int>("AdvSky",vlow_To_vHigh,vl_AdvSky); 
presetValues<int> pr_DynamicSky = presetValues<int>("DynamicSky",vlow_To_vHigh,vl_DynamicSky); 
presetValues<int> pr_SmoothPoints = presetValues<int>("SmoothPoints",vlow_To_vHigh,vl_SmoothPoints);
presetValues<int> pr_SmoothLines = presetValues<int>("SmoothLines",vlow_To_vHigh,vl_SmoothLines);
presetValues<int> pr_FSAALevel = presetValues<int>("FSAALevel",vlow_To_vHigh,vl_FSAALevel); 
presetValues<int> pr_AdvUnitShading = presetValues<int>("AdvUnitShading",vlow_To_vHigh,vl_AdvUnitShading);

presetValues<int> prVal_RenderQuality[10] = { pr_DepthBufferBits, pr_ReflectiveWater, pr_Shadows, pr_3DTrees,
		pr_AdvSky, pr_DynamicSky, pr_SmoothPoints, pr_SmoothLines, pr_FSAALevel, pr_AdvUnitShading };

/** VIDEO MODES ********************************************************/
int vl_Resolution_X[3] = { 800, 1024, 1280 };
int vl_Resolution_Y[3] = { 600,  768, 1024 };
wxString vl_Resolution_Str[3] = { wxT("800 x 600"), wxT("1024 x 768"), wxT("1280 x 1024")};


#endif /*PRESETS_H_*/
