/**
 * Copyright (C) 2010 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.util;

import org.junit.Test;
import static org.junit.Assert.*;

/**
 * @author marc
 *
 */
public class SEMAINEUtilsTest {
	
	@Test
	public void getLongProperty_shouldYieldProperty() {
		// setup
		long expected = 12345;
		String key = "SEMAINEUtilsTest.dummy1";
		System.setProperty(key, String.valueOf(expected));
		// exercise
		long actual = SEMAINEUtils.getLongProperty(key, 0);
		// verify
		assertEquals(expected, actual);
		// teardown
		System.clearProperty(key);
	}
	
	@Test
	public void getLongProperty_shouldYieldFallback1() {
		// setup
		long expected = 12345;
		String key = "SEMAINEUtilsTest.dummy1";
		System.clearProperty(key); // just to be sure
		// exercise
		long actual = SEMAINEUtils.getLongProperty(key, expected);
		// verify
		assertEquals(expected, actual);
	}

	@Test
	public void getLongProperty_shouldYieldFallback2() {
		// setup
		long expected = 12345;
		String key = "SEMAINEUtilsTest.dummy1";
		System.setProperty(key, "not a long");
		// exercise
		long actual = SEMAINEUtils.getLongProperty(key, expected);
		// verify
		assertEquals(expected, actual);
	}

	@Test
	public void parseFloat_shouldConvert() {
		// setup
		float expected = 12.345f;
		// exercise
		float actual = SEMAINEUtils.parseFloat(String.valueOf(expected), 0);
		// verify
		assertEquals(expected, actual, 1.e-7);
	}
	
	@Test
	public void parseFloat_shouldUseFallback() {
		// setup
		float expected = 12.345f;
		// exercise
		float actual = SEMAINEUtils.parseFloat("not a number", expected);
		// verify
		assertEquals(expected, actual, 1.e-7);
	}

	@Test
	public void parseLong_shouldConvert() {
		// setup
		long expected = 12345;
		// exercise
		long actual = SEMAINEUtils.parseLong(String.valueOf(expected), 0);
		// verify
		assertEquals(expected, actual);
	}
	
	@Test
	public void parseLong_shouldUseFallback() {
		// setup
		long expected = 12345;
		// exercise
		long actual = SEMAINEUtils.parseLong("not a number", expected);
		// verify
		assertEquals(expected, actual);
	}

}
