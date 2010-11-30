/* @author Dennis Hofs - Modified for Semaine by Mark ter Maat
 */
// Last modification by: $Author: hofs $
// $Log: SalProject.java,v $
// Revision 1.10  2006/07/04 08:23:46  hofs
// no message
//
// Revision 1.9  2006/06/14 14:48:07  hofs
// *** empty log message ***
//
// Revision 1.8  2006/06/07 12:08:03  hofs
// *** empty log message ***
//
// Revision 1.7  2005/11/10 14:16:41  hofs
// Fixed formatting
//
// Revision 1.6  2005/11/10 14:03:54  hofs
// Added javadoc
//
// Revision 1.5  2005/11/08 09:06:12  hofs
// Updated XML encoding/decoding
//
// Revision 1.4  2005/10/06 12:35:21  hofs
// Added wavfiles
//
// Revision 1.3  2005/09/29 11:26:37  hofs
// *** empty log message ***
//

package eu.semaine.components.dialogue.woz;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

import javax.xml.XMLConstants;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.validation.Schema;
import javax.xml.validation.SchemaFactory;

import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

/**
 * <p>A SAL project, identified by a name ({@link #getName() getName()}),
 * contains the following information.</p>
 *
 * <p><ul>
 * <li>a list of speaker IDs ({@link #getSpeakers() getSpeakers()})</li>
 * <li>a matrix with state IDs ({@link #getStateMatrix()
 * getStateMatrix()})</li>
 * <li>speaker descriptions, mapping a speaker ID to a button text (allowing
 * the user to select a speaker, {@link #getSpeakerButton(String)
 * getSpeakerButton()}) and a long description (shown in headers, {@link
 * #getSpeakerDescription(String) getSpeakerDescription()})</li>
 * <li>state descriptions, mapping a state ID to a button text ({@link
 * #getStateButton(String) getStateButton()}) and a long description ({@link
 * #getStateDescription(String) getStateDescription()})</li>
 * <li>a welcome text with title, presented at startup ({@link
 * #getWelcomeTitle() getWelcomeTitle()}, {@link #getWelcomeText()
 * getWelcomeText()}</li>
 * <li>an introduction text with title, presented on user request ({@link
 * #getIntroTitle() getIntroTitle()}, {@link #getIntroText()
 * getIntroText()})</li>
 * </ul></p>
 * 
 * <p>For each speaker:</p>
 * 
 * <p><ul>
 * <li>a list of startup phrases ({@link #getSpeakerStartup(String)
 * getSpeakerStartup()})</li>
 * <li>a list of repair phrases ({@link #getSpeakerRepair(String)
 * getSpeakerRepair()})</li>
 * </ul></p>
 * 
 * <p>For each speaker-state combination:</p>
 * 
 * <p><ul>
 * <li>a list of categories, each category consisting of a title and a list of
 * phrases ({@link #getStatePhrases(String, String) getStatePhrases()})</li>
 * </ul></p>
 * 
 * <p>A project can be read from XML with {@link #read(InputStream) read()} and
 * written to XML with {@link #write(OutputStream) write()}. Reading a project
 * from XML is the only way to obtain an instance of this class.</p>
 * 
 * @author Dennis Hofs
 */
public class SalProject {
	
	private String name;
	private List<String> speakers;
	private List<List<String>> states;
	private HashMap<String,Description> speakerDescrs;
	private HashMap<String,Description> stateDescrs;
	private String welcomeTitle;
	private String welcomeText;
	private String introTitle;
	private String introText;
	private HashMap<String,SpeakerPhrases> speakerPhrases;
	
	/**
	 * <p>Constructs a new uninitialized SAL project. This private constructor
	 * is used in {@link #read(InputStream) read()}.</p>
	 */
	private SalProject() {
	}

	/**
	 * <p>Returns the name of this project.</p>
	 * 
	 * @return the name
	 */
	public String getName() {
		return name;
	}

	/**
	 * <p>Returns the list of speaker IDs.</p>
	 * 
	 * @return the list of speaker IDs
	 */
	public List<String> getSpeakers() {
		return speakers;
	}
	
