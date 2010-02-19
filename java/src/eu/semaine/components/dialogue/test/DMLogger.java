package eu.semaine.components.dialogue.test;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.GregorianCalendar;
import java.util.HashMap;

import javax.swing.JFrame;
import javax.swing.JPanel;

import com.sun.image.codec.jpeg.JPEGCodec;
import com.sun.image.codec.jpeg.JPEGImageEncoder;

public class DMLogger
{
	private static DMLogger ref;
	private PrintWriter out;

	private boolean willLog;

	private ArrayList<String> log = new ArrayList<String>();
	private long lastTime = 0;

	private ArrayList<DetectedWords> detectedWords = new ArrayList<DetectedWords>();

	private String dateAndTime;
	private String file = "DMlog.txt";
	private String path = "";

	private DMLogger( boolean willLogParam )
	{
		GregorianCalendar cal = new GregorianCalendar();
		dateAndTime = cal.get(Calendar.YEAR) + "-" + (cal.get(Calendar.MONTH)+1) + "-" + cal.get(Calendar.DAY_OF_MONTH) + "_" + cal.get(Calendar.HOUR_OF_DAY) + "." + cal.get(Calendar.MINUTE);
		file = "DMLog_"+dateAndTime+".txt";
		Object obj = System.getProperties().get("semaine.dialogue.logging");
		if( obj != null ) {
			willLog = Boolean.parseBoolean( obj.toString() );
		} else {
			willLog = willLogParam;
		}
		
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
			ref = new DMLogger( false );
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

	public void logWords( long starttime, String keywords, String times, String confidences )
	{
		
//		long oldStarttime;
//		if( detectedWords.size() == 0 ) {
//			oldStarttime = -100;
//		} else {
//			oldStarttime = detectedWords.get( detectedWords.size()-1 ).starttime;
//		}
//		if( Math.abs(starttime - oldStarttime) > 10 ) {
//			detectedWords.add(new DetectedWords(starttime, keywords,times));
//		}
		if( Double.parseDouble( confidences.split(" ")[0]) != 0 ) {
			detectedWords.add(new DetectedWords(starttime, keywords,times));
		}
	}

	public void createTimeline()
	{
		String words = "";
		String times = "";
		for( DetectedWords value : detectedWords ) {
			words = words + value.words + " ";
			times = times + value.times + " ";
		}
		words = words.trim();
		times = times.trim();

		TimeLineComponent timeLine = new TimeLineComponent(lastTime, words, times);
		timeLine.setBackground(Color.WHITE);
		timeLine.importLog(log);
		Dimension size = timeLine.getSize();
		BufferedImage myImage = new BufferedImage(size.width, size.height, BufferedImage.TYPE_INT_RGB);
		Graphics2D g2 = myImage.createGraphics();
		timeLine.paintComponent(g2);
		try {
			OutputStream out = new FileOutputStream("DMLog_"+dateAndTime+".jpg");
			JPEGImageEncoder encoder = JPEGCodec.createJPEGEncoder(out);
			encoder.encode(myImage);
			out.close();
		} catch (Exception e) {
			System.out.println(e); 
		}
	}

	private class DetectedWords {
		public long starttime;
		public String words;
		public String times;

		public DetectedWords( long starttime, String words, String times )
		{
			this.starttime = starttime;
			this.words = words;
			this.times = times;
		}
	}
}
