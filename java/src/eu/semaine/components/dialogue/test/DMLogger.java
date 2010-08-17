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
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.GregorianCalendar;
import java.util.HashMap;

import javax.swing.JFrame;
import javax.swing.JPanel;

import com.sun.image.codec.jpeg.JPEGCodec;
import com.sun.image.codec.jpeg.JPEGImageEncoder;

import eu.semaine.components.dialogue.datastructures.DMProperties;

public class DMLogger
{
	private static DMLogger ref;
	private PrintWriter out;
	private PrintWriter out2;

	private boolean willLog;

	private ArrayList<String> log = new ArrayList<String>();
	private long lastTime = 0;

	private ArrayList<DetectedWords> detectedWords = new ArrayList<DetectedWords>();
	private HashMap<String,Integer> missingISParameters = new HashMap<String,Integer>();

	private String dateAndTime;
	private String file = "DMlog.txt";
	private String responseFile = "DMlog_Responses.txt";
	private String path = "logs/";

	private DMLogger( boolean willLogParam )
	{
		GregorianCalendar cal = new GregorianCalendar();
		DecimalFormat twoPlaces = new DecimalFormat("00");
		
		//String month = ""+;
		//if( month.length() == 1 ) month = "0" + month;
		//String day = ""+;
		//if( day.length() == 1 ) day = "0" + day;
		dateAndTime = cal.get(Calendar.YEAR) + "-" + twoPlaces.format(cal.get(Calendar.MONTH)+1) + "-" + twoPlaces.format(cal.get(Calendar.DAY_OF_MONTH)) + "_" + twoPlaces.format(cal.get(Calendar.HOUR_OF_DAY)) + "." + twoPlaces.format(cal.get(Calendar.MINUTE));
		file = "DMLog_"+dateAndTime+".txt";
		responseFile = "DMLog_" + dateAndTime + "_Responses.txt";
		
		willLog = DMProperties.getLogging();
		
		if( willLog ) {
			try {
				out = new PrintWriter( new FileWriter(path+file) );
				out2 = new PrintWriter( new FileWriter(path+responseFile) );
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
				for( String par : missingISParameters.keySet() ) {
					out.println("Missing IS-parameter: " + par + " ("+missingISParameters.get(par)+")");
				}
				out.flush();
				out.close();
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
	
	public void logUserTurn( long time, String response, String context )
	{
		if( willLog ) {
			out2.println(context + time + " : " + response);
			out2.println();
			out2.flush();
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
	
	public void logMissingISParameter( String parameter )
	{
		Integer i = missingISParameters.get(parameter);
		if( i == null ) i = new Integer(0);
		i = i + 1;
		missingISParameters.put( parameter,i );
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
			OutputStream out = new FileOutputStream(path+"DMLog_"+dateAndTime+".jpg");
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
