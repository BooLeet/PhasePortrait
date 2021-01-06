#include "TrailRenderer.h"
#include "Engine/ShaderLoader.h"

void PrintVec3(const vec3& vector)
{
	std::cout << "[" << vector.x << ", " << vector.y << ", " << vector.x << "]\n";
}


void TrailRenderer::Start()
{

	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");
}

void TrailRenderer::Render(mat4 projectionViewMatrix)
{
	size_t currentSamplesSize = samples.size();
	if (currentSamplesSize == 0)
		return;

	size_t length = currentSamplesSize * 3;

	// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
	// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
	GLfloat* g_vertex_buffer_data = new GLfloat[length]{ 0 };
	GLfloat* g_color_buffer_data = new GLfloat[length]{ 0 };

	std::deque<vec3> samplesCopy = samples;

	vec3 trailHead = samplesCopy.front();

	for (int i = 0; i < currentSamplesSize; ++i)
	{
		vec3 currentVertex = samplesCopy.front() - trailHead;
		samplesCopy.pop_front();


		g_vertex_buffer_data[i * 3] = currentVertex.x;
		g_vertex_buffer_data[i * 3 + 1] = currentVertex.y;
		g_vertex_buffer_data[i * 3 + 2] = currentVertex.z;

		float colorInterpolationParameter = i / (float)currentSamplesSize;
		
		vec3 newColor = (1 - colorInterpolationParameter) * trailHeadColor + colorInterpolationParameter * trailTailColor;

		g_color_buffer_data[i * 3] = newColor.x;
		g_color_buffer_data[i * 3 + 1] = newColor.y;
		g_color_buffer_data[i * 3 + 2] = newColor.z;
	}


	
	/*for (int i = 0; i < currentSamplesSize; ++i)
	{
		GLfloat value = i;

		g_color_buffer_data[i * 3] = 0;
		g_color_buffer_data[i * 3 + 1] = 0;
		g_color_buffer_data[i * 3 + 2] = 0;
	}*/

	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * length, g_vertex_buffer_data, GL_STATIC_DRAW);

	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * length, g_color_buffer_data, GL_STATIC_DRAW);

	delete[] g_vertex_buffer_data;
	delete[] g_color_buffer_data;

	glUseProgram(programID);

	// Model matrix
	glm::mat4 Model = sceneObject->transform.GetModelMatrix(); //glm::translate(glm::mat4(1.0), glm::vec3(3, 1, 5));;

	glm::mat4 MVP = projectionViewMatrix * Model;

	// Send our transformation to the currently bound shader, 
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : colors
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);


	glPointSize(10);
	GLenum mode = GL_LINE_STRIP;
	if (renderMode == RenderMode::Points)
		mode = GL_POINTS;
	else if (renderMode == RenderMode::Triangles)
		mode = GL_TRIANGLE_STRIP;

	glDrawArrays(mode, 0, currentSamplesSize); // 12*3 indices starting at 0 -> 12 triangles

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
}

void TrailRenderer::Update()
{
	testAngle += 2 * engine->GetDeltaTime();
	sceneObject->transform.SetPosition(((float)3) * vec3(cos(testAngle), sin(2 * testAngle), 0));

	if(samples.size() > 0)
		samples.pop_front();
	samples.emplace_front(sceneObject->transform.GetPosition());
	sampleTimeCounter += engine->GetDeltaTime();

	if (sampleTimeCounter >= sampleLifeTime)
	{
		sampleTimeCounter -= sampleLifeTime;
		samples.emplace_front(sceneObject->transform.GetPosition());
		if (samples.size() > maxSamples)
			samples.pop_back();
	}
}

void TrailRenderer::OnDestroy()
{
	// Cleanup VBO and shader
	
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);
}
