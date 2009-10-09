/**
 * Copyright (C) 2008 University of Twente, HMI. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.components.dialogue.datastructures;

/**
 * This class is a container for an utterance plus all its features
 * 
 * @author Mark tM
 * @version 0.1 - dummy class
 *
 */

public class DialogueAct 
{
	public enum Length {SHORT, NORMAL, LONG}
	
	private String utterance;
	private String taggedUtterance;
	private String[] tags;
	private long starttime;
	
	private boolean positive = false;
	private boolean negative = false;
	private boolean agree = false;
	private boolean disagree = false;
	private boolean about_other_people = false;
	private boolean about_other_character = false;
	private boolean about_current_character = false;
	private boolean about_own_feelings = false;
	private boolean pragmatic = false;
	private boolean talk_about_self = false;
	private boolean future = false;
	private boolean past = false;
	private boolean event = false;
	private boolean action = false;
	private boolean laugh = false;
	private boolean change_speaker = false;
	private Length length = Length.NORMAL; 
	private String target_character;
	
	
	/**
	 * Creates a new DialogueAct with the given utterance
	 * @param utterance
	 */
	public DialogueAct( String utterance, long starttime )
	{
		this.utterance = utterance;
		this.starttime = starttime;
	}
	
	/**
	 * Creates a new DialogueAct which combines the two given acts.
	 * @param act1
	 * @param act2
	 */
	public DialogueAct( DialogueAct act1, DialogueAct act2 )
	{
		utterance = act1.getUtterance() + " " + act2.getUtterance();
		starttime = Math.min(act1.getStarttime(), act2.getStarttime());
		positive = (act1.isPositive() || act2.isPositive());
		negative = (act1.isNegative() || act2.isNegative());
		agree = (act1.isAgree() || act2.isAgree());
		disagree = (act1.isDisagree() || act2.isDisagree());
		about_other_people = (act1.isAboutOtherPeople() || act2.isAboutOtherPeople());
		about_other_character = (act1.isAboutOtherCharacter() || act2.isAboutOtherCharacter());
		about_current_character = (act1.isAboutCurrentCharacter() || act2.isAboutCurrentCharacter());
		about_own_feelings = (act1.isAboutOwnFeelings() || act2.isAboutOwnFeelings());
		pragmatic = (act1.isPragmatic() || act2.isPragmatic());
		talk_about_self = (act1.isTalkAboutSelf() || act2.isTalkAboutSelf());
		future = (act1.isFuture() || act2.isFuture());
		past = (act1.isPast() || act2.isPast());
		event = (act1.isEvent() || act2.isEvent());
		action = (act1.isAction() || act2.isAction());
		laugh = (act1.isLaugh() || act2.isLaugh());
		if( act1.getLength() == Length.LONG && act2.getLength() == Length.LONG ) {
			length = Length.LONG;
		} else {
			length = Length.NORMAL;
		}
		change_speaker = (act1.isChangeSpeaker() || act2.isChangeSpeaker());
		
		target_character = act1.getTargetCharacter() + " " + act2.getTargetCharacter();
	}

	/**
	 * @return the utterance
	 */
	public String getUtterance() {
		return utterance;
	}

	/**
	 * @param utterance the utterance to set
	 */
	public void setUtterance(String utterance) {
		this.utterance = utterance;
	}

	/**
	 * @return the taggedUtterance
	 */
	public String getTaggedUtterance() {
		return taggedUtterance;
	}

	/**
	 * @param taggedUtterance the taggedUtterance to set
	 */
	public void setTaggedUtterance(String taggedUtterance) {
		this.taggedUtterance = taggedUtterance;
	}

	/**
	 * @return the tags
	 */
	public String[] getTags() {
		return tags;
	}

	/**
	 * @param tags the tags to set
	 */
	public void setTags(String[] tags) {
		this.tags = tags;
	}

	/**
	 * @return the positive
	 */
	public boolean isPositive() {
		return positive;
	}

	/**
	 * @param positive the positive to set
	 */
	public void setPositive(boolean positive) {
		this.positive = positive;
	}

	/**
	 * @return the negative
	 */
	public boolean isNegative() {
		return negative;
	}

	/**
	 * @param negative the negative to set
	 */
	public void setNegative(boolean negative) {
		this.negative = negative;
	}

	/**
	 * @return the agree
	 */
	public boolean isAgree() {
		return agree;
	}

	/**
	 * @param agree the agree to set
	 */
	public void setAgree(boolean agree) {
		this.agree = agree;
	}

	/**
	 * @return the disagree
	 */
	public boolean isDisagree() {
		return disagree;
	}

	/**
	 * @param disagree the disagree to set
	 */
	public void setDisagree(boolean disagree) {
		this.disagree = disagree;
	}

	/**
	 * @return the about_other_people
	 */
	public boolean isAboutOtherPeople() {
		return about_other_people;
	}

	/**
	 * @param about_other_people the about_other_people to set
	 */
	public void setAboutOtherPeople(boolean about_other_people) {
		this.about_other_people = about_other_people;
	}

	/**
	 * @return the about_other_character
	 */
	public boolean isAboutOtherCharacter() {
		return about_other_character;
	}

	/**
	 * @param about_other_character the about_other_character to set
	 */
	public void setAboutOtherCharacter(boolean about_other_character) {
		this.about_other_character = about_other_character;
	}

