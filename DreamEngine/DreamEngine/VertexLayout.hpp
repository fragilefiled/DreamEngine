#pragma once
#pragma once
#include "Object.h"
#include <vulkan/vulkan.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include"glm/glm.hpp"
#include"glm/ext/matrix_transform.hpp"
#include"glm/ext/matrix_clip_space.hpp"
namespace Graphics {

    class VertexLayout :
        public Dream::Object
    {
    public:

		struct VertexTest
		{
			// position
			glm::vec3 Position;
			// normal
			glm::vec3 Normal;
			// texCoords
			glm::vec2 TexCoords;
			// tangent
			glm::vec3 Tangent;
			// bitangent
			glm::vec3 Bitangent;

			static VkVertexInputBindingDescription getBindingDescription() {
				VkVertexInputBindingDescription bindingDescription{};
				bindingDescription.binding = 0;
				bindingDescription.stride = sizeof(VertexTest);
				bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
				return bindingDescription;
			}
			static std::array<VkVertexInputAttributeDescription, 5> getAttributeDescriptions() {
				std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions{};
				attributeDescriptions[0].binding = 0;
				attributeDescriptions[0].location = 0;
				attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[0].offset = offsetof(VertexTest, Position);

				attributeDescriptions[1].binding = 0;
				attributeDescriptions[1].location = 1;
				attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[1].offset = offsetof(VertexTest, Normal);

				attributeDescriptions[2].binding = 0;
				attributeDescriptions[2].location = 2;
				attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[2].offset = offsetof(VertexTest, TexCoords);

				attributeDescriptions[3].binding = 0;
				attributeDescriptions[3].location = 3;
				attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[3].offset = offsetof(VertexTest, Tangent);

				attributeDescriptions[4].binding = 0;
				attributeDescriptions[4].location = 4;
				attributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[4].offset = offsetof(VertexTest, Bitangent);
				return attributeDescriptions;
			}
		};


    };

}
