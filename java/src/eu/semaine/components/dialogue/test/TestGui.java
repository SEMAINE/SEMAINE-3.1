/**
 * Copyright (C) 2008 University of Twente, HMI. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.dialogue.test;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.util.List;
import java.util.Random;

import javax.jms.JMSException;
import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JEditorPane;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.JViewport;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.xml.EMMA;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.jms.IOBase.Event;
import eu.semaine.jms.message.SEMAINEEmmaMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.EmmaReceiver;
import eu.semaine.jms.receiver.FMLReceiver;
import eu.semaine.jms.receiver.UserStateReceiver;
import eu.semaine.jms.sender.EmmaSender;
import eu.semaine.jms.sender.FeatureSender;
import eu.semaine.util.XMLTool;
import eu.semaine.datatypes.xml.BML;

/**
 * This class creates a GUI to test the Dialogue Management.
 * It provides a method to send input to the DM and to receive the output.
 * 
 * @author Mark tM
 * @version 0.1 - dummy class
 *
 */

public class TestGui extends Component
{
	/* GUI Elements */
	public JFrame jframe;
	private JTextField input;
	private JEditorPane output;
	private JScrollPane scroller;
	
	/* The text in the textArea */
	private String outputText = "++++++++ Welcome ++++++++";
	
	/* The last time a keypress was registered */
	public long lastTickTime = System.currentTimeMillis();
	
	/* A boolean to check if the GUI is started or not */
	private boolean started = false;
	
	/* A boolean to keep track if the user is typing or not */
	public boolean typing = false;
	
	/* Senders and Receivers */
	private EmmaSender userStateSender;
	private FeatureSender featureSender;
	private FMLReceiver fmlReceiver;
	private EmmaReceiver emmaReceiver;
	
	Random r = new Random();
	
	
	/**
	 * Creates a new GUI
	 * + Creates the Senders and Receivers
	 * + Creates and starts a timer class
	 * + Initializes the interface
	 * @throws JMSException
	 */
	public TestGui() throws JMSException
	{
		super("GUI");
		userStateSender = new EmmaSender("semaine.data.state.user", "GUI");
		senders.add(userStateSender); // so it can be started etc
		featureSender = new FeatureSender("semaine.data.analysis.features.voice", "", "GUI", 10);
		String[] featureNames = new String[2];
		featureNames[0] = "logEnergy";
		featureNames[1] = "F0strength";
		featureSender.setFeatureNames(featureNames);
		senders.add(featureSender);
		
		fmlReceiver = new FMLReceiver("semaine.data.action.candidate.function");
		receivers.add(fmlReceiver); // to set up properly
		emmaReceiver = new EmmaReceiver("semaine.data.state.user", "datatype = 'EMMA'");
		receivers.add(emmaReceiver);
		
		initGui();
	}
	
	public void act()
	{
		if( !started ) {
			MyTimer timer = new MyTimer(this);
			timer.start();
			System.out.println("Started");
			started = true;
		}
	}
	
	/**
	 * Called when a message is received
	 * Checks if the message contains FML.
	 * If so, it displays the message it contains in the GUI
	 */
	@Override
	public void react(SEMAINEMessage m) throws JMSException
	{
		if ( m instanceof SEMAINEXMLMessage ) {
			SEMAINEXMLMessage xm = (SEMAINEXMLMessage)m;
			boolean isFML = "FML".equals(xm.getDatatype());
			if (isFML) {
				Element bml = XMLTool.needChildElementByTagNameNS(xm.getDocument().getDocumentElement(), BML.BML, BML.namespace);
				Element speech = XMLTool.needChildElementByTagNameNS(bml, BML.SPEECH, BML.namespace);
				Element text = XMLTool.needChildElementByTagNameNS(speech, BML.TEXT, BML.namespace);
				printLine( "+ " + text.getTextContent() );
			}
		}
		if( isSentence(m) ) {
			printLine(getSentence(m));
		}
		
	}
	
