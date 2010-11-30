package eu.semaine.components.visualisation;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;

import javax.jms.JMSException;
import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextArea;

import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.datatypes.xml.BML;
import eu.semaine.datatypes.xml.EMMA;
import eu.semaine.datatypes.xml.EmotionML;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.jms.message.SEMAINEEmmaMessage;
import eu.semaine.jms.message.SEMAINEFeatureMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEStateMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.EmmaReceiver;
import eu.semaine.jms.receiver.FeatureReceiver;
import eu.semaine.jms.receiver.StateReceiver;
import eu.semaine.util.XMLTool;

public class Visualisation extends Component
{
	private String[] audioFeatureNames = { "F0frequency", "voiceProb", "pitchDirection", "pitchDirectionScore", "RMSenergy", "loudness" };
	private String[] emotionNames = { "Valence", "Arousal", "Potency", "Unpredictability", "Interest" };
	
	private final static double THRESHOLD_ACTIONUNITS = 0.3;
	private static final float VALENCE_THRESHOLD = 0.2f;
	private static final float AROUSAL_THRESHOLD = 0.2f;
	private static final float POTENCY_THRESHOLD = 0.2f;
	private static final float UNPREDICTABILITY_THRESHOLD = 0.2f;
	
	private static int WIDTH = 840;
	private static int HEIGHT = 500;
	
	private int xResolution = 640;
	private int yResolution = 480;
	
	private ArrayList<AU> actionUnits = new ArrayList<AU>();
	private JTextArea detectedWords;
	private FacePanel facePanel;
	private Box2DPanel box2D;
	private JPanel AUPanel;
	private GraphPanel graphPanelAudio;
	private GraphPanel graphPanelEmotion1;
	private JComboBox audioChooser;
	private JComboBox emotionChooser1;
	
	private ArrayList<String> keywords = new ArrayList<String>();
	
	private JFrame jframe;
	
	public Visualisation() throws JMSException
	{
		super("InputVisualisation");
		createGui();
		
		receivers.add(new EmmaReceiver("semaine.data.state.user.emma.nonverbal.*"));
		receivers.add(new EmmaReceiver("semaine.data.state.user.emma.emotion.*"));
		receivers.add(new FeatureReceiver("semaine.data.analysis.features.voice"));
		receivers.add(new EmmaReceiver("semaine.data.state.user.emma.emotion"));
		receivers.add(new StateReceiver("semaine.data.state.user.behaviour", StateInfo.Type.UserState));
		receivers.add(new FeatureReceiver("semaine.data.analysis.features.video.facedetection"));
		receivers.add(new StateReceiver("semaine.data.state.system", StateInfo.Type.SystemState));
		receivers.add(new FeatureReceiver("semaine.data.analysis.features.video.2dheadmotion"));
	}
	
