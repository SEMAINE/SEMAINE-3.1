package eu.semaine.components.dialogue.informationstate;

import java.util.ArrayList;

/**
 * A List contains an ordered list of Items of the same type (can be any Item-type).
 * It is possible to get a Item at a certain index, or get the first or last Item.
 * However, in contrasts with normal lists, when using a string-path, this List can only be used as a queue or a stack. This means you can only add items to the beginning or the end of the list, and you can only get items from the beginning or the end of the list too.
 * 
 * @version 0.1.1
 * Added support for the removal of elements given a Path.
 * 
 * @version 0.1.2
 * Changed the way in which setting a new variable works.
 * 
 * @version 0.1
 * First version
 * 
 * @author Mark ter Maat
 *
 */

public class List
{
	public final static String FIRST = "_first";
	public final static String LAST = "_last";
	public final static String ADDFIRST = "_addfirst";
	public final static String ADDLAST = "_addlast";
	
	/* The list with Items */
	private ArrayList<Item> list = new ArrayList<Item>();
	
	/**
	 * Creates a new List.
	 */
	public List()
	{
		
	}
	
	/**
	 * Returns the Item of the given Path. The path contains the location of the Item (_first/_last).
	 * After that (if the Item is a Record of a List), it is possible to specify the path of a substructure.
	 * It is also possible to create a new Item at the beginning or the end of a list, using '_addfirst' or '_addlast'. This will create a new Item at the required place and return this.
	 * 
	 * @param path - the path of the wanted Item
	 * @return the wanted Item
	 */
	public Item getValueOfPath( String path )
	{
		/* Get required index of list */
		int index = -1;
		String position;
		String valuePath;
		if( path.contains(".") ) {
			position = path.substring(0, path.indexOf("."));
			valuePath = path.substring(path.indexOf(".")+1, path.length());
		} else {
			position = path;
			valuePath = null;
		}
		if( position.toLowerCase().equals(FIRST) ) {
			index = 0;
		}
		if( position.toLowerCase().equals(LAST) ) {
			index = list.size()-1;
		}
//		if( position.toLowerCase().equals(ADDFIRST) ) {
//			list.add(0, new Item(list.get(0).getType()));
//			index = 0;
//		}
//		if( position.toLowerCase().equals(ADDLAST) ) {
//			list.add(new Item(list.get(0).getType()));
//			index = list.size()-1;
//		}
		try {
			int parseIndex = Integer.parseInt(position);
			index = parseIndex;
		}catch( NumberFormatException e ){}
		
		/* Return the value of the required index */
		if( index != -1 ) {
			return list.get(index).getValueOfPath(valuePath);
		} else {
			System.err.println("Problems parsing list-index: " + path);
			return null;
		}
	}
	
	protected void set( String path, Object value )
	{
		/* Get required index of list */
		int index = -1;
		String position;
		String valuePath;
		if( path.contains(".") ) {
			position = path.substring(0, path.indexOf("."));
			valuePath = path.substring(path.indexOf(".")+1, path.length());
		} else {
			position = path;
			valuePath = null;
		}
		
		if( position.toLowerCase().equals(FIRST) ) {
			Item i = list.get(0);
			i.set(valuePath,value);
		} else if( position.toLowerCase().equals(LAST) ) {
			Item i = list.get(list.size()-1);
			i.set(valuePath,value);
		} else if( position.toLowerCase().equals(ADDFIRST) ) {
			if( valuePath == null ) {
				Item i = new Item(value);
				list.add(0, i);
			} else {
				Item i;
				if( valuePath.startsWith("_") ) {
					// Next item is a list
					i = new Item(new List());
				} else {
					// Next item is a Record
					i = new Item(new InformationState());
				}
				list.add(0, i);
				i.set(valuePath, value);
			}
		} else if( position.toLowerCase().equals(ADDLAST) ) {
			if( valuePath == null ) {
				Item i = new Item(value);
				list.add(i);
			} else {
				Item i;
				if( valuePath.startsWith("_") ) {
					// Next item is a list
					i = new Item(new List());
				} else {
					// Next item is a Record
					i = new Item(new InformationState());
				}
				list.add(i);
				i.set(valuePath, value);
			}
		}
	}
	
	/**
	 * Adds a new Item at the end of the list, with the given String as value
	 * @param value - the new String value
	 */
	public void addItemEnd( String value )
	{
		list.add(new Item(value));
	}
	
	/**
	 * Adds a new Item at the end of the list, with the given Integer as value
	 * @param value - the new Integer value
	 */
	public void addItemEnd( Integer value )
	{
		list.add(new Item(value));
	}
	
	/**
	 * Adds a new Item at the end of the list, with the given Double as value
	 * @param value - the new Double value
	 */
	public void addItemEnd( Double value )
	{
		list.add(new Item(value));
	}
	
	/**
	 * Adds a new Item at the end of the list, with the given InformationState as value
	 * @param value - the new InformationState value
	 */
	public void addItemEnd( InformationState value )
	{
		list.add(new Item(value));
	}
	
