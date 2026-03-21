/******************************************************************************
This file is part of the Newcastle Vulkan Tutorial Series

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////

#version 450
#extension GL_ARB_separate_shader_objects  : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec2 inTex;
layout (location = 1) in vec3 inNormal;

layout (location = 0) out vec4 fragColor;

void main() {
   fragColor = vec4(inTex.x,inTex.y,0,1);
   fragColor.xyz = inNormal * 0.5f + 0.5f;
}