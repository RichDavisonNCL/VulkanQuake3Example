/******************************************************************************
This file is part of the Newcastle Vulkan Tutorial Series

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////

#version 450
#extension GL_ARB_separate_shader_objects  : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 pos;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 inNormal;

layout (location = 0) out vec2 outTex;
layout (location = 1) out vec3 outNormal;

layout (set = 0, binding  = 0) uniform  CameraMatrices 
{
	mat4 viewMatrix;
	mat4 projMatrix;
};

layout(push_constant) uniform PushConstantVert{
	mat4 modelMatrix;
};

void main() {
   outTex 		= texCoord;
   outNormal	= inNormal;

   vec4 worldPos = modelMatrix * vec4(pos.xyz, 1);
   
   gl_Position 	    = projMatrix  * viewMatrix   * worldPos;
}
