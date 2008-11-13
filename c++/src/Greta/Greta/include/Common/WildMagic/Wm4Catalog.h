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
// Version: 4.5.1 (2007/08/07)

#ifndef WM4CATALOG_H
#define WM4CATALOG_H

#include "Wm4GraphicsLIB.h"
#include "Wm4System.h"

namespace Wm4
{

template <class T>
class Catalog
{
public:
    Catalog (const std::string& rkName);
    ~Catalog ();

    const std::string& GetName () const;
    bool Insert (T* ptObject);
    bool Remove (T* ptObject);
    T* Find (const std::string& rkName);
    bool PrintContents (const std::string& rkFilename) const;

    static void SetActive (Catalog* pkActive);
    static Catalog<T>* GetActive ();

private:
    std::string m_kName;

    typedef std::map<std::string,T*> Map;
    typedef typename Map::iterator MapIterator;
    typedef typename Map::const_iterator MapCIterator;
    Map m_kEntry;

    static Catalog* ms_pkActive;
};

#include "Wm4Catalog.inl"

}

#endif
