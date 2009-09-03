package eu.semaine.components.dialogue.test;

import java.awt.Dimension;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.util.ArrayList;

import com.sun.image.codec.jpeg.JPEGCodec;
import com.sun.image.codec.jpeg.JPEGImageEncoder;

public class DMLogger
{
	private static boolean WILL_LOG = false;
	
	private static DMLogger ref;
	private PrintWriter out;
	
	private boolean willLog;
	
	private ArrayList<String> log = new ArrayList<String>();
	private long lastTime = 0;
	
	private String file = "log.txt";
	private String path = "";
	
	private DMLogger( boolean willLog )
	{
		this.willLog = willLog;
		if( willLog ) {
			try {
				out = new PrintWriter( new FileWriter(path+file) );
			}catch( IOException e ) {
				e.printStackTrace();
			}
			addShutdownHook();
		}
	}

	private void addShutdownHook() {
		Runtime.getRuntime().addShutdownHook(new Thread() {
			public void run() {
				createTimeline();
			}
		});
	}
	
	public static DMLogger getLogger()
	{
		if( ref == null ) {
			ref = new DMLogger( WILL_LOG );
		}
		return ref;
	}
	
	public void log( long time, String msg )
	{
		if( willLog ) {
			out.println(time + "	" + msg);
			out.flush();
			if( msg.contains(":") ) {
				log.add(time + "	" + msg);
			}
			lastTime = time;
		}
	}
	
	public void createTimeline()
	{
		TimeLineComponent timeLine = new TimeLineComponent(lastTime);
		timeLine.importLog(log);
		Dimension size = timeLine.getSize();
		BufferedImage myImage = new BufferedImage(size.width, size.height, BufferedImage.TYPE_INT_RGB);
		Graphics2D g2 = myImage.createGraphics();
		timeLine.paint(g2);
		try {
			OutputStream out = new FileOutputStream("log.jpg");
			JPEGImageEncoder encoder = JPEGCodec.createJPEGEncoder(out);
			encoder.encode(myImage);
			out.close();
		} catch (Exception e) {
			System.out.println(e); 
		}
	}
}
