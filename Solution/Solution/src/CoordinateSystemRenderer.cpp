#include "CoordinateSystemRenderer.h"
#include "Engine/EngineLibrary.h"
#include "Engine/ShaderLoader.h"

void CoordinateSystemRenderer::Render(mat4 projectionViewMatrix)
{
	mesh->Render(projectionViewMatrix, sceneObject->transform.GetModelMatrix());
}

void CoordinateSystemRenderer::Start()
{
	programID = LoadShaders("TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader");
	CreateMesh();
}

void CoordinateSystemRenderer::SetRadius(double newRadius)
{
	radius = newRadius;
	if(mesh != nullptr)
		CreateMesh();
}

void CoordinateSystemRenderer::SetSegmentLength(double newLength)
{
	segmentLength = newLength;
	if (mesh != nullptr)
		CreateMesh();
}

void CoordinateSystemRenderer::SetAxis(bool x, bool y, bool z)
{
	xAxis = x;
	yAxis = y;
	zAxis = z;
	if (mesh != nullptr)
		CreateMesh();
}

void CoordinateSystemRenderer::OnDestroyRenderer()
{
	delete mesh;
}

void CoordinateSystemRenderer::CreateMesh()
{
	delete mesh;

	std::vector<GLfloat> vertexData;
	size_t axisSegmentCount = abs(radius / segmentLength);


	if (xAxis)
	{
		vertexData.push_back(radius);
		vertexData.push_back(0);
		vertexData.push_back(0);

		vertexData.push_back(-radius);
		vertexData.push_back(0);
		vertexData.push_back(0);

		for (size_t i = 0; i < axisSegmentCount; ++i)
		{
			if (yAxis)
			{
				vertexData.push_back(i * segmentLength);
				vertexData.push_back(segmentNotchSize);
				vertexData.push_back(0);

				vertexData.push_back(i * segmentLength);
				vertexData.push_back(-segmentNotchSize);
				vertexData.push_back(0);

				vertexData.push_back(i * -segmentLength);
				vertexData.push_back(segmentNotchSize);
				vertexData.push_back(0);

				vertexData.push_back(i * -segmentLength);
				vertexData.push_back(-segmentNotchSize);
				vertexData.push_back(0);
			}
			
			if (zAxis)
			{
				vertexData.push_back(i * segmentLength);
				vertexData.push_back(0);
				vertexData.push_back(segmentNotchSize);

				vertexData.push_back(i * segmentLength);
				vertexData.push_back(0);
				vertexData.push_back(-segmentNotchSize);

				vertexData.push_back(i * -segmentLength);
				vertexData.push_back(0);
				vertexData.push_back(segmentNotchSize);

				vertexData.push_back(i * -segmentLength);
				vertexData.push_back(0);
				vertexData.push_back(-segmentNotchSize);
			}
		}
	}
	
	if (yAxis)
	{
		vertexData.push_back(0);
		vertexData.push_back(radius);
		vertexData.push_back(0);

		vertexData.push_back(0);
		vertexData.push_back(-radius);
		vertexData.push_back(0);

		for (size_t i = 0; i < axisSegmentCount; ++i)
		{
			if (xAxis)
			{
				vertexData.push_back(segmentNotchSize);
				vertexData.push_back(i * segmentLength);
				vertexData.push_back(0);

				vertexData.push_back(-segmentNotchSize);
				vertexData.push_back(i * segmentLength);
				vertexData.push_back(0);

				vertexData.push_back(segmentNotchSize);
				vertexData.push_back(i * -segmentLength);
				vertexData.push_back(0);

				vertexData.push_back(-segmentNotchSize);
				vertexData.push_back(i * -segmentLength);
				vertexData.push_back(0);
			}

			if (zAxis)
			{
				vertexData.push_back(0);
				vertexData.push_back(i * segmentLength);
				vertexData.push_back(segmentNotchSize);

				vertexData.push_back(0);
				vertexData.push_back(i * segmentLength);
				vertexData.push_back(-segmentNotchSize);

				vertexData.push_back(0);
				vertexData.push_back(i * -segmentLength);
				vertexData.push_back(segmentNotchSize);

				vertexData.push_back(0);
				vertexData.push_back(i * -segmentLength);
				vertexData.push_back(-segmentNotchSize);
			}
		}
	}
	
	if (zAxis)
	{
		vertexData.push_back(0);
		vertexData.push_back(0);
		vertexData.push_back(radius);

		vertexData.push_back(0);
		vertexData.push_back(0);
		vertexData.push_back(-radius);

		for (size_t i = 0; i < axisSegmentCount; ++i)
		{
			if (xAxis)
			{
				vertexData.push_back(segmentNotchSize);
				vertexData.push_back(0);
				vertexData.push_back(i * segmentLength);

				vertexData.push_back(-segmentNotchSize);
				vertexData.push_back(0);
				vertexData.push_back(i * segmentLength);

				vertexData.push_back(segmentNotchSize);
				vertexData.push_back(0);
				vertexData.push_back(i * -segmentLength);

				vertexData.push_back(-segmentNotchSize);
				vertexData.push_back(0);
				vertexData.push_back(i * -segmentLength);
			}

			if (yAxis)
			{
				vertexData.push_back(0);
				vertexData.push_back(segmentNotchSize);
				vertexData.push_back(i * segmentLength);

				vertexData.push_back(0);
				vertexData.push_back(-segmentNotchSize);
				vertexData.push_back(i * segmentLength);

				vertexData.push_back(0);
				vertexData.push_back(segmentNotchSize);
				vertexData.push_back(i * -segmentLength);

				vertexData.push_back(0);
				vertexData.push_back(-segmentNotchSize);
				vertexData.push_back(i * -segmentLength);
			}
		}
	}
	
	std::vector<GLfloat> colorData(vertexData.size(), 0.7);

	mesh = new Mesh(vertexData, colorData, programID, GL_LINES);
}
