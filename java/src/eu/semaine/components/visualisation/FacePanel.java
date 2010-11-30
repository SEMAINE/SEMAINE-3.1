package eu.semaine.components.visualisation;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;

import javax.swing.JPanel;

public class FacePanel extends JPanel
{
	private int width;
	private int height;
	
	private int xpos = 15;
	private int ypos = 15;
	private int faceWidth = 20;
	private int faceHeight = 20;
	private double rotation = 0;
	
	public FacePanel( int width, int height )
	{
		this.width = width;
		this.height = height;
		this.setPreferredSize(new Dimension(width+2,height+2));
	}
	
	public void paint( Graphics g )
	{
		Graphics2D g2d = (Graphics2D)g;
		g2d.setColor(Color.WHITE);
		g2d.fillRect(0, 0, width, height);
		g2d.setColor(Color.BLACK);
		g2d.drawRect(0, 0, width, height);

		g2d.rotate(deg2rad(rotation), xpos+(faceWidth/2), ypos+(faceHeight/2));
//		g2d.rotate(rotation);
		
		g2d.setColor(Color.BLACK);
		g2d.drawRect(xpos, ypos, faceWidth, faceHeight);
	}
	
	public int getWidth() {
		return width;
	}

	public void setWidth(int width) {
		this.width = width;
	}

	public int getHeight() {
		return height;
	}

	public void setHeight(int height) {
		this.height = height;
	}

	public int getXpos() {
		return xpos;
	}

	public void setXpos(int xpos) {
		this.xpos = xpos;
	}

	public int getYpos() {
		return ypos;
	}

	public void setYpos(int ypos) {
		this.ypos = ypos;
	}

	public int getFaceWidth() {
		return faceWidth;
	}

	public void setFaceWidth(int faceWidth) {
		this.faceWidth = faceWidth;
	}

	public int getFaceHeight() {
		return faceHeight;
	}

	public void setFaceHeight(int faceHeight) {
		this.faceHeight = faceHeight;
	}

	public double getRotation() {
		return rotation;
	}

	public void setRotation(double rotation) {
		this.rotation = rotation;
	}

	public double deg2rad( double deg )
	{
		return deg/360*2*Math.PI;
	}
}
