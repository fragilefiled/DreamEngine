#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include "TextureAsset.hpp"
#include"glm/glm.hpp"
#include"glm/ext/matrix_transform.hpp"
#include"glm/ext/matrix_clip_space.hpp"
#include "TextureAssetUtil.h"
#define MAX_BONE_INFLUENCE 4

namespace DreamAsset 
{
    template<typename Vertex>
    class Mesh {
    public:
        /*  网格数据  */
        std::vector<Vertex> vertices;
        std::vector<uint16_t> indices;
        std::vector<TextureAsset> textures;
        std::vector<uint16_t> textureIndexArray;
        uint16_t minTextureCount;
        /*  函数  */
        Mesh(std::vector<Vertex> vertices, std::vector<uint16_t> indices,std::vector<TextureAsset> textures, std::vector<uint16_t> textureIndexArray) {
            this->vertices = vertices;
            this->indices = indices;
            this->textures = textures;
            this->textureIndexArray = textureIndexArray;
            //generateGraphicsResources();
        };
        void release() {
            m_graphicsModel.reset();
            m_graphicsDescriptorSetLayout.reset();
            m_graphicsDescriptorSet.reset();
        }

        VkBuffer getVertexBuffer() {
            return m_graphicsModel->getVertexBuffer();
        }
        VkBuffer getIndexBuffer() {
            return m_graphicsModel->getIndexBuffer();
        }

        void generateGraphicsResources() {
            m_graphicsModel = std::make_shared<Graphics::GraphicsModel<Vertex>>(vertices, indices);
        }

        void constructGraphicsDescriptorSetLayout() {

            m_graphicsDescriptorSetLayout = std::make_shared<Graphics::GraphicsDescriptorSetLayout>(minTextureCount);
        }
        void constructGraphicsDescriptorSets(std::shared_ptr<Graphics::GraphicsDescriptorPool> graphicsDescriptorPool
            , std::vector<VkBuffer> uniformBuffers, uint16_t uboSize) {
            while (textures.size() < minTextureCount) {
                textures.push_back(*(DreamAsset::TextureAssetUtil::m_defaultTextureAsset));
            }
            m_graphicsDescriptorSet = std::make_shared<Graphics::GraphicsDescriptorSet>(m_graphicsDescriptorSetLayout
                , graphicsDescriptorPool, textures, uniformBuffers, uboSize);
        }

        VkDescriptorSetLayout getVkDescriptorSetLayout() {
            return m_graphicsDescriptorSetLayout->GetDescriptSetLayout();
        }

        VkDescriptorSet getVkDescriptorSet(int frame) {
            return m_graphicsDescriptorSet->getDescriptorSets()[frame];;
        }
    private:
        std::shared_ptr<Graphics::GraphicsModel<Vertex>> m_graphicsModel;
        /*  函数  */
        std::shared_ptr<Graphics::GraphicsDescriptorSetLayout> m_graphicsDescriptorSetLayout;
        std::shared_ptr<Graphics::GraphicsDescriptorSet> m_graphicsDescriptorSet;
    };
    template<typename Vertex>
    class ModelAsset
    {
    public:
        // model data 
        std::vector<TextureAsset> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
        std::vector<Mesh<Vertex>>    meshes;
        std::string directory;
        bool gammaCorrection;

        // constructor, expects a filepath to a 3D model.
        ModelAsset(std::string const& path)
        {
            loadModel(path);
        }

        // draws the model, and thus all its meshes

    private:
        // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
        void loadModel(std::string const& path)
        {
            // read file via ASSIMP
            Assimp::Importer importer;
            const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
            // check for errors
            if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
            {
                throw std::runtime_error( importer.GetErrorString()) ;
                return;
            }
            // retrieve the directory path of the filepath
            directory = path.substr(0, path.find_last_of('/'));
            directory += "/";
            // process ASSIMP's root node recursively
            processNode(scene->mRootNode, scene);
        }

        // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
        void processNode(aiNode* node, const aiScene* scene)
        {
            // process each mesh located at the current node
            for (unsigned int i = 0; i < node->mNumMeshes; i++)
            {
                // the node object only contains indices to index the actual objects in the scene. 
                // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
                aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
                meshes.push_back(processMesh(mesh, scene));
            }
            // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
            for (unsigned int i = 0; i < node->mNumChildren; i++)
            {
                processNode(node->mChildren[i], scene);
            }

        }

