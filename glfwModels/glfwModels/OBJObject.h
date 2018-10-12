#ifndef OBJOBJECT_H
#define OBJOBJECT_H
#define BUNNY_SCALE (float) 7.5
#define DRAGON_SCALE (float) 9
#define BEAR_SCALE (float) 0.9

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>


class OBJObject
{
private:
	std::vector<unsigned int> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	glm::mat4 toWorld;

public:
	OBJObject(const char* filepath);

	float size;
	float angle;

	void parse(const char* filepath);
	void draw();
	void update();

	void spin(float);
	void transfrom(OBJObject* model, GLFWwindow* window);
};

#endif