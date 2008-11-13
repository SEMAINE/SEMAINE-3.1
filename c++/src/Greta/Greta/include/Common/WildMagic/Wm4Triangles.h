// Wild Magic Source Code
// David Eberly
// http://www.geometrictools.com
// Copyright (c) 1998-2007
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.  The license is available for reading at
// either of the locations:
//     http://www.gnu.org/copyleft/lgpl.html
//     http://www.geometrictools.com/License/WildMagicLicense.pdf
//
// Version: 4.0.0 (2006/08/13)

#ifndef WM4TRIANGLES_H
#define WM4TRIANGLES_H

#include "Wm4GraphicsLIB.h"
#include "Wm4Geometry.h"
#include "Wm4Triangle3.h"

namespace Wm4
{

class WM4_GRAPHICS_ITEM Triangles : public Geometry
{
    WM4_DECLARE_RTTI;
    WM4_DECLARE_NAME_ID;
    WM4_DECLARE_STREAM;

public:
    // abstract base class
    virtual ~Triangles ();

    // These functions depend on the interpretation of the index buffer of
    // the triangle primitive.  The triangle <V0,V1,V2> is counterclockwise
    // order.
    virtual int GetTriangleQuantity () const = 0;
    virtual bool GetTriangle (int i, int& riV0, int& riV1, int& riV2)
        const = 0;

    bool GetModelTriangle (int i, Triangle3f& rkMTri) const;
    bool GetWorldTriangle (int i, Triangle3f& rkWTri) const;
    void GenerateNormals ();

protected:
    Triangles ();
    Triangles (VertexBuffer* pkVBuffer, IndexBuffer* pkIBuffer);

    // geometric updates
    virtual void UpdateModelNormals ();
};

WM4_REGISTER_STREAM(Triangles);
typedef Pointer<Triangles> TrianglesPtr;

}

#endif
