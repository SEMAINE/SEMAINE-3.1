package eu.semaine.components.dialogue.informationstate;

/**
 * An Item contains a specific value in the InformationState, and the type of this value.
 * After asking it its type the value be asked directly, for example with getString().
 * 
 * @version 0.1.2
 * Changed the way setting a new variable works
 * 
 * @version 0.1
 * First version
 *
 * @author Mark ter Maat
 */

public class Item
{
	/* The possible types of an Item */
	public enum Type {String, Integer, Double, Record, List};
	
	/* The possible values of this Item*/
	private String stringValue = null;
	private Integer integerValue = null;
	private Double doubleValue = null;
	private InformationState recordValue = null;
	private List listValue = null;
	
	/* The Type of this Item */
	private Type type;
	
	/**
	 * Creates a new Item with the given Object as a value. First determines what the type of the object is,
	 * then it will set the correct value and type.
	 * @param o - the new Value
	 */
	public Item( Object o )
	{
		if( o instanceof String ) {
			stringValue = (String)o;
			type = Type.String;
		}
		else if( o instanceof Integer ) {
			integerValue = (Integer)o;
			type = Type.Integer;
		}
		else if( o instanceof Double ) {
			doubleValue = (Double)o;
			type = Type.Double;
		}
		else if( o instanceof InformationState ) {
			recordValue = (InformationState)o;
			type = Type.Record;
		}
		else if( o instanceof List ) {
			listValue = (List)o;
			type = Type.List;
		}
	}

	/**
	 * Creates a new Item with the given Type, but with not value yet.
	 * @param type - the new Type of the Item
	 */
	public Item( Type type )
	{
		this.type = type;
	}
	
	/**
	 * @return the String value of this Item
	 */
	public String getString()
	{
		return stringValue;
	}
	
	/**
	 * @return the Integer value of this Item
	 */
	public Integer getInteger()
	{
		return integerValue;
	}
	
	/**
	 * @return the Double value of this Item
	 */
	public Double getDouble()
	{
		return doubleValue;
	}
	
	/**
	 * @return the Record value of this Item
	 */
	public InformationState getRecord()
	{
		return recordValue;
	}
	
	/**
	 * @return the List value of this Item
	 */
	public List getList()
	{
		return listValue;
	}
	
	/**
	 * @param stringValue the stringValue to set
	 */
	public void setStringValue(String stringValue) {
		this.stringValue = stringValue;
	}

	/**
	 * @param integerValue the integerValue to set
	 */
	public void setIntegerValue(Integer integerValue) {
		this.integerValue = integerValue;
	}

	/**
	 * @param doubleValue the doubleValue to set
	 */
	public void setDoubleValue(Double doubleValue) {
		this.doubleValue = doubleValue;
	}

	/**
	 * @param recordValue the recordValue to set
	 */
	public void setRecordValue(InformationState recordValue) {
		this.recordValue = recordValue;
	}

	/**
	 * @param listValue the listValue to set
	 */
	public void setListValue(List listValue) {
		this.listValue = listValue;
	}

	/**
	 * @return the Type of this Item
	 */
	public Type getType()
	{
		return type;
	}
	
	/**
	 * @return the value of this Item as an Object
	 */
	public Object getValue()
	{
		if( type == Type.String ) return stringValue;
		if( type == Type.Integer ) return integerValue;
		if( type == Type.Double ) return doubleValue;
		if( type == Type.Record ) return recordValue;
		if( type == Type.List ) return listValue;
		return null;
	}
	
	/**
	 * Retrieves the value of the give path. If the path is NULL, or if the type of this Item is a String, Integer, or Double it will return the value of this Item.
	 * If the path is not NULL (and the type is a Record or a List), it will pass the path to the Record/List, and return the resulting Item. 
	 * 
	 * @param path - the substructure-path of the item you want
	 * @return the wanted Item
	 */
	public Item getValueOfPath( String path )
	{
		if( type == Type.String ) return this;
		if( type == Type.Integer ) return this;
		if( type == Type.Double ) return this;
		if( type == Type.Record ) {
			if( path == null ) {
				return this;
			} else {
				return recordValue.getValueOfPath(path);
			}
		}
		if( type == Type.List ) {
			
			if( path == null ) {
				return this;
			} else {
				return listValue.getValueOfPath(path);
			}
		}
		return null;
	}
	
	protected void set( String path, Object value )
	{
		if( path == null ) {
			if( type == Type.String && value instanceof String ) {
				stringValue = (String)value;
			} else if( type == Type.Integer && value instanceof Integer ) {
				integerValue = (Integer)value;
			} else if( type == Type.Double && value instanceof Double ){
				doubleValue = (Double)value;
			} else if( type == Type.Record && value instanceof InformationState ) {
				recordValue = (InformationState)value;
			} else if( type == Type.List && value instanceof List ) {
				listValue = (List)value;
			}
		} else {
			if( type == Type.Record ) {
				if( path != null ) {
					recordValue.set(path,value);
				}
			} else if( type == Type.List ) {
				if( path != null ) {
					listValue.set(path,value);
				}
			}
		}
	}
	
	/**
	 * Prints this Item as a textual representation, with the given String put before every line.
	 * 
	 * @param pre - the String to put in front of each line
	 */
	public void print(String pre)
	{
		if( type == Type.String ) System.out.println(stringValue);
		if( type == Type.Integer ) System.out.println(integerValue);
		if( type == Type.Double ) System.out.println(doubleValue);
		if( type == Type.Record ) {
			System.out.println("Record\r\n"+pre+"[");
			recordValue.print(pre+"  ");
			System.out.println(pre+"]");
		}
		if( type == Type.List ) {
			System.out.println(" List");
			listValue.print( pre );
		}
	}
}
