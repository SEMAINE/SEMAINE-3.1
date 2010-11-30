/**
 * Copyright (C) 2010 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.testing;

import java.util.HashMap;
import java.util.Map;

import javax.jms.JMSException;

import marytts.util.math.MathUtils;

import org.w3c.dom.Element;

import eu.semaine.components.Component;
import eu.semaine.datatypes.xml.SemaineML;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.message.SEMAINEXMLMessage;
import eu.semaine.jms.receiver.Receiver;
import eu.semaine.jms.receiver.XMLReceiver;
import eu.semaine.util.XMLTool;

/**
 * @author marc
 *
 */
public class MeasureDelay extends Component {
	private static final long REPORT_PERIOD = 10000; // 10 s;
	private long lastReportTime = 0;
	private Map<String, Record> records = new HashMap<String, Record>();
	private static final String directTopic = "semaine.data.action.selected.function";
	private static final String prepareTopic = "semaine.data.synthesis.lowlevel.command";
	private static final String callbackTopic = "semaine.callback.output.Animation";

	public MeasureDelay() throws JMSException {
		super("MeasureDelay");
		// No need to look inside the messages for these two:
		receivers.add(new Receiver(directTopic));
		receivers.add(new Receiver(prepareTopic, SEMAINEMessage.SOURCE+"='UtteranceActionProposer'"));
		// XML:
		receivers.add(new XMLReceiver(callbackTopic));
	}
	
	@Override
	protected void act() throws JMSException {
		if (meta.getTime() - lastReportTime >= REPORT_PERIOD) {
			logStats();
			lastReportTime = meta.getTime();
		}
	}
	
	@Override
	protected void react(SEMAINEMessage m) throws JMSException {
		if (m instanceof SEMAINEXMLMessage) {
			SEMAINEXMLMessage xm = (SEMAINEXMLMessage) m;
			assert xm.getTopicName().equals(callbackTopic);
			Element root = xm.getDocument().getDocumentElement();
			if(!root.getLocalName().equals(SemaineML.E_CALLBACK)) {
				return;
			}
			if (!root.getNamespaceURI().equals(SemaineML.namespaceURI)) {
				return;
			}
			Element event = XMLTool.getChildElementByLocalNameNS(root, SemaineML.E_EVENT, SemaineML.namespaceURI);
			if (event == null) {
				return;
			}
			String type = event.getAttribute(SemaineML.A_TYPE);
			// We are only interested in start messages:
			if (!type.equals(SemaineML.V_START)) {
				return;
			}
			String id = event.getAttribute(SemaineML.A_ID);
			if (!records.containsKey(id)) { // not a relevant id
				return;
			}
			Record r = records.get(id);
			//String timeString = event.getAttribute(SemaineML.A_TIME);
			//r.startTime = Long.parseLong(timeString);
			r.startTime = meta.getTime();
			return;
		}
		// We are seeing the creation of the new play request
		Record r = new Record();
		if (m.getTopicName().equals(directTopic)) {
			r.direct = true;
		} else {
			if (!m.getTopicName().equals(prepareTopic)) {
				return;
			}
			r.direct = false;
		}
		r.id = m.getContentID();
		//r.creationTime = m.getUsertime();
		r.creationTime = meta.getTime();
		if (records.containsKey(r.id)) {
			log.debug("ignoring duplicate entry '"+r.id+"' (previous creationTime was "+records.get(r.id).creationTime+", new would have been "+r.creationTime+")");
			return;
		}
		records.put(r.id, r);
	}
	
	private void logStats() throws JMSException {
		int nDirect = 0;
		int nPrepared = 0;
		double medianDirect = 0;
		double medianPrepared = 0;
		for (Record r : records.values()) {
			if (r.startTime == 0) { // not started yet
				continue;
			}
			//log.debug("Record "+r.id+" dur = "+(r.startTime - r.creationTime) + "(created at "+r.creationTime+", started at "+r.startTime+")");
			if (r.direct) {
				nDirect++;
			} else {
				nPrepared++;
			}
		}
		if (nDirect > 0) {
			double[] durs = new double[nDirect];
			int i = 0;
			for (Record r : records.values()) {
				if (r.startTime == 0) { // not started yet
					continue;
				}
				if (r.direct) {
					durs[i++] = r.startTime - r.creationTime;
				}
			}
			medianDirect = MathUtils.median(durs);
			StringBuilder sb = new StringBuilder();
			for (int j=0; j<durs.length; j++) {
				if (sb.length() > 0) {
					sb.append(", ");
				}
				sb.append(durs[j]);
			}
			log.debug("Direct durations: "+sb.toString());
		}
		if (nPrepared > 0) {
			double[] durs = new double[nPrepared];
			int i = 0;
			for (Record r : records.values()) {
				if (r.startTime == 0) { // not started yet
					continue;
				}
				if (!r.direct) {
					durs[i++] = r.startTime - r.creationTime;
				}
			}
			medianPrepared = MathUtils.median(durs);
			StringBuilder sb = new StringBuilder();
			for (int j=0; j<durs.length; j++) {
				if (sb.length() > 0) {
					sb.append(", ");
				}
				sb.append(durs[j]);
			}
			log.debug("Prepared durations: "+sb.toString());

		}
		log.info("Median time to animation: direct "+medianDirect+" ms (n="+nDirect+"), prepared "+medianPrepared+" ms (n="+nPrepared+")");
	}
	
	
	
	private static class Record {
		String id;
		boolean direct;
		long creationTime;
		long startTime;
		
		@Override
		public int hashCode() { return id.hashCode(); }
		@Override
		public boolean equals(Object o) {
			if (!(o instanceof Record)) { return false; }
			return ((Record)o).id.equals(id);
		}
	}
}
