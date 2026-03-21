/******************************************************************************
This file is part of the Newcastle Vulkan Tutorial Series

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#include "Quake3Example.h"
#include "Assets.h"

using namespace NCL;
using namespace Rendering;
using namespace Vulkan;
using namespace idTechLoaders;

TUTORIAL_ENTRY(Quake3Example)

Quake3Example::Quake3Example(Window& window, VulkanInitialisation& vkInit) : VulkanTutorial(window, vkInit)	{
	Initialise();

	mesh	= new VulkanMesh();
	map		= new Quake3Map(Assets::MESHDIR + "q3dm6.bsp", mesh);
	UploadMeshWait(*mesh);

	FrameContext const& context = m_renderer->GetFrameContext();

	pipeline = PipelineBuilder(context.device)
		.WithVertexInputState(mesh->GetVertexInputState())
		.WithTopology(vk::PrimitiveTopology::eTriangleList)
		.WithColourAttachment(context.colourFormat)
		.WithDepthAttachment(context.depthFormat, vk::CompareOp::eLessOrEqual, true, true)
		.WithShaderBinary("Q3Map.vert.spv", vk::ShaderStageFlagBits::eVertex)
		.WithShaderBinary("Q3Map.frag.spv", vk::ShaderStageFlagBits::eFragment)
	.Build("Basic Pipeline");
}

void Quake3Example::RenderFrame(float dt) {	
	FrameContext const& context = m_renderer->GetFrameContext();

	context.cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
	context.cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline.layout, 0, 1, &*m_cameraDescriptor, 0, nullptr);

	const float scale = 1.0f / 8.0f;
	Matrix4 model = Matrix::Scale(Vector3(scale, scale, scale));
	context.cmdBuffer.pushConstants(*pipeline.layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(Matrix4), (void*)&model);
	mesh->DrawAllLayers(context.cmdBuffer);
}