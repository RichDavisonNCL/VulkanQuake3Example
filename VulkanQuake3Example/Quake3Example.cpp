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

	m_camera.SetPosition({ -575, -171, 571 });

	//m_camera.SetPosition({ -999999999.0f, 42, -16 });

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

	const float scale = 1.0f;// 1.0f / 8.0f;
	Matrix4 model = Matrix::Scale(Vector3(scale, scale, scale));
	context.cmdBuffer.pushConstants(*pipeline.layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(Matrix4), (void*)&model);

	Vector3 camPos = m_camera.GetPosition() * scale;

	if (map->IsPositionInMap(camPos)) {	
	//	//Work out the subset of visible clusters...
		std::vector<uint32_t> visibleFaces;
		map->BuildVisibleSubmeshList(camPos, visibleFaces);
		for (uint32_t index : visibleFaces) {
			mesh->DrawLayer(index, context.cmdBuffer);
		}
		std::cout << "In map: rendering " << visibleFaces.size() << " faces\n";
	}
	else {
		//Outside the map bounds, just draw everything!
		mesh->DrawAllLayers(context.cmdBuffer);
		std::cout << "Outside map: rendering all faces\n";
	}
}