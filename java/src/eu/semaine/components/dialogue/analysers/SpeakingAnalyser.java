package eu.semaine.components.dialogue.analysers;

import java.util.ArrayList;

import javax.jms.JMSException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.components.dialogue.util.CyclicBuffer;
import eu.semaine.datatypes.xml.EMMA;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.jms.IOBase.Event;
import eu.semaine.jms.message.SEMAINEFeatureMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.FeatureReceiver;
import eu.semaine.jms.sender.EmmaSender;
import eu.semaine.util.XMLTool;

public class SpeakingAnalyser extends Component
{
	private FeatureReceiver featureReceiver;
	private EmmaSender userStateSender;
	private boolean userSpeaking = false;
	private CyclicBuffer<Float> logEnergyBuffer = new CyclicBuffer<Float>(20);
	private CyclicBuffer<Float> foStrengthBuffer = new CyclicBuffer<Float>(30);

	public SpeakingAnalyser() throws JMSException
	{
		super("SpeakingAnalyser");
		
		/* Define receivers */
		featureReceiver = new FeatureReceiver("semaine.data.analysis.features.voice");
		receivers.add(featureReceiver);
		
		/* Define senders */
		userStateSender = new EmmaSender("semaine.data.state.user", "SpeakingAnalyser");
		senders.add(userStateSender);
	}
	
	/**
	 * Checks if the user is speaking or silent
	 */
	@Override
	public void react( SEMAINEMessage m ) throws JMSException
	{
		if( m instanceof SEMAINEFeatureMessage ) {
			SEMAINEFeatureMessage fm = (SEMAINEFeatureMessage)m;
			String[] featureNames = fm.getFeatureNames();
			float[] featuresValues = fm.getFeatureVector();
			for( int i=0; i<featureNames.length; i++ ) {
				String name = featureNames[i];
				if( name.equals("logEnergy") ) {
					
					float value = featuresValues[i];
					logEnergyBuffer.putValue(value);
					ArrayList<Float> values = logEnergyBuffer.getValues();
					float avg = 0;
					for( Float f : values ) {
						if( f != null ) {
							avg = avg + f;
						}
					}
					avg = avg / values.size();
					if( userSpeaking ) {
						if( avg < -10 ) {
							userSpeaking = false;
							sendSilent();
							return;
						}
					} else {
						if( avg > -8 ) {
							userSpeaking = true;
							sendSpeaking();
							return;
						}
					}
					
				} else if( name.equals("F0strength") ) {
					float value = featuresValues[i];
					foStrengthBuffer.putValue(value);
					ArrayList<Float> values = foStrengthBuffer.getValues();
					
					int peakCount = 0;
					int peakSubCount = 0;
					float threshold;
					if( userSpeaking ) {
						threshold = 0.45f;
					} else {
						threshold = 0.55f;
					}
					for( Float f : values ) {
						if( f != null ) {
							if( f >= threshold ) {
								peakSubCount++;
								
							} else {
								if( peakSubCount >= 3 ) {
									peakCount++;
									peakSubCount = 0;
								}
							}
						}
					}
					if( peakCount >= 3 && !userSpeaking ) {
						userSpeaking = true;
						sendSpeaking();
						return;
					} else if( peakCount < 3 & userSpeaking ) {
						userSpeaking = false;
						sendSilent();
						return;
					}
				}
			}
		}
	}
	
	public void sendSpeaking() throws JMSException
	{
		Document document = XMLTool.newDocument(EMMA.E_EMMA, EMMA.namespaceURI, EMMA.version);
		Element interpretation = XMLTool.appendChildElement(document.getDocumentElement(), EMMA.E_INTERPRETATION);
		Element behaviour = XMLTool.appendChildElement(interpretation, SemaineML.E_BEHAVIOUR, SemaineML.namespaceURI);
		behaviour.setAttribute( SemaineML.A_NAME, "speaking" );
		userStateSender.sendXML(document, meta.getTime(), Event.single);
	}
	
	public void sendSilent() throws JMSException
	{
		Document document = XMLTool.newDocument(EMMA.E_EMMA, EMMA.namespaceURI, EMMA.version);
		Element interpretation = XMLTool.appendChildElement(document.getDocumentElement(), EMMA.E_INTERPRETATION);
		Element behaviour = XMLTool.appendChildElement(interpretation, SemaineML.E_BEHAVIOUR, SemaineML.namespaceURI);
		behaviour.setAttribute( SemaineML.A_NAME, "silence" );
		userStateSender.sendXML(document, meta.getTime(), Event.single);
	}
}
