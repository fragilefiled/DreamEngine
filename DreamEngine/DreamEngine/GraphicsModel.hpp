#pragma once
#include"GraphicsTexture.hpp"
#include "GraphicsCommandPool.hpp"
namespace Graphics {
	template<typename T>
	class GraphicsModel
	{
	public:
		GraphicsModel( std::vector<T> vertices, std::vector<uint16_t> indices)
		{
			m_device = GraphicsDevice::getInstance()->getLogicDevice();
			m_commandPool = Graphics::GraphicsUtil::getInstance()->getMainCommandPool();
			m_queue = GraphicsDevice::getInstance()->getGraphicsQueue();
			m_vertices = vertices;
			m_indices = indices;
			m_physicalDevice = GraphicsDevice::getInstance()->getPhysicDevice();
			GraphicsCommandBuffer tempCmd = GraphicsCommandBuffer(m_device, m_commandPool, m_queue);
			createVertexBuffer(tempCmd);
			createIndexBuffer(tempCmd);
		};


		void createVertexBuffer(GraphicsCommandBuffer tempCmd) {
			auto graphicsHelper = GraphicsUtil::getInstance();
			VkDeviceSize bufferSize = sizeof(m_vertices[0]) * m_vertices.size();
			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;

			graphicsHelper->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer
				, stagingBufferMemory, m_device, m_physicalDevice);
			void* data;
			vkMapMemory(m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
			memcpy(data, m_vertices.data(), (size_t)bufferSize);
			vkUnmapMemory(m_device, stagingBufferMemory);


			graphicsHelper->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vertexBuffer
				, m_vertexBufferMemory, m_device, m_physicalDevice);
			tempCmd.copyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);

			vkDestroyBuffer(m_device, stagingBuffer, nullptr);
			vkFreeMemory(m_device, stagingBufferMemory, nullptr);
		}
		void createIndexBuffer(GraphicsCommandBuffer tempCmd) {
			auto graphicsHelper = GraphicsUtil::getInstance();
			VkDeviceSize bufferSize = sizeof(m_indices[0]) * m_indices.size();
			VkBuffer stagingBuffer;
			VkDeviceMemory stagingBufferMemory;

			graphicsHelper->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer
				, stagingBufferMemory, m_device, m_physicalDevice);
			void* data;
			vkMapMemory(m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
			memcpy(data, m_indices.data(), (size_t)bufferSize);
			vkUnmapMemory(m_device, stagingBufferMemory);


			graphicsHelper->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_indexBuffer
				, m_indexBufferMemory, m_device, m_physicalDevice);
			tempCmd.copyBuffer(stagingBuffer, m_indexBuffer, bufferSize);

			vkDestroyBuffer(m_device, stagingBuffer, nullptr);
			vkFreeMemory(m_device, stagingBufferMemory, nullptr);
		}

		VkBuffer getIndexBuffer() {
			return m_indexBuffer;		
		}

		VkBuffer getVertexBuffer() {
			return m_vertexBuffer;
		}

		VkDeviceMemory getVertexBufferMemory() {
			return m_vertexBufferMemory;
		}

		VkDeviceMemory getIndexBufferMemory() {
			return m_indexBufferMemory;
		}
		~GraphicsModel() 
		{
			vkDestroyBuffer(m_device, m_vertexBuffer, nullptr);
			vkFreeMemory(m_device, m_vertexBufferMemory, nullptr);
			vkDestroyBuffer(m_device, m_indexBuffer, nullptr);
			vkFreeMemory(m_device, m_indexBufferMemory, nullptr);

		}
	private:
		VkDevice m_device;
		VkPhysicalDevice m_physicalDevice;
		VkCommandPool m_commandPool;
		VkQueue m_queue;
		VkBuffer m_vertexBuffer;
		VkDeviceMemory m_vertexBufferMemory;
		VkBuffer m_indexBuffer;
		VkDeviceMemory m_indexBufferMemory;
		std::vector<T> m_vertices;
		std::vector<uint16_t> m_indices;
	};
}