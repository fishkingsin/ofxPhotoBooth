/*
** Photoshop & misc math
** Blending modes, RGB/HSL/Contrast/Desaturate, levels control
**
** Romain Dura | Romz
** Blog: http://blog.mouaif.org
** Post: http://blog.mouaif.org/?p=94
*/


/*
** Desaturation
*/
vec4 Desaturate(vec3 color, float Desaturation)
{
	vec3 grayXfer = vec3(0.3, 0.59, 0.11);
	vec3 gray = vec3(dot(grayXfer, color));
	return vec4(mix(color, gray, Desaturation), 1.0);
}


/*
** Hue, saturation, luminance
*/

vec3 RGBToHSL(vec3 color)
{
	vec3 hsl; // init to 0 to avoid warnings ? (and reverse if + remove first part)
	
	float fmin = min(min(color.r, color.g), color.b);    //Min. value of RGB
	float fmax = max(max(color.r, color.g), color.b);    //Max. value of RGB
	float delta = fmax - fmin;             //Delta RGB value

	hsl.z = (fmax + fmin) / 2.0; // Luminance

	if (delta == 0.0)		//This is a gray, no chroma...
	{
		hsl.x = 0.0;	// Hue
		hsl.y = 0.0;	// Saturation
	}
	else                                    //Chromatic data...
	{
		if (hsl.z < 0.5)
			hsl.y = delta / (fmax + fmin); // Saturation
		else
			hsl.y = delta / (2.0 - fmax - fmin); // Saturation
		
		float deltaR = (((fmax - color.r) / 6.0) + (delta / 2.0)) / delta;
		float deltaG = (((fmax - color.g) / 6.0) + (delta / 2.0)) / delta;
		float deltaB = (((fmax - color.b) / 6.0) + (delta / 2.0)) / delta;

		if (color.r == fmax )
			hsl.x = deltaB - deltaG; // Hue
		else if (color.g == fmax)
			hsl.x = (1.0 / 3.0) + deltaR - deltaB; // Hue
		else if (color.b == fmax)
			hsl.x = (2.0 / 3.0) + deltaG - deltaR; // Hue

		if (hsl.x < 0.0)
			hsl.x += 1.0; // Hue
		else if (hsl.x > 1.0)
			hsl.x -= 1.0; // Hue
	}

	return hsl;
}

float HueToRGB(float f1, float f2, float hue)
{
	if (hue < 0.0)
		hue += 1.0;
	else if (hue > 1.0)
		hue -= 1.0;
	float res;
	if ((6.0 * hue) < 1.0)
		res = f1 + (f2 - f1) * 6.0 * hue;
	else if ((2.0 * hue) < 1.0)
		res = f2;
	else if ((3.0 * hue) < 2.0)
		res = f1 + (f2 - f1) * ((2.0 / 3.0) - hue) * 6.0;
	else
		res = f1;
	return res;
}

vec3 HSLToRGB(vec3 hsl)
{
	vec3 rgb;
	
	if (hsl.y == 0.0)
		rgb = vec3(hsl.z); // Luminance
	else
	{
		float f2;
		
		if (hsl.z < 0.5)
			f2 = hsl.z * (1.0 + hsl.y);
		else
			f2 = (hsl.z + hsl.y) - (hsl.y * hsl.z);
			
		float f1 = 2.0 * hsl.z - f2;
		
		rgb.r = HueToRGB(f1, f2, hsl.x + (1.0/3.0));
		rgb.g = HueToRGB(f1, f2, hsl.x);
		rgb.b= HueToRGB(f1, f2, hsl.x - (1.0/3.0));
	}
	
	return rgb;
}


/*
** Contrast, saturation, brightness
** Code of this function is from TGM's shader pack
** http://irrlicht.sourceforge.net/phpBB2/viewtopic.php?t=21057
*/

// For all settings: 1.0 = 100% 0.5=50% 1.5 = 150%
vec3 ContrastSaturationBrightness(vec3 color, float brt, float sat, float con)
{
	// Increase or decrease theese values to adjust r, g and b color channels seperately
	const float AvgLumR = 0.5;
	const float AvgLumG = 0.5;
	const float AvgLumB = 0.5;
	
	const vec3 LumCoeff = vec3(0.2125, 0.7154, 0.0721);
	
	vec3 AvgLumin = vec3(AvgLumR, AvgLumG, AvgLumB);
	vec3 brtColor = color * brt;
	vec3 intensity = vec3(dot(brtColor, LumCoeff));
	vec3 satColor = mix(intensity, brtColor, sat);
	vec3 conColor = mix(AvgLumin, satColor, con);
	return conColor;
}


