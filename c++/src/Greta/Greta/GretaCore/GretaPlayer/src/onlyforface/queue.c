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

#include "queue.h"

queue *head;
queue *tail;

int isEmptyQ()
{
	if (!head&&!tail)
		return 1;
	return 0;
}


void addQ(queue *el)
{
	if (isEmptyQ())
	{
		head=el;
		tail=el;
		return;
	}

	tail->next=el;
	el->prev=tail;
	tail=el;
}

void removeQ()
{
	queue *tmp_el;

	if (head==tail)
	{
		free(head);
		head=NULL;
		tail=NULL;
	}
	else
	{
		tmp_el=head;
		head=tmp_el->next;
		tmp_el->prev=NULL;
		free(tmp_el);
	}
}

queue* headQ()
{
	return head;
}


