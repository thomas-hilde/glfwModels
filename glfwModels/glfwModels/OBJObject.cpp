#include "OBJObject.h"
#include <iostream> // Needed to use cerr and endl
#include <limits>       // std::numeric_limits
#include "Window.h"

OBJObject::OBJObject(const char *filepath)
{
	toWorld = glm::mat4(1.0f);
	parse(filepath);
}

void OBJObject::parse(const char *filepath)
{
	//TODO parse the OBJ file
	FILE* fp;     // file pointer
	float x, y, z;  // vertex coordinates
	float r, g, b;  // vertex color
	float scale;  // Scale model size

	// Used to reduce code clutter
	float min = (float)std::numeric_limits<int>::min();
	float max = (float)std::numeric_limits<int>::max();

	// Used to find center point of model
	float min_x = max, min_y = max, min_z = max;
	float max_x = min, max_y = min, max_z = min;
	float center_x, center_y, center_z;
	int c1, c2;    // characters read from file


	fp = fopen(filepath, "rb");  // make the file name configurable so you can load other files
	if (fp == NULL) { std::cerr << "error loading file" << std::endl; exit(-1); }  // just in case the file can't be found or is corrupt

	// Set scale values according to model
	if (filepath == "bunny.obj") { scale = (float) 7.5; }
	if (filepath == "dragon.obj") { scale = (float)9; }
	if (filepath == "bear.obj") { scale = (float) 0.9; }

	// Get initial characters of first line
	c1 = fgetc(fp);
	c2 = fgetc(fp);

	while (c1 != EOF && c2 != EOF) {
		// Handle vertex lines
		if ((c1 == 'v') && (c2 == ' '))
		{
			fscanf(fp, "%f %f %f %f %f %f", &x, &y, &z, &r, &g, &b);
			if (x < min_x) { min_x = x; }
			if (x > max_x) { max_x = x; }
			if (y < min_y) { min_y = y; }
			if (y > max_y) { max_y = y; }
			if (z < min_z) { min_z = z; }
			if (z > max_z) { max_z = z; }
			vertices.push_back(glm::vec3(x, y, z));
		}
		// read normal data accordingly
		else if ((c1 == 'v') && (c2 == 'n'))
		{
			fscanf(fp, "%f %f %f", &x, &y, &z);
			normals.push_back(glm::normalize(glm::vec3(x, y, z)));
		}

		// Get first two character of next line
		c1 = fgetc(fp);
		c2 = fgetc(fp);
	}

	// Find Center point
	center_x = (min_x + max_x) / 2;
	center_y = (min_y + max_y) / 2;
	center_z = (min_z + max_z) / 2;

	// Center & Scale the model
	for (unsigned int i = 0; i < vertices.size(); ++i)
	{
		vertices[i].x = (vertices[i].x - center_x) * scale;
		vertices[i].y = (vertices[i].y - center_y) * scale;
		vertices[i].z = (vertices[i].z - center_z) * scale;
	}

	fclose(fp);   // make sure you don't forget to close the file when done
	// Populate the face indices, vertices, and normals vectors with the OBJ Object data
}

void OBJObject::draw()
{
	glMatrixMode(GL_MODELVIEW);

	// Push a save state onto the matrix stack, and multiply in the toWorld matrix
	glPushMatrix();
	glMultMatrixf(&(toWorld[0][0]));

	glBegin(GL_POINTS);

	// Loop through all the vertices of this OBJ Object and render them
	for (unsigned int i = 0; i < vertices.size(); ++i)
	{
		glNormal3f(normals[i].x, normals[i].y, normals[i].z);
		glColor3f((normals[i].x + 1) / 2, (normals[i].y + 1) / 2, (normals[i].z + 1) / 2);
		glVertex3f(vertices[i].x, vertices[i].y, vertices[i].z);
	}
	glEnd();

	// Pop the save state off the matrix stack
	// This will undo the multiply we did earlier
	glPopMatrix();
}

void OBJObject::update()
{
	spin(1.0f);
}

void OBJObject::spin(float deg)
{
	this->angle += deg;
	if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
	// This creates the matrix to rotate the current model
	this->toWorld = glm::rotate(glm::mat4(1.0f), this->angle / 180.0f * glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));
}

void OBJObject::transfrom(OBJObject* model, GLFWwindow* window) {

	OBJObject next_model = *model;
	std::vector<glm::vec3> curr_vertices;
	std::vector<glm::vec3> translations;
	float increments = 100;

	// Check which model is bigger
	if (this->vertices.size() >= next_model.vertices.size()) {

		// Set up point value matrix
		curr_vertices = this->vertices;
		next_model.normals = this->normals;

		// Set up translation matrix
		int k = 0;
		for (int i = 0; i < curr_vertices.size(); i++) {

			if (k >= next_model.vertices.size()) {
				k = 0;
			}

			translations.push_back(glm::vec3(
				(model->vertices[k][0] - curr_vertices[i][0]) / increments,
				(model->vertices[k][1] - curr_vertices[i][1]) / increments,
				(model->vertices[k][2] - curr_vertices[i][2]) / increments));

			k++;

		}

		// Copy over values into new model
		next_model.vertices = curr_vertices;

		// Move points from current model space to desired model space
		for (int i = 0; i < increments; i++) {

			//Update point values
			for (int j = 0; j < next_model.vertices.size(); j++) {
				next_model.vertices[j][0] = next_model.vertices[j][0] + translations[j][0];
				next_model.vertices[j][1] = next_model.vertices[j][1] + translations[j][1];
				next_model.vertices[j][2] = next_model.vertices[j][2] + translations[j][2];
			}

			// Clear the color and depth buffers
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// Set the matrix mode to GL_MODELVIEW
			glMatrixMode(GL_MODELVIEW);
			// Load the identity matrix
			glLoadIdentity();

			// Render objects
			next_model.draw();

			// Swap buffers
			glfwSwapBuffers(window);

		}

	}
	else {

		// Setup values matrix
		int j = 0;
		int i = 0;
		for (i = 0; i < next_model.vertices.size(); i++) {

			if (j >= this->vertices.size()) {
				j = 0;
			}

			curr_vertices.push_back(this->vertices[j]);
			j++;
		}

		// Set up translation matrix
		for (int i = 0; i < next_model.vertices.size(); i++) {
			translations.push_back(glm::vec3(
				(next_model.vertices[i][0] - curr_vertices[i][0]) / increments,
				(next_model.vertices[i][1] - curr_vertices[i][1]) / increments,
				(next_model.vertices[i][2] - curr_vertices[i][2]) / increments));
		}

		// Copy vertices to new model
		next_model.vertices = curr_vertices;

		// Move points from current model space to desired model space
		for (int i = 0; i < increments; i++) {

			//Update point values
			for (int j = 0; j < next_model.vertices.size(); j++) {
				next_model.vertices[j][0] = next_model.vertices[j][0] + translations[j][0];
				next_model.vertices[j][1] = next_model.vertices[j][1] + translations[j][1];
				next_model.vertices[j][2] = next_model.vertices[j][2] + translations[j][2];

			}

			// Clear the color and depth buffers
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// Set the matrix mode to GL_MODELVIEW
			glMatrixMode(GL_MODELVIEW);
			// Load the identity matrix
			glLoadIdentity();

			// Render objects
			next_model.draw();

			// Swap buffers
			glfwSwapBuffers(window);

		}
	}
}
