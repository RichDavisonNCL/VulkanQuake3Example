/******************************************************************************
This file is part of the Newcastle Vulkan Tutorial Series

Author:Rich Davison
Contact:richgdavison@gmail.com
License: MIT (see LICENSE file at the top of the source tree)
*//////////////////////////////////////////////////////////////////////////////
#pragma once
#include "VulkanTutorial.h"
#include "../idTechLoaders/Quake3MapLoader.h"

namespace NCL::Rendering::Vulkan {
	class Quake3Example : public VulkanTutorial	{
	public:
		Quake3Example(Window& window, VulkanInitialisation& vkInit);
		~Quake3Example() {
			delete mesh;
		} 
	protected:
		void RenderFrame(float dt) override;

		VulkanPipeline		pipeline;

		vk::UniqueDescriptorSet			bindlessSet;
		vk::UniqueDescriptorSetLayout	bindlessLayout;

		idTechLoaders::Quake3Map* map;
		VulkanMesh* mesh;

		std::vector<UniqueVulkanTexture> lightmaps;
	};
}