	/**
	 * <p>Returns the button text for the specified speaker. This text is shown
	 * in the button that allows the user to select the speaker.</p>
	 * 
	 * @param speaker the speaker ID
	 * @return the button text
	 */
	public String getSpeakerButton(String speaker) {
		return speakerDescrs.get(speaker).getButton();
	}

	/**
	 * <p>Returns the description for the specified speaker. This description is
	 * shown in a header when the speaker is selected.</p>
	 * 
	 * @param speaker the speaker ID
	 * @return the description
	 */
	public String getSpeakerDescription(String speaker) {
		return speakerDescrs.get(speaker).getDescription();
	}
	
	/**
	 * <p>Returns the matrix with state IDs. The state selection buttons will be
	 * layed out according to this matrix.</p>
	 * 
	 * @return the matrix with state IDs
	 */
	public List<List<String>> getStateMatrix() {
		return states;
	}
	
	/**
	 * <p>Returns the button text for the specified state. This text is shown
	 * in the button that allows the user to select the state.</p>
	 * 
	 * @param state the state ID
	 * @return the button text
	 */
	public String getStateButton(String state) {
		return stateDescrs.get(state).getButton();
	}
	
	/**
	 * <p>Returns the description for the specified state. This description is
	 * shown in a header when the state is selected.</p>
	 * 
	 * @param state the state ID
	 * @return the description
	 */
	public String getStateDescription(String state) {
		return stateDescrs.get(state).getDescription();
	}
	
	/**
	 * <p>Returns the title of the welcome page, shown at startup.</p>
	 * 
	 * @return the welcome title
	 */
	public String getWelcomeTitle() {
		return welcomeTitle;
	}

	/**
	 * <p>Returns the HTML text for the welcome page, shown at startup. The
	 * text will be embedded in the &lt;body&gt; element.</p>
	 * 
	 * @return the welcome text
	 */
	public String getWelcomeText() {
		return welcomeText;
	}

	/**
	 * <p>Returns the title of the introduction page, shown at user request.</p>
	 * 
	 * @return the introduction title
	 */
	public String getIntroTitle() {
		return introTitle;
	}

	/**
	 * <p>Returns the HTML text for the introduction page, shown at user
	 * request. The text will be embedded in the &lt;body&gt; element.</p>
	 * 
	 * @return the introduction text
	 */
	public String getIntroText() {
		return introText;
	}
	
	/**
	 * <p>Returns the list of startup phrases for the specified speaker.</p>
	 * 
	 * @param speaker the speaker ID
	 * @return the list of startup phrases
	 */
	public List<PhraseString> getSpeakerStartup(String speaker) {
		return speakerPhrases.get(speaker).getStartup();
	}
	
	/**
	 * <p>Returns the list of repair phrases for the specified speaker.</p>
	 * 
	 * @param speaker the speaker ID
	 * @return the list of repair phrases
	 */
	public List<PhraseString> getSpeakerRepair(String speaker) {
		return speakerPhrases.get(speaker).getRepair();
	}
	
	/**
	 * <p>Returns the list of categories, each consisting of a title and a
	 * list of phrases, for the specified speaker and state.</p>
	 * 
	 * @param speaker the speaker ID
	 * @param state the state ID
	 * @return the list of categories
	 */
	public List<StatePhrases> getStatePhrases(String speaker, String state) {
		return speakerPhrases.get(speaker).getStates().get(state);
	}
	
	/**
	 * <p>A description defines how a speaker or state (identified by ID in this
	 * class) is presented to the user. Each speaker ID and state ID is mapped
	 * to an instance of this class.</p>
	 * 
	 * <p>It consists of a button text (shown in the button that allows the user
	 * to select the speaker/state) and a long description (shown in a header
	 * when the speaker/state is selected).</p>
	 */
	private class Description {
		private String button;
		private String description;
		
		/**
		 * <p>Constructs a new description.</p>
		 * 
		 * @param button the button text
		 * @param description the long description
		 */
		public Description(String button, String description) {
			this.button = button;
			this.description = description;
		}
		
