#include "CoordinateSystemRenderer.h"
#include "Engine/EngineLibrary.h"
#include "Engine/ShaderLoader.h"

void CoordinateSystemRenderer::Render(mat4 projectionViewMatrix)
{
	mesh->Render(projectionViewMatrix, sceneObject->transform.GetModelMatrix());
}

void CoordinateSystemRenderer::Start()
{
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
	if (newLength == 0)
		return;

	segmentLength = newLength;
	if (mesh != nullptr)
		CreateMesh();
}

void CoordinateSystemRenderer::SetSegmentNotchSize(double newSize)
{
	segmentNotchSize = newSize;
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

void CoordinateSystemRenderer::SetColorX(vec3 color)
{
	xColor = color;
	if (mesh != nullptr)
		CreateMesh();
}

void CoordinateSystemRenderer::SetColorY(vec3 color)
{
	yColor = color;
	if (mesh != nullptr)
		CreateMesh();
}

void CoordinateSystemRenderer::SetColorZ(vec3 color)
{
	zColor = color;
	if (mesh != nullptr)
		CreateMesh();
}

void CoordinateSystemRenderer::OnDestroyRenderer()
{
	delete mesh;
}

void PushVertexColorData(vec3 vertex, vec3 color, std::vector<GLfloat>& vertexData, std::vector<GLfloat>& colorData)
{
	vertexData.push_back(vertex.x);
	vertexData.push_back(vertex.y);
	vertexData.push_back(vertex.z);

	colorData.push_back(color.x);
	colorData.push_back(color.y);
	colorData.push_back(color.z);
}

void CoordinateSystemRenderer::CreateMesh()
{
	delete mesh;

	std::vector<GLfloat> vertexData;
	std::vector<GLfloat> colorData;

	size_t axisSegmentCount = abs(radius / segmentLength);

	if (xAxis)
	{
		PushVertexColorData(vec3(radius, 0, 0), xColor, vertexData, colorData);
		PushVertexColorData(vec3(-radius, 0, 0), xColor, vertexData, colorData);

		for (size_t i = 0; i < axisSegmentCount; ++i)
		{
			if (yAxis)
			{
				PushVertexColorData(vec3(i * segmentLength, segmentNotchSize, 0), xColor, vertexData, colorData);
				PushVertexColorData(vec3(i * segmentLength, -segmentNotchSize, 0), xColor, vertexData, colorData);

				PushVertexColorData(vec3(i * -segmentLength, segmentNotchSize, 0), xColor, vertexData, colorData);
				PushVertexColorData(vec3(i * -segmentLength, -segmentNotchSize, 0), xColor, vertexData, colorData);
			}
			
			if (zAxis)
			{
				PushVertexColorData(vec3(i * segmentLength, 0, segmentNotchSize), xColor, vertexData, colorData);
				PushVertexColorData(vec3(i * segmentLength, 0, -segmentNotchSize), xColor, vertexData, colorData);

				PushVertexColorData(vec3(i * -segmentLength, 0, segmentNotchSize), xColor, vertexData, colorData);
				PushVertexColorData(vec3(i * -segmentLength, 0, -segmentNotchSize), xColor, vertexData, colorData);
			}
		}
	}
	
	if (yAxis)
	{
		PushVertexColorData(vec3(0, radius, 0), yColor, vertexData, colorData);
		PushVertexColorData(vec3(0, -radius, 0), yColor, vertexData, colorData);


		for (size_t i = 0; i < axisSegmentCount; ++i)
		{
			if (xAxis)
			{
				PushVertexColorData(vec3(segmentNotchSize, i * segmentLength, 0), yColor, vertexData, colorData);
				PushVertexColorData(vec3(-segmentNotchSize, i * segmentLength, 0), yColor, vertexData, colorData);

				PushVertexColorData(vec3(segmentNotchSize, i * -segmentLength, 0), yColor, vertexData, colorData);
				PushVertexColorData(vec3(-segmentNotchSize, i * -segmentLength, 0), yColor, vertexData, colorData);
			}

			if (zAxis)
			{
				PushVertexColorData(vec3(0, i * segmentLength, segmentNotchSize), yColor, vertexData, colorData);
				PushVertexColorData(vec3(0, i * segmentLength, -segmentNotchSize), yColor, vertexData, colorData);

				PushVertexColorData(vec3(0, i * -segmentLength, segmentNotchSize), yColor, vertexData, colorData);
				PushVertexColorData(vec3(0, i * -segmentLength, -segmentNotchSize), yColor, vertexData, colorData);
			}
		}
	}
	
	if (zAxis)
	{
		PushVertexColorData(vec3(0, 0, radius), zColor, vertexData, colorData);
		PushVertexColorData(vec3(0, 0, -radius), zColor, vertexData, colorData);

		for (size_t i = 0; i < axisSegmentCount; ++i)
		{
			if (xAxis)
			{
				PushVertexColorData(vec3(segmentNotchSize, 0, i* segmentLength), zColor, vertexData, colorData);
				PushVertexColorData(vec3(-segmentNotchSize, 0, i* segmentLength), zColor, vertexData, colorData);

				PushVertexColorData(vec3(segmentNotchSize, 0, i * -segmentLength), zColor, vertexData, colorData);
				PushVertexColorData(vec3(-segmentNotchSize, 0, i * -segmentLength), zColor, vertexData, colorData);
			}

			if (yAxis)
			{
				PushVertexColorData(vec3(0, segmentNotchSize, i* segmentLength), zColor, vertexData, colorData);
				PushVertexColorData(vec3(0, -segmentNotchSize, i* segmentLength), zColor, vertexData, colorData);

				PushVertexColorData(vec3(0, segmentNotchSize, i * -segmentLength), zColor, vertexData, colorData);
				PushVertexColorData(vec3(0, -segmentNotchSize, i * -segmentLength), zColor, vertexData, colorData);
			}
		}
	}
	mesh = new Mesh(vertexData, colorData, engine->GetDefaultShader().GetProgram(), GL_LINES);
}