/*
** Float blending modes
** Adapted from here: http://www.nathanm.com/photoshop-blending-math/
** But I modified the HardMix (wrong condition), Overlay, SoftLight, ColorDodge, ColorBurn, VividLight, PinLight (inverted layers) ones to have correct results
*/

#define BlendLinearDodgef 			BlendAddf
#define BlendLinearBurnf 			BlendSubstractf
#define BlendAddf(base, blend) 		min(base + blend, 1.0)
#define BlendSubstractf(base, blend) 	max(base + blend - 1.0, 0.0)
#define BlendLightenf(base, blend) 		max(blend, base)
#define BlendDarkenf(base, blend) 		min(blend, base)
#define BlendLinearLightf(base, blend) 	(blend < 0.5 ? BlendLinearBurnf(base, (2.0 * blend)) : BlendLinearDodgef(base, (2.0 * (blend - 0.5))))
#define BlendScreenf(base, blend) 		(1.0 - ((1.0 - base) * (1.0 - blend)))
#define BlendOverlayf(base, blend) 	(base < 0.5 ? (2.0 * base * blend) : (1.0 - 2.0 * (1.0 - base) * (1.0 - blend)))
#define BlendSoftLightf(base, blend) 	((blend < 0.5) ? (2.0 * base * blend + base * base * (1.0 - 2.0 * blend)) : (sqrt(base) * (2.0 * blend - 1.0) + 2.0 * base * (1.0 - blend)))
#define BlendColorDodgef(base, blend) 	((blend == 1.0) ? blend : min(base / (1.0 - blend), 1.0))
#define BlendColorBurnf(base, blend) 	((blend == 0.0) ? blend : max((1.0 - ((1.0 - base) / blend)), 0.0))
#define BlendVividLightf(base, blend) 	((blend < 0.5) ? BlendColorBurnf(base, (2.0 * blend)) : BlendColorDodgef(base, (2.0 * (blend - 0.5))))
#define BlendPinLightf(base, blend) 	((blend < 0.5) ? BlendDarkenf(base, (2.0 * blend)) : BlendLightenf(base, (2.0 *(blend - 0.5))))
#define BlendHardMixf(base, blend) 	((BlendVividLightf(base, blend) < 0.5) ? 0.0 : 1.0)
#define BlendReflectf(base, blend) 		((blend == 1.0) ? blend : min(base * base / (1.0 - blend), 1.0))


/*
** Vector3 blending modes
*/

// Component wise blending
#define Blend(base, blend, funcf) 		vec3(funcf(base.r, blend.r), funcf(base.g, blend.g), funcf(base.b, blend.b))

#define BlendNormal(base, blend) 		(blend)
#define BlendLighten				BlendLightenf
#define BlendDarken				BlendDarkenf
#define BlendMultiply(base, blend) 		(base * blend)
#define BlendAverage(base, blend) 		((base + blend) / 2.0)
#define BlendAdd(base, blend) 		min(base + blend, vec3(1.0))
#define BlendSubstract(base, blend) 	max(base + blend - vec3(1.0), vec3(0.0))
#define BlendDifference(base, blend) 	abs(base - blend)
#define BlendNegation(base, blend) 	(vec3(1.0) - abs(vec3(1.0) - base - blend))
#define BlendExclusion(base, blend) 	(base + blend - 2.0 * base * blend)
#define BlendScreen(base, blend) 		Blend(base, blend, BlendScreenf)
#define BlendOverlay(base, blend) 		Blend(base, blend, BlendOverlayf)
#define BlendSoftLight(base, blend) 	Blend(base, blend, BlendSoftLightf)
#define BlendHardLight(base, blend) 	BlendOverlay(blend, base)
#define BlendColorDodge(base, blend) 	Blend(base, blend, BlendColorDodgef)
#define BlendColorBurn(base, blend) 	Blend(base, blend, BlendColorBurnf)
#define BlendLinearDodge			BlendAdd
#define BlendLinearBurn			BlendSubstract
// Linear Light is another contrast-increasing mode
// If the blend color is darker than midgray, Linear Light darkens the image by decreasing the brightness. If the blend color is lighter than midgray, the result is a brighter image due to increased brightness.
#define BlendLinearLight(base, blend) 	Blend(base, blend, BlendLinearLightf)
#define BlendVividLight(base, blend) 	Blend(base, blend, BlendVividLightf)
#define BlendPinLight(base, blend) 		Blend(base, blend, BlendPinLightf)
#define BlendHardMix(base, blend) 		Blend(base, blend, BlendHardMixf)
#define BlendReflect(base, blend) 		Blend(base, blend, BlendReflectf)
#define BlendGlow(base, blend) 		BlendReflect(blend, base)
#define BlendPhoenix(base, blend) 		(min(base, blend) - max(base, blend) + vec3(1.0))
#define BlendOpacity(base, blend, F, O) 	(F(base, blend) * O + blend * (1.0 - O))


