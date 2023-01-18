#pragma once
#pragma once
#include <vulkan/vulkan.h>
#include"GraphicsRenderPass.hpp"
#include "FileOperation.h"
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#endif
#include "GraphicsDescriptorSetLayout.hpp"
namespace Graphics
{
	class GraphicsPipelineInternal {

	public:
		GraphicsPipelineInternal(std::shared_ptr<GraphicsRenderPass> graphicsRenderPass, std::shared_ptr<GraphicsDescriptorSetLayout> graphicsDescriptorSetLayout) {
			m_device = GraphicsDevice::getInstance()->getLogicDevice();
			m_graphicsRenderPass = graphicsRenderPass;
			m_graphicsDescriptorSetLayout = graphicsDescriptorSetLayout;
			m_graphicsSwapChain = graphicsRenderPass->getGraphicsSwapChain();
			m_swapChainExtent = m_graphicsSwapChain->getswapChainExtent();
			m_descriptorSetLayout = graphicsDescriptorSetLayout->GetDescriptSetLayout();
			m_renderPass = graphicsRenderPass->getRenderPass();
			createGraphicsPipline();
		};
		struct Vertex {
			glm::vec3 pos;
			glm::vec3 color;
			glm::vec2 texCoord;
			static VkVertexInputBindingDescription getBindingDescription() {
				VkVertexInputBindingDescription bindingDescription{};
				bindingDescription.binding = 0;
				bindingDescription.stride = sizeof(Vertex);
				bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
				return bindingDescription;
			}
			static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
				std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
				attributeDescriptions[0].binding = 0;
				attributeDescriptions[0].location = 0;
				attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[0].offset = offsetof(Vertex, pos);

				attributeDescriptions[1].binding = 0;
				attributeDescriptions[1].location = 1;
				attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
				attributeDescriptions[1].offset = offsetof(Vertex, color);

				attributeDescriptions[2].binding = 0;
				attributeDescriptions[2].location = 2;
				attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
				attributeDescriptions[2].offset = offsetof(Vertex, texCoord);
				return attributeDescriptions;
			}
		};
		void createGraphicsPipline() {

			auto vertShaderCode = FileOperation::readfile("Resources/Shaders/shader1.vert.spv");
			auto fragShaderCode = FileOperation::readfile("Resources/Shaders/shader1.frag.spv");
			VkShaderModule vertShaderModule = GraphicsUtil::getInstance()->createShaderModule(m_device, vertShaderCode);
			VkShaderModule fragShaderModule = GraphicsUtil::getInstance()->createShaderModule(m_device, fragShaderCode);

			VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
			vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertShaderStageInfo.module = vertShaderModule;
			vertShaderStageInfo.pName = "main";//选择入口

			VkPipelineShaderStageCreateInfo fragShaderStageInfo{};//pSpecializationInfo 可以创建编译前的全局变量
			//，相较于在Shader中编译效率更改
			fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragShaderStageInfo.module = fragShaderModule;
			fragShaderStageInfo.pName = "main";//选择入口
			VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

			std::vector<VkDynamicState> dynamicStates = {
				VK_DYNAMIC_STATE_VIEWPORT,
				VK_DYNAMIC_STATE_SCISSOR
			};

			VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
			dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			dynamicStateInfo.pDynamicStates = dynamicStates.data();
			dynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());

			auto bindingDescription = Vertex::getBindingDescription();
			auto attributeDescription = Vertex::getAttributeDescriptions();


			VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
			vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();
			vertexInputInfo.vertexBindingDescriptionCount = 1;
			vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;

