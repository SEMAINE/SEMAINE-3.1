package eu.semaine.components.dialogue.test;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.util.ArrayList;

import javax.swing.JComponent;


public class TimeLineComponent extends JComponent
{
	/* The heights of the lines where the speech-objects and signal-objects are drawn on */
	private static int AGENT_SPEECH_LINE_HEIGHT = 65;
	private static int USER_SPEECH_LINE_HEIGHT = 105;
	private static int USER_WORDS_LINE_HEIGHT = 145;
	private static int USER_FEATURES_LINE_HEIGHT = 185;
	private static int USER_HEADMOVEMENT_LINE_HEIGHT = 225;
	private static int USER_VALENCE_AROUSAL_LINE_HEIGHT = 265;
	private static int USER_POTENCY_INTEREST_LINE_HEIGHT = 305;
	
	private static int NAME_SPACE = 150;
	
	private ArrayList<UserSpeech> userSpeechObjects = new ArrayList<UserSpeech>();
	private ArrayList<AgentSpeech> agentSpeechObjects = new ArrayList<AgentSpeech>();
	private ArrayList<HeadMovement> headMovements = new ArrayList<HeadMovement>();
	private ArrayList<Features> featureList = new ArrayList<Features>();
	private ArrayList<Emotion> emotions = new ArrayList<Emotion>();
	
	/* The height and width of the drawComponent */
	private int height;
	private int width;
	
	private String type = null;
	private String utterance = null;
	
	private String detectedWords;
	private String detectedTimes;
	
	private long userSpeechStartTime = 0;
	private long agentSpeechPrepTime = 0;
	private long agentSpeechStartTime = 0;
	
	public TimeLineComponent( long time, String words, String times )
	{
		height = 300;
		width = ((int)(time/10 + NAME_SPACE));
		setBackground(Color.WHITE);
		setOpaque(true);
		setPreferredSize(new Dimension(width, height));
		
		this.detectedWords = words;
		this.detectedTimes = times;
	}
	
	public Dimension getSize()
	{
		return new Dimension(width,height);
	}
	
	public void importLog( ArrayList<String> log )
	{
		for( String line : log ) {
			//System.out.println(line);
			long time = Long.parseLong( line.substring(0, line.indexOf("	")) );
			if( line.contains("AgentAction:SendUtterance") ) {
				agentSpeechPrepTime = time;
				type = line.substring( line.indexOf("type=")+5, line.indexOf(",",line.indexOf("type=")) );
				utterance = line.substring( line.indexOf("utterance=")+10,line.length() );
			} else if( line.contains("AgentAction:UtteranceStarted") ) {
				agentSpeechStartTime = time;
			} else if( line.contains("AgentAction:UtteranceStopped") ) {
				if( agentSpeechStartTime == 0 ) {
					agentSpeechStartTime = time-500;
				}
				if( agentSpeechPrepTime == 0 || type == null || utterance == null ) {
					System.out.println("Something is wrong in the log! Line: " + line);
				} else {
					//System.out.println(line);
					// TODO: Deze info staat bij de start, niet bij de stop ;)
					agentSpeechObjects.add(new AgentSpeech(agentSpeechPrepTime, agentSpeechStartTime, time, type, utterance) );
					agentSpeechPrepTime = 0;
					agentSpeechStartTime = 0;
					type = null;
					utterance = null;
				}
			} else if( line.contains("UserAction:UserStartedSpeaking") ) {
				userSpeechStartTime = time;
			} else if( line.contains("UserAction:UserStoppedSpeaking") ) {
				if( userSpeechStartTime == 0 ) {
					System.out.println("Something is wrong in the log!");
				} else {
					String keywords = "";
					if( line.indexOf("words=")+6 < line.length() ) {
						keywords = line.substring( line.indexOf("words=")+6, line.length() );
					}
					userSpeechObjects.add( new UserSpeech(userSpeechStartTime, time, keywords) );
					userSpeechStartTime = 0;
				}
			} else if( line.contains("UserAction:HeadMovement") ){
				String movement = line.substring( line.indexOf("movement=")+9, line.indexOf("starttime=",line.indexOf("movement=") ) );
				long starttime = Long.parseLong( line.substring( line.indexOf("starttime=")+10, line.indexOf(" ",line.indexOf("starttime=") ) ) );
				long endtime = Long.parseLong( line.substring( line.indexOf("endtime=")+8, line.length() ) );
				headMovements.add( new HeadMovement(movement,starttime,endtime) );
			} else if( line.contains("UserAction:ContentFeatures") ) {
				String features = line.substring( line.indexOf("features=")+9, line.indexOf("starttime=",line.indexOf("features=") ) );
				long starttime = Long.parseLong( line.substring( line.indexOf("starttime=")+10, line.indexOf(" ",line.indexOf("starttime=") ) ) );
				long endtime = Long.parseLong( line.substring( line.indexOf("endtime=")+8, line.length() ) );
				featureList.add( new Features(features,starttime,endtime) );
			} else if( line.contains("UserAction:Valence") ) {
				float value = Float.parseFloat( line.substring( line.indexOf("value=")+6, line.length() ) );
				emotions.add( new Emotion("valence", value, time ) );
			} else if( line.contains("UserAction:Arousal") ) {
				float value = Float.parseFloat( line.substring( line.indexOf("value=")+6, line.length() ) );
				emotions.add( new Emotion("arousal", value, time ) );
			} else if( line.contains("UserAction:Potency") ) {
				float value = Float.parseFloat( line.substring( line.indexOf("value=")+6, line.length() ) );
				emotions.add( new Emotion("potency", value, time ) );
			} else if( line.contains("UserAction:Interest") ) {
				float value = Float.parseFloat( line.substring( line.indexOf("value=")+6, line.indexOf("value=")+11 ) );
				emotions.add( new Emotion("interest", value, time ) );
			}
		}
	}
	