	public void createGui()
	{
		jframe = new JFrame("Semaine Visualisation");
		jframe.setPreferredSize(new Dimension(WIDTH, HEIGHT));
		jframe.setLayout(new BorderLayout());
		
		AUPanel = createAUPanel();		
		jframe.add(AUPanel, BorderLayout.NORTH);
		
		JPanel leftPanel = new JPanel();
		leftPanel.setPreferredSize(new Dimension(110,HEIGHT-70));
		leftPanel.add(new JLabel("Valence/Arousal"));
		box2D = new Box2DPanel("Valence", "Arousal", 100, 100, 3);
		leftPanel.add(box2D, BorderLayout.NORTH);
		leftPanel.add(new JLabel("Face position"));
		facePanel = new FacePanel(100,100);
		leftPanel.add(facePanel, BorderLayout.NORTH);
		JButton agentStateButton = new JButton("Agent State");
		//leftPanel.add(agentStateButton, BorderLayout.SOUTH);
		jframe.add(leftPanel, BorderLayout.WEST);
		
		JPanel rightPanel = new JPanel();
		rightPanel.setLayout(new FlowLayout());
		rightPanel.setPreferredSize(new Dimension(600,HEIGHT-70));
		detectedWords = new JTextArea();
		detectedWords.setPreferredSize(new Dimension(600,90));
		detectedWords.setText("User speech...");
		detectedWords.setLineWrap(true);
		detectedWords.setWrapStyleWord(true);
		//JScrollPane scroller = new JScrollPane (detectedWords, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
		rightPanel.add(detectedWords);
		
		JPanel audioFeaturePanel = new JPanel();		
		audioChooser = new JComboBox();
		audioChooser.setPreferredSize(new Dimension(100,20));
		for( String str : audioFeatureNames ) audioChooser.addItem(str);
		audioFeaturePanel.add(audioChooser);
		graphPanelAudio = new GraphPanel(audioChooser.getSelectedItem().toString());
		graphPanelAudio.setPreferredSize(new Dimension(600,105));
		audioFeaturePanel.add(graphPanelAudio);
		rightPanel.add(audioFeaturePanel);
		
		JPanel emotionPanel1 = new JPanel();
		ArrayList<Color> colours = new ArrayList<Color>();
		colours.add(Color.GREEN);
		colours.add(Color.RED);
		colours.add(Color.ORANGE);
		emotionChooser1 = new JComboBox();
		emotionChooser1.setPreferredSize(new Dimension(100,20));
		for( String str : emotionNames ) emotionChooser1.addItem(str);
		emotionPanel1.add(emotionChooser1);
		graphPanelEmotion1 = new GraphPanel(emotionChooser1.getSelectedItem().toString(), 3, colours);
		graphPanelEmotion1.setPreferredSize(new Dimension(600,105));
		emotionPanel1.add(graphPanelEmotion1);
		rightPanel.add(emotionPanel1);
		
		
		
//		graphPanelEnergy = new GraphPanel("RMS Energy");
//		graphPanelEnergy.setPreferredSize(new Dimension(600,105));
//		rightPanel.add(graphPanelEnergy);
		
		jframe.add(rightPanel, BorderLayout.CENTER);
		
		jframe.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		jframe.pack();
		jframe.setLocationRelativeTo(null);
		jframe.setResizable(false);
		jframe.setVisible(true);
	}
	
	public JPanel createAUPanel()
	{
		JPanel AUPanel = new JPanel();
		AUPanel.setPreferredSize(new Dimension(WIDTH, 60));
		
		actionUnits.add(new AU(1, "AU1_on.png", "AU1_off.png"));
		actionUnits.add(new AU(2, "AU2_on.png", "AU2_off.png"));
		actionUnits.add(new AU(4, "AU4_on.png", "AU4_off.png"));
		actionUnits.add(new AU(12, "AU12_on.png", "AU12_off.png"));
		actionUnits.add(new AU(15, "AU15_on.png", "AU15_off.png"));
		actionUnits.add(new AU(25, "AU25_on.png", "AU25_off.png"));
		actionUnits.add(new AU(27, "AU27_on.png", "AU27_off.png"));
		
//		AUPanel.add(new JLabel("Action Units"));
		for( AU au : actionUnits ) {
			AUPanel.add(au.jLabel);
		}
		
		
		return AUPanel;
	}
	
