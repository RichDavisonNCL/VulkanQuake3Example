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

const int _NumSamplers = 1024;

Quake3Example::Quake3Example(Window& window, VulkanInitialisation& vkInit) : VulkanTutorial(window, vkInit)	{
	static vk::PhysicalDeviceDescriptorIndexingFeatures indexingFeatures;
	indexingFeatures.descriptorBindingUniformBufferUpdateAfterBind = true;
	indexingFeatures.descriptorBindingSampledImageUpdateAfterBind = true;
	indexingFeatures.descriptorBindingPartiallyBound = true;
	indexingFeatures.descriptorBindingVariableDescriptorCount = true;
	indexingFeatures.runtimeDescriptorArray = true;

	m_vkInit.features.push_back(&indexingFeatures);

	m_vkInit.deviceExtensions.emplace_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);

	m_vkInit.defaultDescriptorPoolFlags			= vk::DescriptorPoolCreateFlagBits::eUpdateAfterBindEXT;
	m_vkInit.defaultDescriptorPoolImageCount	= _NumSamplers;

	Initialise();
	FrameContext const& context = m_renderer->GetFrameContext();

	vk::UniqueCommandBuffer cmdBuffer = Vulkan::CmdBufferCreateBegin(context.device, context.commandPools[CommandType::Graphics]);

	TextureBuilder builder(context.device, *m_memoryManager);

	builder.WithCommandBuffer(*cmdBuffer).WithMips(false);

	m_camera.SetPosition({ -571, 260, 521 }).SetFarPlane(20000.0f);

	std::vector<char*> dataToDelete;

	LightmapHandlingFunc lightMapHandler = [&](char* data, uint32_t x, uint32_t y, uint32_t bpp) {
		size_t byteCount = x * y * bpp;

		char* uploadData = data;

		if (bpp != 4) {
			//Exciting! Most Vulkan implementations won't handle RGB images, need to pad it out
			size_t oldByteCount = byteCount;
			byteCount = x * y * 4;
			uploadData = new char[byteCount];
			char* writePtr = uploadData;
			for (int i = 0; i < oldByteCount; i += 3) {
				*writePtr++ = data[i + 0];
				*writePtr++ = data[i + 1];
				*writePtr++ = data[i + 2];
				*writePtr++ = 255;//New alpha channel
			}
			dataToDelete.push_back(uploadData);
		}
		lightmaps.emplace_back(builder.WithDimension(x, y, 1).BuildFromData(uploadData, byteCount, "Lightmap"));
	};

	mesh	= new VulkanMesh();
	map		= new Quake3Map(Assets::MESHDIR + "q3dm6.bsp", mesh, lightMapHandler);
	UploadMeshWait(*mesh);
	Vulkan::CmdBufferEndSubmitWait(*cmdBuffer, context.device, context.queues[CommandType::Graphics]);

	for (int i = 0; i < dataToDelete.size(); ++i) {
		delete dataToDelete[i];
	}

	bindlessLayout = DescriptorSetLayoutBuilder(context.device)
		.WithImageSamplers(0, _NumSamplers, vk::ShaderStageFlagBits::eFragment, vk::DescriptorBindingFlagBits::ePartiallyBound | vk::DescriptorBindingFlagBits::eVariableDescriptorCount)
		.WithCreationFlags(vk::DescriptorSetLayoutCreateFlagBits::eUpdateAfterBindPool)
		.Build("Bindless Data");
	bindlessSet = CreateDescriptorSet(context.device, context.descriptorPool, *bindlessLayout, _NumSamplers);

	for (int i = 0; i < lightmaps.size(); ++i) {
		WriteCombinedImageDescriptor(context.device, *bindlessSet, 0, i, *lightmaps[i], *m_defaultSampler);
	}

	pipeline = PipelineBuilder(context.device)
		.WithVertexInputState(mesh->GetVertexInputState())
		.WithTopology(vk::PrimitiveTopology::eTriangleList)
		.WithColourAttachment(context.colourFormat)
		.WithDepthAttachment(context.depthFormat, vk::CompareOp::eLessOrEqual, true, true)
		.WithShaderBinary("Q3Map.vert.spv", vk::ShaderStageFlagBits::eVertex)
		.WithShaderBinary("Q3Map.frag.spv", vk::ShaderStageFlagBits::eFragment)
		.WithDescriptorSetLayout(1, *bindlessLayout)	//All textures Set 1
	.Build("Basic Pipeline");
}

void Quake3Example::RenderFrame(float dt) {	
	FrameContext const& context = m_renderer->GetFrameContext();

	context.cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
	context.cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline.layout, 0, 1, &*m_cameraDescriptor, 0, nullptr);

	vk::DescriptorSet sets[] = {
		*m_cameraDescriptor,
		*bindlessSet
	};

	context.cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, *pipeline.layout, 0, std::size(sets), sets, 0, nullptr);

	const float scale = 1.0f;
	Matrix4 model = Matrix::Scale(Vector3(scale, scale, scale));
	context.cmdBuffer.pushConstants(*pipeline.layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(Matrix4), (void*)&model);

	Vector3 camPos = m_camera.GetPosition() / scale;
	std::vector<uint32_t> visibleFaces;

	const std::vector<Quake3FaceMaterial>& materials = map->GetTextureSet();

	mesh->BindToCommandBuffer(context.cmdBuffer);

	if (map->BuildVisibleSubmeshList(camPos, visibleFaces)) {
		for (uint32_t index : visibleFaces) {

			context.cmdBuffer.pushConstants(*pipeline.layout, vk::ShaderStageFlagBits::eFragment, sizeof(Matrix4), sizeof(Quake3FaceMaterial), (void*)&materials[index]);
			mesh->DrawLayer(index, context.cmdBuffer);
		}
	}
	else {//Outside the map bounds, just draw everything!	
		mesh->DrawAllLayers(context.cmdBuffer);
	}
}