	/**
	 * Adds a new Item at the end of the list, with the given List as value
	 * @param value - the new List value
	 */
	public void addItemEnd( List value )
	{
		list.add(new Item(value));
	}
	
	/**
	 * Adds a new Item at the start of the list, with the given String as value
	 * @param value - the new String value
	 */
	public void addItemStart( String value )
	{
		list.add(0, new Item(value));
	}
	
	/**
	 * Adds a new Item at the start of the list, with the given Integer as value
	 * @param value - the new Integer value
	 */
	public void addItemStart( Integer value )
	{
		list.add(0, new Item(value));
	}
	
	/**
	 * Adds a new Item at the start of the list, with the given Double as value
	 * @param value - the new Double value
	 */
	public void addItemStart( Double value )
	{
		list.add(0, new Item(value));
	}
	
	/**
	 * Adds a new Item at the start of the list, with the given InformationState as value
	 * @param value - the new InformationState value
	 */
	public void addItemStart( InformationState value )
	{
		list.add(0, new Item(value));
	}
	
	/**
	 * Adds a new Item at the start of the list, with the given List as value
	 * @param value - the new List value
	 */
	public void addItemStart( List value )
	{
		list.add(0, new Item(value));
	}
	
	/**
	 * Returns the String value at the given index
	 * 
	 * @param index - the index of the wanted Item
	 * @return the wanted String
	 */
	public String getString( int index )
	{
		String result;
		try {
			result = (String)list.get(index).getValue();
		}catch( ClassCastException e ) {
			System.err.println("Illegal String request to List.");
			return null;
		}
		return result;
	}
	
	/**
	 * Returns the Integer value at the given index
	 * 
	 * @param index - the index of the wanted Item
	 * @return the wanted Integer
	 */
	public Integer getInteger( int index )
	{
		Integer result;
		try {
			result = (Integer)list.get(index).getValue();
		}catch( ClassCastException e ) {
			System.err.println("Illegal Integer request to List.");
			return null;
		}
		return result;
	}
	
	/**
	 * Returns the Double value at the given index
	 * 
	 * @param index - the index of the wanted Item
	 * @return the wanted Double
	 */
	public Double getDouble( int index )
	{
		Double result;
		try {
			result = (Double)list.get(index).getValue();
		}catch( ClassCastException e ) {
			System.err.println("Illegal Double request to List.");
			return null;
		}
		return result;
	}
	
	/**
	 * Returns the InformationState (Record) value at the given index
	 * 
	 * @param index - the index of the wanted Item
	 * @return the wanted InformationState
	 */
	public InformationState getRecord( int index )
	{
		InformationState result;
		try {
			result = (InformationState)list.get(index).getValue();
		}catch( ClassCastException e ) {
			System.err.println("Illegal Record request to List.");
			return null;
		}
		return result;
	}
	
	/**
	 * Returns the List value at the given index
	 * 
	 * @param index - the index of the wanted Item
	 * @return the wanted List
	 */
	public List getList( int index )
	{
		List result;
		try {
			result = (List)list.get(index).getValue();
		}catch( ClassCastException e ) {
			System.err.println("Illegal List request to List.");
			return null;
		}
		return result;
	}
	
	/**
	 * Returns the Item value at the given index
	 * 
	 * @param index - the index of the wanted Item
	 * @return the wanted Item
	 */
	public Item getItem( int index )
	{
		return list.get(index);
	}
	
	/**
	 * If the Items in this list are of type String, returns a list with all values as Strings in it. Else, returns NULL.
	 * @return the list with Strings
	 */
	public ArrayList<String> getStringList()
	{
		ArrayList<String> aList = new ArrayList<String>();
		for( Item item : list ) {
			String value;
			try {
				value = (String)item.getValue();
			}catch( ClassCastException e ) {
				System.err.println("ClassCastException during String-list creation.");
				return null;
			}
			aList.add(value);
		}
		return aList;
	}
	
	/**
	 * If the Items in this list are of type Integer, returns a list with all values as Integers in it. Else, returns NULL.
	 * @return the list with Integers
	 */
	public ArrayList<Integer> getIntegerList()
	{
		ArrayList<Integer> aList = new ArrayList<Integer>();
		for( Item item : list ) {
			Integer value;
			try {
				value = (Integer)item.getValue();
			}catch( ClassCastException e ) {
				System.err.println("ClassCastException during Integer-list creation.");
				return null;
			}
			aList.add(value);
		}
		return aList;
	}
	
	/**
	 * If the Items in this list are of type Double, returns a list with all values as Doubles in it. Else, returns NULL.
	 * @return the list with Doubles
	 */
	public ArrayList<Double> getDoubleList()
	{
		ArrayList<Double> aList = new ArrayList<Double>();
		for( Item item : list ) {
			Double value;
			try {
				value = (Double)item.getValue();
			}catch( ClassCastException e ) {
				if( item.getType() == Item.Type.Integer ) {
					value = item.getInteger().doubleValue();
				} else {
					System.err.println("ClassCastException during Double-list creation.");
					return null;
				}
			}
			aList.add(value);
		}
		return aList;
	}
	