		/**
		 * <p>Returns the button text.</p>
		 * 
		 * @return the button text
		 */
		public String getButton() {
			return button;
		}
		
		/**
		 * <p>Returns the long description.</p>
		 * 
		 * @return the long description
		 */
		public String getDescription() {
			return description;
		}
	}

	/**
	 * <p>This class contains all phrases for one speaker. The phrases are
	 * subdivided into startup phrases, repair phrases, and for each state
	 * there is a list of categories, each consisting of a title and
	 * phrases. Each speaker ID is mapped to an instance of this class.</p>
	 */
	private class SpeakerPhrases {
		private List<PhraseString> startup;
		private List<PhraseString> repair;
		private HashMap<String,List<StatePhrases>> states;
		
		/**
		 * <p>Constructs a new instance of this class.</p>
		 * 
		 * @param startup the list of startup phrases
		 * @param repair the list of repair phrases
		 * @param states a map that maps state IDs to a list of categories
		 */
		public SpeakerPhrases(List<PhraseString> startup,
				List<PhraseString> repair,
				HashMap<String,List<StatePhrases>> states) {
			this.startup = startup;
			this.repair = repair;
			this.states = states;
		}
		
		/**
		 * <p>Returns the list of startup phrases.</p>
		 * 
		 * @return the list of startup phrases
		 */
		public List<PhraseString> getStartup() {
			return startup;
		}
		
		/**
		 * <p>Returns the list of repair phrases.</p>
		 * 
		 * @return the list of repair phrases
		 */
		public List<PhraseString> getRepair() {
			return repair;
		}
		
		/**
		 * <p>Returns the map that maps state IDs to categories.</p>
		 * 
		 * @return the map that maps state IDs to categories
		 */
		public HashMap<String,List<StatePhrases>> getStates() {
			return states;
		}
	}
	
	/**
	 * <p>This class represents a category, consisting of a title and list
	 * of phrases, which is associated with a speaker-state combination.</p>
	 */
	public class StatePhrases {
		private String category;
		private List<PhraseString> phrases;
		
		/**
		 * <p>Constructs a new category.</p>
		 * 
		 * @param category the category title
		 * @param phrases the list of phrases
		 */
		public StatePhrases(String category, List<PhraseString> phrases) {
			this.category = category;
			this.phrases = phrases;
		}
		
		/**
		 * <p>Returns the category title.</p>
		 * 
		 * @return the category title
		 */
		public String getCategory() {
			return category;
		}
		
		/**
		 * <p>Returns the list of phrases in this category.</p>
		 * 
		 * @return the list of phrases in this category
		 */
		public List<PhraseString> getPhrases() {
			return phrases;
		}
	}
	
	/**
	 * <p>A phrase consists of a string and the path to an audio resource (like
	 * a WAV file). The resource path will be resolved using {@link
	 * ClassLoader#getSystemResource(java.lang.String)
	 * ClassLoader.getSystemResource()}.</p>
	 */
	public class PhraseString {
		private String phrase;
		private String wavfile;
		
		/**
		 * <p>Constructs a new phrase string.</p>
		 * 
		 * @param phrase the phrase
		 * @param wavfile the path to the audio resource
		 */
		public PhraseString(String phrase, String wavfile) {
			this.phrase = phrase;
			this.wavfile = wavfile;
		}
		
		/**
		 * <p>Returns the phrase.</p>
		 * 
		 * @return the phrase
		 */
		public String getPhrase() {
			return phrase;
		}
		
		/**
		 * <p>Returns the path to the audio resource.</p>
		 * 
		 * @return the path to the audio resource
		 */
		public String getWavFile() {
			return wavfile;
		}
	}
	
