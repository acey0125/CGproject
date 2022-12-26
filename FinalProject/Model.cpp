#include "Model.h"

void Model::load(std::string filepath)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);
	// �쳣����
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "��ȡģ�ͳ��ִ���: " << import.GetErrorString() << std::endl;
		exit(-1);
	}
	// ģ���ļ����·��
	std::string rootPath = filepath.substr(0, filepath.find_last_of('/'));

	// ѭ������ mesh
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		// ���õ�ǰmesh
		meshes.push_back(Mesh());
		Mesh& mesh = meshes.back();

		// ��ȡ assimp �Ķ�ȡ���� aimesh ����
		aiMesh* aimesh = scene->mMeshes[i];

		// ���ǽ����ݴ��ݸ������Զ����mesh
		for (int j = 0; j < aimesh->mNumVertices; j++)
		{
			// ����
			glm::vec3 vvv;
			vvv.x = aimesh->mVertices[j].x;
			vvv.y = aimesh->mVertices[j].y;
			vvv.z = aimesh->mVertices[j].z;
			mesh.vertexPosition.push_back(vvv);
			if (this->maxX <= aimesh->mVertices[j].x)//��ȡ���ֵ
				this->maxX = aimesh->mVertices[j].x;
			if (this->maxY <= aimesh->mVertices[j].y)//��ȡ���ֵ
				this->maxY = aimesh->mVertices[j].y;
			if (this->maxZ <= aimesh->mVertices[j].z)//��ȡ���ֵ
				this->maxZ = aimesh->mVertices[j].z;
			if (this->minX >= aimesh->mVertices[j].x)//��ȡ��Сֵ
				this->minX = aimesh->mVertices[j].x;
			if (this->minY >= aimesh->mVertices[j].y)//��ȡ��Сֵ
				this->minY = aimesh->mVertices[j].y;
			if (this->minZ >= aimesh->mVertices[j].z)//��ȡ��Сֵ
				this->minZ = aimesh->mVertices[j].z;
			// ����
			vvv.x = aimesh->mNormals[j].x;
			vvv.y = aimesh->mNormals[j].y;
			vvv.z = aimesh->mNormals[j].z;
			mesh.vertexNormal.push_back(vvv);

			// ��������: �����������롣assimp Ĭ�Ͽ����ж���������� ����ȡ��һ����0������
			glm::vec2 vv(0, 0);
			if (aimesh->mTextureCoords[0])
			{
				vv.x = aimesh->mTextureCoords[0][j].x;
				vv.y = aimesh->mTextureCoords[0][j].y;
			}
			mesh.vertexTexcoord.push_back(vv);
		}

		// ����в��ʣ���ô���ݲ���
		if (aimesh->mMaterialIndex >= 0)
		{
			// ��ȡ��ǰ aimesh �Ĳ��ʶ���
			aiMaterial* material = scene->mMaterials[aimesh->mMaterialIndex];

			// ��ȡ diffuse ��ͼ�ļ�·������ ����ֻȡ1����ͼ ���� 0 ����
			aiString aistr;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &aistr);
			std::string texpath = aistr.C_Str();
			texpath = rootPath + '/' + texpath;   // ȡ���·��

			// ���û���ɹ�������ô������
			if (textureMap.find(texpath) == textureMap.end())
			{
				// ��������
				//TextureMap(faces);
				GLuint tex;
				glGenTextures(1, &tex);
				glBindTexture(GL_TEXTURE_2D, tex);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
				int textureWidth, textureHeight;
				unsigned char* image = SOIL_load_image(texpath.c_str(), &textureWidth, &textureHeight, 0, SOIL_LOAD_RGB);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureWidth, textureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);   // ��������
				delete[] image;

				textureMap[texpath] = tex;
			}

			// ��������
			mesh.diffuseTexture = textureMap[texpath];
		}

		// ������Ƭ����
		for (GLuint j = 0; j < aimesh->mNumFaces; j++)
		{
			aiFace face = aimesh->mFaces[j];
			for (GLuint k = 0; k < face.mNumIndices; k++)
			{
				mesh.index.push_back(face.mIndices[k]);
			}
		}
		mesh.bindData();
	}
}
void Model::draw(GLuint program)
{
	// ��ģ�;���
	glm::mat4 unit(    // ��λ����
		glm::vec4(1, 0, 0, 0),
		glm::vec4(0, 1, 0, 0),
		glm::vec4(0, 0, 1, 0),
		glm::vec4(0, 0, 0, 1)
	);
	glm::mat4 scale = glm::scale(unit, this->scale);
	glm::mat4 translate = glm::translate(unit, this->translate);

	glm::mat4 rotate = unit;    // ��ת
	rotate = glm::rotate(rotate, glm::radians(this->rotate.x), glm::vec3(1, 0, 0));
	rotate = glm::rotate(rotate, glm::radians(this->rotate.y), glm::vec3(0, 1, 0));
	rotate = glm::rotate(rotate, glm::radians(this->rotate.z), glm::vec3(0, 0, 1));

	// ģ�ͱ任����
	glm::mat4 model = translate * rotate * scale;
	GLuint mlocation = glGetUniformLocation(program, "model");    // ��Ϊmodel��uniform������λ������
	glUniformMatrix4fv(mlocation, 1, GL_FALSE, glm::value_ptr(model));   // �����Ⱦ���

	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].draw(program);
	}
}
void Model::Change(double ScaleX, double ScaleY, double ScaleZ, double TransX, double TransY, double TransZ) {
	maxX *= ScaleX;
	minX *= ScaleX;
	maxY *= ScaleY;
	minY *= ScaleY;
	maxZ *= ScaleZ;
	minZ *= ScaleZ;
	maxX += TransX;
	minX += TransX;
	maxY += TransY;
	minY += TransY;
	maxZ += TransZ;
	minZ += TransZ;
}