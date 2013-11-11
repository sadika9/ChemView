#ifndef GEOMETRYENGINE_H
#define GEOMETRYENGINE_H

#include "cubegeometry.h"

class GeometryEngine
{
public:
    GeometryEngine();

    CubeGeometry *cube() const;

private:
    CubeGeometry *m_cube;
};

#endif // GEOMETRYENGINE_H
