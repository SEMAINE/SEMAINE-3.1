#ifndef _ETAT_H
#define _ETAT_H

class Etat;
class Etat
{
public:
	int value;		// of the reached triangle
	int* history;	// all the triangles followed until this one
public:
	Etat()
	{
	}
	Etat(int value)
	{
		this->value=value;
		if(!(this->history=(int*)malloc(2*sizeof(int))))
		{
			printf("ERROR: malloc failed on history\n");
			MessageBox(NULL, "ERROR: malloc failed on history", "Access error", MB_OK|MB_ICONERROR);
			exit(2);
		}
		this->history[0]=2;
		this->history[1]=value;
	}
	Etat(int value, int* history)
	{
		this->value=value;
		this->history=history;
	}
	Etat(int value, Etat* precedent)
	{
		this->value=value;
		int sizeofHistory=precedent->history[0]+1;
		if(!(this->history = (int*)malloc(sizeofHistory*sizeof(int))))
		{
			printf("ERROR: malloc failed on history\n");
			MessageBox(NULL, "ERROR: malloc failed on history", "Access error", MB_OK|MB_ICONERROR);
			exit(2);
		}
		this->history[0]=sizeofHistory;
		for(int i=1;i<sizeofHistory-1;i++)
			this->history[i]=precedent->history[i];
		this->history[sizeofHistory-1]=value;
	}
};

#endif	// _ETAT_H