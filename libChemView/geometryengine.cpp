#include "geometryengine.h"

GeometryEngine::GeometryEngine() :
    m_cube(new CubeGeometry)
{
}

CubeGeometry *GeometryEngine::cube() const
{
    return m_cube;
}
