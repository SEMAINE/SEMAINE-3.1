package eu.semaine.components.dialogue.test;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.util.ArrayList;


public class TimeLineComponent extends Component
{
	/* The heights of the lines where the speech-objects and signal-objects are drawn on */
	private static int AGENT_SPEECH_LINE_HEIGHT = 65;
	private static int USER_SPEECH_LINE_HEIGHT = 95;
	
	private static int NAME_SPACE = 70;
	
	private ArrayList<UserSpeech> userSpeechObjects = new ArrayList<UserSpeech>();
	private ArrayList<AgentSpeech> agentSpeechObjects = new ArrayList<AgentSpeech>();
	
	/* The height and width of the drawComponent */
	private int height;
	private int width;
	
	private long userSpeechStartTime = 0;
	private long agentSpeechPrepTime = 0;
	private long agentSpeechStartTime = 0;
	
	public TimeLineComponent( long time )
	{
		height = 250;
		width = ((int)(time/10 + NAME_SPACE));
		setPreferredSize(new Dimension(width, height));
	}
	
	public Dimension getSize()
	{
		return new Dimension(width,height);
	}
	
	public void importLog( ArrayList<String> log )
	{
		for( String line : log ) {
			long time = Long.parseLong( line.substring(0, line.indexOf("	")) );
			if( line.contains("AgentAction:SendUtterance") ) {
				agentSpeechPrepTime = time;
			} else if( line.contains("AgentAction:UtteranceStarted") ) {
				agentSpeechStartTime = time;
			} else if( line.contains("AgentAction:UtteranceStopped") ) {
				if( agentSpeechPrepTime == 0 || agentSpeechStartTime == 0 ) {
					System.out.println("Something is wrong in the log!");
				} else {
					System.out.println(line);
					// TODO: Deze info staat bij de start, niet bij de stop ;)
					String type = line.substring( line.indexOf("type=")+5, line.indexOf(",",line.indexOf("type=")) );
					String utterance = line.substring( line.indexOf("utterance=")+10,line.length() );
					agentSpeechObjects.add(new AgentSpeech(agentSpeechPrepTime, agentSpeechStartTime, time, type, utterance) );
					agentSpeechPrepTime = 0;
					agentSpeechStartTime = 0;
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
	
	public void paint( Graphics g )
	{
		Graphics2D g2d = (Graphics2D)g;
		drawBasics(g2d);
		
		for( AgentSpeech obj : agentSpeechObjects ) {
			drawAgentSpeech( g2d, obj );
		}
		//drawUserSpeech();
	}
	
	/**
	 * Draws the basics of the screen: the names of the agents and the lines to put the events on
	 * @param g2d	-	 the Graphics-object to draw on
	 */
	public void drawBasics( Graphics2D g2d )
	{
		g2d.setColor(getBackground());
		g2d.fillRect(0, 0, getWidth(), getHeight());
		g2d.setColor(getForeground());
		
		g2d.drawString("Agent", 10, 70);
		g2d.drawString("User", 20, 140);
		g2d.setStroke(new BasicStroke(4, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
		g2d.drawLine(60, 10, 60, 180);
		g2d.drawLine(10, 20, width-10, 20);
		
		g2d.setStroke(new BasicStroke(2, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
		g2d.drawLine( NAME_SPACE, AGENT_SPEECH_LINE_HEIGHT, width-20, AGENT_SPEECH_LINE_HEIGHT );
		g2d.drawLine( NAME_SPACE, USER_SPEECH_LINE_HEIGHT, width-20, USER_SPEECH_LINE_HEIGHT );
	}
	
	public void drawAgentSpeech( Graphics2D g2d, AgentSpeech obj )
	{
		int rectHeight = 20;
		int tierHeight = AGENT_SPEECH_LINE_HEIGHT;
		
		
		/* Draw the outline of the object */
		g2d.setColor( Color.BLACK );
		g2d.drawRect((int)(obj.starttime/10)+NAME_SPACE, tierHeight-(rectHeight/2), (int)((obj.endtime/10)-(obj.starttime/10)), rectHeight);
		
		/* Set the fill-color of the object*/
		g2d.setColor( new Color(0, 153, 0) );
		
		/* Fill the object */
		g2d.fillRect( (int)(obj.starttime/10)+NAME_SPACE, tierHeight-(rectHeight/2), (int)((obj.endtime/10)-(obj.starttime/10)), rectHeight);
	}
	
	public void drawUserSpeech( Graphics2D g2d, UserSpeech obj )
	{
		
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
