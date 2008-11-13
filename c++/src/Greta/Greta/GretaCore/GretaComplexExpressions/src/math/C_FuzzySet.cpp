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

// C_FuzzySet.cpp: implementation of the  FuzzySet class.
//
//////////////////////////////////////////////////////////////////////

/*
 * FuzzySet.cpp
 *
 * Created on 14 styczeñ 2006, 10:20
 *
 *
 */

#include "C_FuzzySet.h"

    
int C_FuzzySet::getA() 
{
	return a;
}
int C_FuzzySet::getB() 
{
	return b;
}
int C_FuzzySet::getC() 
{
	return c;
}
int C_FuzzySet::getD() 
{return d;
}
    

C_FuzzySet::C_FuzzySet(int x, int d1) 
{                        
        //intervals
        d15 = d1/15;
        b = x-(d15/2);
        c = x+(d15/2);
        a = b - d15;
        d = c + d15;
        
        
        //"fuzzyfication" of a crisp value
        if ((a==b)&&(b==c)&&(c==d))
		{
            b--;
            c++;
            a=a-2;
            d=d+2;
        }
    }
    

	C_FuzzySet::C_FuzzySet(int x, int d1, double factor) 
	{
                        
        //intervals
        d15 = (int)((double)d1/factor);
        b = x-(int)(factor/(double)2);
        c = x+(int)(factor/(double)2);
        a = b - d15;
        d = c + d15;
        
        
        //"fuzzyfication" of a crisp value
        if ((a==b)&&(b==c)&&(c==d)) 
		{
            b--;
            c++;
            a=a-2;
            d=d+2;
        }
    }
    
    /**
     * a value of membership function
     * @param x argument
     * @return a membership value
     */
	
	double C_FuzzySet::include(int x)
	{
        if ((x<a)||(x>d)) return 0.0;
        if ((x>=b)&&(x<=c)) return 1.0;
        if ((x>=a)&&(x<=b)) 
		{
            //double alfa = 1.0/d15;
            //double beta = 1.0 - alfa*b;
            //return 1.0 - alfa*x;
            return (double)((double)x-(double)a)/(double)((double)b-(double)a);                        
        }
        if ((x>=c)&&(x<=d)) 
		{
            //double alfa = -1.0/d15;
            //double beta = 1.0 - alfa*c;
            //return 1.0 - alfa*c;
            return (double)((double)x-(double)d)/(double)((double)c-(double)d);
            
        }
        return 0.0;        
    }

	/*
	*
	*deconstructor
	*
	*/
	C_FuzzySet::~C_FuzzySet()
	{
	}