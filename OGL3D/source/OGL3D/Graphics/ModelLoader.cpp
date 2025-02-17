#include <OGL3D/Graphics/ModelLoader.h>

ModelLoader::ModelLoader(string const& path, bool gamma)
{
    gammaCorrection = gamma;
    loadModel(path);
    OGL3D_INFO("Model loaded successfully:"<<path.c_str()<<"\n");
}

void ModelLoader::Draw(ShaderPtr shader)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}

void ModelLoader::loadModel(string const& path)
{
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate |aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
        return;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);
}

void ModelLoader::processNode(aiNode* node, const aiScene* scene)
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

Mesh ModelLoader::processMesh(aiMesh* mesh, const aiScene* scene)
{
    // data to fill
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    vector<Texture> textures;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        OVec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.m_x = mesh->mVertices[i].x;
        vector.m_y = mesh->mVertices[i].y;
        vector.m_z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector.m_x = mesh->mNormals[i].x;
            vector.m_y = mesh->mNormals[i].y;
            vector.m_z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        /*OVec4 col;
        if (mesh->HasVertexColors(i))
        {
            col.m_x = mesh->mColors[i]->r;
            col.m_y = mesh->mColors[i]->g;
            col.m_z = mesh->mColors[i]->b;
            col.m_w = mesh->mColors[i]->a;
        }
        vertex.Color = col;*/
        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            OVec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.m_x = mesh->mTextureCoords[0][i].x;
            vec.m_y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
            // tangent
            vector.m_x = mesh->mTangents[i].x;
            vector.m_y = mesh->mTangents[i].y;
            vector.m_z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.m_x = mesh->mBitangents[i].x;
            vector.m_y = mesh->mBitangents[i].y;
            vector.m_z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        }
        else
            vertex.TexCoords = OVec2(0.0f, 0.0f);

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
    
    // 1. diffuse maps
    vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    int counter = 1;
    while (textures.size() == 0 && counter <= 21)
    {
        //try everything
        if (counter == 18)
        {
            counter++;
            continue;

        }
        diffuseMaps = loadMaterialTextures(material, aiTextureType(counter), "texture_ambient");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        counter++;
    }
    


    return Mesh(vertices, indices, textures);
}

vector<Texture> ModelLoader::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
    vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if (!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }
    }
    
    return textures;
}
unsigned int TextureFromFile(const char* path, const string& directory, bool gamma)
{
    string filename = string(path);
    filename = filename.substr(filename.find_last_of('/') + 1);
    filename = directory + "/textures/" + filename;

    string wild = filename.substr(filename.find_last_of('.') + 1);
    if (wild.compare("tga") == 0)
    {
        filename = filename + ".png";
    }
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        glBindTexture(GL_TEXTURE_2D,0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        try
        {
            glTexImage2D(GL_TEXTURE_2D,0,format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        }
        catch (...)
        {
            std::cout << "lol\n";
        }

        try
        {
        
        glGenerateMipmap(GL_TEXTURE_2D);
        }
        catch (...)
        {
            std::cout << "lol\n";
        }


        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
