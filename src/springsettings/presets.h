/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SETTINGS_PRESETS_H_
#define SETTINGS_PRESETS_H_
#include <wx/intl.h>
#include <map>
#include <string>
/** \brief Presets for tab_simple go here
 * presets are only used for performance relevant (render detail + AA) settings
 * and screen resolutions
 */
template <typename T, int valueCount>
class presetValues
{

public:
	std::map<wxString, T> values;
	wxString key;

	//levelLabels and values_arg MUST have same length
	presetValues(wxString key_arg, const wxString* levelLabels, const T* values_arg)
	{
		for (int i = 0; i < valueCount; ++i) {
			values[levelLabels[i]] = values_arg[i];
		}
		key = key_arg;
	}
};
const int levels_low_To_High_size = 3;
const int levels_vlow_To_vHigh_size = 5;
const wxString levels_low_To_High[levels_low_To_High_size] = {_("low"), _("medium"), _("high")};
const wxString levels_vlow_To_vHigh[levels_vlow_To_vHigh_size] = {_("very low"), _("low"), _("medium"), _("high"), _("very high")};

//TODO generally rethink preset values
/** RENDER_DETAIL ***************************************************************/
const int vl_ShadowMapSize[3] = {1024, 4096, 8192}; //shadowmapsize
const int vl_TreeRadius[3] = {600, 1900, 3000};     //tree view distance
const int vl_GroundDetail[3] = {20, 70, 120};       //terrain detail
const int vl_UnitLodDist[3] = {100, 350, 1000};     //unit LOD distance
const int vl_GrassDetail[3] = {0, 15, 30};	  //grass detail
const int vl_GroundDecals[3] = {0, 0, 1};	   // ground decals
const int vl_UnitIconDist[3] = {100, 550, 1000};    // unit icon distance
const int vl_MaxParticles[3] = {100, 4000, 20000};
const int vl_MaxNanoParticles[3] = {100, 6000, 20000};
//const int vl_SM3MaxTextureStages[3] = 	{ 1,		6,		20		};

const presetValues<int, 3> pr_ShadowMapSize = presetValues<int, 3>(_T("ShadowMapSize"), levels_low_To_High, vl_ShadowMapSize);
const presetValues<int, 3> pr_TreeRadius = presetValues<int, 3>(_T("TreeRadius"), levels_low_To_High, vl_TreeRadius);
const presetValues<int, 3> pr_GroundDetail = presetValues<int, 3>(_T("GroundDetail"), levels_low_To_High, vl_GroundDetail);
const presetValues<int, 3> pr_UnitLodDist = presetValues<int, 3>(_T("UnitLodDist"), levels_low_To_High, vl_UnitLodDist);
const presetValues<int, 3> pr_GrassDetail = presetValues<int, 3>(_T("GrassDetail"), levels_low_To_High, vl_GrassDetail);
const presetValues<int, 3> pr_GroundDecals = presetValues<int, 3>(_T("GroundDecals"), levels_low_To_High, vl_GroundDecals);
const presetValues<int, 3> pr_UnitIconDist = presetValues<int, 3>(_T("UnitIconDist"), levels_low_To_High, vl_UnitIconDist);
const presetValues<int, 3> pr_MaxParticles = presetValues<int, 3>(_T("MaxParticles"), levels_low_To_High, vl_MaxParticles);
const presetValues<int, 3> pr_MaxNanoParticles = presetValues<int, 3>(_T("MaxNanoParticles"), levels_low_To_High, vl_MaxNanoParticles);
//const presetValues<int, 3> pr_SM3MaxTextureStages = presetValues<int, 3>(wxT("SM3MaxTextureStages"),levels_low_To_High,vl_SM3MaxTextureStages);

const int prVal_RenderDetail_size = 9;
const presetValues<int, 3> prVal_RenderDetail[prVal_RenderDetail_size] = {pr_ShadowMapSize, pr_TreeRadius, pr_GroundDetail,
									  pr_UnitLodDist, pr_GrassDetail, pr_GroundDecals,
									  pr_UnitIconDist, pr_MaxParticles, pr_MaxNanoParticles};

/** RENDER QUALITY **********************************************************/
const int vl_DepthBufferBits[5] = {16, 16, 16, 24, 24};
const int vl_ReflectiveWater[5] = {0, 0, 1, 3, 2};
const int vl_Shadows[5] = {0, 0, 0, 0, 1};
const int vl_3DTrees[5] = {0, 1, 1, 1, 1};
const int vl_AdvSky[5] = {0, 0, 0, 0, 1};     //high-res clouds
const int vl_DynamicSky[5] = {0, 0, 0, 0, 1}; //dynamic clouds
const int vl_SmoothPoints[5] = {0, 1, 1, 1, 1};
const int vl_SmoothLines[5] = {0, 0, 1, 1, 1};
const int vl_FSAA[5] = {0, 0, 0, 0, 1};		  //fullscreen aa enable/disable
const int vl_FSAALevel[5] = {0, 0, 0, 0, 1};      //fullscene aa samples
const int vl_AdvUnitShading[5] = {0, 0, 0, 1, 1}; //refl. units