	/**
	 * <p>Validates the project data. This method checks the constraints
	 * described in {@link #read(InputStream) read()}. If the project does not
	 * conform to those constraints, this method throws an exception.</p>
	 * 
	 * @throws Exception if the project does not conform to the constraints
	 */
	private void validate() throws Exception {
		List<String> foundSpeakers = new ArrayList<String>();
		for (String sp : speakers) {
			if (foundSpeakers.contains(sp)) {
				throw new Exception("Found duplicate speaker \"" + sp + "\"");
			}
			foundSpeakers.add(sp);
		}
		List<String> foundStates = new ArrayList<String>();
		for (List<String> row : states) {
			for (String st : row) {
				if (foundStates.contains(st)) {
					throw new Exception("Found duplicate state \"" + st + "\"");
				}
				foundStates.add(st);
			}
		}
		for (String sp : foundSpeakers) {
			if (!speakerPhrases.containsKey(sp))
				throw new Exception("No phrases for speaker \"" + sp + "\"");
		}
		for (String sp : speakerPhrases.keySet()) {
			if (!foundSpeakers.contains(sp)) {
				throw new Exception("Found phrases for unknown speaker \"" + 
						sp + "\"");
			}
			HashMap<String,List<StatePhrases>> speakerStates =
					speakerPhrases.get(sp).getStates();
			for (String st : foundStates) {
				if (!speakerStates.containsKey(st)) {
					throw new Exception("No phrases for speaker \"" + sp +
							"\" and state \"" + st + "\"");
				}
			}
			for (String st : speakerStates.keySet()) {
				if (!foundStates.contains(st)) {
					throw new Exception("Found phrases for unknown state \"" +
							st + "\" for speaker \"" + sp + "\"");
				}
				List<String> foundCats = new ArrayList<String>();
				for (StatePhrases stp : speakerStates.get(st)) {
					String cat = stp.getCategory();
					if (foundCats.contains(cat)) {
						throw new Exception(
								"Found duplicate category for speaker \"" + sp +
								"\" and state \"" + st + "\": \"" + cat + "\"");
					}
					foundCats.add(cat);
				}
			}
		}
	}
	
	/**
	 * <p>Returns the first element node encountered, starting at the specified
	 * node. If the specified node is an element node, this method returns that
	 * node. Otherwise it returns the first element node following it. If no
	 * element node is found, this method returns null.</p>
	 * 
	 * <p>This method is useful for example to skip white-space text nodes.</p>
	 * 
	 * @param node a node
	 * @return the first element node or null
	 */
	private static Node getFirstElement(Node node) {
		while (node != null && node.getNodeType() != Node.ELEMENT_NODE) {
			node = node.getNextSibling();
		}
		return node;
	}
	
	/**
	 * <p>The specified node should be a "speaker" element containing the
	 * attributes "name", "button" and "description". This method gets the
	 * data from the element and adds it to this project.</p>
	 * 
	 * @param node a "speaker" element
	 */
	private void addSpeaker(Node node) {
		String name = node.getAttributes().getNamedItem("name").getNodeValue();
		String button = node.getAttributes().getNamedItem(
				"button").getNodeValue();
		String descr = node.getAttributes().getNamedItem(
				"description").getNodeValue();
		speakers.add(name);
		speakerDescrs.put(name,new Description(button,descr));
	}
	
	/**
	 * <p>The specified node should be a "state_matrix" element containing
	 * "state_row" child elements. Each state row should contain "state"
	 * child elements with the attributes "name", "button" and "description".
	 * This method gets the data from the "state_matrix" element and adds the
	 * data to this project.
	 * 
	 * @param node a "state_matrix" element
	 */
	private void addStates(Node node) {
		states = new ArrayList<List<String>>();
		stateDescrs = new HashMap<String,Description>();
		Node rowNode = getFirstElement(node.getFirstChild());
		while (rowNode != null) {
			List<String> row = new ArrayList<String>();
			Node stateNode = getFirstElement(rowNode.getFirstChild());
			while (stateNode != null) {
				String name = stateNode.getAttributes().getNamedItem(
						"name").getNodeValue();
				String button = stateNode.getAttributes().getNamedItem(
						"button").getNodeValue();
				String descr = stateNode.getAttributes().getNamedItem(
						"description").getNodeValue();
				row.add(name);
				stateDescrs.put(name,new Description(button,descr));
				stateNode = getFirstElement(stateNode.getNextSibling());
			}
			states.add(row);
			rowNode = getFirstElement(rowNode.getNextSibling());
		}
	}
	
