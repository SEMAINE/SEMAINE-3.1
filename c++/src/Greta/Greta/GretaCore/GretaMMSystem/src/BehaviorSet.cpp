//Copyright 1999-2008 Catherine Pelachaud - c.pelachaud@iut.univ-paris8.fr
//
//This file is part of Greta.
//
//Greta is free software; you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation; either version 2 of the License, or
//(at your option) any later version.
//
//Greta is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with Greta; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

#include "BehaviorSet.h"

/**@#-*/
using namespace MMSystemSpace;
/**@#+*/

BehaviorSet::BehaviorSet(void)
{
}

BehaviorSet::~BehaviorSet(void)
{
}

void BehaviorSet::ApplyCore(SignalVector n)
{
	std::vector<MultimodalSignal>::iterator mms;
	mms=this->multimodalsignals.begin();
	while(mms!=this->multimodalsignals.end())
	{
		int howmany=0;

		for(int i=0;i<n.size();i++)
		{
			for(int j=0;j<(*mms).size();j++)
			{
				if(n[i].id==(*mms)[j].id)
					howmany++;
			}
		}

		if(howmany!=n.size())
			mms=this->multimodalsignals.erase(mms);
		else
			mms++;
	}
}

void BehaviorSet::ApplyImplications(ImplicationList impls)
{
	std::vector<MultimodalSignal>::iterator mms;
	std::vector<Signal>::iterator sig;
	mms=this->multimodalsignals.begin();
	while(mms!=this->multimodalsignals.end())
	{
		bool flag;
		flag=true;
		for(sig=(*mms).begin();sig!=(*mms).end();sig++)
		{
			ImplicationList::iterator impl;
			for(impl=impls.begin();impl!=impls.end();impl++)
			{
				if((*(*impl).ifpresent.begin()).id==(*sig).id)
				{
					SignalVector::iterator thenpresent;
					thenpresent=(*impl).thenpresent.begin();
					while((flag==true)&&(thenpresent!=(*impl).thenpresent.end()))
					{
						bool found;
						found=false;
						std::vector<Signal>::iterator sig2;
						for(sig2=(*mms).begin();sig2!=(*mms).end();sig2++)
						{
							if((*sig2).id==(*thenpresent).id)
							{
								found=true;
								break;
							}
						}
						if(found==false)
						{
							flag=false;
							break;
						}
						thenpresent++;
					}

					if(flag==true)
					{
						SignalVector::iterator thennotpresent;
						thennotpresent=(*impl).thennotpresent.begin();
						while((flag==true)&&(thennotpresent!=(*impl).thennotpresent.end()))
						{
							bool found;
							found=false;
							std::vector<Signal>::iterator sig2;
							for(sig2=(*mms).begin();sig2!=(*mms).end();sig2++)
							{
								if((*sig2).id==(*thennotpresent).id)
								{
									found=true;
									break;
								}
							}
							if(found==true)
							{
								flag=false;
								break;
							}
							thennotpresent++;
						}
					}
				}
				if(flag==false)
					break;
			}
			if(flag==false)
				break;
		}
		if(flag==false)
			mms=this->multimodalsignals.erase(mms);
		else
			mms++;
	}
}

void BehaviorSet::PrintMMSignals(FILE *outfile)
{
	if(outfile!=0)
		fprintf(outfile,"Multimodal signals for %s;<br/>\n",
				this->name.c_str());
	for(int i=0;i<this->multimodalsignals.size();i++)
	{
		for(int j=0;j<this->multimodalsignals[i].size();j++)
		{
			if(outfile!=0)
				fprintf(outfile,"&nbsp;&nbsp;&nbsp;&nbsp;%s.%s;",
				this->multimodalsignals[i][j].modality.c_str(),
				this->multimodalsignals[i][j].reference.c_str());
		}
		if(outfile!=0)
			fprintf(outfile,"<br/>\n");
	}
}