	/**
	 * Checks if the Message is ASR output and if it contains a sentence.
	 * @param m the message to check
	 * @return true if the message says that the user is currently silent
	 */
	public boolean isSentence( SEMAINEMessage m ) throws JMSException
	{
		if( m instanceof SEMAINEEmmaMessage ) {
			SEMAINEEmmaMessage em = (SEMAINEEmmaMessage)m;
			Element interpretation = em.getTopLevelInterpretation();
			if (interpretation != null && interpretation.getAttribute("processed").equals("")) {
				List<Element> texts = em.getTextElements(interpretation);
				for (Element text : texts) {
					/*
					String utterance = text.getTextContent();
					if( utterance != null ) {
						return true;
					}
					*/
					if( text.getAttribute("name") != null )
						return true;
				}
			}
		}
		return false;
	}

	/**
	 * Returns the detected sentence in the SEMAINEMessage m
	 * @param m the message to detect the sentence in
	 * @return the detected sentence
	 */
	public String getSentence( SEMAINEMessage m ) throws JMSException
	{
		if( m instanceof SEMAINEEmmaMessage ) {
			SEMAINEEmmaMessage em = (SEMAINEEmmaMessage)m;
			Element interpretation = em.getTopLevelInterpretation();
			if (interpretation != null) {
				List<Element> texts = em.getTextElements(interpretation);
				for (Element text : texts) {
					
					String utterance = text.getTextContent();
					if( utterance != null ) {
						return utterance;
					}
					/*
					if( text.getAttribute("name") != null ) {
						return text.getAttribute("name");
					}*/
				}
			}
		}
		return null;
	}
	
	/**
	 * Initializes the GUI
	 */
	public void initGui()
	{
		/* Creates new Frame */
		jframe = new JFrame();
		jframe.setLayout( new BorderLayout() );
		//jframe.setPreferredSize(new Dimension(600,200));
		
		/* Create output textarea */
		output = new JEditorPane( "text/html", "<html><body>" + outputText + "</body></html>" );
		output.setEditable( false );
		scroller = new JScrollPane (output);
		scroller.setPreferredSize( new Dimension( 600, 200 ) );
		jframe.add( scroller, BorderLayout.CENTER );
		
		/* Create input area */
		JPanel panel = createInputSide();
		jframe.add( panel, BorderLayout.SOUTH );
		
		/* Finishes the frame */
		jframe.setTitle("ChatBot");
		jframe.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		jframe.pack();
		jframe.setLocationRelativeTo(null);
		jframe.setResizable(false);
		jframe.setVisible(true);
		
		/* Sets the cursor in the JTextField */
		input.requestFocus();
		input.setCaretPosition(0);
	}
	
	/**
	 * Creates the inputside of the gui, puts this in a JPanel and returns this.
	 * This version creates a JTextField and a Send-button.
	 * @return the JPanel with the inputside GUI
	 */
	public JPanel createInputSide()
	{
		/* Create new Panel */
		JPanel panel = new JPanel();
		
		/* Create input textfield */
		input = new JTextField(45);
		input.addActionListener(new MyActionListener(this));
		input.addKeyListener( new MyKeyListener(this) );
		input.setCaretPosition(0);
		panel.add(input);
		
		/* Create Send-button */
		JButton send = new JButton("send");
		send.addActionListener( new MyActionListener(this) );
		panel.add( send );
		
		return panel;
	}
	
	/**
	 * This method is called when input is entered with the input textfield
	 */
	public void newInput()
	{
		String newLine = input.getText();
		input.setText("");
		//printLine( newLine );
		sendUtterance( newLine );
	}
	
	/**
	 * This method prints the given line in the output textarea
	 * @param line - the line to print
	 */
	public void printLine( String line )
	{
		outputText = outputText + "<br>" + line;
		output.setText( "<html><body>" + outputText + "</body></html>" );
		JViewport view = scroller.getViewport();
		view.setViewPosition(new java.awt.Point(10, 99999999));
	}
	
