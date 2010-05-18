/* @author Dennis Hofs - Modified for Semaine by Mark ter Maat
 */
// Last modification by: $Author: hofs $
// $Log: InteractionFrame.java,v $
// Revision 1.12  2007/05/30 07:45:29  hofs
// Finished plugin implementation
//
// Revision 1.11  2006/08/29 11:07:48  hofs
// Updates for Java 6
//
// Revision 1.10  2006/08/03 07:00:56  hofs
// no message
//
// Revision 1.9  2006/07/18 14:27:44  hofs
// no message
//
// Revision 1.8  2006/07/18 08:52:26  hofs
// Added mechanism to wait until transmitted stream has arrived at the other side
//
// Revision 1.7  2006/04/11 09:35:54  hofs
// *** empty log message ***
//
// Revision 1.6  2006/02/23 13:32:25  hofs
// Media socket updates
//
// Revision 1.5  2005/11/28 14:46:22  hofs
// *** empty log message ***
//
// Revision 1.4  2005/11/24 13:16:14  hofs
// *** empty log message ***
//
// Revision 1.3  2005/11/09 12:52:25  hofs
// Added counters
//
// Revision 1.2  2005/11/09 09:45:05  hofs
// Improved layout
//
// Revision 1.1  2005/11/08 09:03:41  hofs
// Initial version
//
// Revision 1.7  2005/10/26 08:16:16  hofs
// Cleaned up code
//
// Revision 1.6  2005/10/24 13:25:10  hofs
// errorOccurred -> serverErrorOccurred
//
// Revision 1.5  2005/10/06 12:35:03  hofs
// Supports WavPlayer
//
// Revision 1.4  2005/09/29 15:12:56  hofs
// *** empty log message ***
//
// Revision 1.3  2005/09/29 11:26:36  hofs
// *** empty log message ***
//

package eu.semaine.components.dialogue.woz;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Frame;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ComponentAdapter;
import java.awt.event.ComponentEvent;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.Reader;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.List;
import java.util.Map;

import javax.jms.JMSException;
import javax.swing.AbstractAction;
import javax.swing.Action;
import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JSplitPane;
import javax.swing.JTextPane;
import javax.swing.text.html.HTMLDocument;
import javax.swing.text.html.HTMLEditorKit;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;

import eu.semaine.components.Component;
import eu.semaine.datatypes.stateinfo.ContextStateInfo;
import eu.semaine.datatypes.stateinfo.DialogStateInfo;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.datatypes.xml.BML;
import eu.semaine.datatypes.xml.FML;
import eu.semaine.datatypes.xml.SSML;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.XMLReceiver;
import eu.semaine.jms.sender.FMLSender;
import eu.semaine.jms.sender.StateSender;
import eu.semaine.util.XMLTool;

public class WOzComponent extends Component implements PhraseLabelListener	
{
	private FMLSender fmlSender;
	private StateSender contextSender;
	private StateSender dialogStateSender;
	private XMLReceiver callbackReceiver;
	
	private String WOZ_FILE = "/eu/semaine/components/dialogue/data/woz_sentences.xml"; 
	
	private static final String GOTO_BOX = "goto";
	private static final String STATE_BOX = "state";
	private static final String SPEAKER_BOX = "speaker";
	private static final String RECEIVER_BOX = "receiver_box";
	private static final String TRANSMITTER_BOX = "transmitter";
	private static final String PLUGIN_BOX = "plugin";
	
	private JFrame jframe;
	private BoxPanel controlPanel;
	private JPanel gotoPanel;
	private JPanel statePanel;
	private JPanel speakerPanel;
	private JPanel pluginPanel;
	private JLabel interactLabel;
	private JPanel interactPanel;
	private Insets margins;
	
	private JScrollPane textScroller = null;
	private PhraseLabel playingLabel = null;
	private SalProject project = null;
	private String currentSpeaker = null;
	
	private Object mediaMonitor = new Object();
	
	private Map<String,PhrasesPanel> speakerStartup;
	private Map<String,PhrasesPanel> speakerRepair;
	private Map<String,Map<String,PhrasesPanel>> speakerStatePhrases;
	
