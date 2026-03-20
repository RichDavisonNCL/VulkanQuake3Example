/******************************************************************************
This file is part of the Newcastle Vulkan Tutorial Series

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#include "Quake3Example.h"

using namespace NCL;
using namespace Rendering;
using namespace Vulkan;

TUTORIAL_ENTRY(Quake3Example)

Quake3Example::Quake3Example(Window& window, VulkanInitialisation& vkInit) : VulkanTutorial(window, vkInit)	{
	Initialise();

	FrameContext const& context = m_renderer->GetFrameContext();

	pipeline = PipelineBuilder(context.device)
		//.WithVertexInputState(m_triangleMesh->GetVertexInputState())
		.WithTopology(vk::PrimitiveTopology::eTriangleList)
		.WithColourAttachment(context.colourFormat)
		.WithDepthAttachment(context.depthFormat)
		.WithShaderBinary("BasicGeometry.vert.spv", vk::ShaderStageFlagBits::eVertex)
		.WithShaderBinary("BasicGeometry.frag.spv", vk::ShaderStageFlagBits::eFragment)
	.Build("Basic Pipeline");
}

void Quake3Example::RenderFrame(float dt) {
	FrameContext const& context = m_renderer->GetFrameContext();
	context.cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
}