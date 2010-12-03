/**
 * Copyright (C) 2010 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.util;

import org.junit.Test;

/**
 * @author marc
 *
 */
public class XMLToolTest {

	public static final String XPATHEXAMPLE_VALID = "/emma:emma/emma:interpretation/semaine:face-present/@statusChange";
	public static final String XPATHEXAMPLE_INVALID_STRUCTURE = "/emma:emma/emma:/semaine:face-present/@statusChange";
	public static final String XPATHEXAMPLE_UNKNOWN_PREFIX = "/emma:emma/wrong:interpretation/semaine:face-present/@statusChange";

	@Test
	public void xpath2doc_shouldSucceed() {
		XMLTool.xpath2doc(XPATHEXAMPLE_VALID, null, null, null);
	}
	
	@Test(expected = NullPointerException.class)
	public void xpath2doc_shouldThrowNPE() {
		XMLTool.xpath2doc(null, null, null, null);
	}
	

	@Test(expected = IllegalArgumentException.class)
	public void xpath2doc_shouldThrowIAE1() {
		XMLTool.xpath2doc(XPATHEXAMPLE_INVALID_STRUCTURE, null, null, null);
	}

	@Test(expected = IllegalArgumentException.class)
	public void xpath2doc_shouldThrowIAE2() {
		XMLTool.xpath2doc(XPATHEXAMPLE_UNKNOWN_PREFIX, null, null, null);
	}

	
}
