package eu.semaine.components.dialogue.datastructures;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Properties;

public class DMProperties
{
	private static DMProperties dmProperties;
	private static Properties props;
	
	private DMProperties()
	{
		props = new Properties();
		try {
			props.load(this.getClass().getResourceAsStream("/eu/semaine/components/dialogue/data/DM.config")); 
		} catch (IOException e) { }
	}
	
	private static String getProperty( String key )
	{
		if( dmProperties == null ) {
			dmProperties = new DMProperties();
		}
		return props.getProperty(key);
	}
	
	/* ================= Get-methods ====================== */
	
	/* +++ Location of files +++ */
	public static String getSentenceFile()
	{
		String f = getProperty("sentences_file");
		if( f == null ) {
			f = "/eu/semaine/components/dialogue/data/sentences.xml";
		}
		return f;
	}
	
	public static String getContextFile()
	{
		String f = getProperty("context_file");
		if( f == null ) {
			f = "/eu/semaine/components/dialogue/data/Context.csv";
		}
		return f;
	}
	
	/* +++ Introduction properties +++ */
	public static boolean isBriefing()
	{
		Boolean b = Boolean.parseBoolean(getProperty("briefing"));
		if( b == null ) {
			b = false;
		}
		return b;
	}
	
	public static String getIntroduction()
	{
		String f = getProperty("introduction");
		if( f == null ) {
			f = "short";
		}
		return f;
	}
	
	/* +++ Utterance models +++ */
	public static boolean useAfterSilenceModel()
	{
		Boolean b = Boolean.parseBoolean(getProperty("afterSilence_model"));
		if( b == null ) {
			b = true;
		}
		return b;
	}
	
	public static boolean useLinkingSentenceModel()
	{
		Boolean b = Boolean.parseBoolean(getProperty("linkingSentences_model"));
		if( b == null ) {
			b = true;
		}
		return b;
	}
	
	public static boolean useContextModel()
	{
		Boolean b = Boolean.parseBoolean(getProperty("context_model"));
		if( b == null ) {
			b = true;
		}
		return b;
	}
	
	public static boolean useArousalModel()
	{
		Boolean b = Boolean.parseBoolean(getProperty("arousal_model"));
		if( b == null ) {
			b = true;
		}
		return b;
	}
	
	public static boolean useHistoryCorrection()
	{
		Boolean b = Boolean.parseBoolean(getProperty("history_correction"));
		if( b == null ) {
			b = true;
		}
		return b;
	}
	
	/* +++ Turn taking values +++ */
	public static int getTurnTakingThresholdPoppy()
	{
		int i = 60;
		System.out.println(getProperty("TurnTakingThreshold_Poppy"));
		try {
			i = Integer.parseInt(getProperty("TurnTakingThreshold_Poppy"));
		}catch( NumberFormatException e ) {
			System.out.println("TurnTaking threshold of Poppy is not a correct number, using default.");
		}
		return i;
	}
	
	public static int getTurnTakingThresholdPrudence()
	{
		int i = 80;
		try {
			i = Integer.parseInt(getProperty("TurnTakingThreshold_Prudence"));
		}catch( NumberFormatException e ) {
			System.out.println("TurnTaking threshold of Prudence is not a correct number, using default.");
		}
		return i;
	}
	
	public static int getTurnTakingThresholdSpike()
	{
		int i = 30;
		try {
			i = Integer.parseInt(getProperty("TurnTakingThreshold_Spike"));
		}catch( NumberFormatException e ) {
			System.out.println("TurnTaking threshold of Spike is not a correct number, using default.");
		}
		return i;
	}
	
	public static int getTurnTakingThresholdObadiah()
	{
		int i = 100;
		try {
			i = Integer.parseInt(getProperty("TurnTakingThreshold_Obadiah"));
		}catch( NumberFormatException e ) {
			System.out.println("TurnTaking threshold of Obadiah is not a correct number, using default.");
		}
		return i;
	}
	
	public static String getResponseFile()
	{
		String f = getProperty("response_file");
		if( f == null ) {
			f = "/eu/semaine/components/dialogue/data/responses.xml";
		}
		return f;
	}
	
	public static ArrayList<String> getTemplateFiles()
	{
		String f = getProperty("template_files");
		if( f == null ) {
			// TODO: Fixen.
			return new ArrayList<String>();
		}
		ArrayList<String> files = new ArrayList<String>(Arrays.asList(f.split("\\|")));
		return files;
	}
	
	/* +++ Logging +++ */
	public static boolean getLogging()
	{
		Boolean b = Boolean.parseBoolean(getProperty("logging"));
		if( b == null ) {
			b = true;
		}
		return b;
	}
}