	private boolean started = false;
	
	public WOzComponent() throws JMSException
	{
		super("WOzComponent");
		
		Object obj = System.getProperties().get("semaine.dialogue.wozfile");
		if( obj != null ) {
			WOZ_FILE = obj.toString();
		} else {
			WOZ_FILE = "/eu/semaine/components/dialogue/data/woz_sentences.xml";
		}
		
		fmlSender = new FMLSender("semaine.data.action.candidate.function", getName());
		senders.add(fmlSender);
		contextSender = new StateSender("semaine.data.state.context", StateInfo.Type.ContextState, getName());
		senders.add(contextSender);
		dialogStateSender = new StateSender("semaine.data.state.dialog", StateInfo.Type.DialogState, getName());
		senders.add(dialogStateSender);
		
		callbackReceiver = new XMLReceiver("semaine.callback.output.audio");
		receivers.add(callbackReceiver);
		
		speakerStartup = new Hashtable<String,PhrasesPanel>();
		speakerRepair = new Hashtable<String,PhrasesPanel>();
		speakerStatePhrases = new Hashtable<String,Map<String,PhrasesPanel>>();
		createGui();
		margins = new Insets(10,320,40,10);
	}
	
	public void act()
	{
		if( !started ) {
			started = true;
			loadProject();
		}
	}
	
	public void react( eu.semaine.jms.message.SEMAINEMessage m ) throws JMSException
	{
		if( m instanceof SEMAINEXMLMessage ){
			SEMAINEXMLMessage xm = ((SEMAINEXMLMessage)m);
			
			if( !xm.getText().contains("fml_lip") ) {
				if( speechReady(xm) ) {
					sendListening();
				}
			}
		}
	}
	
	/**
	 * This method checks if the given message contains the end signal of the animation that the agent
	 * is playing.
	 * @param xm the message to check
	 * @return true if the message contains the end signal, false if it does not.
	 */
	public boolean speechReady( SEMAINEXMLMessage xm ) throws JMSException
	{
		Element callbackElem = XMLTool.getChildElementByLocalNameNS(xm.getDocument(), "callback", SemaineML.namespaceURI);
		if( callbackElem != null ) {
			Element eventElem = XMLTool.getChildElementByLocalNameNS(callbackElem, "event", SemaineML.namespaceURI);
			if( eventElem != null ) {
				if( eventElem.hasAttribute("type") && eventElem.getAttribute("type").equals("end") ) {
					return true;
				}
			}
		}
		return false;
	}
	
	public void createGui()
	{
		jframe = new JFrame("WOz");
		jframe.setBounds(10,10,760,560);
		jframe.setExtendedState(Frame.MAXIMIZED_BOTH);
		
		controlPanel = new BoxPanel();
		controlPanel.setPreferredSize(new Dimension(300,1));
		
		gotoPanel = new JPanel();
		gotoPanel.setBackground(Color.WHITE);
		controlPanel.addBox(GOTO_BOX,"Go to",gotoPanel);
		
		statePanel = new JPanel();
		statePanel.setBackground(Color.WHITE);
		controlPanel.addBox(STATE_BOX,"Change state to",statePanel);
		
		speakerPanel = new JPanel();
		speakerPanel.setBackground(Color.WHITE);
		controlPanel.addBox(SPEAKER_BOX,"Change speaker to",speakerPanel);
		
		controlPanel.addBox(RECEIVER_BOX,"Respondent media",null);
		controlPanel.addBox(TRANSMITTER_BOX,"Operator media",null);
		
		pluginPanel = new JPanel();
		controlPanel.addBox(PLUGIN_BOX, "Plugins", pluginPanel);
		
		JPanel interactBox = new JPanel();
		interactBox.setBorder(BorderFactory.createCompoundBorder(
				BorderFactory.createEtchedBorder(),
				BorderFactory.createEmptyBorder(8,8,8,8)));
		interactBox.setLayout(new BorderLayout(8,8));
		interactBox.setBackground(Color.WHITE);
		interactLabel = new JLabel();
		interactLabel.setBorder(BorderFactory.createEmptyBorder(0,8,0,8));
		interactLabel.setOpaque(true);
		interactLabel.setFont(new Font("SansSerif",Font.BOLD,24));
		interactLabel.setBackground(Color.BLACK);
		interactLabel.setForeground(Color.WHITE);
		interactBox.add(interactLabel,BorderLayout.NORTH);
		interactPanel = new JPanel();
		interactPanel.setBackground(Color.WHITE);
		interactPanel.addComponentListener(new ComponentAdapter() {
			public void componentResized(ComponentEvent e) {
				//setInteractPanelSizes();
			}
		});
		interactBox.add(interactPanel,BorderLayout.CENTER);

		JPanel leftPanel = new JPanel(new BorderLayout(8,8));
		leftPanel.setBorder(BorderFactory.createEmptyBorder(8,8,8,8));
		leftPanel.add(controlPanel,BorderLayout.CENTER);
		JPanel rightPanel = new JPanel(new BorderLayout(8,8));
		rightPanel.setBorder(BorderFactory.createEmptyBorder(8,8,8,8));
		rightPanel.add(interactBox,BorderLayout.CENTER);
		JSplitPane splitter = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT,true,
				leftPanel,rightPanel);
		jframe.getContentPane().add(splitter);
		