	/**
	 * <p>The specified node should be a "phrases" element containing the
	 * following child elements:</p>
	 * 
	 * <p><ul>
	 * <li>"welcome" with attribute "title" and a child text node containing
	 * the welcome text in HTML (to be embedded in a &lt;body&gt; element)</li>
	 * <li>"instructions" with attribute "title" and a child text node
	 * containing the instructions text in HTML</li>
	 * <li>"speaker" elements to be parsed by {@link #addSpeakerPhrases(Node)
	 * addSpeakerPhrases()}</li>
	 * </ul></p>
	 * 
	 * <p>This method gets the data from the "phrases" element and adds the
	 * data to this project.</p>
	 * 
	 * @param node a "phrases" element
	 */
	private void addPhrases(Node node) {
		Node child = getFirstElement(node.getFirstChild());
		welcomeTitle = child.getAttributes().getNamedItem("title").getNodeValue();
		Node text = child.getFirstChild();
		if (text == null)
			welcomeText = "";
		else
			welcomeText = text.getNodeValue();
		child = getFirstElement(child.getNextSibling());
		introTitle = child.getAttributes().getNamedItem("title").getNodeValue();
		text = child.getFirstChild();
		if (text == null)
			introText = "";
		else
			introText = text.getNodeValue();
		child = getFirstElement(child.getNextSibling());
		speakerPhrases = new HashMap<String,SpeakerPhrases>();
		while (child != null) {
			addSpeakerPhrases(child);
			child = getFirstElement(child.getNextSibling());
		}
	}
	
	/**
	 * <p>The specified node should be a "speaker" element with attribute
	 * "name" and the following child elements:</p>
	 * 
	 * <p><ul>
	 * <li>"startup" with startup phrases to be parsed by {@link
	 * #getPhrases(Node) getPhrases()}</li>
	 * <li>"repair" with repair phrases to be parsed by {@link #getPhrases(Node)
	 * getPhrases()}</li>
	 * <li>"state" elements with attribute "name" and child elements "category".
	 * Each "category" element should have attribute "name" and list of phrases,
	 * which are obtained by passing the "category" element to {@link
	 * #getPhrases(Node) getPhrases()}</li>
	 * </ul></p>
	 * 
	 * <p>This method gets the data from the "speaker" element and adds the
	 * data to this project.</p>
	 * 
	 * @param node a "speaker" element
	 */
	private void addSpeakerPhrases(Node node) {
		String speaker = node.getAttributes().getNamedItem("name").getNodeValue();
		Node child = getFirstElement(node.getFirstChild());
		List<PhraseString> startup = getPhrases(child);
		child = getFirstElement(child.getNextSibling());
		List<PhraseString> repair = getPhrases(child);
		HashMap<String,List<StatePhrases>> states = new HashMap<String,List<StatePhrases>>();
		Node stateNode = getFirstElement(child.getNextSibling());
		while (stateNode != null) {
			String state = stateNode.getAttributes().getNamedItem("name").getNodeValue();
			List<StatePhrases> statePhrases = new ArrayList<StatePhrases>();
			Node catNode = getFirstElement(stateNode.getFirstChild());
			while (catNode != null) {
				String cat = catNode.getAttributes().getNamedItem("name").getNodeValue();
				List<PhraseString> phrases = getPhrases(catNode);
				StatePhrases stp = new StatePhrases(cat,phrases);
				statePhrases.add(stp);
				catNode = getFirstElement(catNode.getNextSibling());
			}
			states.put(state,statePhrases);
			stateNode = getFirstElement(stateNode.getNextSibling());
		}
		SpeakerPhrases spp = new SpeakerPhrases(startup,repair,states);
		speakerPhrases.put(speaker,spp);
	}
	
