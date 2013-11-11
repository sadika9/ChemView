#ifndef GEOMETRYENGINE_H
#define GEOMETRYENGINE_H

#include "cubegeometry.h"
#include "mesh.h"

class GeometryEngine
{
public:
    GeometryEngine();

    CubeGeometry *cube() const;

    Mesh *sphere() const;

private:
    CubeGeometry *m_cube;

    Mesh *m_sphere;
};

#endif // GEOMETRYENGINE_H
