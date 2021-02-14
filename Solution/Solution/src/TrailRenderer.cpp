#include "TrailRenderer.h"

#include "Engine/Mesh.h"
#include <vector>

void PrintVec3(const vec3& vector)
{
	std::cout << "[" << vector.x << ", " << vector.y << ", " << vector.x << "]\n";
}


void TrailRenderer::Start()
{
	
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

	if (currentSamplesSize < maxSamples)
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
	else
	{
		for (int i = 0; i < currentSamplesSize - 1; ++i)
		{
			vec3 currentVertex = samplesCopy.front() - trailHead;
			if (i != currentSamplesSize - 2)
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
		if (samplesCopy.size() > 1)
		{
			vec3 previousVertex = samplesCopy.front() - trailHead;
			samplesCopy.pop_front();
			vec3 currentVertex = samplesCopy.front() - trailHead;
			float positionInterpolationParameter = 1 - sampleTimeCounter / sampleLifeTime;
			vec3 interpolatedVertex = (1 - positionInterpolationParameter) * previousVertex + positionInterpolationParameter * currentVertex;

			vertexData[(currentSamplesSize - 1) * 3] = interpolatedVertex.x;
			vertexData[(currentSamplesSize - 1) * 3 + 1] = interpolatedVertex.y;
			vertexData[(currentSamplesSize - 1) * 3 + 2] = interpolatedVertex.z;

			colorData[(currentSamplesSize - 1) * 3] = trailTailColor.x;
			colorData[(currentSamplesSize - 1) * 3 + 1] = trailTailColor.y;
			colorData[(currentSamplesSize - 1) * 3 + 2] = trailTailColor.z;
		}
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


