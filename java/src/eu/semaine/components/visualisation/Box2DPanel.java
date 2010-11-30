package eu.semaine.components.visualisation;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.util.concurrent.ArrayBlockingQueue;

import javax.swing.JPanel;

public class Box2DPanel extends JPanel
{
	private static int DOT_SIZE = 10;
	
	private String xLabel;
	private String yLabel;
	private int width;
	private int height;
	private int nrOfPoints;
	
	private ArrayBlockingQueue<Integer> xPoints;
	private ArrayBlockingQueue<Integer> yPoints;

	public Box2DPanel( String xLabel, String yLabel, int width, int height, int nrOfPoints )
	{
		this.xLabel = xLabel;
		this.yLabel = yLabel;
		this.width = width;
		this.height = height;
		this.nrOfPoints = nrOfPoints;

		this.setPreferredSize(new Dimension(width+2,height+2));
		
		xPoints = new ArrayBlockingQueue<Integer>(nrOfPoints);
		yPoints = new ArrayBlockingQueue<Integer>(nrOfPoints);
	}

	public void paint( Graphics g )
	{
		Graphics2D g2d = (Graphics2D) g;

		/* paint box */
		g2d.setColor(Color.WHITE);
		g2d.fillRect(0, 0, width, height);
		g2d.setColor(Color.BLACK);
		g2d.drawRect(0, 0, width, height);
		g2d.drawLine(0, height/2, width, height/2);
		g2d.drawLine(width/2, 0, width/2, height);
		
		g2d.setFont(new Font("Helvetica", Font.PLAIN,  10));
		g2d.setStroke(new BasicStroke(1, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
		g2d.drawString(xLabel, width/2+10, height/2+10);
		g2d.drawString(yLabel, width/2+1, 10);
		
		Integer[] xArray = (Integer[])xPoints.toArray(new Integer[nrOfPoints]);
		Integer[] yArray = (Integer[])yPoints.toArray(new Integer[nrOfPoints]);
		g2d.setColor(Color.RED);
		for( int i=0; i<xArray.length; i++ ) {
			if( xArray[i] != null && yArray[i] != null ) {
				if( i == xArray.length-1 ) {
					g2d.setColor(Color.GREEN);
				}
				g2d.fillOval(xArray[i]-(DOT_SIZE/2), yArray[i]-(DOT_SIZE/2), DOT_SIZE, DOT_SIZE);
			}
		}
	}
	
	/**
	 * Add a point to the box
	 * @param xValue the x-value, must be between -1 and 1
	 * @param yValue the y-value, must be between -1 and 1
	 */
	public void addPoint( float xValue, float yValue )
	{
		try {
			if( xPoints.remainingCapacity() == 0 ) {
				xPoints.take();
				yPoints.take();
			}
			xPoints.put(new Float((xValue*width/2) + (width/2)).intValue());
			yPoints.put(new Float((yValue*height/2) + (height/2)).intValue());
		} catch( InterruptedException e ) {
			e.printStackTrace();
		}
		repaint();
	}
}
