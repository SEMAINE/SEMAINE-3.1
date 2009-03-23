package eu.semaine.examples.swimmer;

import javax.jms.JMSException;
import javax.swing.Icon;
import javax.swing.ImageIcon;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JLayeredPane;
import javax.swing.JOptionPane;

import eu.semaine.components.Component;
import eu.semaine.jms.message.SEMAINEMessage;
import eu.semaine.jms.receiver.Receiver;

public class SwimmerDisplay extends Component {
	private JFrame frame;
	private JLabel swimmer;
	private int availableWidth;
	private int yMid;
	
	public SwimmerDisplay()	throws JMSException {
		super("SwimmerDisplay", false, true/*is output*/);
		receivers.add(new Receiver("semaine.data.swimmer.position"));
		setupGUI();
	}
	
	
	@Override protected void react(SEMAINEMessage m) throws JMSException {
		float percent = Float.parseFloat(m.getText());
		updateSwimmerPosition(percent);
		String message = percent <= 0 ? "You lost!" : percent >= 100 ? "You won!!!" : null;
		if (message != null) {
			JOptionPane.showMessageDialog(frame, message, "Game Over", JOptionPane.INFORMATION_MESSAGE);
			System.exit(0);
		}
	}

	
	private void setupGUI() {
		frame  = new JFrame("Swimmer game");
		JLayeredPane layeredPane = new JLayeredPane();
		layeredPane.setOpaque(true);
		frame.setContentPane(layeredPane);
		Icon waterIcon = new ImageIcon(this.getClass().getResource("water.png"));
		int width = waterIcon.getIconWidth(), height = waterIcon.getIconHeight();
		JLabel water = new JLabel(waterIcon);
        water.setBounds(0, 0, width, height);
		layeredPane.add(water, 0);
		layeredPane.setPreferredSize(water.getPreferredSize());
		ImageIcon swimmerIcon = new ImageIcon(this.getClass().getResource("swimmer.png"));
		swimmer = new JLabel(swimmerIcon);
		int swimmerWidth = swimmerIcon.getIconWidth(), swimmerHeight = swimmerIcon.getIconHeight();
		yMid = (height-swimmerHeight) / 2;
		availableWidth = width - 50 - swimmerWidth;
		swimmer.setBounds(availableWidth / 2, yMid, swimmerWidth, swimmerHeight);
		layeredPane.add(swimmer, new Integer(1));
		frame.pack();
		frame.setVisible(true);
	}
	
	private void updateSwimmerPosition(float percent) {
		swimmer.setLocation((int) (availableWidth*percent/100), yMid);
	}
}