	/**
	 * Called when the user presses a key. Used for turn taking behaviour
	 */
	public void keyPressed()
	{
		lastTickTime = System.currentTimeMillis();
		if( !typing ) {
			typing = true;
			//sendSpeakingSignal();
		}
	}
	
	/**
	 * Sends the given utterance to the userstate using an EmmaMessage
	 * @param line
	 */
	public void sendUtterance( String line )
	{
		System.out.println("Sending line");
		Document document = XMLTool.newDocument(EMMA.EMMA, EMMA.namespace, EMMA.version);
		Element interpretation = XMLTool.appendChildElement(document.getDocumentElement(), EMMA.INTERPRETATION);
		Element text = XMLTool.appendChildElement(interpretation, SemaineML.TEXT, SemaineML.namespace);
		text.setTextContent( line );
		interpretation.setAttribute(EMMA.START, String.valueOf(System.currentTimeMillis()));
		try {
			userStateSender.sendXML(document, System.currentTimeMillis(), Event.single);
		}catch( JMSException e ){}
	}
	
	/**
	 * Sends a signal that the user is speaking (in the gui -> typing)
	 */
	public void sendSpeakingSignal()
	{
		float[] features = new float[2];
		features[0] = -1;
		if( r.nextInt(6) == 0 ) {
			features[1] = 0;
		} else {
			features[1] = 1;
		}

		try {
			featureSender.sendFeatureVector(features, System.currentTimeMillis());
		}catch( JMSException e ){e.printStackTrace();}
	}
	
	/**
	 * Sends a signal that the user is silent (in the gui -> not typing)
	 */
	public void sendSilenceSignal()
	{
		float[] features = new float[2];
		features[0] = -15;
		features[1] = 0;
		try {
			featureSender.sendFeatureVector(features, System.currentTimeMillis());
		}catch( JMSException e ){e.printStackTrace();}
	}
}

/**
 * This class is used when the user presses Enter in the TextField or 
 * when he presses the Send-button.
 * @author MaatM
 *
 */
class MyActionListener implements ActionListener
{
	/** The parent GUI */
	private TestGui gui;

	/**
	 * The constructor of the ActionListener
	 * @param panel		the parent GUI
	 */
	MyActionListener( TestGui gui ) 
	{
		this.gui = gui;
	}

	/**
	 * Is called when the user presses Enter in the TextField or
	 * when he presses the Send-button, and sends a signal to the GUI that
	 * this action is performed.
	 * @param e the action event
	 */
	public void actionPerformed(ActionEvent e) 
	{
		gui.keyPressed();
		gui.newInput();
	}
}


/**
 * Listens to key-strokes
 * @author MaatM
 *
 */
class MyKeyListener implements KeyListener
{
	/* The parent GUI */
	private TestGui gui;
	
	public MyKeyListener( TestGui gui )
	{
		this.gui = gui;
	}

	public void keyPressed(KeyEvent keyEvent) 
	{
	}

	public void keyReleased(KeyEvent keyEvent) 
	{
	}

	public void keyTyped(KeyEvent keyEvent) 
	{
		gui.keyPressed();
	}
}

/**
 * This class is used to add timed events
 */
class MyTimer extends Thread
{
	private boolean stop = false;
	private TestGui gui;
	
	public MyTimer( TestGui gui )
	{
		this.gui = gui;
	}
	
	public void run()
	{
		while( !stop ) {
			try {
				Thread.sleep(10);
			} catch( InterruptedException e ) {
				e.printStackTrace();
			}
			if( gui.lastTickTime + 200 < System.currentTimeMillis() && gui.typing ) {
				gui.typing = false;
				//gui.sendSilenceSignal();
			}
			if( gui.typing ) {
				gui.sendSpeakingSignal();
			} else {
				gui.sendSilenceSignal();
			}
		}
	}

	public void stopThread()
	{
		stop = true;
	}
}
