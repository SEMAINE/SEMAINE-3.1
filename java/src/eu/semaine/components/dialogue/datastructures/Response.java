package eu.semaine.components.dialogue.datastructures;

import java.util.ArrayList;
import java.util.HashMap;

public class Response
{
	private String id;
	private String response;
	private ArrayList<String> argNames = new ArrayList<String>();
	private ArrayList<String> argValues = new ArrayList<String>();
	
	public Response( String id, String response )
	{
		this.id = id;
		this.response = response;
	}
	
	public Response( Response r1, Response r2 )
	{
		id = r1.id + r2.id;
		response = r1.response + " " + r2.response;
		argNames.addAll(r1.getArgNames());
		argNames.addAll(r2.getArgNames());
		argValues.addAll(r1.getArgValues());
		argValues.addAll(r2.getArgValues());
	}
	
	public void addArgument( String name, String value )
	{
		argNames.add(name);
		argValues.add(value);
	}
	
	public String getId()
	{
		return id;
	}
	
	public String getResponse()
	{
		return response;
	}
	
	public ArrayList<String> getArgNames()
	{
		return argNames;
	}
	
	public ArrayList<String> getArgValues()
	{
		return argValues;
	}
	
	@Override
	public String toString() {
		return id+" "+response;
	}
}