	public void paintComponent( Graphics g )
	{
		System.out.println("Painting log...");
		Graphics2D g2d = (Graphics2D)g;
		drawBasics(g2d);
		
		for( AgentSpeech obj : agentSpeechObjects ) {
			drawAgentSpeech( g2d, obj );
		}
		for( UserSpeech obj : userSpeechObjects ) {
			drawUserSpeech( g2d, obj );
		}
		drawUserWords( g2d );
		for( HeadMovement obj : headMovements ) {
			drawHeadMovement( g2d, obj );
		}
		for( Features obj : featureList ) {
			drawFeatures( g2d, obj );
		}
		for( Emotion obj : emotions ) {
			drawEmotion( g2d, obj );
		}
	}
	
	/**
	 * Draws the basics of the screen: the names of the agents and the lines to put the events on
	 * @param g2d	-	 the Graphics-object to draw on
	 */
	public void drawBasics( Graphics2D g2d )
	{
		g2d.setColor(Color.WHITE);
		g2d.fillRect(0, 0, width, height);
		g2d.setColor(Color.BLACK);
		
		g2d.drawString("Agent", NAME_SPACE-60, AGENT_SPEECH_LINE_HEIGHT+5);
		g2d.drawString("User", NAME_SPACE-55, USER_SPEECH_LINE_HEIGHT+5);
		g2d.drawString("User words", NAME_SPACE-90, USER_WORDS_LINE_HEIGHT+5);
		g2d.drawString("Content Features", NAME_SPACE-135, USER_FEATURES_LINE_HEIGHT+5);
		g2d.drawString("Head movements", NAME_SPACE-120, USER_HEADMOVEMENT_LINE_HEIGHT+5);
		g2d.drawString("Valence/Arousal", NAME_SPACE-130, USER_VALENCE_AROUSAL_LINE_HEIGHT+5);
		g2d.drawString("Potency/Interest", NAME_SPACE-135, USER_POTENCY_INTEREST_LINE_HEIGHT+5);
		g2d.setStroke(new BasicStroke(4, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
		g2d.drawLine(NAME_SPACE-10, 10, NAME_SPACE-10, height-10);
		g2d.drawLine(10, 20, width-10, 20);
		
		g2d.setColor(Color.LIGHT_GRAY);
		g2d.setStroke(new BasicStroke(2, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
		g2d.drawLine( NAME_SPACE, AGENT_SPEECH_LINE_HEIGHT, width-20, AGENT_SPEECH_LINE_HEIGHT );
		g2d.drawLine( NAME_SPACE, USER_WORDS_LINE_HEIGHT, width-20, USER_WORDS_LINE_HEIGHT );
		g2d.drawLine( NAME_SPACE, USER_SPEECH_LINE_HEIGHT, width-20, USER_SPEECH_LINE_HEIGHT );
		g2d.drawLine( NAME_SPACE, USER_FEATURES_LINE_HEIGHT, width-20, USER_FEATURES_LINE_HEIGHT );
		g2d.drawLine( NAME_SPACE, USER_HEADMOVEMENT_LINE_HEIGHT, width-20, USER_HEADMOVEMENT_LINE_HEIGHT );
		g2d.drawLine( NAME_SPACE, USER_VALENCE_AROUSAL_LINE_HEIGHT, width-20, USER_VALENCE_AROUSAL_LINE_HEIGHT );
		g2d.drawLine( NAME_SPACE, USER_POTENCY_INTEREST_LINE_HEIGHT, width-20, USER_POTENCY_INTEREST_LINE_HEIGHT );
		
		/* Set time markers */
		int j=0;
		for( int i=NAME_SPACE; i<width; i=i+20 ) {
			if( j % 5 == 0 ) {
				g2d.setColor(Color.BLACK);
				g2d.drawString(""+(j*20*10)+"ms",i-5,10);
			} else {
				g2d.setColor(Color.LIGHT_GRAY);
			}
			g2d.drawLine(i,10,i,20);
			j++;
		}
	}
	
	public void drawAgentSpeech( Graphics2D g2d, AgentSpeech obj )
	{
		int rectHeight = 20;
		int tierHeight = AGENT_SPEECH_LINE_HEIGHT;
		
		
		/* Draw the outline of the object */
		g2d.setColor( Color.BLACK );
		g2d.drawRect((int)(obj.starttime/10)+NAME_SPACE, tierHeight-(rectHeight/2), (int)((obj.endtime/10)-(obj.starttime/10)), rectHeight);
		g2d.drawLine((int)(obj.preptime/10)+NAME_SPACE, tierHeight, (int)(obj.starttime/10)+NAME_SPACE, tierHeight);
		g2d.drawLine((int)(obj.preptime/10)+NAME_SPACE, tierHeight-(rectHeight/2), (int)(obj.preptime/10)+NAME_SPACE, tierHeight+(rectHeight/2));
		
		/* Set the fill-color of the object*/
		g2d.setColor( new Color(128, 255, 128) );
		
		/* Fill the object */
		g2d.fillRect( (int)(obj.starttime/10)+NAME_SPACE, tierHeight-(rectHeight/2), (int)((obj.endtime/10)-(obj.starttime/10)), rectHeight);
		
		/* Fill in the utterance */
		g2d.setColor( Color.BLACK );
		g2d.drawString(obj.utterance+" ("+obj.type+")", (int)(obj.starttime/10)+NAME_SPACE+10, tierHeight+(rectHeight/4));
	}
	
	public void drawUserSpeech( Graphics2D g2d, UserSpeech obj )
	{
		int rectHeight = 20;
		int tierHeight = USER_SPEECH_LINE_HEIGHT;
		
		
		/* Draw the outline of the object */
		g2d.setColor( Color.BLACK );
		g2d.drawRect((int)(obj.starttime/10)+NAME_SPACE, tierHeight-(rectHeight/2), (int)((obj.endtime/10)-(obj.starttime/10)), rectHeight);
		
		/* Set the fill-color of the object*/
		g2d.setColor( new Color(128, 255, 128) );
		
		/* Fill the object */
		g2d.fillRect( (int)(obj.starttime/10)+NAME_SPACE, tierHeight-(rectHeight/2), (int)((obj.endtime/10)-(obj.starttime/10)), rectHeight);
	}
	
	public void drawUserWords( Graphics2D g2d )
	{
		int rectHeight = 20;
		int tierHeight = USER_WORDS_LINE_HEIGHT;
		
		if( detectedWords.length() == 0 ) {
			return;
		}
		
		String[] words = detectedWords.trim().split(" ");
		String[] times = detectedTimes.trim().split(" ");
		String word;
		long time;
		
		ArrayList<String> wordList = new ArrayList<String>();
		ArrayList<Long> timeList = new ArrayList<Long>();
		
		for( int i=0; i<words.length; i++ ) {
			if( wordList.size() > 0 ) {
				time = Long.parseLong(times[i]);
				if( timeList.get(timeList.size()-1) == time ) {
					wordList.set(wordList.size()-1, wordList.get(wordList.size()-1) + " " + words[i]);
				} else {
					wordList.add(words[i]);
					timeList.add(time);
				}
			} else {
				wordList.add(words[i]);
				timeList.add( Long.parseLong(times[i]) );
			}
		}
		
		for( int i=0; i<wordList.size(); i++ ) {
			word = wordList.get(i);
			time = timeList.get(i);
			
			g2d.setColor( Color.BLACK );
			g2d.drawLine((int)time/10+NAME_SPACE, tierHeight-(rectHeight/2), (int)time/10+NAME_SPACE, tierHeight+(rectHeight/2));
			
			g2d.drawString(word, (int)(time/10)+5+NAME_SPACE, tierHeight+(rectHeight/4));
		}
	}
	
	public void drawFeatures( Graphics2D g2d, Features obj )
	{
		int rectHeight = 20;
		int tierHeight = USER_FEATURES_LINE_HEIGHT;
		
		/* Draw the outline of the object */
		//g2d.setColor( Color.BLACK );
		//g2d.drawRect((int)(obj.starttime/10)+NAME_SPACE, tierHeight-(rectHeight/2), (int)((obj.endtime/10)-(obj.starttime/10)), rectHeight);
		
		/* Set the fill-color of the object*/
		//g2d.setColor( new Color(128, 255, 128) );
		
		/* Fill the object */
		//g2d.fillRect( (int)(obj.starttime/10)+NAME_SPACE, tierHeight-(rectHeight/2), (int)((obj.endtime/10)-(obj.starttime/10)), rectHeight);
		
		/* Fill in the utterance */
		g2d.setColor( Color.BLACK );
		g2d.drawLine((int)obj.starttime/10+NAME_SPACE, tierHeight-(rectHeight/2), (int)obj.starttime/10+NAME_SPACE, tierHeight+(rectHeight/2));
		g2d.drawString(obj.features, (int)(obj.starttime/10)+NAME_SPACE+10, tierHeight+(rectHeight/4));
	}
	
	public void drawHeadMovement( Graphics2D g2d, HeadMovement obj )
	{
		int rectHeight = 20;
		int tierHeight = USER_HEADMOVEMENT_LINE_HEIGHT;
		
		/* Draw the outline of the object */
		g2d.setColor( Color.BLACK );
		g2d.drawRect((int)(obj.starttime/10)+NAME_SPACE, tierHeight-(rectHeight/2), (int)((obj.endtime/10)-(obj.starttime/10)), rectHeight);
		
		/* Set the fill-color of the object*/
		g2d.setColor( new Color(128, 255, 128) );
		
		/* Fill the object */
		g2d.fillRect( (int)(obj.starttime/10)+NAME_SPACE, tierHeight-(rectHeight/2), (int)((obj.endtime/10)-(obj.starttime/10)), rectHeight);
		
		/* Fill in the utterance */
		g2d.setColor( Color.BLACK );
		g2d.drawString(obj.movement, (int)(obj.starttime/10)+NAME_SPACE+10, tierHeight+(rectHeight/4));
	}
	
	public void drawEmotion( Graphics2D g2d, Emotion obj )
	{
		int rectHeight = 10;
		int rectWidth = 50;
		int tierHeight = 0;
		
		if( obj.emotion.equals("valence") ) {
			tierHeight = USER_VALENCE_AROUSAL_LINE_HEIGHT - (rectHeight/2)-2;
		} else if( obj.emotion.equals("arousal") ) {
			tierHeight = USER_VALENCE_AROUSAL_LINE_HEIGHT + (rectHeight/2)+2;
		} else if( obj.emotion.equals("potency") ) {
			tierHeight = USER_POTENCY_INTEREST_LINE_HEIGHT - (rectHeight/2)-2;
		} else if( obj.emotion.equals("interest") ) {
			tierHeight = USER_POTENCY_INTEREST_LINE_HEIGHT - (rectHeight/2)+2;
		}
		
		/* Draw the outline of the object */
		g2d.setColor( Color.BLACK );
		g2d.drawRect((int)(obj.time/10)+NAME_SPACE, tierHeight-(rectHeight/2), rectWidth, rectHeight);
		
		/* Set the fill-color of the object*/
		g2d.setColor( new Color(128, 255, 128) );
		
		/* Fill the object */
		g2d.fillRect( (int)(obj.time/10)+NAME_SPACE, tierHeight-(rectHeight/2), rectWidth, rectHeight);
		
		/* Fill in the utterance */
		g2d.setColor( Color.BLACK );
		g2d.drawString(""+obj.value, (int)(obj.time/10)+NAME_SPACE+10, tierHeight+(rectHeight/4)+2);
	}
	
	private class UserSpeech
	{
		public long starttime;
		public long endtime;
		public String keywords;
		
		public UserSpeech( long starttime, long endtime, String keywords )
		{
			this.starttime = starttime;
			this.endtime = endtime;
			this.keywords = keywords;
		}
	}
	
	private class AgentSpeech
	{
		public long preptime;
		public long starttime;
		public long endtime;
		public String type;
		public String utterance;
		
		public AgentSpeech( long preptime, long starttime, long endtime, String type, String utterance )
		{
			this.preptime = preptime;
			this.starttime = starttime;
			this.endtime = endtime;
			this.type = type;
			this.utterance = utterance;
		}
	}
	
	private class HeadMovement
	{
		public String movement;
		public long starttime;
		public long endtime;
		
		public HeadMovement( String movement, long starttime, long endtime )
		{
			this.movement = movement;
			this.starttime = starttime;
			this.endtime = endtime;
		}
	}
	
	private class Features
	{
		public String features;
		public long starttime;
		public long endtime;
		
		public Features( String features, long starttime, long endtime )
		{
			this.features = features;
			this.starttime = starttime;
			this.endtime = endtime;
		}
	}
	
	private class Emotion
	{
		public String emotion;
		public float value;
		public long time;
		
		public Emotion( String emotion, Float value, long time )
		{
			this.emotion = emotion;
			this.value = value;
			this.time = time;
		}
	}
}
