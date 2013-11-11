#include "geometryengine.h"

GeometryEngine::GeometryEngine() :
    m_cube(new CubeGeometry),
    m_sphere(new Mesh)
{
}

CubeGeometry *GeometryEngine::cube() const
{
    return m_cube;
}

Mesh *GeometryEngine::sphere() const
{
    return m_sphere;
}