	/**
	 * <p>The specified node should contain "phrase" child elements. Each
	 * "phrase" element should contain attribute "wavfile" and a child text
	 * node containing a phrase. This method returns the phrases.</p>
	 * 
	 * @param node a node that contains "phrase" child elements
	 * @return the list of phrases
	 */
	private List<PhraseString> getPhrases(Node node) {
		List<PhraseString> result = new ArrayList<PhraseString>();
		Node child = getFirstElement(node.getFirstChild());
		while (child != null) {
			String wavfile = child.getAttributes().getNamedItem(
					"wavfile").getNodeValue();
			Node text = child.getFirstChild();
			if (text == null)
				result.add(new PhraseString("",wavfile));
			else
				result.add(new PhraseString(text.getNodeValue(),wavfile));
			child = getFirstElement(child.getNextSibling());
		}
		return result;
	}
	
	/**
	 * <p>Parses an XML document and reads the data into the specified SAL
	 * project. The document should contain valid XML code. After calling this
	 * method, call {@link #validate() validate()} to check for further
	 * constraints on the project data.</p>
	 * 
	 * @param doc the XML document
	 * @param proj the SAL project (it should still be empty, constructed with
	 * the default constructor)
	 */
	private static void parseDocument(Document doc, SalProject proj) {
		proj.speakers = new ArrayList<String>();
		proj.speakerDescrs = new HashMap<String,Description>();
		Node salProj = doc.getDocumentElement();
		proj.name = salProj.getAttributes().getNamedItem("name").getNodeValue();
		Node child = getFirstElement(salProj.getFirstChild());
		while (child != null) {
			if (child.getNodeName().equals("speaker"))
				proj.addSpeaker(child);
			else if (child.getNodeName().equals("state_matrix"))
				proj.addStates(child);
			else
				proj.addPhrases(child);
			child = getFirstElement(child.getNextSibling());
		}
	}
	
	/**
	 * <p>Reads a SAL project from the specified input stream, which should be
	 * positioned at the start of a valid XML document. After parsing the XML
	 * code, the document will be validated against the schema found in the
	 * resource obtained with {@link
	 * ClassLoader#getSystemResource(java.lang.String)
	 * ClassLoader.getSystemResource("project/sal/project.xsd")}. When the
	 * document has been analyzed and read into a {@link SalProject SalProject},
	 * it will be further validated against these constraints:</p>
	 * 
	 * <p><ul>
	 * <li>every speaker ID must be unique</li>
	 * <li>every state ID must be unique</li>
	 * <li>each speaker ID must be mapped to a set of speaker phrases</li>
	 * <li>each set of speaker phrases must correspond to an existing speaker
	 * ID</li>
	 * <li>each speaker-state ID combination must be mapped to a set of state
	 * phrases</li>
	 * <li>each set of state phrases must correspond to an existing speaker ID
	 * and existing state ID</li>
	 * <li>each category in a set of state phrases must have a unique name</li>
	 * </ul></p>
	 * 
	 * <p>In case of any error, this method throws an exception.</p>
	 *
	 * @param in the input stream
	 * @return the SAL project
	 * @throws SAXException if the XML code is invalid (the XML parser cannot
	 * parse the code)
	 * @throws IOException if a reading error occurs on the input stream
	 * @throws Exception if validation against the schema or the extra
	 * constraints has failed
	 */
	public static SalProject read(InputStream in)
	throws SAXException, IOException, Exception {
		SalProject result = new SalProject();
		DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
		SchemaFactory schemaFactory = SchemaFactory.newInstance(
				XMLConstants.W3C_XML_SCHEMA_NS_URI);
		URL schemaUrl = result.getClass().getResource("/eu/semaine/components/dialogue/data/woz_project.xsd");
		if (schemaUrl == null) {
			throw new Exception(
					"Can't find resource \"/eu/semaine/components/dialogue/data/woz_project.xsd\"");
		}
		Schema schema = schemaFactory.newSchema(schemaUrl);
		factory.setSchema(schema);
		DocumentBuilder builder = factory.newDocumentBuilder();
		XmlErrorHandler errorHandler = new XmlErrorHandler();
		builder.setErrorHandler(errorHandler);
		InputSource src = new InputSource(in); 
		Document doc = builder.parse(src);
		if (errorHandler.getSaxException() != null)
			throw errorHandler.getSaxException();
		parseDocument(doc,result);
		result.validate();
		return result;
	}
	
