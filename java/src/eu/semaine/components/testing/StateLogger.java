/**
 * Copyright (C) 2010 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.testing;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.text.DecimalFormat;
import java.util.Calendar;
import java.util.GregorianCalendar;
import java.util.Map;
import java.util.Set;

import javax.jms.JMSException;

import eu.semaine.components.Component;
import eu.semaine.datatypes.stateinfo.StateInfo;
import eu.semaine.exceptions.SystemConfigurationException;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEStateMessage;
import eu.semaine.jms.receiver.StateReceiver;

/**
 * A component that creates a CSV-type log file from the context, user, agent and dialogue state information that is available.
 * The aim is to give a timestamped trace of the system's view of what happened in the dialogue.
 * @author marc
 *
 */
public class StateLogger extends Component {

	private static final String userStateTopic = "semaine.data.state.user.behaviour";
	private static final String agentStateTopic = "semaine.data.state.agent";
	private static final String dialogStateTopic = "semaine.data.state.dialog";
	private static final String contextStateTopic = "semaine.data.state.context";
	
	private static final String userStatePrefix = "u.";
	private static final String agentStatePrefix = "a.";
	private static final String dialogStatePrefix = "d.";
	private static final String contextStatePrefix = "c.";
	
	private static final String FIELD_SEPARATOR = ",";
	private static final String FIELD_QUOTE = "\"";
	
	private Set<String> userStates;
	private Set<String> agentStates;
	private Set<String> dialogStates;
	private Set<String> contextStates;
	
	private PrintWriter out;
	
	/**
	 * @param componentName
	 * @throws JMSException
	 */
	public StateLogger() throws JMSException, SystemConfigurationException {
		super("StateLogger");
		receivers.add(new StateReceiver(userStateTopic, StateInfo.Type.UserState));
		receivers.add(new StateReceiver(agentStateTopic, StateInfo.Type.AgentState));
		receivers.add(new StateReceiver(dialogStateTopic, StateInfo.Type.DialogState));
		receivers.add(new StateReceiver(contextStateTopic, StateInfo.Type.ContextState));
		
		userStates = StateInfo.getShortNames(StateInfo.Type.UserState);
		agentStates = StateInfo.getShortNames(StateInfo.Type.AgentState);
		dialogStates = StateInfo.getShortNames(StateInfo.Type.DialogState);
		contextStates = StateInfo.getShortNames(StateInfo.Type.ContextState);
		
		try {
			out = setupLogDestination();
			logHeader();
		} catch (Exception e) {
			throw new SystemConfigurationException("Cannot setup log output", e);
		}
	}

	@Override
	protected void react(SEMAINEMessage m) throws JMSException {
		if (!(m instanceof SEMAINEStateMessage)) {
			return;
		}
		SEMAINEStateMessage sm = (SEMAINEStateMessage) m;
		StateInfo state = sm.getState();
		logStateChange(state.getType(), state.getInfos(), sm.getUsertime());
	}
	
	private void logHeader() {
		StringBuilder buf = new StringBuilder();
		buf.append("time");
		for (String s : userStates) {
			buf.append(FIELD_SEPARATOR).append(userStatePrefix).append(s);
		}
		for (String s : agentStates) {
			buf.append(FIELD_SEPARATOR).append(agentStatePrefix).append(s);
		}
		for (String s : dialogStates) {
			buf.append(FIELD_SEPARATOR).append(dialogStatePrefix).append(s);
		}
		for (String s : contextStates) {
			buf.append(FIELD_SEPARATOR).append(contextStatePrefix).append(s);
		}
		out.println(buf.toString());
	}
	
	private PrintWriter setupLogDestination() throws IOException {
		GregorianCalendar cal = new GregorianCalendar();
		DecimalFormat twoPlaces = new DecimalFormat("00");
		String dateAndTime = cal.get(Calendar.YEAR) + "-" + twoPlaces.format(cal.get(Calendar.MONTH)+1) + "-" + twoPlaces.format(cal.get(Calendar.DAY_OF_MONTH)) 
			+ "_" + twoPlaces.format(cal.get(Calendar.HOUR_OF_DAY)) + "." + twoPlaces.format(cal.get(Calendar.MINUTE));
		String componentConfigFilename = System.getProperty("semaine.config-file");
		assert componentConfigFilename != null;
		File configFolder = new File(componentConfigFilename).getAbsoluteFile().getParentFile();
		File parentOfConfigFolder = configFolder.getParentFile();
		File logFolder = new File(parentOfConfigFolder, "logs");
		logFolder.mkdir(); // make sure it exists
		File logFile = new File(logFolder, "statelog-"+dateAndTime+".csv");
		return new PrintWriter(new OutputStreamWriter(new FileOutputStream(logFile), "UTF-8"), true);
	}
	
	private void logStateChange(StateInfo.Type stateType, Map<String, String> infos, long usertime) {
		StringBuilder buf = new StringBuilder();
		// Order is: user, agent, dialog, context
		buf.append(usertime);
		buf.append(logSection(StateInfo.Type.UserState, userStates, stateType, infos));
		buf.append(logSection(StateInfo.Type.AgentState, agentStates, stateType, infos));
		buf.append(logSection(StateInfo.Type.DialogState, dialogStates, stateType, infos));
		buf.append(logSection(StateInfo.Type.ContextState, contextStates, stateType, infos));
		out.println(buf.toString());
	}

	private String logSection(StateInfo.Type targetType, Set<String> keys, StateInfo.Type stateType, Map<String, String> infos) {
		StringBuilder buf = new StringBuilder();
		for (String s : keys) {
			buf.append(FIELD_SEPARATOR);
			String value;
			boolean needQuotes = false;
			if (stateType == targetType && infos.containsKey(s)) {
				value = infos.get(s);
				if (value == null) {
					value = "";
				} else if (value.contains(FIELD_QUOTE)) {
					value = value.replaceAll(FIELD_QUOTE, "\\"+FIELD_QUOTE);
					needQuotes = true;
				} else if (value.contains(FIELD_SEPARATOR)) {
					needQuotes = true;
				}
			} else { // we don't have this info
				value = "";
			}
			if (needQuotes) {
				buf.append(FIELD_QUOTE).append(value).append(FIELD_QUOTE);
			} else {
				buf.append(value);
			}
		}
		return buf.toString();

	}
}