			VkPipelineInputAssemblyStateCreateInfo inputAssemlyInfo{};
			inputAssemlyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssemlyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			inputAssemlyInfo.primitiveRestartEnable = VK_FALSE;


			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)m_swapChainExtent.width;
			viewport.height = (float)m_swapChainExtent.height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			VkRect2D scissor{};
			scissor.offset = { 0, 0 };
			scissor.extent = m_swapChainExtent;

			VkPipelineViewportStateCreateInfo viewportState{};
			viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.viewportCount = 1;
			viewportState.scissorCount = 1;
			//如果Viewport没有设置动态状态
			//viewportState.pViewports = &viewport;
			//viewportState.pScissors = &scissor;
			VkPipelineRasterizationStateCreateInfo rasterizationInfo{};
			rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizationInfo.depthClampEnable = VK_FALSE;//f depthClampEnable is set to VK_TRUE, then fragments that are beyond the near and far planes are clamped to them as opposed to discarding them
			//如果超出远近平面之外，深度会被Clamp
			rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
			rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
			rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
			rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
			rasterizationInfo.depthBiasEnable = VK_FALSE;
			rasterizationInfo.depthBiasConstantFactor = 0.0f; // Optional
			rasterizationInfo.depthBiasClamp = 0.0f; // Optional
			rasterizationInfo.depthBiasSlopeFactor = 0.0f; // Optional
			//
			rasterizationInfo.lineWidth = 1.0f; //默认值

			VkPipelineMultisampleStateCreateInfo multisampleInfo{};
			multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampleInfo.sampleShadingEnable = VK_FALSE;
			multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			multisampleInfo.minSampleShading = 1.0f; // Optional
			multisampleInfo.pSampleMask = nullptr; // Optional
			multisampleInfo.alphaToCoverageEnable = VK_FALSE; // Optional
			multisampleInfo.alphaToOneEnable = VK_FALSE; // Optional

			VkPipelineColorBlendAttachmentState colorBlendAttachmentInfo{};
			colorBlendAttachmentInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			colorBlendAttachmentInfo.blendEnable = VK_FALSE;
			colorBlendAttachmentInfo.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
			colorBlendAttachmentInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
			colorBlendAttachmentInfo.colorBlendOp = VK_BLEND_OP_ADD; // Optional
			colorBlendAttachmentInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
			colorBlendAttachmentInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
			colorBlendAttachmentInfo.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

			VkPipelineColorBlendStateCreateInfo colorBlending{};
			colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlending.logicOpEnable = VK_FALSE;
			colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
			colorBlending.attachmentCount = 1;
			colorBlending.pAttachments = &colorBlendAttachmentInfo;
			colorBlending.blendConstants[0] = 0.0f; // Optional
			colorBlending.blendConstants[1] = 0.0f; // Optional
			colorBlending.blendConstants[2] = 0.0f; // Optional
			colorBlending.blendConstants[3] = 0.0f; // Optional

			VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutInfo.setLayoutCount = 1; // Optional
			pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout; // Optional
			pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
			pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

			if (vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
				throw std::runtime_error("failed to create pipeline layout!");
			}

			VkPipelineDepthStencilStateCreateInfo depthStencil{};
			depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			depthStencil.depthTestEnable = VK_TRUE;
			depthStencil.depthWriteEnable = VK_TRUE;
			depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
			depthStencil.depthBoundsTestEnable = VK_FALSE;
			depthStencil.minDepthBounds = 0.0f; // Optional
			depthStencil.maxDepthBounds = 1.0f; // Optional
			depthStencil.stencilTestEnable = VK_FALSE;
			depthStencil.front = {}; // Optional
			depthStencil.back = {}; // Optional

			VkGraphicsPipelineCreateInfo pipelineInfo{};
			pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipelineInfo.stageCount = 2;
			pipelineInfo.pStages = shaderStages;
			pipelineInfo.pVertexInputState = &vertexInputInfo;
			pipelineInfo.pInputAssemblyState = &inputAssemlyInfo;
			pipelineInfo.pViewportState = &viewportState;
			pipelineInfo.pRasterizationState = &rasterizationInfo;
			pipelineInfo.pMultisampleState = &multisampleInfo;
			pipelineInfo.pDepthStencilState = &depthStencil; // Optional
			pipelineInfo.pColorBlendState = &colorBlending;
			pipelineInfo.pDynamicState = &dynamicStateInfo;
			pipelineInfo.layout = m_pipelineLayout;
			pipelineInfo.renderPass = m_renderPass;
			pipelineInfo.subpass = 0;

			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
			pipelineInfo.basePipelineIndex = -1; // Optional
			if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline) != VK_SUCCESS) {
				throw std::runtime_error("failed to create graphics pipeline!");
			}
			vkDestroyShaderModule(m_device, fragShaderModule, nullptr);
			vkDestroyShaderModule(m_device, vertShaderModule, nullptr);
		}
		~GraphicsPipelineInternal() {
			m_graphicsSwapChain.reset();
			m_graphicsRenderPass.reset();
			m_graphicsDescriptorSetLayout.reset();
			vkDestroyPipeline(m_device, m_pipeline, nullptr);
			vkDestroyPipelineLayout(m_device, m_pipelineLayout, nullptr);

		}

		VkPipeline getPipeline() {
			return m_pipeline;
		}
		VkPipelineLayout getPipelineLayout() {
			return m_pipelineLayout;
		}

		std::shared_ptr<GraphicsRenderPass> getGraphicsRenderPass() {
			return m_graphicsRenderPass;
		}
		std::shared_ptr<GraphicsDescriptorSetLayout> getGraphicsDescriptorSetLayout() {
			return m_graphicsDescriptorSetLayout;
		}
	private:
		VkDevice m_device;
		std::shared_ptr<GraphicsSwapChain> m_graphicsSwapChain;
		std::shared_ptr<GraphicsRenderPass> m_graphicsRenderPass;
		std::shared_ptr <GraphicsDescriptorSetLayout> m_graphicsDescriptorSetLayout;
		VkDescriptorSetLayout m_descriptorSetLayout;
		VkExtent2D m_swapChainExtent;
		VkRenderPass m_renderPass;
		VkPipeline m_pipeline;
		VkPipelineLayout m_pipelineLayout;

	};

}