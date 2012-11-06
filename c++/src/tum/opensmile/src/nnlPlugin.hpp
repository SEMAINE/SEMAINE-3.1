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

/*  RT NNL

*/

#ifndef __NNLPLUGIN_HPP
#define __NNLPLUGIN_HPP

#define COMPONENT_DESCRIPTION_NNL "nnlPlugin, unsupported"
#define COMPONENT_NAME_NNLSINK "nnlPlugin"

// add this for plugins to compile properly in MSVC++
#ifdef SMILEPLUGIN
#ifdef class
#undef class
#endif
#endif

#include <dataSink.hpp>

#ifdef HAVE_RTNNLLIB

/**
	Visual Studio Issue: The Makro in smileCommon.hpp class class __declspec(dllimport)
	causes errors when a class declares __declspec on methods. Since this is done
	in nnl the nnl header must be included before this header is included.
	So here no nnl-classes can be used. (To include the nnl header here caused
	errors independent from its location in this file)

  Flo: This issue should now be resolved since the macro is no more defined in smileCommon.hpp
*/

#undef class
class DLLEXPORT nnlPlugin : public cDataSink {
  private:

	int lag;
    const char* netconfigFile;
	const char* dataDimensionsFile;
	const char* standardizeFile;
	void* controller;

	double* means;
	double* stddevs;

	void initStandardizationValues(long dim);

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);


  public:
    //static sComponentInfo * registerComponent(cConfigManager *_confman);
    //static cSmileComponent * create(const char *_instname);
    SMILECOMPONENT_STATIC_DECL

    nnlPlugin(const char *_name);

    virtual ~nnlPlugin();
};

#endif // __NNLPLUGIN_HPP

#endif //HAVE_RTNNLLIB
