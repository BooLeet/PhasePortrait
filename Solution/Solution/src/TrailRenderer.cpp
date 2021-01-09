#include "TrailRenderer.h"
#include "Engine/ShaderLoader.h"

#include "Engine/Mesh.h"
#include <vector>

void PrintVec3(const vec3& vector)
{
	std::cout << "[" << vector.x << ", " << vector.y << ", " << vector.x << "]\n";
}


void TrailRenderer::Start()
{
	programID = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");
}

void TrailRenderer::Render(mat4 projectionViewMatrix)
{
	size_t currentSamplesSize = samples.size();
	if (currentSamplesSize == 0)
		return;

	size_t length = currentSamplesSize * 3;
	std::vector<GLfloat> vertexData(length);
	std::vector<GLfloat> colorData(length);


	std::deque<vec3> samplesCopy = samples;

	vec3 trailHead = samplesCopy.front();

	for (int i = 0; i < currentSamplesSize; ++i)
	{
		vec3 currentVertex = samplesCopy.front() - trailHead;
		samplesCopy.pop_front();

		vertexData[i * 3] = currentVertex.x;
		vertexData[i * 3 + 1] = currentVertex.y;
		vertexData[i * 3 + 2] = currentVertex.z;

		float colorInterpolationParameter = i / (float)currentSamplesSize;
		
		vec3 newColor = (1 - colorInterpolationParameter) * trailHeadColor + colorInterpolationParameter * trailTailColor;

		colorData[i * 3] = newColor.x;
		colorData[i * 3 + 1] = newColor.y;
		colorData[i * 3 + 2] = newColor.z;
	}

	
	GLenum mode = GL_LINE_STRIP;
	if (renderMode == RenderMode::Points)
	{
		glPointSize(10);
		mode = GL_POINTS;
	}
	else if (renderMode == RenderMode::Triangles)
		mode = GL_TRIANGLE_STRIP;

	Mesh meshToDraw = Mesh(vertexData, colorData, programID, mode);
	meshToDraw.Render(projectionViewMatrix, sceneObject->transform.GetModelMatrix());

}

void TrailRenderer::Update()
{
	testAngle += 2 * engine->GetDeltaTime();

	//sceneObject->transform.SetPosition(((float)3) * vec3(cos(testAngle), sin(2 * testAngle), 0));

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
	//glDeleteVertexArrays(1, &VertexArrayID);
}
