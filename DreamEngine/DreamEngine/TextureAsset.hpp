#pragma once
#include "Object.h"
#include "GraphicsTexture.hpp"
namespace DreamAsset {
    enum TexutreType {
        Depth,
        Color
    };
    enum TexutreFormat {
        R8G8B8A8_SRGB
    };
    class TextureAsset :
        public Dream::Object
    {
        public:
            uint16_t width, height;
            TexutreType type;
            TexutreFormat format;
            std::string path;
            TextureAsset(std::string name, TexutreType type, TexutreFormat format, std::string path)
            {
                this->path = path;
                this->name = name;
                this->type = type;
                this->format = format;
                //generateGraphicsResources()
            };

            void release() {
                m_graphicsTexture.reset();
            }

            void generateGraphicsResources() 
            {
                if (format == R8G8B8A8_SRGB)
                {
                    VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
                    VkFormat rhiFormat = VK_FORMAT_R8G8B8A8_SRGB;
                    m_graphicsTexture = std::make_shared<Graphics::GraphicsTexture>(Graphics::GraphicsUtil::getInstance()->getMainCommandPool(), aspectFlags, rhiFormat, path);
                    width = m_graphicsTexture->getWidth();
                    height = m_graphicsTexture->getHeight();
                }
            }
            std::shared_ptr<Graphics::GraphicsTexture> getGraphicsTexture() {
                return m_graphicsTexture;
            }
            void setGraphicsTexture(std::shared_ptr<Graphics::GraphicsTexture> graphicsTexture) {
                m_graphicsTexture = graphicsTexture;
            }

        private:
            std::shared_ptr<Graphics::GraphicsTexture> m_graphicsTexture;

    };

}
