package eu.semaine.util;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.StringWriter;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;
import java.util.StringTokenizer;

import eu.semaine.jms.JMSLogger;

public class SEMAINEUtils 
{
    public static String getStreamAsString(InputStream inputStream, String encoding) throws IOException
    {
        return getReaderAsString(new InputStreamReader(inputStream, encoding));
    }
    
    public static String getReaderAsString(Reader reader) throws IOException
    {
        StringWriter sw = new StringWriter();
        BufferedReader in = new BufferedReader(reader);
        char[] buf = new char[8192];
        int n;
        while ((n=in.read(buf))> 0) {
            sw.write(buf, 0, n);
        }
        return sw.toString();
        
    }

    public static Locale string2locale(String localeString)
    {
        Locale locale = null;
        StringTokenizer localeST = new StringTokenizer(localeString, "_-");
        String language = localeST.nextToken();
        String country = "";
        String variant = "";
        if (localeST.hasMoreTokens()) {
            country = localeST.nextToken();
            if (localeST.hasMoreTokens()) {
                variant = localeST.nextToken();
            }
         }
        locale = new Locale(language, country, variant);
        return locale;
    }
    
    /**
     * Convert a locale into a string that is conform with XML's xml:lang attribute.
     * Basically it is language-COUNTRY, e.g. en-US.
     * @param locale a locale, must not be null
     * @return
     * @throws IllegalArgumentException if locale is null
     */
    public static String locale2xmllang(Locale locale) 
    {
        if (locale == null) throw new IllegalArgumentException("Locale must not be null");
        String country = locale.getCountry();
        if (!"".equals(country))
            return locale.getLanguage()+"-"+country;
        return locale.getLanguage();
    }
    
    /**
     * For a given string containing lines of key-value pairs, construct a map
     * of key-value pairs.
     * This assumes that the String contains whitespace-separated key-value pairs as follows:
     * <code>
     *   key1 value1
     *   key2 value2
     * </code>
     * Leading or trailing whitespace is ignored.
     * If a line contains only a single field, it is treated as a key that maps to the empty string.
     * Empty lines are ignored. 
     * @param s
     * @param keysFirst if true, the first item in a line is the key, and the rest is the value; if false, the first item in the line is the value and the rest is the key.
     * @return
     */
    public static Map<String, String> string2map(String s, boolean keysFirst) {
    	String[] lines = s.split("\n");
    	Map<String, String> m = new HashMap<String, String>();
    	for (String l : lines) {
    		l = l.trim();
    		if (l.equals("")) {
    			continue;
    		}
    		String[] parts = l.split("\\s+", 2);
    		if (parts.length == 1) { // only key
    			m.put(parts[0], "");
    		} else {
    			assert parts.length == 2;
    			if (keysFirst) {
    				m.put(parts[0], parts[1]);
    			} else {
    				m.put(parts[1], parts[0]);
    			}
    		}
    	}
    	return m;
    }
    
    /**
     * Convert the given string into a float value, if possible.
     * @param valueString string to be converted into a float.
     * @param defaultValue value to use if the string cannot be converted into a float.
     * @return the best approximation that can be made for the float value given in valueString. If valueString
     * cannot be converted into any float, returns defaultValue.
     */
    public static float parseFloat(String valueString, float defaultValue) {
    	float value;
    	try {
    		value = Float.parseFloat(valueString);
    	} catch (NumberFormatException nfe) {
    		return defaultValue;
    	}
    	return value;
    }
    

    
}
