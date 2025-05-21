#version 330 core
out vec4 FragColor;
  
in vec2 TexCoord;

uniform sampler2D BaseTexture;
uniform float TextureScaleVertical;
uniform float TextureScaleHorizontal;
uniform sampler2D LightMap;
uniform int LightMapOffset;
uniform bool Emissive;

void main()
{
    if (Emissive) {
        FragColor = texture(BaseTexture, vec2(TexCoord.x*TextureScaleHorizontal,TexCoord.y*TextureScaleVertical));
    } else {
        vec2 lmTex = vec2(TexCoord.x, TexCoord.y * ( (64.0/1024.0) * LightMapOffset) );
        vec4 lm = vec4(texture(LightMap, lmTex).r,texture(LightMap, lmTex).r,texture(LightMap, lmTex).r,1.0);
        FragColor = texture(BaseTexture, vec2(TexCoord.x*TextureScaleHorizontal,TexCoord.y*TextureScaleVertical)) * lm;
    }
    //FragColor = texture(BaseTexture, TexCoord * TextureScale);
    //FragColor = lm;
    //FragColor = mix(texture(BaseTexture, TexCoord), texture(LightMap, TexCoord), 0.2);
}