        Mesh<Vertex> processMesh(aiMesh* mesh, const aiScene* scene)
        {
            // data to fill
            std::vector<Vertex> vertices;
            std::vector<uint16_t> indices;
            std::vector<TextureAsset> textures;

            // walk through each of the mesh's vertices
            for (unsigned int i = 0; i < mesh->mNumVertices; i++)
            {
                Vertex vertex;
                glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
                // positions
                vector.x = mesh->mVertices[i].x;
                vector.y = mesh->mVertices[i].y;
                vector.z = mesh->mVertices[i].z;
                vertex.Position = vector;
                // normals
                if (mesh->HasNormals())
                {
                    vector.x = mesh->mNormals[i].x;
                    vector.y = mesh->mNormals[i].y;
                    vector.z = mesh->mNormals[i].z;
                    vertex.Normal = vector;
                }
                // texture coordinates
                if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
                {
                    glm::vec2 vec;
                    // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
                    // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
                    vec.x = mesh->mTextureCoords[0][i].x;
                    vec.y = mesh->mTextureCoords[0][i].y;
                    vertex.TexCoords = vec;
                    // tangent
                    vector.x = mesh->mTangents[i].x;
                    vector.y = mesh->mTangents[i].y;
                    vector.z = mesh->mTangents[i].z;
                    vertex.Tangent = vector;
                    // bitangent
                    vector.x = mesh->mBitangents[i].x;
                    vector.y = mesh->mBitangents[i].y;
                    vector.z = mesh->mBitangents[i].z;
                    vertex.Bitangent = vector;
                }
                else
                    vertex.TexCoords = glm::vec2(0.0f, 0.0f);

                vertices.push_back(vertex);
            }
            // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
            for (unsigned int i = 0; i < mesh->mNumFaces; i++)
            {
                aiFace face = mesh->mFaces[i];
                // retrieve all indices of the face and store them in the indices vector
                for (unsigned int j = 0; j < face.mNumIndices; j++)
                    indices.push_back(face.mIndices[j]);
            }
            // process materials
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
            // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
            // Same applies to other texture as the following list summarizes:
            // diffuse: texture_diffuseN
            // specular: texture_specularN
            // normal: texture_normalN

            std::vector<uint16_t> textureIndexArray{};
            // 1. diffuse maps
            std::vector<TextureAsset> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", textureIndexArray);
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            // 2. specular maps
            std::vector<TextureAsset> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", textureIndexArray);
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            // 3. normal maps
            std::vector<TextureAsset> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", textureIndexArray);
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            // 4. height maps
            std::vector<TextureAsset> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height", textureIndexArray);
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

            // return a mesh object created from the extracted mesh data
            return Mesh(vertices, indices, textures, textureIndexArray);
        }

        // checks all material textures of a given type and loads the textures if they're not loaded yet.
        // the required info is returned as a Texture struct.
        std::vector<TextureAsset> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, std::vector<uint16_t>& textureIndexArray)
        {
            std::vector<TextureAsset> textures;
            for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
            {
                aiString str;
                mat->GetTexture(type, i, &str);
                // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
                bool skip = false;
                std::string textureName = std::string(str.C_Str());
                std::string path = directory + textureName;
                for (unsigned int j = 0; j < textures_loaded.size(); j++)
                {
                    if (std::strcmp(textures_loaded[j].name.data(), str.C_Str()) == 0)
                    {
                        textures.push_back(textures_loaded[j]);
                        textureIndexArray.push_back(j);
                        skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                        break;
                    }
                }
                if (!skip)
                {   // if texture hasn't been loaded already, load it
                    TextureAsset texture = TextureAsset(textureName, TexutreType::Color, R8G8B8A8_SRGB, path);
                    textureIndexArray.push_back(textures_loaded.size());
                    textures.push_back(texture);
                    textures_loaded.push_back(texture);
                }
            }
            return textures;
        }
        void release() {
            for (unsigned int i = 0; i < textures_loaded.size(); i++)
                textures_loaded[i].release();
            for (unsigned int i = 0; i < meshes.size(); i++)
                meshes[i].release();
        }

        public:
        ~ModelAsset() 
        {
            release();
        }
        void draw(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout,  int frame)
        {
            for (int i = 0; i < meshes.size(); i++) {
                VkBuffer vertexBuffers[] = { meshes[i].getVertexBuffer() };
                VkDeviceSize offsets[] = { 0 };
                VkDescriptorSet descroiptorSet = meshes[i].getVkDescriptorSet(frame);
                vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(commandBuffer, meshes[i].getIndexBuffer(), 0, VK_INDEX_TYPE_UINT16);
                vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descroiptorSet, 0, nullptr);
                vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(meshes[i].indices.size()), 1, 0, 0, 0);
            }
        }

        void generateGraphicsResources() {
            for (int i = 0; i < textures_loaded.size(); i++) 
            {
                textures_loaded[i].generateGraphicsResources();
            }
            for (int i = 0; i < meshes.size(); i++) {
                meshes[i].generateGraphicsResources();
                for (int j = 0; j < meshes[i].textureIndexArray.size(); j++) 
                {
                    uint16_t index = meshes[i].textureIndexArray[j];
                    meshes[i].textures[j].setGraphicsTexture(textures_loaded[index].getGraphicsTexture());
                    meshes[i].textures[j].width = textures_loaded[index].width;
                    meshes[i].textures[j].height = textures_loaded[index].height;
                }

            }
        }

        void constructGraphicsDescriptorSetLayout() {
            int minTextureCount = -1;
            for (int i = 0; i < meshes.size(); i++) {
                minTextureCount = std::max(minTextureCount, (int)meshes[i].textures.size());
            }
            for (int i = 0; i < meshes.size(); i++) {
                meshes[i].minTextureCount = minTextureCount;
                meshes[i].constructGraphicsDescriptorSetLayout();
            }
        }
        void constructGraphicsDescriptorSets(std::shared_ptr<Graphics::GraphicsDescriptorPool> graphicsDescriptorPool
            , std::vector<VkBuffer> uniformBuffers, uint16_t uboSize) {
            for (int i = 0; i < meshes.size(); i++) {
                meshes[i].constructGraphicsDescriptorSets(graphicsDescriptorPool, uniformBuffers, uboSize);
            }
        }

        std::vector<VkDescriptorSetLayout> getDescriptorSetLayout() {
            std::vector<VkDescriptorSetLayout> vkDescriptorSetLayouts;
            vkDescriptorSetLayouts.resize(meshes.size());
            for (int i = 0; i < meshes.size(); i++) {
                vkDescriptorSetLayouts[i] = meshes[i].getVkDescriptorSetLayout();
            }
            return vkDescriptorSetLayouts;
        }


    };


}