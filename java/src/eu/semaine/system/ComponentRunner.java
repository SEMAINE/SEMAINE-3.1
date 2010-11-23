/**
 * Copyright (C) 2008 DFKI GmbH. All rights reserved.
 * Use is subject to license terms -- see license.txt.
 */
package eu.semaine.system;

import java.io.FileInputStream;
import java.io.IOException;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.log4j.BasicConfigurator;
import org.apache.log4j.ConsoleAppender;
import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.apache.log4j.PatternLayout;

import eu.semaine.components.Component;
import eu.semaine.exceptions.SystemConfigurationException;
import eu.semaine.jms.IOBase;

/**
 * @author marc
 *
 */
public class ComponentRunner 
{
	private Log log;
	private List<Component> components;
	
	@SuppressWarnings("unchecked")
	public ComponentRunner(String configFile)
	throws SystemConfigurationException, IOException, 
		ClassNotFoundException, NoSuchMethodException, IllegalAccessException,
		InstantiationException, InvocationTargetException
	{
		log = LogFactory.getLog(ComponentRunner.class);
		components = new ArrayList<Component>();
		Properties p = new Properties();
		p.load(new FileInputStream(configFile));
		// make config settings available in the entire system, as system properties:
		System.getProperties().putAll(p);
		// Also remember the config file itself:
		System.getProperties().setProperty("semaine.config-file", configFile);
		
		// Start embedded ActiveMQ broker?
		// This is on by default, and can be switched off by setting the config entry to "false":
		if (Boolean.parseBoolean(System.getProperty("semaine.use.embedded.broker", "true"))) {
			try {
				IOBase.useEmbeddedBroker();
			} catch (Exception e) {
				throw new SystemConfigurationException("Cannot start embedded ActiveMQ broker: ", e);
			}
		}
		
		
		String[] componentClassDescriptions = p.getProperty("semaine.components").split("\\|");
		for (String d : componentClassDescriptions) {
			// component class descriptions are expected to have one of the following formats:
			// 1. my.class.which.extends.Component
			// 2. my.class.which.extends.Component(any, string, arguments)
			// 3. my.class.which.extends.Component(arguments, $my.special.property, other, arguments)
			//    where 'my.special.property' is a property in the property file.
			d = d.trim();
			if (d.equals("")) continue;
			String[] args = null;
			String className = null;
			if (d.contains("(")) { // arguments
				if (!d.contains(")")) {
					throw new SystemConfigurationException("Error in config file '"+configFile+"', property 'semaine.components':\n"
							+"The following component entry contains an open bracket but no closing bracket:\n"
							+"'"+d+"'");
				}
				className = d.substring(0, d.indexOf('(')).trim();
				args = d.substring(d.indexOf('(')+1, d.indexOf(')')).split("\\s*,\\s*");
				for (int i=0; i<args.length; i++) {
					if (args[i].startsWith("$")) {
						// replace value with content of property named after the $
						args[i] = p.getProperty(args[i].substring(1));
					}
				}
			} else { // no arguments
				className = d;
			}
			log.info("Now initiating class '"+className+"'");
			try {
				Class<? extends Component> theClass = Class.forName(className).asSubclass(Component.class);
				Component component = null;
				// Now invoke Constructor with args.length String arguments
				if (args != null) {
					Class<String>[] constructorArgTypes = new Class[args.length];
					Object[] constructorArgs = new Object[args.length];
					for (int i=0; i<args.length; i++) {
						constructorArgTypes[i] = String.class;
						constructorArgs[i] = args[i];
					}
					Constructor<? extends Component> constructor = (Constructor<? extends Component>) theClass.getConstructor(constructorArgTypes);
					component = constructor.newInstance(constructorArgs);
				} else {
					component = theClass.newInstance();
				}
				components.add(component);
			} catch (ClassCastException e) {
				throw new SystemConfigurationException("Error in config file '"+configFile+"', property 'semaine.components':\n"
						+"The following entry represents a class which is not a subclass of eu.semaine.components.Component:\n"
						+"'"+d+"'", e);
			} catch (Throwable e) {
				throw new SystemConfigurationException("Error in config file '"+configFile+"', property 'semaine.components':\n"
						+"The following entry cannot be instantiated:\n"
						+"'"+d+"'", e);
			}
		}
		addShutdownHook();
	}

	private void addShutdownHook() {
		// When shutting down, 
		/*
		Runtime.getRuntime().addShutdownHook(new Thread() {
			public void run() {
				log.info("Shutting down components...");
				for (Component c : components) {
					c.requestExit();
				}
				for (Component c : components) {
					try {
						c.join();
						log.info("   ... "+c.getClass().getSimpleName()+" shut down.");
					} catch (InterruptedException ie) {
						ie.printStackTrace(System.err);
					}
				}
			}
		});
		*/
	}
	
	/**
	 * Start all components, and return.
	 */
	public void go()
	{
		for (Component c : components) {
			c.start();
		}
	}
	
	/**
	 * Block until all components exit. This may never return.
	 */
	public void waitUntilCompleted()
	{
		for (Component c : components) {
			try {
				c.join();
			} catch (InterruptedException ie) {}
		}
		
	}
	
	/**
	 * @param args
	 */
	public static void main(String[] args)
	throws Exception
	{
		if (args.length == 0) {
			System.err.println("Usage:");
			System.err.println("java eu.semaine.system.ComponentRunner configfile");
			System.err.println();
			System.err.println("where configfile defines which components to start");
		}
		PatternLayout layout = new PatternLayout("%d [%t] %-5p %-10c %m\n");
		BasicConfigurator.configure(new ConsoleAppender(layout));
		Logger.getRootLogger().setLevel(Level.DEBUG);
		Logger.getLogger("org.apache").setLevel(Level.INFO);
		try {
			ComponentRunner runner = new ComponentRunner(args[0]);
			runner.go();
		} catch (Exception e) {
			System.err.println("Cannot start: ");
			e.printStackTrace();
			System.exit(1);
		}
	}

}
