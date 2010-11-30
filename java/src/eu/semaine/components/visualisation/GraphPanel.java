package eu.semaine.components.visualisation;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.util.ArrayList;
import java.util.concurrent.ArrayBlockingQueue;

import javax.swing.JPanel;

public class GraphPanel extends JPanel
{
	private static int TEXT_WIDTH = 90;

	public String name;

	private ArrayList<ArrayBlockingQueue<Float>> queues = new ArrayList<ArrayBlockingQueue<Float>>();
	private ArrayList<Color> colours = new ArrayList<Color>();

	public GraphPanel(String name)
	{
		ArrayBlockingQueue<Float> queue = new ArrayBlockingQueue<Float>(500);
		queues.add(queue);
		colours.add(Color.RED);
		this.name = name;
		initQueues();
	}

	public GraphPanel( String name, int nrOfLines, ArrayList<Color> colours )
	{
		this.name = name;
		for( int i=0; i<nrOfLines; i++ ) {
			ArrayBlockingQueue<Float> queue = new ArrayBlockingQueue<Float>(500);
			queues.add(queue);
			this.colours.add(colours.get(i));
		}
		initQueues();
	}

	public void initQueues()
	{
		for( ArrayBlockingQueue<Float> queue : queues ) {
			while( queue.remainingCapacity() > 0 ) {
				try {
					queue.put(0f);
				} catch( InterruptedException e ) {
					e.printStackTrace();
				}
			}
		}
	}

	public void paint( Graphics g )
	{
		Graphics2D g2d = (Graphics2D)g;
		g2d.setBackground(Color.WHITE);
		g2d.setColor(Color.WHITE);
		g2d.fillRect(TEXT_WIDTH, 0, 500+TEXT_WIDTH, 100);
		g2d.setColor(Color.BLACK);
		g2d.drawRect(TEXT_WIDTH, 0, 500+TEXT_WIDTH, 101);
		g2d.clearRect(0, 0, 90, 100);
		g2d.drawString(name, 0, 50);
		drawLines(g2d);
	}

	public void drawLines( Graphics2D g2d )
	{
		for( int j=0; j<queues.size(); j++ ) {
			ArrayBlockingQueue<Float> queue = queues.get(j);
			g2d.setColor(colours.get(j));
			//		Float[] elements = new Float[500];
			Float[] elements = (Float[])queue.toArray(new Float[500]);
			for( int i=2; i<elements.length; i++ ) {
				try {
					g2d.drawLine(i+TEXT_WIDTH-1, Math.min(100-((int)(elements[i-1]*100)),100), i+TEXT_WIDTH, Math.min(100-((int)(elements[i]*100)),100));
				}catch( Exception e ) {
					System.out.println("i: " + i + ", elements[i-1]: " + elements[i-1] + "elements[i]: " + elements[i]);
				}
			}
		}
	}
	
	public void addValue(float value)
	{
		addValue(value, 0);
	}
	
	public void addValue( float value, int queueNr )
	{
		ArrayBlockingQueue<Float> queue = queues.get(queueNr);
		try {
			if( queue.remainingCapacity() == 0 ) {
				queue.take();
			}
			queue.put(value);
		} catch( InterruptedException e ) {
			e.printStackTrace();
		}
		repaint();
		queues.set(queueNr, queue);
	}
}
