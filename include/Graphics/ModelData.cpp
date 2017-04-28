#include "ModelData.h"

unsigned int ModelData::getVertArray(Graphics* graphics)
{
	if (vaoHandle != 0) return vaoHandle;
	vaoHandle = graphics->createVertexArrayObject(vboHandles);
	return vaoHandle;
}
