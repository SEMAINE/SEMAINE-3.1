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
	
	private static int NAME_SPACE = 70;
	
	private ArrayList<UserSpeech> userSpeechObjects = new ArrayList<UserSpeech>();
	private ArrayList<AgentSpeech> agentSpeechObjects = new ArrayList<AgentSpeech>();
	
	/* The height and width of the drawComponent */
	private int height;
	private int width;
	
	private String type = null;
	private String utterance = null;
	
	private long userSpeechStartTime = 0;
	private long agentSpeechPrepTime = 0;
	private long agentSpeechStartTime = 0;
	
	public TimeLineComponent( long time )
	{
		height = 200;
		width = ((int)(time/10 + NAME_SPACE));
		setBackground(Color.WHITE);
		setOpaque(true);
		setPreferredSize(new Dimension(width, height));
	}
	
	public Dimension getSize()
	{
		return new Dimension(width,height);
	}
	
	public void importLog( ArrayList<String> log )
	{
		for( String line : log ) {
			System.out.println(line);
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
					System.out.println("Something is wrong in the log!");
				} else {
					System.out.println(line);
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
					userSpeechObjects.add( new UserSpeech(userSpeechStartTime, time) );
					userSpeechStartTime = 0;
				}
			}
		}
	}
	
	public void paintComponent( Graphics g )
	{
		System.out.println("Painting...");
		Graphics2D g2d = (Graphics2D)g;
		drawBasics(g2d);
		
		for( AgentSpeech obj : agentSpeechObjects ) {
			drawAgentSpeech( g2d, obj );
		}
		for( UserSpeech obj : userSpeechObjects ) {
			drawUserSpeech( g2d, obj );
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
		
		g2d.drawString("Agent", 10, 70);
		g2d.drawString("User", 20, 110);
		g2d.setStroke(new BasicStroke(4, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
		g2d.drawLine(60, 10, 60, 180);
		g2d.drawLine(10, 20, width-10, 20);
		
		g2d.setColor(Color.LIGHT_GRAY);
		g2d.setStroke(new BasicStroke(2, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
		g2d.drawLine( NAME_SPACE, AGENT_SPEECH_LINE_HEIGHT, width-20, AGENT_SPEECH_LINE_HEIGHT );
		g2d.drawLine( NAME_SPACE, USER_SPEECH_LINE_HEIGHT, width-20, USER_SPEECH_LINE_HEIGHT );
		
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
	
	private class UserSpeech
	{
		public long starttime;
		public long endtime;
		
		public UserSpeech( long starttime, long endtime )
		{
			this.starttime = starttime;
			this.endtime = endtime;
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
}
