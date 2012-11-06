/*F******************************************************************************
 *
 * openSMILE - open Speech and Music Interpretation by Large-space Extraction
 *       the open-source Munich Audio Feature Extraction Toolkit
 * Copyright (C) 2008-2009  Florian Eyben, Martin Woellmer, Bjoern Schuller
 *
 *
 * Institute for Human-Machine Communication
 * Technische Universitaet Muenchen (TUM)
 * D-80333 Munich, Germany
 *
 *
 * If you use openSMILE or any code from openSMILE in your research work,
 * you are kindly asked to acknowledge the use of openSMILE in your publications.
 * See the file CITING.txt for details.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ******************************************************************************E*/


/*  openSMILE component:


*/


#ifndef __MY_COMPONENT_HPP
#define __MY_COMPONENT_HPP

#include <smileCommon.hpp>
#include <smileComponent.hpp>

#define COMPONENT_DESCRIPTION_CMYCOMPONENT "a good template..."
#define COMPONENT_NAME_CMYCOMPONENT "cMyComponent"

#undef class
class DLLEXPORT cMyComponent : public cSmileComponent {
  private:

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

	virtual void fetchConfig();

  public:
    SMILECOMPONENT_STATIC_DECL

    //cMyComponent(cConfigManager *cm) : cSmileComponent("myComponentTemplate",cm) {}
    cMyComponent(const char *_name);

    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);
    virtual int manualConfig(); // custom function with arbirtrary parameters, use instead of fetchConfig when passing NULL for cConfigManager to constructor

    virtual ~cMyComponent() {}
};




#endif // __MY_COMPONENT_HPP
