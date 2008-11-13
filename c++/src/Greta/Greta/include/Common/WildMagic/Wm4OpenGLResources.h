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
// Version: 4.0.0 (2006/06/28)

#ifndef WM4OPENGLRESOURCES_H
#define WM4OPENGLRESOURCES_H

#include "Wm4OpenGLRendererLIB.h"
#include "Wm4Attributes.h"
#include "Wm4Bindable.h"

namespace Wm4
{

class WM4_RENDERER_ITEM VProgramID : public ResourceIdentifier
{
public:
    unsigned int ID;   // OpenGL's identifier for the resource.
};

class WM4_RENDERER_ITEM PProgramID : public ResourceIdentifier
{
public:
    unsigned int ID;   // OpenGL's identifier for the resource.
};

class WM4_RENDERER_ITEM TextureID : public ResourceIdentifier
{
public:
    unsigned int ID;   // OpenGL's identifier for the resource.
    Texture* TextureObject;
};

class WM4_RENDERER_ITEM VBufferID : public ResourceIdentifier
{
public:
    Attributes IAttr;  // For matching inputs when doing multipass.
    Attributes OAttr;
    unsigned int ID;   // OpenGL's identifier for the resource.
};

class WM4_RENDERER_ITEM IBufferID : public ResourceIdentifier
{
public:
    unsigned int ID;   // OpenGL's identifier for the resource.
};

}

#endif
