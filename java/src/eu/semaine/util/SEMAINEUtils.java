package eu.semaine.util;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.StringWriter;

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

}