	/**
	 * If the Items in this list are of type InformationState (Record), returns a list with all values as InformationStates in it. Else, returns NULL.
	 * @return the list with InformationStates
	 */
	public ArrayList<InformationState> getRecordList()
	{
		ArrayList<InformationState> aList = new ArrayList<InformationState>();
		for( Item item : list ) {
			InformationState value;
			try {
				value = (InformationState)item.getValue();
			}catch( ClassCastException e ) {
				System.err.println("ClassCastException during String-list creation.");
				return null;
			}
			aList.add(value);
		}
		return aList;
	}
	
	/**
	 * Removes the variable with the given path.
	 * @param path - the name of the variable to delete
	 */
	public void remove( String path )
	{
		/* Get required index of list */
		int index = -1;
		String position;
		String valuePath;
		if( path.contains(".") ) {
			position = path.substring(0, path.indexOf("."));
			valuePath = path.substring(path.indexOf("."+1, path.length()));
		} else {
			position = path;
			valuePath = null;
		}
		if( position.toLowerCase().equals("_first") ) {
			index = 0;
		}
		if( position.toLowerCase().equals("_last") ) {
			index = list.size()-1;
		}
		if( position.toLowerCase().equals("_addfirst") ) {
			list.add(0, new Item(list.get(0).getType()));
			index = 0;
		}
		if( position.toLowerCase().equals("_addlast") ) {
			list.add(new Item(list.get(0).getType()));
			index = list.size()-1;
		}
		try {
			int parseIndex = Integer.parseInt(position);
			index = parseIndex;
		}catch( NumberFormatException e ){}
		
		Item i = list.get(index);
		if( i == null ) {
			// Done, nothing to remove.
			return;
		} else if( i.getType() == Item.Type.List ) {
			i.getList().remove(valuePath);
		} else if( i.getType() == Item.Type.Record ) {
			i.getRecord().remove(valuePath);
		} else {
			list.remove(index);
		}
	}
	
	/**
	 * Returns the size of this list
	 * @return size of the list
	 */
	public int size()
	{
		return list.size();
	}
	
	/**
	 * Prints this List as a textual representation, with the given String put before every line.
	 * 
	 * @param pre - the String to put in front of each line
	 */
	public void print( String pre )
	{
		for( Item i : list ) {
			if( i.getType() == Item.Type.String || i.getType() == Item.Type.Integer || i.getType() == Item.Type.Double ) {
				System.out.println(pre + "-" + i.getValue().toString());
			} else if( i.getType() == Item.Type.Record ) {
				System.out.println(pre + "-Record [");
				i.getRecord().print(pre + "  ");
			} else if( i.getType() == Item.Type.List ) {
				System.out.println(pre + "-List [");
				i.getList().print(pre + "  ");
			}
		}
	}
	
	/**
	 * Checks if the list contains the Object o (this Object can be a String, Integer, or Double).
	 * 
	 * @param o - the value the list should contain
	 * @return true if the list contains the value, false if it does not
	 */
	public boolean contains( Object o )
	{
		Item item = new Item(o);
		if( list.get(0).getType() != item.getType() ) {
			return false;
		} else {
			for( Item listItem : list ) {
				if( listItem.getType() == Item.Type.String ) {
					if( listItem.getString().equals(item.getString()) ) {
						return true;
					}
				} else if( listItem.getType() == Item.Type.Integer ) {
					if( listItem.getInteger().equals(item.getInteger()) ) {
						return true;
					}
				} else if( listItem.getType() == Item.Type.Double ) {
					if( listItem.getDouble().equals(item.getDouble()) ) {
						return true;
					}
				} 
			}
			return false;
		}
	}
	
	/**
	 * Checks if the list does not contain the Object o (this Object can be a String, Integer, or Double).
	 * 
	 * @param o - the value the list should not contain
	 * @return true if the list does not contain the value, false if it does or if there is a type-mismatch
	 */
	public boolean notContains( Object o )
	{
		Item item = new Item(o);
		if( list.get(0).getType() != item.getType() ) {
			return false;
		} else {
			boolean not_contains = true;
			for( Item listItem : list ) {
				if( listItem.getType() == Item.Type.String ) {
					if( listItem.getString().equals(item.getString()) ) {
						not_contains = false;
					}
				} else if( listItem.getType() == Item.Type.Integer ) {
					if( listItem.getInteger().equals(item.getInteger()) ) {
						not_contains = false;
					}
				} else if( listItem.getType() == Item.Type.Double ) {
					if( listItem.getDouble().equals(item.getDouble()) ) {
						not_contains = false;
					}
				}
			}
			return not_contains;
		}
	}
}