	@Override
    public void react(SEMAINEMessage m) throws Exception
    {
		if( !graphPanelAudio.name.equals(audioChooser.getSelectedItem().toString()) ) {
			graphPanelAudio.name = audioChooser.getSelectedItem().toString();
		}
		if( !graphPanelEmotion1.name.equals(emotionChooser1.getSelectedItem().toString()) ) {
			graphPanelEmotion1.name = emotionChooser1.getSelectedItem().toString();
		}
		if( m instanceof SEMAINEStateMessage && m.getTopicName().equals("semaine.data.state.system") ) {
			SEMAINEStateMessage sm = (SEMAINEStateMessage)m;
			StateInfo stateInfo = sm.getState();
			if( stateInfo.hasInfo("cameraXResolution") ) {
				xResolution = Integer.parseInt(stateInfo.getInfo("cameraXResolution"));
			}
			if( stateInfo.hasInfo("cameraYResolution") ) {
				yResolution = Integer.parseInt(stateInfo.getInfo("cameraYResolution"));
			}
		}
		if( m instanceof SEMAINEFeatureMessage && m.getTopicName().equals("semaine.data.analysis.features.video.facedetection") ) {
			SEMAINEFeatureMessage fm = (SEMAINEFeatureMessage)m;
			String[] names = fm.getFeatureNames();
			float[] values = fm.getFeatureVector();
			for( int i=0; i<names.length; i++ ) {
				if( names[i].equals("xPositioTopLeft") ) {
					facePanel.setXpos((int)((values[i]/((float)xResolution))*100));
				}
				if( names[i].equals("yPositionTopLeft") ) {
					facePanel.setYpos((int)((values[i]/((float)yResolution))*100));
				}
				if( names[i].equals("width") ) {
					facePanel.setFaceWidth((int)((values[i]/((float)xResolution))*100));
				}
				if( names[i].equals("height") ) {
					facePanel.setFaceHeight((int)((values[i]/((float)yResolution))*100));
				}
			}
			facePanel.repaint();
		}
		if( m instanceof SEMAINEFeatureMessage && m.getTopicName().equals("semaine.data.analysis.features.voice") ) {
			SEMAINEFeatureMessage fm = (SEMAINEFeatureMessage)m;
			String[] names = fm.getFeatureNames();
			float[] values = fm.getFeatureVector();
			for( int i=0; i<names.length; i++ ) {
				if( names[i].equals(audioChooser.getSelectedItem().toString()) ) {
					if( names[i].equals("F0frequency") ) {
						graphPanelAudio.addValue(values[i]/500);
					} else {
						graphPanelAudio.addValue(values[i]);
					}
				}
			}
			graphPanelEmotion1.addValue(0, 0);
			graphPanelEmotion1.addValue(0, 1);
			graphPanelEmotion1.addValue(0, 2);
		}
		if( m instanceof SEMAINEEmmaMessage && m.getTopicName().equals("semaine.data.state.user.emma.emotion") ) {
			//To Test
			float[] emotions = getEmotions((SEMAINEEmmaMessage)m);
			if( emotions[0] != 0 && emotions[1] != 0 ) {
				box2D.addPoint(emotions[0], emotions[1]);
				box2D.repaint();
			}
		}
		if( m instanceof SEMAINEEmmaMessage && m.getTopicName().contains("semaine.data.state.user.emma.emotion") ) {
			float[] emotions = getEmotions((SEMAINEEmmaMessage)m);
			if( emotions[0] != 0 ) {
				if( emotionChooser1.getSelectedItem().toString().equals("Valence") ) {
					if( m.getTopicName().equals("semaine.data.state.user.emma.emotion") ) graphPanelEmotion1.addValue(emotions[0], 0);
					if( m.getTopicName().equals("semaine.data.state.user.emma.emotion.face") ) graphPanelEmotion1.addValue(emotions[0], 1);
					if( m.getTopicName().equals("semaine.data.state.user.emma.emotion.voice") ) graphPanelEmotion1.addValue(emotions[0], 2);
				}
			}
			if( emotions[1] != 0 ) {
				if( emotionChooser1.getSelectedItem().toString().equals("Arousal") ) {
					if( m.getTopicName().equals("semaine.data.state.user.emma.emotion") ) graphPanelEmotion1.addValue(emotions[0], 0);
					if( m.getTopicName().equals("semaine.data.state.user.emma.emotion.face") ) graphPanelEmotion1.addValue(emotions[0], 1);
					if( m.getTopicName().equals("semaine.data.state.user.emma.emotion.voice") ) graphPanelEmotion1.addValue(emotions[0], 2);
				}
			}
			if( emotions[2] != 0 ) {
				if( emotionChooser1.getSelectedItem().toString().equals("Potency") ) {
					if( m.getTopicName().equals("semaine.data.state.user.emma.emotion") ) graphPanelEmotion1.addValue(emotions[0], 0);
					if( m.getTopicName().equals("semaine.data.state.user.emma.emotion.face") ) graphPanelEmotion1.addValue(emotions[0], 1);
					if( m.getTopicName().equals("semaine.data.state.user.emma.emotion.voice") ) graphPanelEmotion1.addValue(emotions[0], 2);
				}
			}
			if( emotions[3] != 0 ) {
				if( emotionChooser1.getSelectedItem().toString().equals("Unpredictability") ) {
					if( m.getTopicName().equals("semaine.data.state.user.emma.emotion") ) graphPanelEmotion1.addValue(emotions[0], 0);
					if( m.getTopicName().equals("semaine.data.state.user.emma.emotion.face") ) graphPanelEmotion1.addValue(emotions[0], 1);
					if( m.getTopicName().equals("semaine.data.state.user.emma.emotion.voice") ) graphPanelEmotion1.addValue(emotions[0], 2);
				}
			}
			if( emotions[4] != 0 ) {
				if( emotionChooser1.getSelectedItem().toString().equals("Interest") ) {
					if( m.getTopicName().equals("semaine.data.state.user.emma.emotion") ) graphPanelEmotion1.addValue(emotions[0], 0);
					if( m.getTopicName().equals("semaine.data.state.user.emma.emotion.face") ) graphPanelEmotion1.addValue(emotions[0], 1);
					if( m.getTopicName().equals("semaine.data.state.user.emma.emotion.voice") ) graphPanelEmotion1.addValue(emotions[0], 2);
				}
			}
		}
		if( m instanceof SEMAINEEmmaMessage && m.getTopicName().equals("semaine.data.state.user.emma.nonverbal.face") ) {
			SEMAINEEmmaMessage em = (SEMAINEEmmaMessage)m;
			System.out.println(m.getText());
			Element interpretation = em.getTopLevelInterpretation();
			ArrayList<Integer> AU_on = new ArrayList<Integer>();
			if( interpretation != null ) {
			
				double confidence;
				if( interpretation.hasAttribute(EMMA.A_CONFIDENCE) ) {
					confidence = Double.parseDouble(interpretation.getAttribute(EMMA.A_CONFIDENCE));
				} else {
					return;
				}
				
				/* Process single Action Unit */
				List<Element> bmlList = em.getBMLElements(interpretation);
				if( bmlList.size() > 0 ) {
					HashSet<Integer> auList = new HashSet<Integer>();
					for( Element bml : bmlList ) {
						Element face = XMLTool.getChildElementByTagNameNS(bml, BML.E_FACE, BML.namespaceURI);
						if( face != null && face.hasAttribute("au") ) {
							if( confidence >= THRESHOLD_ACTIONUNITS ) {
								AU_on.add(Integer.parseInt(face.getAttribute("au").replaceAll("au", "")));
							}
						}
					}
				}
			}
			
			/* Process Group of Action Units */
			Element group = em.getGroup();
			if( group != null ) {
				HashSet<Integer> auList = new HashSet<Integer>();
				String time = ""+meta.getTime();
				
				List<Element> interpretationList = XMLTool.getChildrenByLocalNameNS(group, EMMA.E_INTERPRETATION, EMMA.namespaceURI);
				for( Element inter : interpretationList ) {
					double conf = 0;
					if( inter.hasAttribute(EMMA.A_OFFSET_TO_START) ) {
						time = inter.getAttribute(EMMA.A_OFFSET_TO_START);
					}
					if( inter.hasAttribute(EMMA.A_CONFIDENCE) ) {
						conf = Double.parseDouble(inter.getAttribute(EMMA.A_CONFIDENCE));
					}
					List<Element> bmlList = em.getBMLElements(inter);
					for( Element bml : bmlList ) {
						Element face = XMLTool.getChildElementByTagNameNS(bml, BML.E_FACE, BML.namespaceURI);
						if( face != null && face.hasAttribute("au") ) {
							if( conf >= THRESHOLD_ACTIONUNITS ) {
								AU_on.add(Integer.parseInt(face.getAttribute("au").replaceAll("au", "")));
							}
						}
					}
				}
			}
			for( AU au : actionUnits ) {
				au.setOn(AU_on.contains(new Integer(au.auNumber)));
			}
			AUPanel.repaint();
		}
		if( m instanceof SEMAINEStateMessage ) {
			SEMAINEStateMessage sm = ((SEMAINEStateMessage)m);
			StateInfo stateInfo = sm.getState();
			if (stateInfo.getType() == StateInfo.Type.UserState) {
				if( stateInfo.hasInfo("userUtterance") && stateInfo.hasInfo("userUtteranceStartTime") ) {
					String keywordString = stateInfo.getInfo("userUtterance");
					keywords.add(keywordString);
					if( keywords.size() > 5 ) {
						keywords.remove(0);
					}
					String words = "";
					for( String str : keywords ) {
						words = words + "\n" + str;
					}
					words = words.substring(1);
					detectedWords.setText(words);
				}
			}
		}
    }
	
