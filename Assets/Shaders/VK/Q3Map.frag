/******************************************************************************
This file is part of the Newcastle Vulkan Tutorial Series

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////

#version 450
#extension GL_ARB_separate_shader_objects	: enable
#extension GL_ARB_shading_language_420pack	: enable
#extension GL_EXT_nonuniform_qualifier		: enable

layout (location = 0) in vec2 inTex;
layout (location = 1) in vec3 inNormal;

layout (location = 2) in vec2 inLightmap;

layout (location = 0) out vec4 fragColor;

layout (set = 1, binding = 0) uniform sampler2D objectTextures[];

layout(push_constant) uniform PushConstantMaterial{
	layout(offset = 64) int diffuseID;
	layout(offset = 68) int lightmapID;
};

void main() {
	vec4 lightmapSample 	= texture(objectTextures[lightmapID], inLightmap);
   fragColor = lightmapSample;
  // fragColor.xyz = inNormal * 0.5f + 0.5f;
}