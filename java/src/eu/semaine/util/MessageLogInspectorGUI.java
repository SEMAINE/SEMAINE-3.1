/*
 * MessageLogInspectorGUI.java
 *
 * Created on 26. November 2009, 08:12
 */

package eu.semaine.util;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeSet;

import javax.swing.JComboBox;
import javax.swing.ListSelectionModel;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;
import javax.swing.table.DefaultTableModel;

/**
 *
 * @author  marc
 */
public class MessageLogInspectorGUI extends javax.swing.JFrame {
    class LogEntry {
        LogEntry(String topic, String text) {
            this.topic = topic;
            this.text = text;
        }
        final String topic;
        final String text;
    }
    
    private List<LogEntry> logEntries = new ArrayList<LogEntry>();
    private Set<String> topicNames = new TreeSet<String>();
    
    /** Creates new form MessageLogInspectorGUI */
    public MessageLogInspectorGUI(File logfile) throws IOException {
        parseLogFile(logfile);
        initComponents();
        customInitComponents();
    }
    
    private void parseLogFile(File logfile) throws IOException {
        BufferedReader br = new BufferedReader(new InputStreamReader(new FileInputStream(logfile), "UTF-8"));
        String line = null;
        String topic = null;
        StringBuilder buf = new StringBuilder();
        while ((line = br.readLine()) != null) {
            if (line.startsWith("DEBUG semaine.log.")
                || line.startsWith("INFO  semaine.log.")
                || line.startsWith("WARN  semaine.log.")
                || line.startsWith("ERROR semaine.log.")) {
                // New section starts
                if (topic != null && buf.length() > 0) {
                    topicNames.add(topic);
                    logEntries.add(new LogEntry(topic, buf.toString()));
                }
                topic = null;
                buf.setLength(0);
                
                if (line.startsWith("DEBUG semaine.log.MessageLog")) {
                    // It is a message
                    int topicStart = line.indexOf("topic://") + "topic://".length();
                    if (line.startsWith("semaine.data.", topicStart)) {
                        topicStart += "semaine.data.".length();
                    }
                    int topicEnd = line.indexOf(' ', topicStart);
                    topic = line.substring(topicStart, topicEnd);
                    buf.append(line).append("\n");
                }
            } else {
                buf.append(line).append("\n");
            }
        }
        if (topic != null && buf.length() > 0) {
            topicNames.add(topic);
            logEntries.add(new LogEntry(topic, buf.toString()));
        }
    }
    
    
    private void customInitComponents() {
        int i=1;
        for (JComboBox combo : new JComboBox[] {cbColumn1, cbColumn2, cbColumn3, cbColumn4, cbColumn5}) {
            for (String topic : topicNames) {
                combo.addItem(topic);
            }
            if (combo.getItemCount() > i) {
                combo.setSelectedIndex(i);
            } else {
                combo.setSelectedIndex(0);
            } 
            i++;
        }
        
        resetTable();

        topicTable.getSelectionModel().addListSelectionListener(new ListSelectionListener() {
            public void valueChanged(ListSelectionEvent e) {
                if (e.getValueIsAdjusting()) { // wait until finished adjusting
                    return;
                }
                ListSelectionModel lsm = (ListSelectionModel)e.getSource();
                int index = lsm.getLeadSelectionIndex();
                updateDetailedView(index);
            }
        });

    }

    
    private void resetTable() {
        ArrayList<String> selectedTopics = new ArrayList<String>();
        for (JComboBox combo : new JComboBox[] {cbColumn1, cbColumn2, cbColumn3, cbColumn4, cbColumn5}) {
            String selectedTopic = (String) combo.getSelectedItem();
            if (!"(none)".equals(selectedTopic)) {
                selectedTopics.add(selectedTopic);
            }
        }
        Map<String, Integer> topic2index = new HashMap<String, Integer>();
        int numTopics = selectedTopics.size();
        for (int i=0; i<numTopics; i++) {
            topic2index.put(selectedTopics.get(i), i);
        }
        ArrayList<String[]> tableRows = new ArrayList<String[]>();
        for (LogEntry e : logEntries) {
            if (topic2index.containsKey(e.topic)) {
                int index = topic2index.get(e.topic);
                String[] row = new String[numTopics];
                row[index] = e.text;
                tableRows.add(row);
            }
        }
        
        topicTable.setModel(new DefaultTableModel(
            tableRows.toArray(new String[0][]),
            selectedTopics.toArray(new String[0])) {
                public boolean isCellEditable(int rowIndex, int columnIndex) {
                    return false;
                }
        });
        topicTable.revalidate();
    }
    
    /**
     * Update the detailed view with the data contained in the given row of the topic table.
     * @param rowNumber 
     */
    private void updateDetailedView(int rowNumber) {
        String value = null;
        for (int i=0; i<topicTable.getColumnCount(); i++) {
            value = (String) topicTable.getValueAt(rowNumber, i);
            if (value != null) {
                System.out.println("In row "+rowNumber+", found non-null value in column "+i);
                break;
            }
        }
        if (value != null) {
            tpDetail.setText(value);
            tpDetail.setCaretPosition(0);
        }
    }
    
    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc=" Generated Code ">//GEN-BEGIN:initComponents
    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;

