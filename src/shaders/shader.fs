#version 330 core
out vec4 FragColor;
  
in vec3 vertexColor;
in vec2 TexCoord;

uniform sampler2D BaseTexture;
uniform float TextureScale;
uniform sampler2D LightMap;

void main()
{
    //vec4 lm = vec4(texture(LightMap, TexCoord).r,texture(LightMap, TexCoord).r,texture(LightMap, TexCoord).r,1.0);
    //FragColor = texture(BaseTexture, TexCoord*16.0) * lm * vec4(vertexColor, 1.0);
    FragColor = texture(BaseTexture, TexCoord * TextureScale);
    //FragColor = lm;
    //FragColor = mix(texture(BaseTexture, TexCoord), texture(LightMap, TexCoord), 0.2);
}