	/**
	 * Retrieves the emotions from the given Message
	 * @param m - the message to read
	 * @return	- the values for the emotions in this order: [valence, arousal, potency, unpredictability, interest]
	 */
	public float[] getEmotions( SEMAINEEmmaMessage em ) throws JMSException
	{
		// valence, arousal, interest (in this order)
		float[] emotions = new float[5];
		emotions[0] = 0;
		emotions[1] = 0;
		emotions[2] = 0;
		emotions[3] = 0;
		emotions[4] = 0;
		boolean emotionsChanged = false;

		//System.out.println(em.getText());
		/* Reading valence and arousal */
		Element interpretation = XMLTool.getChildElementByLocalNameNS(em.getDocument().getDocumentElement(), EMMA.E_INTERPRETATION, EMMA.namespaceURI);
		if( interpretation != null ) {
			List<Element> emotionElements = em.getEmotionElements(interpretation);
			for( Element emotion : emotionElements ) {
				List<Element> dimensions = XMLTool.getChildrenByLocalNameNS(emotion, EmotionML.E_DIMENSION, EmotionML.namespaceURI);
				for (Element dim : dimensions) {
					String name = dim.getAttribute(EmotionML.A_NAME);
					float value = Float.parseFloat(dim.getAttribute(EmotionML.A_VALUE));
					try {
						if (name.equals(EmotionML.VOC_FSRE_DIMENSION_VALENCE)) {
							if( Float.parseFloat(dim.getAttribute(EmotionML.A_CONFIDENCE)) >= VALENCE_THRESHOLD ) {
								emotions[0] = EmotionML.fsreValence2SemaineValence(value);
								emotionsChanged = true;
							}
						} else if (name.equals(EmotionML.VOC_FSRE_DIMENSION_AROUSAL)) {
							if( Float.parseFloat(dim.getAttribute(EmotionML.A_CONFIDENCE)) >= AROUSAL_THRESHOLD ) {
								emotions[1] = EmotionML.fsreArousal2SemaineArousal(value);
								emotionsChanged = true;
							}
						}  else if (name.equals(EmotionML.VOC_FSRE_DIMENSION_POTENCY)) {
							if( Float.parseFloat(dim.getAttribute(EmotionML.A_CONFIDENCE)) >= POTENCY_THRESHOLD ) {
								emotions[2] = EmotionML.fsrePotency2SemainePower(value);
								emotionsChanged = true;
							}
						}  else if (name.equals(EmotionML.VOC_FSRE_DIMENSION_UNPREDICTABILITY)) {
							if( Float.parseFloat(dim.getAttribute(EmotionML.A_CONFIDENCE)) >= UNPREDICTABILITY_THRESHOLD ) {
								emotions[3] = EmotionML.fsreUnpredictability2SemaineExpectation(value);
								emotionsChanged = true;
							}
						}
					}catch( NumberFormatException e ) {
						
					}
				}
			}
		}

		/* Reading interest */
		Element oneOf = em.getOneOf();
		if(oneOf != null) {
			List<Element> interpretations = XMLTool.getChildrenByLocalNameNS(oneOf, EMMA.E_INTERPRETATION, EMMA.namespaceURI);
			if( interpretations.size() > 0 ) {
				float bored = 0;
				float neutral = 0;
				float interested = 0;
				for( Element interpretationElem : interpretations ) {
					List<Element> emotionElements = em.getEmotionElements(interpretationElem);
					for( Element emotion : emotionElements ) {
						Element category = XMLTool.getChildElementByLocalNameNS(emotion, EmotionML.E_CATEGORY, EmotionML.namespaceURI);
						if( category != null && category.hasAttribute(EmotionML.A_NAME) ) {
							String cat = category.getAttribute(EmotionML.A_NAME);
							if( cat.equals("bored") ) {
								bored = Float.parseFloat( interpretationElem.getAttribute(EMMA.A_CONFIDENCE) );
							} else if( cat.equals("neutral") ) {
								neutral = Float.parseFloat( interpretationElem.getAttribute(EMMA.A_CONFIDENCE) );
							} else if( cat.equals("interested") ) {
								interested = Float.parseFloat( interpretationElem.getAttribute(EMMA.A_CONFIDENCE) );
							}
						}
					}
				}
				/* Calculate combined interest-value */
				/*     ------- OLD CODE removed by Florian: emotions[3] = -1*bored + interested; */
				if (  (( bored == 0.0 ) && (neutral == 0.0) && (interested >= 0.99))  
						||(( bored == 0.0 ) && (neutral >= 0.99) && (interested == 0.00))
						||(( bored >= 0.99 ) && (neutral >= 0.00) && (interested == 0.00)) ) {
					// the above cases indicate an invalid classifier output, in this case we assume neutral interest...
					emotions[4] = 0;
				} else {
					// a continuous emotion value is computed from the class probabilities:
					emotions[4] = ( 1 * bored + 2 * neutral + 3 * interested ) - 2;
				}
				/*     ------------ */

				emotionsChanged = true;
			}
		}
		
		//System.out.println("***** " + emotions[0] + " " + emotions[1] + " " + emotions[2] + " " + emotions[3]);
		return emotions;
	}
	
	public static void main( String[] args )
	{
		try {
			new Visualisation();
		}catch( JMSException e ){e.printStackTrace();}
	}
	
	private class AU
	{
		private int auNumber;
		private ImageIcon icon;
		private ImageIcon greyIcon;
		private boolean isOn = false;
		protected JLabel jLabel;
		
		public AU( int auNumber, String iconFile, String greyIconFile )
		{
			this.auNumber = auNumber;
			icon = new ImageIcon(getClass().getResource("/eu/semaine/components/visualisation/"+iconFile));
			greyIcon = new ImageIcon(getClass().getResource("/eu/semaine/components/visualisation/"+greyIconFile));
			jLabel = new JLabel(greyIcon);
		}
		
		public void setOn( boolean newStatus )
		{
			if( newStatus && !isOn ) {
				isOn = newStatus;
				jLabel.setIcon(icon);
			} else if( !newStatus && isOn ) {
				isOn = newStatus;
				jLabel.setIcon(greyIcon);
			}
		}
	}
}
