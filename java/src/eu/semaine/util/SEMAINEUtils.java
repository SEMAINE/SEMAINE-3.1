package eu.semaine.util;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.StringWriter;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Locale;
import java.util.Map;
import java.util.Properties;
import java.util.StringTokenizer;

import org.apache.log4j.BasicConfigurator;
import org.apache.log4j.ConsoleAppender;
import org.apache.log4j.Level;
import org.apache.log4j.LogManager;
import org.apache.log4j.Logger;
import org.apache.log4j.PatternLayout;
import org.apache.log4j.PropertyConfigurator;


public class SEMAINEUtils 
{
    public static final String LOGPREFIX = "semaine";

    /**
     * Returns true if it appears that log4j have been previously configured. This code
     * checks to see if there are any appenders defined for log4j which is the
     * definitive way to tell if log4j is already initialized
     */
    @SuppressWarnings("unchecked")
	public static boolean isLog4jConfigured() {
        Enumeration appenders = LogManager.getRootLogger().getAllAppenders();
    if (appenders.hasMoreElements()) {
        return true;
    }
    return false;
}
    
	/**
	 * Set up the log4j logging system using a default configuration.
	 */
	public static void setupLog4j() {
		if (isLog4jConfigured()) {
			return;
		}
		// Tell apache commons logging to use log4j:
		System.setProperty("org.apache.commons.logging.Log", "org.apache.commons.logging.impl.Log4JLogger");
		try {
	        Properties logprops = new Properties();
	        InputStream propIS = new BufferedInputStream(new FileInputStream(getConfigFile("semaine.log", "log4j.properties")));
	        logprops.load(propIS);
	        propIS.close();
	        // And allow Properties file (and thus System properties) to overwrite the single entry
	        // log4j.logger.eu.semaine:
	        // (e.g., -Dlog4j.logger.eu.semaine=DEBUG,stderr will log debug level output to stderr)
	        String loggerSemaineKey = "log4j.logger."+LOGPREFIX;
	        String loggerSemaineValue = System.getProperty(loggerSemaineKey);
	        if (loggerSemaineValue != null) {
	            logprops.setProperty(loggerSemaineKey, loggerSemaineValue);
	        }
	        PropertyConfigurator.configure(logprops);
		} catch (Exception ioe) {
			ioe.printStackTrace();
			System.err.println("Cannot read log4j configuration from "+System.getProperty("semaine.log", "log4j.properties")+" -- will log to stderr");
			PatternLayout layout = new PatternLayout("%d [%t] %-5p %-10c %m\n");
			BasicConfigurator.configure(new ConsoleAppender(layout));
			Logger.getRootLogger().setLevel(Level.DEBUG);
			Logger.getLogger("org.apache").setLevel(Level.WARN);
			Logger.getLogger("org.springframework").setLevel(Level.WARN);
		}
	}
	
    /**
     * Provide a Logger object whose name is built from SEMAINEUtils.LOGPREFIX and the given nameSuffix.
     * @param nameSuffix the suffix to use for the logger name.
     * @return
     */
    public static Logger getLogger(String nameSuffix) {
        return Logger.getLogger(LOGPREFIX+"."+nameSuffix);
    }

    /**
     * Provide a Logger object whose name is built from SEMAINEUtils.LOGPREFIX and the name of the given class as a suffix.
     * @param the class whose simple name to use asthe suffix for the logger name.
     * @return
     */
    @SuppressWarnings("unchecked")
	public static Logger getLogger(Class clazz) {
        return getLogger(clazz.getSimpleName());
    }

	/**
	 * Get a config file as defined in the given system property. If the setting
	 * in the system property is a relative path, then this path is interpreted as
	 * relative to the location of the system config file.
	 * @param configSystemProperty the name of a config setting containing the filename and path of the config file to load, e.g. "semaine.character-config".
	 * @return a file object representing the requested config file, or null if the config file could not be found.
	 */
	public static File getConfigFile(String configSystemProperty) {
		return getConfigFile(configSystemProperty, null);
	}

	/**
	 * Get a config file as defined in the given system property, using the given default value if the property is not set. If the setting
	 * in the system property is a relative path, then this path is interpreted as
	 * relative to the location of the system config file.
	 * @param configSystemProperty the name of a config setting containing the filename and path of the config file to load, e.g. "semaine.character-config".
	 * @return a file object representing the requested config file, or null if the config file could not be found.
	 */
	public static File getConfigFile(String configSystemProperty, String defaultValue) {
		String filename = System.getProperty(configSystemProperty, defaultValue);
		if (filename == null) {
			return null;
		}
		File configFile = new File(filename);
		if (!configFile.isAbsolute()) {
			// relative filename is relative to the config folder
			// (i.e., the same folder as the component runner config file)
			String componentConfigFilename = System.getProperty("semaine.config-file");
			assert componentConfigFilename != null;
			File configFolder = new File(componentConfigFilename).getParentFile();
			configFile = new File(configFolder, filename);
		}
		if (!configFile.exists()) {
			return null;
		}
		return configFile;
	}
	
	
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
    
    /**
     * Convert the given string into a long value, if possible.
     * @param valueString string to be converted into a long.
     * @param defaultValue value to use if the string cannot be converted into a long.
     * @return the best approximation that can be made for the long value given in valueString. If valueString
     * cannot be converted into any long, returns defaultValue.
     */
    public static long parseLong(String valueString, long defaultValue) {
    	long value;
    	try {
    		value = Long.parseLong(valueString);
    	} catch (NumberFormatException nfe) {
    		return defaultValue;
    	}
    	return value;
    }

    /**
     * Try to get the system property with the given name as a long value.
     * If the system property is not defined, or cannot be interpreted as a long,
     * return defaultvalue.
     * @param propertyName
     * @param defaultValue
     * @return the property as a long or the default value
     */
    public static long getLongProperty(String propertyName, long defaultValue) {
    	String value = System.getProperty(propertyName);
    	if (value == null) {
    		return defaultValue;
    	}
    	return parseLong(value, defaultValue);
    }

    
}