const presetValues<int, 5> pr_DepthBufferBits = presetValues<int, 5>(wxT("DepthBufferBits"), levels_vlow_To_vHigh, vl_DepthBufferBits);
const presetValues<int, 5> pr_ReflectiveWater = presetValues<int, 5>(_T("ReflectiveWater"), levels_vlow_To_vHigh, vl_ReflectiveWater);
const presetValues<int, 5> pr_Shadows = presetValues<int, 5>(_T("Shadows"), levels_vlow_To_vHigh, vl_Shadows);
const presetValues<int, 5> pr_3DTrees = presetValues<int, 5>(_T("3DTrees"), levels_vlow_To_vHigh, vl_3DTrees);
const presetValues<int, 5> pr_AdvSky = presetValues<int, 5>(_T("AdvSky"), levels_vlow_To_vHigh, vl_AdvSky);
const presetValues<int, 5> pr_DynamicSky = presetValues<int, 5>(_T("DynamicSky"), levels_vlow_To_vHigh, vl_DynamicSky);
const presetValues<int, 5> pr_SmoothPoints = presetValues<int, 5>(_T("SmoothPoints"), levels_vlow_To_vHigh, vl_SmoothPoints);
const presetValues<int, 5> pr_SmoothLines = presetValues<int, 5>(_T("SmoothLines"), levels_vlow_To_vHigh, vl_SmoothLines);
const presetValues<int, 5> pr_FSAA = presetValues<int, 5>(_T("FSAA"), levels_vlow_To_vHigh, vl_FSAA);
const presetValues<int, 5> pr_FSAALevel = presetValues<int, 5>(_T("FSAALevel"), levels_vlow_To_vHigh, vl_FSAALevel);
const presetValues<int, 5> pr_AdvUnitShading = presetValues<int, 5>(_T("AdvUnitShading"), levels_vlow_To_vHigh, vl_AdvUnitShading);

const int prVal_RenderQuality_size = 11;
const presetValues<int, 5> prVal_RenderQuality[prVal_RenderQuality_size] = {pr_DepthBufferBits, pr_ReflectiveWater, pr_Shadows, pr_3DTrees,
									    pr_AdvSky, pr_DynamicSky, pr_SmoothPoints, pr_SmoothLines, pr_FSAA, pr_FSAALevel, pr_AdvUnitShading};


/** VIDEO MODES ********************************************************/
const int vl_Resolution_Str_size = 13;
const int vl_Resolution_X[vl_Resolution_Str_size] = {800, 1024, 1152, 1280, 1280, 1600, 1280, 1440, 1680, 1920, 2048, 2560, 3200};
const int vl_Resolution_Y[vl_Resolution_Str_size] = {600, 768, 864, 960, 1024, 1200, 800, 900, 1050, 1200, 768, 1024, 1200};
const int vl_Resolution_startOfDualScreenRes = 9;
//must be same order
const wxString vl_Resolution_Str[vl_Resolution_Str_size] =
    {_T("800 x 600"), _T("1024 x 768"), _T("1152 x 864"), _T("1280 x 960"), _T("1280 x 1024"), _T("1600 x 1200"), _T("1280 x 800 (widescreen)"), _T("1440 x 900 (widescreen)"), _T("1680 x 1050 (widescreen)"), _T("1920 x 1200 (widescreen)"), _T("2048 x 768 (dual-screen)"), _T("2560 x 1024 (dual-screen)"), _T("3200 x 1200 (dual-screen)")};

/*** Water 4 (bump water) presets ***********************************************/
const int vl_w4_BumpWaterReflection[5] = {0, 0, 1, 1, 1};
const int vl_w4_BumpWaterRefraction[5] = {0, 1, 1, 1, 2};
const int vl_w4_BumpWaterShoreWaves[5] = {0, 0, 0, 1, 1};
const float vl_w4_BumpWaterAnisotropy[5] = {0.f, 0.f, 2.f, 4.f, 6.f};
const int vl_w4_BumpWaterUseDepthTexture[5] = {0, 1, 1, 1, 1};
const int vl_w4_BumpWaterTexSizeReflection[5] = {0, 0, 128, 256, 512};
const int vl_w4_BumpWaterBlurReflection[5] = {0, 0, 0, 0, 1};

const presetValues<int, 5> pr_w4_BumpWaterReflection = presetValues<int, 5>(_T("BumpWaterReflection"), levels_vlow_To_vHigh, vl_w4_BumpWaterReflection);
const presetValues<int, 5> pr_w4_BumpWaterRefraction = presetValues<int, 5>(_T("BumpWaterRefraction"), levels_vlow_To_vHigh, vl_w4_BumpWaterRefraction);
const presetValues<int, 5> pr_w4_BumpWaterShoreWaves = presetValues<int, 5>(_T("BumpWaterShoreWaves"), levels_vlow_To_vHigh, vl_w4_BumpWaterShoreWaves);
const presetValues<float, 5> pr_w4_BumpWaterAnisotropy = presetValues<float, 5>(_T("BumpWaterAnisotropy"), levels_vlow_To_vHigh, vl_w4_BumpWaterAnisotropy);
const presetValues<int, 5> pr_w4_BumpWaterUseDepthTexture = presetValues<int, 5>(_T("BumpWaterUseDepthTexture"), levels_vlow_To_vHigh, vl_w4_BumpWaterUseDepthTexture);
const presetValues<int, 5> pr_w4_BumpWaterTexSizeReflection = presetValues<int, 5>(_T("BumpWaterTexSizeReflection"), levels_vlow_To_vHigh, vl_w4_BumpWaterTexSizeReflection);
const presetValues<int, 5> pr_w4_BumpWaterBlurReflection = presetValues<int, 5>(_T("BumpWaterBlurReflection"), levels_vlow_To_vHigh, vl_w4_BumpWaterBlurReflection);

const int prVal_W4_size = 6;
const presetValues<int, 5> prVal_W4[prVal_W4_size] = {pr_w4_BumpWaterReflection, pr_w4_BumpWaterRefraction, pr_w4_BumpWaterShoreWaves,
						      pr_w4_BumpWaterUseDepthTexture, pr_w4_BumpWaterTexSizeReflection, pr_w4_BumpWaterBlurReflection};

#endif /*PRESETS_H_*/
