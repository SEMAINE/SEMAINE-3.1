package eu.semaine.util;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.StringWriter;
import java.util.Locale;
import java.util.StringTokenizer;

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
}