		/* Finishes the frame */
		jframe.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		//jframe.pack();
		jframe.setLocationRelativeTo(null);
		jframe.setResizable(true);
		jframe.setVisible(true);		
	}
	
	private SalProject getProject()
	{
		// TODO: Mooier maken, niet hardcoded
		InputStream is = this.getClass().getResourceAsStream(WOZ_FILE);
		//File file = new File("sal_project.xml");
		SalProject project = null;
		try {
			 project = SalProject.read( is );
		}catch( Exception e ) {
			e.printStackTrace();
		}
		return project;
	}
	
	public void loadProject() {
		project = getProject();
		//project = server.getProject();
		statePanel.removeAll();
		List<List<String>> stateMatrix = project.getStateMatrix();
		int height = stateMatrix.size();
		int width = 0;
		for (List<String> row : stateMatrix) {
			if (row.size() > width)
				width = row.size();
		}
		
		List<String> states = new ArrayList<String>();
		statePanel.setLayout(new GridLayout(height,width,8,8));
		for (List<String> row : stateMatrix) {
			int x = 0;
			for (String state : row) {
				statePanel.add(new JButton(new StateAction(state)));
				states.add(state);
				x++;
			}
			for (int i = x; i < width; i++) {
				statePanel.add(new JPanel());
			}
		}

		speakerPanel.removeAll();
		List<String> speakers = project.getSpeakers();
		speakerPanel.setLayout(new GridLayout(speakers.size(),1,8,8));
		for (String speaker : speakers) {
			speakerPanel.add(new JButton(new SpeakerAction(speaker)));
		}
		
		speakerStartup.clear();
		speakerRepair.clear();
		speakerStatePhrases.clear();
		for (String speaker : project.getSpeakers()) {
			PhrasesPanel panel = new PhrasesPanel(1);
			panel.addPhraseLabelListener(this);
			panel.addPhrases("Startup",project.getSpeakerStartup(speaker));
			speakerStartup.put(speaker,panel);
			panel = new PhrasesPanel(1);
			panel.addPhraseLabelListener(this);
			panel.addPhrases("Repair",project.getSpeakerRepair(speaker));
			speakerRepair.put(speaker,panel);
			Map<String,PhrasesPanel> statePhrases = new Hashtable<String,PhrasesPanel>();
			speakerStatePhrases.put(speaker,statePhrases);
			for (String state : states) {
				panel = new PhrasesPanel(2);
				panel.addPhraseLabelListener(this);
				List<SalProject.StatePhrases> groups = project.getStatePhrases(
						speaker,state);
				for (SalProject.StatePhrases group : groups) {
					panel.addPhrases(group.getCategory(),group.getPhrases());
				}
				statePhrases.put(state,panel);
			}
		}
		
		loadStartup();
	}
	
	private void loadStartup() {
		currentSpeaker = null;
		gotoPanel.removeAll();
		gotoPanel.setLayout(new GridLayout(1,1,8,8));
		JButton btn = new JButton("<html><center>Introduction</center></html>");
		btn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				loadIntroduction();
			}
		});
		gotoPanel.add(btn);
		controlPanel.setBoxVisible(GOTO_BOX,true);
		controlPanel.setBoxVisible(STATE_BOX,false);
		interactLabel.setText(project.getWelcomeTitle());
		loadHtmlText(project.getWelcomeText());
		btn.requestFocus();
	}
	
	private void loadIntroduction() {
		currentSpeaker = null;
		gotoPanel.removeAll();
		gotoPanel.setLayout(new GridLayout(1,1,8,8));
		JButton btn = new JButton("<html><center>Startup</center></html>");
		btn.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent e) {
				loadStartup();
			}
		});
		gotoPanel.add(btn);
		controlPanel.setBoxVisible(GOTO_BOX,true);
		controlPanel.setBoxVisible(STATE_BOX,false);
		interactLabel.setText(project.getIntroTitle());
		loadHtmlText(project.getIntroText());
		btn.requestFocus();
	}
	
	private void layoutPhrasePanels(PhrasesPanel main, PhrasesPanel repair) {
		interactPanel.removeAll();
		interactPanel.setLayout(new BorderLayout());
		JScrollPane scroller = new JScrollPane(main);
		scroller.setBorder(BorderFactory.createEmptyBorder());
		interactPanel.add(scroller,BorderLayout.CENTER);
		interactPanel.add(repair,BorderLayout.SOUTH);
		interactPanel.repaint();
	}
	
	public void loadSpeaker(String speaker)
	{
		if (currentSpeaker != null && currentSpeaker.equals(speaker))
			return;
		currentSpeaker = speaker;
		// TODO: Does nothing I think
		// server.selectCharacter(speaker);
		interactLabel.setText(project.getSpeakerDescription(speaker) +
				" - startup");
		controlPanel.setBoxVisible(GOTO_BOX,false);
		controlPanel.setBoxVisible(STATE_BOX,true);
		layoutPhrasePanels(speakerStartup.get(speaker), speakerRepair.get(speaker));
		
		Map<String,String> contextInfo = new HashMap<String,String>();
		if( speaker.equals("poppy") ) speaker = "Poppy";
		if( speaker.equals("spike") ) speaker = "Spike";
		if( speaker.equals("obadiah") ) speaker = "Obadiah";
		if( speaker.equals("prudence") ) speaker = "Prudence";
		contextInfo.put("character", speaker);

		try {
			ContextStateInfo csi = new ContextStateInfo(contextInfo);
			contextSender.sendStateInfo( csi, meta.getTime() );
		}catch( JMSException e ) {
			System.out.println("Failed to send speaker-change.");
			e.printStackTrace();
		}
	}
	
	public void loadSpeakerState(String state) {
		// TODO: Does nothing I think
		// server.selectRespondentState(state);
		interactLabel.setText(project.getSpeakerDescription(currentSpeaker) +
				" - " + project.getStateDescription(state));
		controlPanel.setBoxVisible(GOTO_BOX,false);
		controlPanel.setBoxVisible(STATE_BOX,true);
		layoutPhrasePanels(speakerStatePhrases.get(currentSpeaker).get(state),
				speakerRepair.get(currentSpeaker));
	}
	
	private void loadHtmlText(String body) {
		interactPanel.removeAll();
		GridBagLayout gridbag = new GridBagLayout();
		GridBagConstraints c = new GridBagConstraints();
		c.gridwidth = GridBagConstraints.REMAINDER;
		c.fill = GridBagConstraints.VERTICAL;
		c.anchor = GridBagConstraints.WEST;
		c.weightx = 1.0;
		c.weighty = 1.0;
		interactPanel.setLayout(gridbag);
		HTMLEditorKit htmlKit = new HTMLEditorKit();
		HTMLDocument doc = (HTMLDocument)htmlKit.createDefaultDocument();
		doc.getStyleSheet().addRule("body { font-family: sans-serif }");
		Reader html = new StringReader("<html><body>" + body + "</body></html>");
		try {
			htmlKit.read(html,doc,0);
		} catch (Exception ex) {
			/*JOptionPane.showMessageDialog(this,
					"Could not read HTML: " + ex.getMessage(),
					"Error",JOptionPane.ERROR_MESSAGE);*/
			ex.printStackTrace();
		}
		JTextPane textPane = new JTextPane();
		textPane.setEditorKit(htmlKit);
		textPane.setDocument(doc);
		textPane.setEditable(false);
		textScroller = new JScrollPane(textPane);
		textScroller.setBorder(BorderFactory.createEmptyBorder());
		textScroller.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
		gridbag.setConstraints(textScroller,c);
		interactPanel.add(textScroller);
		setInteractPanelSizes();
	}
	
	private void setInteractPanelSizes() {
		if (textScroller != null) {
			int prefWidth = 500;
			if (interactPanel.getWidth() < prefWidth)
				prefWidth = interactPanel.getWidth();
			textScroller.setMinimumSize(new Dimension(prefWidth,0));
			textScroller.setPreferredSize(new Dimension(prefWidth,0));
			interactPanel.doLayout();
		}
	}
	
	public void phraseClicked(PhraseLabel label) {
		if (playingLabel != null) {
			playingLabel.playerStopped();
			playingLabel = null;
		}
		try {
			sendUtterance( label.getText() );
		}catch( JMSException e ) {
			e.printStackTrace();
		}
	}
	
	/**
	 * Sends the given utterance to the output modules.
	 * 
	 * @param utterance
	 * @throws JMSException
	 */
	public void sendUtterance( String response ) throws JMSException
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
		//speech.setAttribute("voice", "activemary");

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
		
		/* Send the speaking-state around */
		sendSpeaking();
	}
	
	/**
	 * Sends around that the agent is speaking
	 * @throws JMSException
	 */
	public void sendSpeaking() throws JMSException
	{
		Map<String,String> dialogInfo = new HashMap<String,String>();		
		dialogInfo.put("agentTurnState", "true");

		DialogStateInfo dsi = new DialogStateInfo(dialogInfo, null);
		dialogStateSender.sendStateInfo(dsi, meta.getTime());
	}
	
	/**
	 * Sends around that the agent is silent
	 * @throws JMSException
	 */
	public void sendListening() throws JMSException
	{
		Map<String,String> dialogInfo = new HashMap<String,String>();		
		dialogInfo.put("agentTurnState", "false");

		DialogStateInfo dsi = new DialogStateInfo(dialogInfo, null);
		dialogStateSender.sendStateInfo(dsi, meta.getTime());
	}
	
	/*private void applyMargins() {
		setBounds(margins.left,margins.top,
				jframe.getWidth()-margins.left-margins.right,
				jframe.getHeight()-margins.top-margins.bottom);
	}
	
	private void updateMargins() {
		margins = new Insets(getY(),getX(),
				jframe.getHeight()-getY()-getHeight(),
				jframe.getWidth()-getX()-getWidth());
	}*/
	
	private class SpeakerAction extends AbstractAction {
		private static final long serialVersionUID = 0;
		private String speaker;
		
		public SpeakerAction(String speaker) {
			String text = project.getSpeakerButton(speaker);
			text = "<html><center>" + text + "</center></html>";
			putValue(Action.NAME,text);
			this.speaker = speaker;
		}
		
		public void actionPerformed(ActionEvent e) {
			loadSpeaker(speaker);
		}
	}

	private class StateAction extends AbstractAction {
		private static final long serialVersionUID = 0;
		private String state;
		
		public StateAction(String state) {
			String text = project.getStateButton(state);
			text = "<html><center>" + text + "</center></html>";
			putValue(Action.NAME,text);
			this.state = state;
		}
		
		public void actionPerformed(ActionEvent e) {
			loadSpeakerState(state);
		}
	}
}