	/**
	 * <p>Encodes this project into XML and writes the XML code to the specified
	 * output stream. The generated XML code can be read back into a SAL project
	 * with {@link #read(InputStream) read()}.</p>
	 * 
	 * @param out the output stream
	 * @throws IOException if a writing error occurs
	 */
	public void write(OutputStream out) throws IOException {
		XmlWriter writer = new XmlWriter();
		writer.appendOpenSTag("sal_project");
		writer.appendAttribute("name",name);
		writer.appendCloseSTag();
		for (String speaker : speakers) {
			writer.appendOpenSTag("speaker");
			Description descr = speakerDescrs.get(speaker);
			writer.appendAttribute("name",speaker);
			writer.appendAttribute("button",descr.getButton());
			writer.appendAttribute("description",descr.getDescription());
			writer.appendCloseEmptyTag();
		}
		List<String> allStates = new ArrayList<String>();
		writer.appendSTag("state_matrix");
		for (List<String> row : states) {
			writer.appendSTag("state_row");
			for (String state : row) {
				writer.appendOpenSTag("state");
				Description descr = stateDescrs.get(state);
				writer.appendAttribute("name",state);
				writer.appendAttribute("button",descr.getButton());
				writer.appendAttribute("description",descr.getDescription());
				writer.appendCloseEmptyTag();
				allStates.add(state);
			}
			writer.appendETag(); // state_row
		}
		writer.appendETag(); // state_matrix
		writer.appendSTag("phrases");
		writer.appendOpenSTag("welcome");
		writer.appendAttribute("title",welcomeTitle);
		writer.appendCloseSTag();
		writer.appendText(welcomeText);
		writer.appendETag(); // welcome
		writer.appendOpenSTag("instructions");
		writer.appendAttribute("title",introTitle);
		writer.appendCloseSTag();
		writer.appendText(introText);
		writer.appendETag(); // instructions
		for (String speaker : speakers) {
			writer.appendOpenSTag("speaker");
			writer.appendAttribute("name",speaker);
			writer.appendCloseSTag();
			SpeakerPhrases phrases = speakerPhrases.get(speaker);
			writer.appendSTag("startup");
			for (PhraseString phrase : phrases.getStartup()) {
				writer.appendOpenSTag("phrase");
				writer.appendAttribute("wavfile",phrase.getWavFile());
				writer.appendCloseSTag();
				writer.appendText(phrase.getPhrase());
				writer.appendETag(); // phrase
			}
			writer.appendETag(); // startup
			writer.appendSTag("repair");
			for (PhraseString phrase : phrases.getRepair()) {
				writer.appendOpenSTag("phrase");
				writer.appendAttribute("wavfile",phrase.getWavFile());
				writer.appendCloseSTag();
				writer.appendText(phrase.getPhrase());
				writer.appendETag(); // phrase
			}
			writer.appendETag(); // repair
			for (String state : allStates) {
				writer.appendOpenSTag("state");
				writer.appendAttribute("name",state);
				writer.appendCloseSTag();
				List<StatePhrases> allPhrases = phrases.getStates().get(state);
				for (StatePhrases statePhrases : allPhrases) {
					writer.appendOpenSTag("category");
					writer.appendAttribute("name",statePhrases.getCategory());
					writer.appendCloseSTag();
					for (PhraseString phrase : statePhrases.getPhrases()) {
						writer.appendOpenSTag("phrase");
						writer.appendAttribute("wavfile",phrase.getWavFile());
						writer.appendCloseSTag();
						writer.appendText(phrase.getPhrase());
						writer.appendETag(); // phrase
					}
					writer.appendETag(); // category
				}
				writer.appendETag(); // state
			}
			writer.appendETag(); // speaker
		}
		writer.appendETag(); // phrases
		writer.appendETag(); // sal_project
		byte[] bs = writer.getBytes();
		out.write(bs);
	}
}
