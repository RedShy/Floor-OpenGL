#ifndef OBJECT_H
#define OBJECT_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "camera.h"
#include "Model.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
#include <iostream>

enum Draw_Type {
	LINES,
	TRIANGLES
};

class Object
{
	public:
		Object(Shader* _shader, const string& modelPath):VAO(0), startVertex(0), nVertices(0), position(glm::vec3(0.0f)),
		shader(_shader), modelTransform(glm::mat4()),scale(glm::vec3(1.0f)), model(NULL),
		color(glm::vec3(0.0f)), texture(0), loadedTexture(false), loadedModel(false),
		collisionVertex(glm::vec3(1.5,0.0f,0.0f)), rotationAngle(0), rotationAxis(glm::vec3(1.0f))
		{
			loadModel(modelPath);
		}

		Object(Shader* _shader, const unsigned int _VAO, 
			const unsigned int _startVertex, const unsigned int _nVertices):position(glm::vec3(0.0f)), 
		shader(_shader),modelTransform(glm::mat4()), scale(glm::vec3(1.0f)), color(glm::vec3(0.0f)),texture(0), VAO(_VAO), 
		startVertex(_startVertex), nVertices(_nVertices), loadedTexture(false), loadedModel(false), collisionVertex(glm::vec3(0.0f)),
		rotationAngle(0), rotationAxis(glm::vec3(1.0f)), model(NULL)
		{

		}

		void draw(Draw_Type type = TRIANGLES)
		{
	        shader->use();

	        modelTransform = glm::mat4();
	        modelTransform = glm::translate(modelTransform, position);
	        modelTransform = glm::scale(modelTransform, scale);
	        modelTransform = glm::rotate(modelTransform, glm::radians(rotationAngle), rotationAxis);
	        shader->setMat4("model",modelTransform);
	        shader->setVec3("color",color);
	        glBindVertexArray(VAO);
	        if(loadedTexture)
	        {	
	        	glBindTexture(GL_TEXTURE_2D, texture);
	        }
	        if(loadedModel)
	        {
	        	model->Draw(*shader);
	        }
	        else
	        {
	        	if(type==TRIANGLES)
	        	{
	        		glDrawArrays(GL_TRIANGLES, startVertex, nVertices);
	        	}
	        	else if(type==LINES)
	        	{
	        		glDrawArrays(GL_LINES, startVertex, nVertices);
	        	}
	        	
	        }
	        

	        glBindVertexArray(0);
	        glBindTexture(GL_TEXTURE_2D, 0);
		}

		void setModelTransform(const glm::mat4& _model)
		{
			modelTransform=_model;
		}

		void setScale(const glm::vec3& _scale)
		{
			scale=_scale;
		}

		void setRotation(const float _rotationAngle,const glm::vec3& _rotationAxis)
		{
			rotationAxis=_rotationAxis;
			rotationAngle=_rotationAngle;
		}

		void setPosition(const glm::vec3& _position)
		{
			position=_position;
		}

		void setColor(const glm::vec3& _color)
		{
			color=_color;
		}

		void setCollisionVertex(const glm::vec3& _collisionVertex)
		{
			collisionVertex=_collisionVertex;
		}

		const glm::vec3& getPosition()
		{
			return position;
		}

		void loadTexture(char const * path)
		{
		    glGenTextures(1, &texture);

		    int width, height, nrComponents;
		    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
		    if (data)
		    {
		        GLenum format;
		        if (nrComponents == 1)
		            format = GL_RED;
		        else if (nrComponents == 3)
		            format = GL_RGB;
		        else if (nrComponents == 4)
		            format = GL_RGBA;

		        glBindTexture(GL_TEXTURE_2D, texture);
		        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		        glGenerateMipmap(GL_TEXTURE_2D);

		        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		        stbi_image_free(data);

		        loadedTexture=true;
		    }
		    else
		    {
		        std::cout << "Texture failed to load at path: " << path << std::endl;
		        stbi_image_free(data);
		        loadedTexture=false;
		    }
		}

		bool isInCollision(const glm::vec3& otherPosition)
		{
			glm::vec3 direction = position - otherPosition;
			
			float collisionDistance=glm::length(glm::vec3 (0.0f,0.5f,0.0f) - (scale * collisionVertex));
			float distance=glm::length(direction);

			//std::cout<<"DISTANCE:"<<distance<<" COLLISION DISTANCE: "<<collisionDistance<<"\n";

			return distance <= collisionDistance;
		}

		void loadModel(const string& path)
		{
			if(model!=NULL)
			{
				delete model;
			}
			model=new Model(path);
			loadedModel=true;
		}

		~Object()
		{
			if(model!=NULL)
				delete model;
		}
	private:
		Shader* shader;
		glm::vec3 position;
		glm::vec3 scale;
		glm::vec3 color;
		glm::mat4 modelTransform;
		unsigned int texture;
		unsigned int VAO;
		unsigned int startVertex;
		unsigned int nVertices;
		glm::vec3 collisionVertex;
		bool loadedTexture;
		bool loadedModel;
		Model* model;
		float rotationAngle;
		glm::vec3 rotationAxis;
		Object (const Object&) = delete;              
 		Object& operator=(const Object&) = delete;

};

#endif