        pSelectTopics = new javax.swing.JPanel();
        cbColumn1 = new javax.swing.JComboBox();
        cbColumn2 = new javax.swing.JComboBox();
        cbColumn3 = new javax.swing.JComboBox();
        cbColumn4 = new javax.swing.JComboBox();
        cbColumn5 = new javax.swing.JComboBox();
        bUpdate = new javax.swing.JButton();
        tablePanel = new javax.swing.JPanel();
        jScrollPane1 = new javax.swing.JScrollPane();
        topicTable = new javax.swing.JTable();
        detailPanel = new javax.swing.JPanel();
        jScrollPane2 = new javax.swing.JScrollPane();
        tpDetail = new javax.swing.JTextPane();

        getContentPane().setLayout(new java.awt.GridBagLayout());

        setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);
        addWindowListener(new java.awt.event.WindowAdapter() {
            public void windowClosed(java.awt.event.WindowEvent evt) {
                formWindowClosed(evt);
            }
        });

        pSelectTopics.setLayout(new javax.swing.BoxLayout(pSelectTopics, javax.swing.BoxLayout.Y_AXIS));

        pSelectTopics.setBorder(javax.swing.BorderFactory.createTitledBorder(javax.swing.BorderFactory.createEtchedBorder(), "Select Topics to display"));
        pSelectTopics.setPreferredSize(new java.awt.Dimension(300, 200));
        cbColumn1.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "(none)" }));
        pSelectTopics.add(cbColumn1);

        cbColumn2.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "(none)" }));
        pSelectTopics.add(cbColumn2);

        cbColumn3.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "(none)" }));
        pSelectTopics.add(cbColumn3);

        cbColumn4.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "(none)" }));
        pSelectTopics.add(cbColumn4);

        cbColumn5.setModel(new javax.swing.DefaultComboBoxModel(new String[] { "(none)" }));
        pSelectTopics.add(cbColumn5);

        bUpdate.setText("Update");
        bUpdate.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                bUpdateActionPerformed(evt);
            }
        });

        pSelectTopics.add(bUpdate);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.weightx = 0.3;
        gridBagConstraints.weighty = 0.1;
        getContentPane().add(pSelectTopics, gridBagConstraints);

        tablePanel.setLayout(new java.awt.BorderLayout());

        tablePanel.setPreferredSize(new java.awt.Dimension(300, 500));
        topicTable.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {

            },
            new String [] {
                "", "", "", "", ""
            }
        ) {
            boolean[] canEdit = new boolean [] {
                false, false, false, false, false
            };

            public boolean isCellEditable(int rowIndex, int columnIndex) {
                return canEdit [columnIndex];
            }
        });
        jScrollPane1.setViewportView(topicTable);

        tablePanel.add(jScrollPane1, java.awt.BorderLayout.CENTER);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 1;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.weightx = 0.3;
        gridBagConstraints.weighty = 0.9;
        getContentPane().add(tablePanel, gridBagConstraints);

        detailPanel.setLayout(new java.awt.BorderLayout());

        detailPanel.setBorder(javax.swing.BorderFactory.createEtchedBorder());
        detailPanel.setPreferredSize(new java.awt.Dimension(500, 800));
        jScrollPane2.setViewportView(tpDetail);

        detailPanel.add(jScrollPane2, java.awt.BorderLayout.CENTER);

        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridheight = 2;
        gridBagConstraints.fill = java.awt.GridBagConstraints.BOTH;
        gridBagConstraints.weightx = 0.7;
        gridBagConstraints.weighty = 1.0;
        getContentPane().add(detailPanel, gridBagConstraints);

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void formWindowClosed(java.awt.event.WindowEvent evt) {//GEN-FIRST:event_formWindowClosed
        System.exit(0);
    }//GEN-LAST:event_formWindowClosed

    private void bUpdateActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_bUpdateActionPerformed
        resetTable();
    }//GEN-LAST:event_bUpdateActionPerformed
    
    /**
     * @param args the command line arguments
     */
    public static void main(String args[]) throws Exception {
        String logfilename = args.length > 0 ? args[0] : "log.txt";
        final File logfile = new File(logfilename);
        if (!logfile.canRead()) {
            throw new IOException("Cannot read log file: "+logfile.getAbsolutePath());
        }
        new MessageLogInspectorGUI(logfile).setVisible(true);
    }
    
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton bUpdate;
    private javax.swing.JComboBox cbColumn1;
    private javax.swing.JComboBox cbColumn2;
    private javax.swing.JComboBox cbColumn3;
    private javax.swing.JComboBox cbColumn4;
    private javax.swing.JComboBox cbColumn5;
    private javax.swing.JPanel detailPanel;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JScrollPane jScrollPane2;
    private javax.swing.JPanel pSelectTopics;
    private javax.swing.JPanel tablePanel;
    private javax.swing.JTable topicTable;
    private javax.swing.JTextPane tpDetail;
    // End of variables declaration//GEN-END:variables
    
}
