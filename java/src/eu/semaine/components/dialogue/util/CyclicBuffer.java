package eu.semaine.components.dialogue.util;

import java.util.ArrayList;

public class CyclicBuffer <type>
{
	private Object[] buffer;
	private int pointer = 0;
	
	public CyclicBuffer( int size )
	{
		buffer = new Object[size];
	}
	
	public void putValue( type value )
	{
		buffer[pointer] = value;
		pointer = (pointer + 1)%buffer.length;
	}
	
	public type getValue()
	{
		int p2 = pointer;
		if( p2 <= 0 ) {
			p2 = p2 + buffer.length;
		}
		return (type)buffer[(p2-1)%buffer.length];
	}
	
	public ArrayList<type> getValues()
	{
		ArrayList<type> result = new ArrayList<type>();
		int p = pointer;
		for( int i=0; i<buffer.length; i++ ) {
			if( (type)buffer[p] != null )result.add( (type)buffer[p] );
			p = (p + 1)%buffer.length;
		}
		return result;
	}
	
	public String toString()
	{
		String result = "";
		ArrayList<type> l = getValues();
		for( type t : l ) {
			if( t != null ) {
				result = result + t.toString() + " ";
			}
		}
		return result;
	}
	
	public static void main( String args[] )
	{
		CyclicBuffer<Integer> b = new CyclicBuffer<Integer>(3);
		System.out.println("Empty buffer");
		System.out.println(b.toString());
		System.out.println("Adding 1 number");
		b.putValue(2);
		System.out.println(b.toString());
		System.out.println("Filling the buffer");
		b.putValue(5);
		b.putValue(10);
		System.out.println(b.toString());
		System.out.println("Overwriting a number");
		b.putValue(-1);
		System.out.println(b.toString());
		
	}
}
