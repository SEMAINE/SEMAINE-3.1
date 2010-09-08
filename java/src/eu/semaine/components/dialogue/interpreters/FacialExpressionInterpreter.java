/**
 * Copyright (C) 2009 University of Twente. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */

package eu.semaine.components.dialogue.interpreters;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.jms.JMSException;

import eu.semaine.components.Component;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.datatypes.stateinfo.UserStateInfo;
import eu.semaine.jms.message.SEMAINEFeatureMessage;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.FeatureReceiver;
import eu.semaine.jms.sender.StateSender;

public class FacialExpressionInterpreter extends Component
{
	private static int WINDOW_SIZE = 500;
	public static double THRESHOLD = 0.6;
	
	private FeatureReceiver facsReceiver;
	private StateSender userStateSender;
	
	private HashMap<String,ArrayList<AUEvent>> auEvents = new HashMap<String,ArrayList<AUEvent>>();
	private HashMap<String,Double> auValues = new HashMap<String,Double>();
	private HashMap<String,Long> startTimes = new HashMap<String,Long>();
	private HashMap<String,Long> endTimes = new HashMap<String,Long>();
	
	private String[] auToIgnoreArray = {"AU25","AU45"};
	private List<String> auToIgnore = (List<String>)Arrays.asList(auToIgnoreArray);
	
	public FacialExpressionInterpreter() throws JMSException
	{
		super("FacialExpressionInterpreter");
		facsReceiver = new FeatureReceiver("semaine.data.analysis.features.video.lbpfacs");
		receivers.add(facsReceiver);
		//receivers.add(new XMLReceiver("semaine.data.analysis.features.video.lbpfacs"));
		//receivers.add(new Receiver("semaine.data.analysis.features.video.lbpfacs"));
		
		userStateSender = new StateSender("semaine.data.state.user.behaviour", StateInfo.Type.UserState, getName());
		senders.add(userStateSender);
	}
	
	public void react( SEMAINEMessage m ) throws JMSException
	{
		if( m instanceof SEMAINEFeatureMessage ) {
			SEMAINEFeatureMessage fm = (SEMAINEFeatureMessage)m;
			
			/* Reads the feature names and values */
			String[] featureNames = fm.getFeatureNames();
			float[] featureValues = fm.getFeatureVector();
			
			for( int i=0; i<featureNames.length; i++ ) {
				String featureName = featureNames[i];
				if( auToIgnore.contains(featureName) ) continue;
				int featureValue = (int)featureValues[i];
				
				ArrayList<AUEvent> auList = auEvents.get(featureName);
				if( auList == null ) {
					auList = new ArrayList<AUEvent>();
					auList.add(new AUEvent(0,meta.getTime()));
					auList.add(new AUEvent(0,meta.getTime()));
					auList.add(new AUEvent(0,meta.getTime()));
				}
				auList.add(new AUEvent(featureValue,meta.getTime()));
				
				while( auList.get(0).time < meta.getTime()-WINDOW_SIZE ) {
					auList.remove(0);
				}
				
				auEvents.put(featureName, auList);
			}
			
			for( String au : auEvents.keySet() ) {
				double avg = 0;
				for( AUEvent auEvent : auEvents.get(au) ) {
					avg = avg + auEvent.active;
				}
				avg = avg / auEvents.get(au).size();
				auValues.put( au, avg );
				
				if( avg >= THRESHOLD ) {
					Long starttime = startTimes.get(au);
					if( starttime == null ) startTimes.put(au,meta.getTime());
					Long endtime = endTimes.get(au);
					if( endtime != null ) endTimes.remove(au);
					processAU( au );
				} else {
					Long starttime = startTimes.get(au);
					if( starttime != null ) startTimes.remove(au);
					Long endtime = endTimes.get(au);
					if( endtime == null ) endTimes.put(au, meta.getTime());
				}
			}
		}
	}
	
	public void processAU( String au ) throws JMSException
	{
		
		Map<String,String> userStateInfo = new HashMap<String,String>();
		String expression = null;
		if( au.equals("AU1") || au.equals("AU2") ) expression = "raised_eyebrows";
		else if( au.equals("AU12") ) expression = "smile";
		else if( au.equals("AU15") ) expression = "sad";
		if( expression == null ) return;
		
		userStateInfo.put("facialExpression", expression);
		userStateInfo.put("facialExpressionStarted", ""+startTimes.get(au));
		if( endTimes.get(au) == null ) userStateInfo.put("facialExpressionStopped", "-1");
		else userStateInfo.put("facialExpressionStopped", ""+endTimes.get(au));
		//System.out.println("Detected the following utterance (t:" + act.getStarttime() + "): " + act.getUtterance() + ". ("+act.getFeatures()+")");
		
		UserStateInfo usi = new UserStateInfo(userStateInfo	);
		userStateSender.sendStateInfo(usi, meta.getTime());	
	}
	
	private class AUEvent
	{
		public int active;
		public long time;
		public AUEvent( int active, long time ) {
			this.active = active;
			this.time = time;
		}
	}
}
