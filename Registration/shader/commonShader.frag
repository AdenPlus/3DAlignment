#version 410

layout(location = 0) out vec4 fragColor;
in vec2 fragTexCoord;
uniform sampler2D tex;
uniform float contrast;
uniform vec3 hsl;
uniform vec2 originalImageSize;
uniform vec2 keystoneImageSize;
uniform mat3 textureMatrix;
uniform bool keystoneEnabled;

float hueToIntensity(float v1, float v2, float h);
vec3 HSLtoRGB(vec3 color);
vec3 RGBtoHSL(vec3 color);
vec4 brigtnessContrastFilter(vec4 pixelColor);
vec4 hueSaturationFilter(vec4 pixelColor);

float hueToIntensity(float v1, float v2, float h) 
{
    h = fract(h);
    if (h < 1.0 / 6.0)
        return v1 + (v2 - v1) * 6.0 * h;
    else if (h < 1.0 / 2.0)
        return v2;
    else if (h < 2.0 / 3.0)
        return v1 + (v2 - v1) * 6.0 * (2.0 / 3.0 - h);
    return v1;
}

vec3 HSLtoRGB(vec3 color) 
{
    float h = color.x;
    float l = color.z;
    float s = color.y;

    if (s < 1.0 / 256.0) return vec3(l);

    float v1;
    float v2;

    if (l < 0.5) v2 = l * (1.0 + s);
    else v2 = (l + s) - (s * l);

    v1 = 2.0 * l - v2;
    float d = 1.0 / 3.0;
    float r = hueToIntensity(v1, v2, h + d);
    float g = hueToIntensity(v1, v2, h);
    float b = hueToIntensity(v1, v2, h - d);
    return vec3(r, g, b);
}

vec3 RGBtoHSL(vec3 color)
{
    float cmin = min(color.r, min(color.g, color.b));
    float cmax = max(color.r, max(color.g, color.b));
    float h = 0.0;
    float s = 0.0;
    float l = (cmin + cmax) / 2.0;
    float diff = cmax - cmin;
    if (diff > 1.0 / 256.0) {
        if (l < 0.5)
            s = diff / (cmin + cmax);
        else
            s = diff / (2.0 - (cmin + cmax));
        if (color.r == cmax)
            h = (color.g - color.b) / diff;
        else if (color.g == cmax)
            h = 2.0 + (color.b - color.r) / diff;
        else
            h = 4.0 + (color.r - color.g) / diff;
        h /= 6.0;
    }
    return vec3(h, s, l);
}

vec4 brigtnessContrastFilter(vec4 pixelColor)
{
    if(contrast == 0.0)
        return pixelColor;

    float brightness = 0.1;
    pixelColor.rgb /= max(1.0/256.0, pixelColor.a);
    float c = 1.0 + contrast;
    float contrastGainFactor = 1.0 + c * c * c * c * step(0.0, contrast);
    pixelColor.rgb = ((pixelColor.rgb - 0.5) * (contrastGainFactor * contrast + 1.0)) + 0.5;
    pixelColor.rgb = mix(pixelColor.rgb, vec3(step(0.0, brightness)), abs(brightness));

    return pixelColor;

}
vec4 hueSaturationFilter(vec4 pixelColor)
{
    pixelColor = vec4(pixelColor.rgb / max(1.0/256.0, pixelColor.a), pixelColor.a);
    pixelColor.rgb = mix(vec3(dot(pixelColor.rgb, vec3(0.2125, 0.7154, 0.0721))), pixelColor.rgb, 1.0 + hsl.y);
    pixelColor.xyz = RGBtoHSL(pixelColor.rgb);
    pixelColor.x = (pixelColor.x + hsl.x);
    pixelColor.rgb = HSLtoRGB(vec3(pixelColor.x, pixelColor.y, pixelColor.z));
    pixelColor.rgb = pixelColor.rgb * (1.0 + hsl.z);
    return vec4(pixelColor.rgb * pixelColor.a, pixelColor.a);
}

void main() 
{
	vec4 pixelColor = vec4(0.0);

	if(keystoneEnabled)
	{
		// Calculate texture position based on keystone matrix in pixels.
		vec2 texturePixelCoord = vec2(fragTexCoord.x * keystoneImageSize.x, fragTexCoord.y * keystoneImageSize.y);

		float determinant = textureMatrix[2][0] * texturePixelCoord.x + textureMatrix[2][1] * texturePixelCoord.y + textureMatrix[2][2];
		float texureX = (textureMatrix[0][0] * texturePixelCoord.x + textureMatrix[0][1] * texturePixelCoord.y + textureMatrix[0][2]) / determinant;
		float texureY = (textureMatrix[1][0] * texturePixelCoord.x + textureMatrix[1][1] * texturePixelCoord.y + textureMatrix[1][2]) / determinant;

		vec2 texCoord = vec2(texureX / originalImageSize.x, texureY / originalImageSize.y);

		pixelColor = texture(tex, texCoord);
	}
	else
	{
		// Calculate texture position based on homography resolution in pixels
		pixelColor = texture(tex, fragTexCoord);
	}

    pixelColor = brigtnessContrastFilter(pixelColor);
    pixelColor = hueSaturationFilter(pixelColor);

	fragColor = vec4(pixelColor.rgb, pixelColor.a);
}
