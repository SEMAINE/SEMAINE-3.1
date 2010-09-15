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
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Random;

import javax.imageio.ImageIO;
import javax.jms.JMSException;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JEditorPane;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

import eu.semaine.components.Component;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.datatypes.stateinfo.UserStateInfo;
import eu.semaine.datatypes.xml.EMMA;
import eu.semaine.datatypes.xml.FML;
import eu.semaine.jms.IOBase.Event;
import eu.semaine.jms.message.SEMAINEEmmaMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEStateMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.FMLReceiver;
import eu.semaine.jms.receiver.Receiver;
import eu.semaine.jms.receiver.StateReceiver;
import eu.semaine.jms.sender.EmmaSender;
import eu.semaine.jms.sender.FMLSender;
import eu.semaine.jms.sender.StateSender;
import eu.semaine.util.XMLTool;
import eu.semaine.datatypes.xml.BML;
import eu.semaine.datatypes.xml.SSML;

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
	/* Specifies if the gui should send speech/silence signals as well */
	private boolean sendSpeechSignals = true;
	
	private ImageIcon userSpeakingIcon;
	private ImageIcon userListeningIcon;
	private ImageIcon agentSpeakingIcon;
	private ImageIcon agentListeningIcon;
	
	/* GUI Elements */
	private JFrame jframe;
	private JTextField input;
	private JEditorPane output;
	private JScrollPane scroller;
	private JLabel userSpeakingState;
	private JLabel agentSpeakingState;
	
	/* The text in the textArea */
	private String outputText = "++++++++ Welcome ++++++++";
	private String currUtterance = "";
	private long currUtteranceTime = 0;
	
	/* The last time a keypress was registered */
	public long lastTickTime = meta.getTime();
	
	/* A boolean to check if the GUI is started or not */
	private boolean started = false;
	
	/* A boolean to keep track if the user is typing or not */
	public boolean typing = false;
	
	/* Senders and Receivers */
	private FMLSender fmlSender;
	private StateSender userStateSender;
	private EmmaSender emmaSender;
	private FMLReceiver fmlReceiver;
	private FMLReceiver queueReceiver;
	private Receiver commandReceiver;
	//private EmmaReceiver emmaReceiver;
	private StateReceiver userStateReceiver;
	
	Random r = new Random();
	
	private HashMap<String,String> preparedResponses = new HashMap<String,String>();
	private ArrayList<String> lines = new ArrayList<String>();
	
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
		userStateSender = new StateSender("semaine.data.state.user.behaviour", StateInfo.Type.UserState, getName());
		senders.add(userStateSender);
		emmaSender = new EmmaSender("semaine.data.state.user.emma.words", "GUI");
		senders.add(emmaSender); // so it can be started etc
		//featureSender = new FeatureSender("semaine.data.analysis.features.voice", "", "GUI", 10);
		//String[] featureNames = new String[2];
		//featureNames[0] = "logEnergy";
		//featureNames[1] = "F0strength";
		//featureSender.setFeatureNames(featureNames);
		//senders.add(featureSender);
		
		// Temp
		fmlSender = new FMLSender("semaine.data.action.candidate.function", getName());
		senders.add(fmlSender);
		// Temp
		
		fmlReceiver = new FMLReceiver("semaine.data.action.selected.function");
		receivers.add(fmlReceiver);
		queueReceiver = new FMLReceiver("semaine.data.action.prepare.function");
		receivers.add(queueReceiver);
		commandReceiver = new Receiver("semaine.data.synthesis.lowlevel.command");
		receivers.add(commandReceiver);
		userStateReceiver = new StateReceiver("semaine.data.state.user.behaviour", StateInfo.Type.UserState);
		receivers.add(userStateReceiver);
		receivers.add( new StateReceiver("semaine.data.state.dialog", StateInfo.Type.DialogState) );
		
		initGui();
	}
	
	public void act()
	{
		if( !started && sendSpeechSignals ) {
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
		if( m.getTopicName().equals("semaine.data.synthesis.lowlevel.command") ) {
			if( m.getDatatype().equals("playCommand") ) {
				String id = m.getContentID();
				if( preparedResponses.get(id) != null ) {
					printLine("* " + preparedResponses.get(id) );
				}
			}
		}
		if ( m instanceof SEMAINEXMLMessage ) {
			SEMAINEXMLMessage xm = (SEMAINEXMLMessage)m;
			boolean isFML = "FML".equals(xm.getDatatype());
			if (isFML) {
				Element bml = XMLTool.getChildElementByTagNameNS(xm.getDocument().getDocumentElement(), BML.E_BML, BML.namespaceURI);
				if( bml != null ) {
					String id;
					if( bml.hasAttribute(BML.A_ID) ) {
						id = bml.getAttribute(BML.A_ID);
					} else {
						id = "Unknown";
					}
					Element speech = XMLTool.getChildElementByTagNameNS(bml, BML.E_SPEECH, BML.namespaceURI);
					if( speech != null ) {
						try {
							if( currUtterance.length() != 0 ) {
								outputText = outputText + "<br>" + currUtterance;
							}
							if( m.getTopicName().equals("semaine.data.action.selected.function") ) {
								currUtterance = "";
								currUtteranceTime = 0;
								printLine( "+ " + speech.getAttribute(BML.E_TEXT) );
							} else if( m.getTopicName().equals("semaine.data.action.prepare.function") ) {
								preparedResponses.put(id, speech.getAttribute(BML.E_TEXT));
							}
						}catch(Exception e){e.printStackTrace();}
						//printLine( "+ " + speech.getTextContent() );
					}
				}
				Element fml = XMLTool.getChildElementByTagNameNS(xm.getDocument().getDocumentElement(), FML.E_FML, FML.namespaceURI);
				if( fml != null ) {
					Element backchannel = XMLTool.getChildElementByTagNameNS(fml, FML.E_BACKCHANNEL, FML.namespaceURI);
//					if( backchannel != null ) {
//						if( currUtterance.length() != 0 ) {
//							outputText = outputText + "<br>" + currUtterance;
//						}
//						currUtterance = "";
//						currUtteranceTime = 0;
//						printLine("** Nods **");
//					}
				}
			}
		}
		
		if( m instanceof SEMAINEStateMessage ) {
			SEMAINEStateMessage sm = ((SEMAINEStateMessage)m);
			StateInfo stateInfo = sm.getState();
			if (stateInfo.getType() == StateInfo.Type.UserState) {
				if( stateInfo.hasInfo("userUtterance") && stateInfo.hasInfo("userUtteranceStartTime") ) {
					String newUtterance = stateInfo.getInfo("userUtterance");
					long newUtteranceTime = Long.parseLong(stateInfo.getInfo("userUtteranceStartTime"));
					//System.out.println("\r\n\r\n\r\n\r\n*********************************************************** "+newUtteranceTime+" ("+currUtteranceTime+")\r\n" + sm.getText());
					if( Math.abs(newUtteranceTime-currUtteranceTime) < 10 ) {
						currUtterance = newUtterance;
						lines.set(lines.size()-1, currUtterance);
					} else {
						//System.out.println("****** New Utterance");
						if( currUtterance.length() != 0 ) {
							outputText = outputText + "<br>" + currUtterance;
							//lines.add(currUtterance);
						}
						currUtterance = newUtterance;
						currUtteranceTime = newUtteranceTime;
						lines.add(currUtterance);
					}
					printLine();
				}
				if( stateInfo.hasInfo("speaking") ) {
					if( stateInfo.getInfo("speaking").equals("true") ) {
						userSpeakingState.setIcon(userSpeakingIcon);
					} else if( stateInfo.getInfo("speaking").equals("false") ) {
						userSpeakingState.setIcon(userListeningIcon);
					}
				}
				if( stateInfo.hasInfo("vocalization") ) {
					String voca = stateInfo.getInfo("vocalization");
					lines.add("<i>"+voca+"</i>");
					printLine();
				}
			}
			if (stateInfo.getType() == StateInfo.Type.DialogState) {
				if( stateInfo.hasInfo("agentTurnState") ) {
					if( stateInfo.getInfo("agentTurnState").equals("true") ) {
						agentSpeakingState.setIcon(agentSpeakingIcon);
					} else {
						agentSpeakingState.setIcon(agentListeningIcon);
					}
				}
			}
		}
	}

	/**
	 * Returns the detected sentence in the SEMAINEMessage m
	 * @param m the message to detect the sentence in
	 * @return the detected sentence
	 */
	public String getSentence( SEMAINEMessage m ) throws JMSException
	{
		String words = "";
		if( m instanceof SEMAINEEmmaMessage ) {
			SEMAINEEmmaMessage em = (SEMAINEEmmaMessage)m;
			
			/* Reading words */
			//System.out.println(em.getText());
			Element wordSequence = em.getSequence();
			if( wordSequence != null ) {
				//System.out.println("WordSequence found");
				List<Element> wordElements = XMLTool.getChildrenByLocalNameNS(wordSequence, EMMA.E_INTERPRETATION, EMMA.namespaceURI);
				if( wordElements.size() > 0 ) {
					//System.out.println("WordElements found");
					String starttime = null;
					for( Element wordElem : wordElements ) {
						if( wordElem.hasAttribute("tokens") ) {
							words = words + wordElem.getAttribute("tokens") + " ";
						}
					}
				}
			}
		}
		if( words.length() == 0 ) {
			return null;
		} else {
			return words.trim();
		}
	}
	
	/**
	 * Initializes the GUI
	 */
	public void initGui()
	{
		try {
			userSpeakingIcon = new ImageIcon(ImageIO.read(this.getClass().getResourceAsStream("/eu/semaine/components/dialogue/data/speaking.png")));
			userListeningIcon = new ImageIcon(ImageIO.read(this.getClass().getResourceAsStream("/eu/semaine/components/dialogue/data/silent.png")));
			agentSpeakingIcon = new ImageIcon(ImageIO.read(this.getClass().getResourceAsStream("/eu/semaine/components/dialogue/data/agentSpeaking.png")));
			agentListeningIcon = new ImageIcon(ImageIO.read(this.getClass().getResourceAsStream("/eu/semaine/components/dialogue/data/agentListening.png")));
		} catch( IOException e ) {
			e.printStackTrace();
		}
		
		
		/* Creates new Frame */
		jframe = new JFrame();
		jframe.setLayout( new BorderLayout() );
		//jframe.setPreferredSize(new Dimension(600,200));
		
		/* Create output textarea */
		output = new JEditorPane( "text/html", "<html><body>" + outputText + "</body></html>" );
		output.setEditable( false );
		output.setPreferredSize( new Dimension( 600, 200 ) );
		lines.add("++++++++ Welcome ++++++++");
		scroller = new JScrollPane (output);
		scroller.setPreferredSize( new Dimension( 600, 200 ) );
		jframe.add( scroller, BorderLayout.CENTER );
		//jframe.add( output, BorderLayout.CENTER );
		
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
		
		userSpeakingState = new JLabel(userListeningIcon);
		agentSpeakingState = new JLabel(agentListeningIcon);
		panel.add(userSpeakingState);
		panel.add(agentSpeakingState);
		
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
		lines.add(line);
		printLines();
		
		/*
		if( currUtterance.length() == 0 ) {
			output.setText( ("<html><body>" + outputText + "<br>" + line + "</body></html>").trim() );
		} else {
			output.setText( ("<html><body>" + outputText + "<br>" + currUtterance + "<br>" + line + "</body></html>").trim() );
		}
		output.setCaretPosition(output.getDocument().getLength());
		outputText = outputText + "<br>" + line;
		*/
		
		//JViewport view = scroller.getViewport();
		//view.setViewPosition(new java.awt.Point(10, 99999999));
	}
	
	/**
	 * This method prints the given line in the output textarea
	 * @param line - the line to print
	 */
	public void printLine( )
	{	
		printLines();
		
		/*
		if( currUtterance.length() == 0 ) {
			output.setText( ("<html><body>" + outputText + "</body></html>").trim() );
		} else {
			output.setText( ("<html><body>" + outputText + "<br>" + currUtterance + "</body></html>").trim() );
		}
		output.setCaretPosition(output.getDocument().getLength());
		*/
		
		//JViewport view = scroller.getViewport();
		//view.setViewPosition(new java.awt.Point(10, 99999999));
	}
	
	public void printLines()
	{
		String text = "<html><body>";
		int startIndex = Math.max(lines.size()-10, 0);
		for( int i=startIndex; i < lines.size(); i++ ) {
			text = text + lines.get(i) + "<br>";
		}
		text = text.substring(0, text.length()-4);
		text = text + "</body></html>";
		output.setText(text);
		output.setCaretPosition(output.getDocument().getLength());
	}
	
	/**
	 * Called when the user presses a key. Used for turn taking behaviour
	 */
	public void keyPressed()
	{
		lastTickTime = meta.getTime();
		if( !typing ) {
			typing = true;
			lastTickTime = meta.getTime();
			sendSpeaking();
		}
	}
	
	/**
	 * Sends the given utterance to the userstate using an EmmaMessage
	 * @param line
	 */
	public void sendUtterance( String line )
	{
//		try {
//			sendUtteranceToMary("Hello there, I'm Prudence, and I am very matter-of-fact. So tell me, what is your name?");
//		}catch(Exception e) {}
		/* If speech signals are not continuously send, send a speaking signal to indicate that an utterance has been spoken */
		if( !sendSpeechSignals ) {
			sendSpeaking();
		}
		
		String[] words = line.split(" ");
		Document document = XMLTool.newDocument(EMMA.ROOT_TAGNAME, EMMA.namespaceURI, EMMA.version);
		document.getDocumentElement().setPrefix("emma");
		Element sequence = XMLTool.appendChildElement(document.getDocumentElement(), EMMA.E_SEQUENCE );
		sequence.setAttribute(EMMA.A_OFFSET_TO_START, String.valueOf(meta.getTime()));
		sequence.setPrefix("emma");
		for( String word : words ) {
			Element interpretation = XMLTool.appendChildElement(sequence, EMMA.E_INTERPRETATION);
			interpretation.setPrefix("emma");
			interpretation.setAttribute(EMMA.A_OFFSET_TO_START, String.valueOf(meta.getTime()));
			interpretation.setAttribute(EMMA.A_TOKENS, word);
			interpretation.setAttribute(EMMA.A_CONFIDENCE, "1");
		}
		try {
			emmaSender.sendXML(document, meta.getTime(), Event.single);
		}catch( JMSException e ){}
		
		/* If speech signals are not continuously send, send a silence signal to indicate that the utterance is over */
		if( !sendSpeechSignals ) {
			sendSilent();
		}
	}
	
	public void sendSpeaking()
	{
		try {
			Map<String,String> userStateInfo = new HashMap<String,String>();
			userStateInfo.put("speaking", "true");
			UserStateInfo usi = new UserStateInfo(userStateInfo	);
			userStateSender.sendStateInfo(usi, meta.getTime());	
		} catch( JMSException e){ e.printStackTrace(); }
	}
	
	public void sendSilent()
	{
		try {
			Map<String,String> userStateInfo = new HashMap<String,String>();
			userStateInfo.put("speaking", "false");
			UserStateInfo usi = new UserStateInfo(userStateInfo	);
			userStateSender.sendStateInfo(usi, meta.getTime());	
		} catch( JMSException e){ e.printStackTrace(); }
	}
	
	public long getTime()
	{
		return meta.getTime();
	}
	
	/**
	 * Sends the given utterance to the output modules.
	 * 
	 * @param utterance
	 * @throws JMSException
	 */
	public void sendUtteranceToMary( String response ) throws JMSException
	{	
		String id = "s1";

		Document doc = XMLTool.newDocument("fml-apml", null, FML.version);
		Element root = doc.getDocumentElement();

		Element bml = XMLTool.appendChildElement(root, BML.E_BML, BML.namespaceURI);
		bml.setAttribute(BML.A_ID, "bml1");
		Element fml = XMLTool.appendChildElement(root, FML.E_FML, FML.namespaceURI);
		fml.setAttribute(FML.A_ID, "fml1");
		Element speech = XMLTool.appendChildElement(bml, BML.E_SPEECH);
		speech.setAttribute(BML.A_ID, id);
		speech.setAttribute(BML.E_TEXT, response);
		speech.setAttribute(BML.E_LANGUAGE, "en-GB");

		//speech.setTextContent(response);

		int counter=1;
		for( String word : response.split(" ") ) {
			Element mark = XMLTool.appendChildElement(speech, SSML.E_MARK, SSML.namespaceURI);
			mark.setAttribute(SSML.A_NAME, id+":tm"+counter);
			Node text = doc.createTextNode(word);
			speech.appendChild(text);
			counter++;
		}
		Element mark = XMLTool.appendChildElement(speech, SSML.E_MARK, SSML.namespaceURI);
		mark.setAttribute(SSML.A_NAME, id+":tm"+counter);

		fmlSender.sendXML(doc, meta.getTime());
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
			if( gui.lastTickTime + 200 < gui.getTime() && gui.typing ) {
				gui.typing = false;
				gui.sendSilent();
			}
		}
	}

	public void stopThread()
	{
		stop = true;
	}
}
