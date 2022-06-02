//#include <OGL3D/Graphics/Model.h>
//
//
//Model::Model(const char* file)
//{
//	// Make a JSON object
//	std::string text = get_file_contents(file);
//	JSON = json::parse(text);
//
//	// Get the binary data
//	Model::file = file;
//	data = getData();
//
//	// Traverse all nodes
//	traverseNode(0);
//
//}
//
//
//void Model::loadMesh(unsigned int indMesh)
//{
//	// Get all accessor indices
//	unsigned int posAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["POSITION"];
//	unsigned int normalAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["NORMAL"];
//	unsigned int texAccInd = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["TEXCOORD_0"];
//	unsigned int indAccInd = JSON["meshes"][indMesh]["primitives"][0]["indices"];
//
//	// Use accessor indices to get all vertices components
//	std::vector<f32> posVec = getFloats(JSON["accessors"][posAccInd]);
//	std::vector<OVec3> positions = groupFloatsVec3(posVec);
//	std::vector<f32> normalVec = getFloats(JSON["accessors"][normalAccInd]);
//	std::vector<OVec3> normals = groupFloatsVec3(normalVec);
//	std::vector<f32> texVec = getFloats(JSON["accessors"][texAccInd]);
//	std::vector<OVec2> texUVs = groupFloatsVec2(texVec);
//
//	// Combine all the vertex components and also get the indices and textures
//	std::vector<Vertex> vertices = assembleVertices(positions, normals, texUVs);
//	std::vector<GLuint> indices = getIndices(JSON["accessors"][indAccInd]);
//	std::vector<Texture> textures = getTextures();
//
//	// Combine the vertices, indices, and textures into a mesh
//	meshes.push_back(Mesh(vertices, indices, textures));
//}
//
//void Model::traverseNode(unsigned int nextNode, OMat4 matrix)
//{
//	// Current node
//	matrix.setIdentity();
//	json node = JSON["nodes"][nextNode];
//
//	// Get translation if it exists
//	OVec3 translation = OVec3(0.0f, 0.0f, 0.0f);
//	if (node.find("translation") != node.end())
//	{
//		f32 transValues[3];
//		for (ui32 i = 0; i < node["translation"].size(); i++)
//			transValues[i] = (node["translation"][i]);
//		std::vector<f32> v(transValues, transValues + 3);
//		translation = OVec3::make_vec3(v);
//	}
//	// Get quaternion if it exists
//	OVec4 rotation = OVec4(1.0f, 0.0f, 0.0f, 0.0f);
//	if (node.find("rotation") != node.end())
//	{
//		float rotValues[4] =
//		{
//			node["rotation"][3],
//			node["rotation"][0],
//			node["rotation"][1],
//			node["rotation"][2]
//		};
//		std::vector<f32> v(rotValues, rotValues + 4);
//		rotation = OVec4::make_vec4(v);
//	}
//	// Get scale if it exists
//	OVec3 scale = OVec3(1.0f, 1.0f, 1.0f);
//	if (node.find("scale") != node.end())
//	{
//		float scaleValues[3];
//		for (unsigned int i = 0; i < node["scale"].size(); i++)
//			scaleValues[i] = (node["scale"][i]);
//		std::vector<f32> v(scaleValues, scaleValues +3);
//		scale = OVec3::make_vec3(v);
//	}
//	// Get matrix if it exists
//	OMat4 matNode = OMat4(1.0f);
//	if (node.find("matrix") != node.end())
//	{
//		float matValues[16];
//		for (unsigned int i = 0; i < node["matrix"].size(); i++)
//			matValues[i] = (node["matrix"][i]);
//		std::vector<f32> v(matValues, matValues+16);
//		matNode = OMat4::make_mat4(v);
//	}
//
//	// Initialize matrices
//	OMat4 trans = OMat4(1.0f);
//	OMat4 rot = OMat4(1.0f);
//	OMat4 sca = OMat4(1.0f);
//
//	trans.setTranslation(translation);
//	rot = OMat4::mat4_cast(rotation);
//	sca.setScale(scale);
//	// Use translation, rotation, and scale to change the initialized matrices
//	
//	// Multiply all matrices together
//	OMat4 matNextNode;
//	matNextNode.setIdentity();
//	matNextNode *= matrix;
//	matNextNode *= matNode;
//	matNextNode *= trans;
//	matNextNode *= rot;
//	matNextNode *=sca;
//
//	// Check if the node contains a mesh and if it does load it
//	if (node.find("mesh") != node.end())
//	{
//		translationsMeshes.push_back(translation);
//		rotationsMeshes.push_back(rotation);
//		scalesMeshes.push_back(scale);
//		matricesMeshes.push_back(matNextNode);
//
//		loadMesh(node["mesh"]);
//	}
//
//	// Check if the node has children, and if it does, apply this function to them with the matNextNode
//	if (node.find("children") != node.end())
//	{
//		for (unsigned int i = 0; i < node["children"].size(); i++)
//			traverseNode(node["children"][i], matNextNode);
//	}
//}
//
//std::vector<unsigned char> Model::getData()
//{
//	// Create a place to store the raw text, and get the uri of the .bin file
//	std::string bytesText;
//	std::string uri = JSON["buffers"][0]["uri"];
//
//	// Store raw text data into bytesText
//	std::string fileStr = std::string(file);
//	std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);
//	bytesText = get_file_contents((fileDirectory + uri).c_str());
//
//	// Transform the raw text data into bytes and put them in a vector
//	std::vector<unsigned char> data(bytesText.begin(), bytesText.end());
//	return data;
//}
//
//std::vector<f32> Model::getFloats(json accessor)
//{
//	std::vector<f32> floatVec;
//
//	// Get properties from the accessor
//	unsigned int buffViewInd = accessor.value("bufferView", 1);
//	unsigned int count = accessor["count"];
//	unsigned int accByteOffset = accessor.value("byteOffset", 0);
//	std::string type = accessor["type"];
//
//	// Get properties from the bufferView
//	json bufferView = JSON["bufferViews"][buffViewInd];
//	unsigned int byteOffset = bufferView["byteOffset"];
//
//	// Interpret the type and store it into numPerVert
//	unsigned int numPerVert;
//	if (type == "SCALAR") numPerVert = 1;
//	else if (type == "VEC2") numPerVert = 2;
//	else if (type == "VEC3") numPerVert = 3;
//	else if (type == "VEC4") numPerVert = 4;
//	else throw std::invalid_argument("Type is invalid (not SCALAR, VEC2, VEC3, or VEC4)");
//
//	// Go over all the bytes in the data at the correct place using the properties from above
//	unsigned int beginningOfData = byteOffset + accByteOffset;
//	unsigned int lengthOfData = count * 4 * numPerVert;
//	for (unsigned int i = beginningOfData; i < beginningOfData + lengthOfData; i)
//	{
//		unsigned char bytes[] = { data[i++], data[i++], data[i++], data[i++] };
//		float value;
//		std::memcpy(&value, bytes, sizeof(float));
//		floatVec.push_back(value);
//	}
//
//	return floatVec;
//}
//
//std::vector<GLuint> Model::getIndices(json accessor)
//{
//	std::vector<GLuint> indices;
//
//	// Get properties from the accessor
//	unsigned int buffViewInd = accessor.value("bufferView", 0);
//	unsigned int count = accessor["count"];
//	unsigned int accByteOffset = accessor.value("byteOffset", 0);
//	unsigned int componentType = accessor["componentType"];
//
//	// Get properties from the bufferView
//	json bufferView = JSON["bufferViews"][buffViewInd];
//	unsigned int byteOffset = bufferView["byteOffset"];
//
//	// Get indices with regards to their type: unsigned int, unsigned short, or short
//	unsigned int beginningOfData = byteOffset + accByteOffset;
//	if (componentType == 5125)
//	{
//		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 4; i)
//		{
//			unsigned char bytes[] = { data[i++], data[i++], data[i++], data[i++] };
//			unsigned int value;
//			std::memcpy(&value, bytes, sizeof(unsigned int));
//			indices.push_back((GLuint)value);
//		}
//	}
//	else if (componentType == 5123)
//	{
//		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i)
//		{
//			unsigned char bytes[] = { data[i++], data[i++] };
//			unsigned short value;
//			std::memcpy(&value, bytes, sizeof(unsigned short));
//			indices.push_back((GLuint)value);
//		}
//	}
//	else if (componentType == 5122)
//	{
//		for (unsigned int i = beginningOfData; i < byteOffset + accByteOffset + count * 2; i)
//		{
//			unsigned char bytes[] = { data[i++], data[i++] };
//			short value;
//			std::memcpy(&value, bytes, sizeof(short));
//			indices.push_back((GLuint)value);
//		}
//	}
//
//	return indices;
//}
//
//std::vector<Texture> Model::getTextures()
//{
//	std::vector<Texture> textures;
//
//	std::string fileStr = std::string(file);
//	std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);
//
//	// Go over all images
//	for (unsigned int i = 0; i < JSON["images"].size(); i++)
//	{
//		// uri of current texture
//		std::string texPath = JSON["images"][i]["uri"];
//
//		// Check if the texture has already been loaded
//		bool skip = false;
//		for (unsigned int j = 0; j < loadedTexName.size(); j++)
//		{
//			if (loadedTexName[j] == texPath)
//			{
//				textures.push_back(loadedTex[j]);
//				skip = true;
//				break;
//			}
//		}
//
//		// If the texture has been loaded, skip this
//		if (!skip)
//		{
//			// Load diffuse texture
//			if (texPath.find("baseColor") != std::string::npos)
//			{
//			OTexture diffuse = OTexture(OTextureDesc{ (fileDirectory + texPath).c_str(),(ui32)(loadedTex.size()),"diffuse" });
//			Texture t;
//			t.id = diffuse.getId();
//			t.path = diffuse.path;
//			t.type = diffuse.type;
//			textures.push_back(t);
//			loadedTex.push_back(t);
//			loadedTexName.push_back(texPath);
//			}
//			// Load specular texture
//			else if (texPath.find("metallicRoughness") != std::string::npos)
//			{
//				OTexture specular = OTexture(OTextureDesc{ (fileDirectory + texPath).c_str(), ui32(loadedTex.size()),"specular" });
//				Texture t;
//				t.id = specular.getId();
//				t.path = specular.path;
//				t.type = specular.type;
//				textures.push_back(t);
//				loadedTex.push_back(t);
//				loadedTexName.push_back(texPath);
//			}
//		}
//	}
//
//	return textures;
//}
//
//std::vector<Vertex> Model::assembleVertices
//(
//	std::vector<OVec3> positions,
//	std::vector<OVec3> normals,
//	std::vector<OVec2> texUVs
//)
//{
//	std::vector<Vertex> vertices;
//	for (int i = 0; i < positions.size(); i++)
//	{
//		vertices.push_back
//		(
//			Vertex
//			{
//				positions[i],
//				normals[i],
//				texUVs[i]
//			}
//		);
//	}
//	return vertices;
//}
//
//std::vector<OVec2> Model::groupFloatsVec2(std::vector<f32> floatVec)
//{
//	std::vector<OVec2> vectors;
//	for (int i = 0; i < floatVec.size(); i)
//	{
//		vectors.push_back(OVec2(floatVec[i++], floatVec[i++]));
//	}
//	return vectors;
//}
//std::vector<OVec3> Model::groupFloatsVec3(std::vector<f32> floatVec)
//{
//	std::vector<OVec3> vectors;
//	for (int i = 0; i < floatVec.size(); i)
//	{
//		vectors.push_back(OVec3(floatVec[i++], floatVec[i++], floatVec[i++]));
//	}
//	return vectors;
//}
//std::vector<OVec4> Model::groupFloatsVec4(std::vector<f32> floatVec)
//{
//	std::vector<OVec4> vectors;
//	for (int i = 0; i < floatVec.size(); i)
//	{
//		vectors.push_back(OVec4(floatVec[i++], floatVec[i++], floatVec[i++], floatVec[i++]));
//	}
//	return vectors;
//}
//std::string Model::get_file_contents(const char* filename)
//{
//	std::ifstream in(filename, std::ios::binary);
//	if (in)
//	{
//		std::string contents;
//		in.seekg(0, std::ios::end);
//		contents.resize(in.tellg());
//		in.seekg(0, std::ios::beg);
//		in.read(&contents[0], contents.size());
//		in.close();
//		return(contents);
//	}
//	throw(errno);
//}