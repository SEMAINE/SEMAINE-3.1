package eu.semaine.components.dialogue.behaviourselection.template.value;

import eu.semaine.components.dialogue.informationstate.InformationState;
import eu.semaine.components.dialogue.informationstate.Item;
import eu.semaine.components.dialogue.informationstate.List;

/**
 * A Value contains the real value of an AbstractValue. It can have different types.
 * 
 * @author Mark ter Maat
 * @version 0.1
 *
 */

public class Value
{
	/* The different types a Value can have */
	public enum Type {String, Integer, Double, Record, List};
	
	/* The values of this Value, based on its type */
	private String stringValue = null;
	private Integer integerValue = null;
	private Double doubleValue = null;
	private InformationState recordValue = null;
	private List listValue = null;
	
	/* The Type of this value */
	private Type type;
	
	/**
	 * Creates a new Value with a String-value
	 * @param value
	 */
	public Value( String value )
	{
		stringValue = value;
		type = Type.String;
	}
	
	/**
	 * Creates a new Value with an Integer-value
	 * @param value
	 */
	public Value( Integer value )
	{
		integerValue = value;
		type = Type.Integer;
	}
	
	/**
	 * Creates a new Value with a Double-value
	 * @param value
	 */
	public Value( Double value )
	{
		doubleValue = value;
		type = Type.Double;
	}
	
	/**
	 * Creates a new Value with an InformationState-value
	 * @param value
	 */
	public Value( InformationState value )
	{
		recordValue = value;
		type = Type.Record;
	}
	
	/**
	 * Creates a new Value with a List-value
	 * @param value
	 */
	public Value( List value )
	{
		listValue = value;
		type = Type.List;
	}
	
	/**
	 * Creates a new Value based on a given Item
	 * @param value
	 */
	public Value( Item item )
	{
		/* Based on the type of the item, a different type of Value is created */
		if( item.getType() == Item.Type.String ) {
			stringValue = item.getString();
			type = Type.String;
		}
		if( item.getType() == Item.Type.Integer ) {
			integerValue = item.getInteger();
			type = Type.Integer;
		}
		if( item.getType() == Item.Type.Double ) {
			doubleValue = item.getDouble();
			type = Type.Double;
		}
		if( item.getType() == Item.Type.Record ) {
			recordValue = item.getRecord();
			type = Type.Record;
		}
		if( item.getType() == Item.Type.List ) {
			listValue = item.getList();
			type = Type.List;
		}
	}

	/**
	 * @return the stringValue
	 */
	public String getStringValue() {
		return stringValue;
	}

	/**
	 * @return the integerValue
	 */
	public Integer getIntegerValue() {
		return integerValue;
	}

	/**
	 * @return the doubleValue
	 */
	public Double getDoubleValue() {
		return doubleValue;
	}

	/**
	 * @return the recordValue
	 */
	public InformationState getRecordValue() {
		return recordValue;
	}

	/**
	 * @return the listValue
	 */
	public List getListValue() {
		return listValue;
	}

	/**
	 * @return the type
	 */
	public Type getType() {
		return type;
	}
	
	/**
	 * Returns a String-representation of this Value
	 */
	public String toString()
	{
		if( listValue != null ) return "List";
		if( recordValue != null ) return "Record";
		if( doubleValue != null ) return ""+doubleValue;
		if( integerValue != null ) return ""+integerValue;
		if( stringValue != null ) return ""+stringValue;
		return "";
	}
}