// Hue Blend mode creates the result color by combining the luminance and saturation of the base color with the hue of the blend color.
vec3 BlendHue(vec3 base, vec3 blend)
{
	vec3 baseHSL = RGBToHSL(base);
	return HSLToRGB(vec3(RGBToHSL(blend).r, baseHSL.g, baseHSL.b));
}

// Saturation Blend mode creates the result color by combining the luminance and hue of the base color with the saturation of the blend color.
vec3 BlendSaturation(vec3 base, vec3 blend)
{
	vec3 baseHSL = RGBToHSL(base);
	return HSLToRGB(vec3(baseHSL.r, RGBToHSL(blend).g, baseHSL.b));
}

// Color Mode keeps the brightness of the base color and applies both the hue and saturation of the blend color.
vec3 BlendColor(vec3 base, vec3 blend)
{
	vec3 blendHSL = RGBToHSL(blend);
	return HSLToRGB(vec3(blendHSL.r, blendHSL.g, RGBToHSL(base).b));
}

// Luminosity Blend mode creates the result color by combining the hue and saturation of the base color with the luminance of the blend color.
vec3 BlendLuminosity(vec3 base, vec3 blend)
{
	vec3 baseHSL = RGBToHSL(base);
	return HSLToRGB(vec3(baseHSL.r, baseHSL.g, RGBToHSL(blend).b));
}


/*
** Gamma correction
** Details: http://blog.mouaif.org/2009/01/22/photoshop-gamma-correction-shader/
*/

#define GammaCorrection(color, gamma)								pow(color, 1.0 / gamma)

/*
** Levels control (input (+gamma), output)
** Details: http://blog.mouaif.org/2009/01/28/levels-control-shader/
*/

#define LevelsControlInputRange(color, minInput, maxInput)				min(max(color - vec3(minInput), vec3(0.0)) / (vec3(maxInput) - vec3(minInput)), vec3(1.0))
#define LevelsControlInput(color, minInput, gamma, maxInput)				GammaCorrection(LevelsControlInputRange(color, minInput, maxInput), gamma)
#define LevelsControlOutputRange(color, minOutput, maxOutput) 			mix(vec3(minOutput), vec3(maxOutput), color)
#define LevelsControl(color, minInput, gamma, maxInput, minOutput, maxOutput) 	LevelsControlOutputRange(LevelsControlInput(color, minInput, gamma, maxInput), minOutput, maxOutput)

#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable
uniform sampler2DRect tex0; // fbo source
uniform sampler2DRect tex1; // background
uniform sampler2DRect tex2;

uniform float tol;

uniform float brightness;
uniform float satuation;
uniform float contrast;
uniform float minLevel;
uniform float maxLevel;
uniform float levelG;
uniform vec4 keycolor;
uniform vec4 keycolor2;