	/**
	 * @return the about_current_character
	 */
	public boolean isAboutCurrentCharacter() {
		return about_current_character;
	}

	/**
	 * @param about_current_character the about_current_character to set
	 */
	public void setAboutCurrentCharacter(boolean about_current_character) {
		this.about_current_character = about_current_character;
	}

	/**
	 * @return the about_own_feelings
	 */
	public boolean isAboutOwnFeelings() {
		return about_own_feelings;
	}

	/**
	 * @param about_own_feelings the about_own_feelings to set
	 */
	public void setAboutOwnFeelings(boolean about_own_feelings) {
		this.about_own_feelings = about_own_feelings;
	}

	/**
	 * @return the pragmatic
	 */
	public boolean isPragmatic() {
		return pragmatic;
	}

	/**
	 * @param pragmatic the pragmatic to set
	 */
	public void setPragmatic(boolean pragmatic) {
		this.pragmatic = pragmatic;
	}

	/**
	 * @return the talk_about_self
	 */
	public boolean isTalkAboutSelf() {
		return talk_about_self;
	}

	/**
	 * @param talk_about_self the talk_about_self to set
	 */
	public void setTalkAboutSelf(boolean talk_about_self) {
		this.talk_about_self = talk_about_self;
	}

	/**
	 * @return the future
	 */
	public boolean isFuture() {
		return future;
	}

	/**
	 * @param future the future to set
	 */
	public void setFuture(boolean future) {
		this.future = future;
	}

	/**
	 * @return the past
	 */
	public boolean isPast() {
		return past;
	}

	/**
	 * @param past the past to set
	 */
	public void setPast(boolean past) {
		this.past = past;
	}

	/**
	 * @return the event
	 */
	public boolean isEvent() {
		return event;
	}

	/**
	 * @param event the event to set
	 */
	public void setEvent(boolean event) {
		this.event = event;
	}

	/**
	 * @return the action
	 */
	public boolean isAction() {
		return action;
	}

	/**
	 * @param action the action to set
	 */
	public void setAction(boolean action) {
		this.action = action;
	}

	/**
	 * @return the laugh
	 */
	public boolean isLaugh() {
		return laugh;
	}

	/**
	 * @param laugh the laugh to set
	 */
	public void setLaugh(boolean laugh) {
		this.laugh = laugh;
	}
	
	/**
	 * @return change_speaker
	 */
	public boolean isChangeSpeaker() {
		return change_speaker;
	}

	/**
	 * @param change_speaker
	 */
	public void setChangeSpeaker(boolean change_speaker) {
		this.change_speaker = change_speaker;
	}
	
	/**
	 * @return change_speaker
	 */
	public String getTargetCharacter() {
		return target_character;
	}

	/**
	 * @param change_speaker
	 */
	public void setTargetCharacter(String target_character) {
		this.target_character = target_character;
	}
	
	public long getStarttime()
	{
		return starttime;
	}
	
	/**
	 * @return the length
	 */
	public Length getLength() {
		return length;
	}

	/**
	 * @param length the length to set
	 */
	public void setLength(Length length) {
		this.length = length;
	}

	public String getFeatures()
	{
		String features = "";
		if( positive ) features += " is_positive";
		if( negative ) features += " is_negative";
		if( agree ) features += " is_agree";
		if( disagree ) features += " is_disagree";
		if( about_other_people ) features += " about_other_people";
		if( about_other_character ) features += " about_other_character";
		if( about_current_character ) features += " about_current_character";
		if( about_own_feelings ) features += " about_own_feelings";
		if( talk_about_self ) features += " talk_about_self";
		if( pragmatic ) features += " pragmatic";
		if( future ) features += " future";
		if( past ) features += " past";
		if( action ) features += " action";
		if( event ) features += " event";
		if( laugh ) features += " laugh";
		if( change_speaker ) features += " change_speaker";
		if( length == Length.SHORT ) features += " short";
		if( length == Length.LONG ) features += " long";
		if( target_character != null && target_character.length() != 0 ) features += " target:" + target_character;
		
		return features;
	}
	
	/**
	 * Returns a string representation of this act
	 */
	public String toString()
	{
		String str = "+++ ";
		str += utterance + " ("+starttime+")\r\n";
		str += taggedUtterance + "\r\n";
		str += "positive:		" + positive + "\r\n";
		str += "negative:		" + negative + "\r\n";
		str += "agree:			" + agree + "\r\n";
		str += "disagree:		" + disagree + "\r\n";
		str += "about other people:	" + about_other_people + "\r\n";
		str += "about other character:	" + about_other_character + "\r\n";
		str += "about current character:" + about_current_character + "\r\n";
		str += "about own feelings:	" + about_own_feelings + "\r\n";
		str += "talk about self:	" + talk_about_self + "\r\n";
		str += "pragmatic:		" + pragmatic + "\r\n";
		str += "future:			" + future + "\r\n";
		str += "past:			" + past + "\r\n";
		str += "action:			" + action + "\r\n";
		str += "event:			" + event + "\r\n";
		str += "laugh:			" + laugh + "\r\n";
		str += "change speaker:	" + change_speaker + "\r\n";
		str += "length: " + length + "\r\n";
		str += "target speaker:	" + target_character + "\r\n";
		return str;
	}
}
