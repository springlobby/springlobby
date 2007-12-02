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


#ifndef SETTINGS_PRESETS_H_
#define SETTINGS_PRESETS_H_
#include <map>
#include <string>
#include <wx/string.h>
/**** Presets for tab_simple go here
 * 
 * 
 */
template <typename T> class presetValues 
{

public: 
	std::map<wxString,T> values;
		std::string key;
	
	//levelLabels and values_arg MUST have same length
		presetValues ( std::string key_arg,const wxString* levelLabels,const T* values_arg);
	
};
typedef std::map<std::string,int> intMap;

//TODO figure out why length = 3 doesnt work
const wxString levels_low_To_High[4] = { wxT("low"),wxT("medium"),wxT("high")};
const wxString levels_vlow_To_vHigh[5] = { wxT("very low"),wxT("low"),wxT("medium"),wxT("high"),wxT("very high")};

//TODO generally rethink preset values
/** RENDER_DETAIL ***************************************************************/
const int vl_ShadowMapSize[3] = 		{ 1024,		4096,	8192 	}; //shadowmapsize
const int vl_TreeRadius[3] = 			{ 600,		1900,	3000 	}; //tree view distance
const int vl_GroundDetail[3] =			{ 20, 		70,		120		}; //terrain detail
const int vl_UnitLodDist[3] =			{ 100,		350,	600		}; //unit detail
const int vl_GrassDetail[3] = 			{ 0,		5,		10		}; //grass detail
const int vl_GroundDecals[3] = 			{ 0,		50,		100		}; // ground decals
const int vl_UnitIconDist[3] = 			{ 100,		550,	1000	}; // unit icon distance
const int vl_MaxParticles[3] = 			{ 100,		9950,	20000	};
const int vl_SM3MaxTextureStages[3] = 	{ 1,		10,		20		};

const presetValues<int> pr_ShadowMapSize = presetValues<int>("ShadowMapSize",levels_low_To_High,vl_ShadowMapSize);
const presetValues<int> pr_TreeRadius = presetValues<int>("TreeRadius",levels_low_To_High,vl_TreeRadius);
const presetValues<int> pr_GroundDetail = presetValues<int>("GroundDetail",levels_low_To_High,vl_GroundDetail);
const presetValues<int> pr_UnitLodDist = presetValues<int>("UnitLodDist",levels_low_To_High,vl_UnitLodDist);
const presetValues<int> pr_GrassDetail = presetValues<int>("GrassDetail",levels_low_To_High,vl_GrassDetail);
const presetValues<int> pr_GroundDecals = presetValues<int>("GroundDecals",levels_low_To_High,vl_GroundDecals);
const presetValues<int> pr_UnitIconDist = presetValues<int>("UnitIconDist",levels_low_To_High,vl_UnitIconDist);
const presetValues<int> pr_MaxParticles = presetValues<int>("MaxParticles",levels_low_To_High,vl_MaxParticles);
const presetValues<int> pr_SM3MaxTextureStages = presetValues<int>("SM3MaxTextureStages",levels_low_To_High,vl_SM3MaxTextureStages);

const int prVal_RenderDetail_size = 9;
const presetValues<int> prVal_RenderDetail[prVal_RenderDetail_size] = {pr_ShadowMapSize, pr_TreeRadius, pr_GroundDetail,
		pr_UnitLodDist, pr_GrassDetail, pr_GroundDecals,
		pr_UnitIconDist, pr_MaxParticles, pr_SM3MaxTextureStages};

/** RENDER QUALITY **********************************************************/
const int vl_DepthBufferBits[5] = {16,16,16,24,24};
const int vl_ReflectiveWater[5] = {0,0,1,3,2};
const int vl_Shadows[5] = {0,0,0,0,1};
const int vl_3DTrees[5] = {0,1,1,1,1};
const int vl_AdvSky[5] = {0,0,1,1,1};//high-res clouds
const int vl_DynamicSky[5] = {0,0,0,1,1};//dynamic clouds
const int vl_SmoothPoints[5] = {0,1,1,1,1};
const int vl_SmoothLines[5] = {0,0,1,1,1};
const int vl_FSAA[5] = {0,1,1,1,1};//fullscreen aa enable/disable
const int vl_FSAALevel[5] = {0,1,2,8,16};//fullscreen aa samples
const int vl_AdvUnitShading[5] = {0,0,0,1,1}; //refl. units

const presetValues<int> pr_DepthBufferBits = presetValues<int>("DepthBufferBits",levels_vlow_To_vHigh,vl_DepthBufferBits);
const presetValues<int> pr_ReflectiveWater = presetValues<int>("ReflectiveWater",levels_vlow_To_vHigh,vl_ReflectiveWater);
const presetValues<int> pr_Shadows = presetValues<int>("Shadows",levels_vlow_To_vHigh,vl_Shadows);
const presetValues<int> pr_3DTrees = presetValues<int>("3DTrees",levels_vlow_To_vHigh,vl_3DTrees);
const presetValues<int> pr_AdvSky = presetValues<int>("AdvSky",levels_vlow_To_vHigh,vl_AdvSky); 
const presetValues<int> pr_DynamicSky = presetValues<int>("DynamicSky",levels_vlow_To_vHigh,vl_DynamicSky); 
const presetValues<int> pr_SmoothPoints = presetValues<int>("SmoothPoints",levels_vlow_To_vHigh,vl_SmoothPoints);
const presetValues<int> pr_SmoothLines = presetValues<int>("SmoothLines",levels_vlow_To_vHigh,vl_SmoothLines);
const presetValues<int> pr_FSAA = presetValues<int>("FSAA",levels_vlow_To_vHigh,vl_FSAA); 
const presetValues<int> pr_FSAALevel = presetValues<int>("FSAALevel",levels_vlow_To_vHigh,vl_FSAALevel); 
const presetValues<int> pr_AdvUnitShading = presetValues<int>("AdvUnitShading",levels_vlow_To_vHigh,vl_AdvUnitShading);

const int prVal_RenderQuality_size = 11;
const presetValues<int> prVal_RenderQuality[prVal_RenderQuality_size] = { pr_DepthBufferBits, pr_ReflectiveWater, pr_Shadows, pr_3DTrees,
		pr_AdvSky, pr_DynamicSky, pr_SmoothPoints, pr_SmoothLines,pr_FSAA, pr_FSAALevel, pr_AdvUnitShading };

//TODO add more video modes
/** VIDEO MODES ********************************************************/
const int vl_Resolution_X[3] = { 800, 1024, 1280 };
const int vl_Resolution_Y[3] = { 600,  768, 1024 };
const int vl_Resolution_Str_size = 3;
const wxString vl_Resolution_Str[vl_Resolution_Str_size] = { wxT("800 x 600"), wxT("1024 x 768"), wxT("1280 x 1024")};


#endif /*PRESETS_H_*/