uniform int step ;
vec2 texCoor[9];
//uniform int offset;
void main( void )
{  
	/*float brightness = 1.3;
	float satuation = 1.3;
	float contrast = 0.85;
	float minLevel = .1;
	float maxLevel = .9;
	float levelG = .9;
	float minValue = 1.;
	float maxValue = 0.;
	int step  = 1;*/
	vec4 sample[9];
	float minValue = 1.;
	float maxValue = 0.;


	//vec4 result;
	//int step = 3;
	
	float nValue[9];
	/*vec2 texCoor[9];
	for(int i = -step ; i < step ; i++)
	{
		for(int j = -step ; j < step ; j++)
		{	
			
		}	
	}*/

	
	texCoor[0]=vec2(-step,-step);
	texCoor[1]=vec2(-step,0);
	texCoor[2]=vec2(-step, step);
	texCoor[3]=vec2(0,-step);
	texCoor[4]=vec2(0,0);
	texCoor[5]=vec2(0, step);
	texCoor[6]=vec2(step,-step);
	texCoor[7]=vec2(step, step);
	texCoor[8]=vec2(step, step);
	float count = 0.;
	for (int i = 0; i < 9; i++)
    {
	vec4 one_third = vec4(0.333333);
	
	vec2 texcoord0 = gl_TexCoord[0].st;
	
	vec4 a = texture2DRect(tex0, texcoord0+ texCoor[i]);
	vec4 b = texture2DRect(tex1, texcoord0+ texCoor[i]);
	sample[i] = a;
//	vec4 a = texture2DRect(tex0, texcoord0);
//	vec4 b = texture2DRect(tex1, texcoord0);
	vec4 source = a;
	//vec4 source = tep;//average neighbour
	vec4 target = keycolor;
	//vec4 target2 = keycolor2;
	// zero our alphas
	source.a = 0.;
	target.a = 0.;
	//target2.a = 0.;
	// measure distance from target
	vec4 vdelta_chroma = abs(source-target);	
	//vec4 vdelta_chroma2 = abs(source-target2);	
	// sum vector distance, scaling by a third
	//one_third = vec4(0.3333333);
	float delta_chroma = dot(vdelta_chroma,one_third);
	//float delta_chroma2 = dot(vdelta_chroma2,one_third); 
	


	target = texture2DRect(tex2, texcoord0+ texCoor[i]);
//	target = texture2DRect(tex2, texcoord0);
	target.a = 0.0;


	// measure distance from target
	vec4 vdelta_bg = abs(source-target);
	
	// sum vector distance, scaling by a third
	float delta_bg = dot(vdelta_bg,one_third); 


	// Pick the closer value of keying value
	float delta = delta_chroma;
	//float delta = min(delta_chroma, delta_bg);
	//float delta =min(delta_chroma,delta_chroma2);
	//float delta = min(delta_chroma, delta_bg);
	// determine scaling coefficient witin our fade range
	float fade = tol / 2.;
	float scale = smoothstep(abs(tol),abs(tol)+abs(fade), delta);

	// invert if necessary
	// -- N.A. --
	//float mixamount = mix(scale,1.-scale,invert);
	float mixamount = mix(scale,1.-scale,0.);

	//find min value neighbour	
	//minValue =min(mixamount,minValue);
		nValue[i] =min(mixamount,minValue);
	}
	float numAlpha = 1.;
	for(int i = 0 ; i < 9 ; i++)
	{
		if(nValue[i]==0.)
		{
			numAlpha++;
		}
		minValue = min(nValue[i],minValue);
		maxValue = max(nValue[i],maxValue);
//		minValue += nValue[i];
	}


//	minValue = ;
//	minValue/=9.; 

	vec2 texcoord0 = gl_TexCoord[0].st;
	vec4 a = texture2DRect(tex0, texcoord0);
	a.rgb = vec3(a.r,(a.g>minLevel && a.g<maxLevel)?a.g*levelG:a.g,a.b);
	a.rgb = ContrastSaturationBrightness(a.rgb, brightness, satuation, contrast);
	vec4 b = texture2DRect(tex1, texcoord0);
	vec4 result = vec4(1,1,1,1);
	//result = mix(b,a, minValue);
	//minValue = ((1.-numAlpha)/9.);
	result = mix(b,
				(numAlpha >2. && numAlpha<8.)?
				((sample[0] + (2.0*sample[1]) + sample[2] + 
				(2.0*sample[3]) + sample[4] + (2.0*sample[5]) + 
                   sample[6] + (1.0*sample[7]) + sample[8]) / 13.0):a,
				minValue);
	//vec4 result = mix(b,a,dot(minValue,(1./(numAlpha))));
	//vec4 result = mix(b,a,smoothstep(maxValue ,minValue, 1.-nValue[5]));
	gl_FragColor = result;

	
	
}