#pragma once
#include "Object.h"
namespace Dream {
    enum TexutreType {
        Depth,
        Color
    };
    class Texture :
        public Object
    {
        public:
            TexutreType type;
            Texture(std::string name, TexutreType type)
            {
                this->name = name;
                this->type = type;
            };

    };

}
