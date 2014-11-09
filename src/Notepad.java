 

import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;
import java.io.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.text.*;
import javax.swing.text.JTextComponent.*;

public class Notepad extends JPanel
{
    JTextArea jta = new JTextArea("", 5, 40);
    JScrollPane jsp = new JScrollPane(jta);
    JMenuBar jmb = new JMenuBar();
    JMenu file = new JMenu("File");
    JMenu edit = new JMenu("Edit");
    JMenu help = new JMenu("Help");
    JMenuItem jmi;
    Clipboard clipbd = getToolkit().getSystemClipboard();

    public Notepad()
    {
        class newL implements ActionListener
        {
            public void actionPerformed(ActionEvent e)
            {
                jta.setDocument(new PlainDocument());
            }
        }

        class openL implements ActionListener
        {
            public void actionPerformed(ActionEvent e)
            {
                Frame frame = new Frame();
                FileDialog d = new FileDialog(frame, "Open File(s)");
                d.setFile("*.*");
                d.setDirectory(".");
                d.show();
                String file = d.getFile();
                if(file == null)
                {
                    return;
                }
                String directory = d.getDirectory();
                File f = new File(directory, file);
                if(f.exists())
                {
                    try
                    {
                        Reader in = new FileReader(f);
                        char[] buff = new char[4096];
                        int nch;
                        while((nch = in.read(buff, 0, buff.length)) != -1)
                        {
                            jta.setDocument(new PlainDocument());
                            jta.append(new String(buff, 0, nch));
                        }
                    }
                    catch (IOException io)
                    {
                        System.err.println("IOException: " + io.getMessage());
                    }
                }else{
                    System.out.println(directory + file + " does not exist.");
                }
            }
        }

        class saveL implements ActionListener
        {
            public void actionPerformed(ActionEvent e)
            {
                Frame frame = new Frame();
                FileDialog d = new FileDialog(frame, "", FileDialog.SAVE);
                d.setFile("*.*");
                d.setDirectory(".");
                d.show();

                String savefile = d.getFile();
                if(savefile == null)
                {
                    return;
                }
                else
                {
                    String directory = d.getDirectory();
                    File f = new File(directory, savefile);
                    String docToSave = jta.getText();
                    if(docToSave != null)
                    {
                        FileOutputStream fstrm = null;
                        BufferedOutputStream ostrm = null;
                        try
                        {
                            fstrm = new FileOutputStream(f);
                            ostrm = new BufferedOutputStream(fstrm);
                            byte[] bytes = null;
                            try
                            {
                                bytes = docToSave.getBytes();
                            }
                            catch(Exception e1)
                            {
                                e1.printStackTrace();
                            }
                            ostrm.write(bytes);
                        }
                       catch(IOException io)
                       {
                        System.err.println("IOException: "
                                + io.getMessage());
                       }
                       finally
                       {
                            try
                            {
                                ostrm.flush();
                                fstrm.close();
                                ostrm.close();
                            }
                            catch(IOException ioe)
                            {
                                System.err.println("IOException: " + ioe.getMessage());
                            }
                        }
                    }
                }
            }
        }

        class exitL implements ActionListener
        {
            public void actionPerformed(ActionEvent e)
            {
                System.exit(-1);
            }
        }

        class copyL implements ActionListener
        {
            public void actionPerformed(ActionEvent e)
            {
                String selection = jta.getSelectedText();
                StringSelection clipString = new StringSelection(selection);
                clipbd.setContents(clipString, clipString);
            }
        }

        class cutL implements ActionListener
        {
            public void actionPerformed(ActionEvent e)
            {
                String selection = jta.getSelectedText();
                StringSelection clipString = new StringSelection(selection);
                clipbd.setContents(clipString, clipString);
                jta.replaceRange("", jta.getSelectionStart(),
                        jta.getSelectionEnd());
            }
        }

        class pasteL implements ActionListener
        {
            public void actionPerformed(ActionEvent e)
            {
                Transferable clipData = clipbd.getContents(Notepad.this);
                try
                {
                    String clipString = (String)clipData.getTransferData(DataFlavor.stringFlavor);
                    jta.replaceRange(clipString, jta.getSelectionStart(), jta.getSelectionEnd());
                }
                catch(Exception ex)
                {
                }
            }
        }
        
        class DBParam implements ActionListener
        {
            public void actionPerformed(ActionEvent e)
            {
                openWebPage("http://www.soi.city.ac.uk/~andym/OKAPI-PACK/appendix-c.html");
            }
        }
        class envVar implements ActionListener
        {
            public void actionPerformed(ActionEvent e)
            {
                openWebPage("http://www.soi.city.ac.uk/~andym/OKAPI-PACK/appendix-b.html");
            }
        }
        class commInterface implements ActionListener
        {
            public void actionPerformed(ActionEvent e)
            {
                openWebPage("http://www.soi.city.ac.uk/~andym/OKAPI-PACK/appendix-j.html");
            }
        }        
        class jtaL implements ActionListener
        {
            public void actionPerformed(ActionEvent e)
            {
            }
        }

        file.add(jmi = new JMenuItem("New",KeyEvent.VK_N));
        jmi.addActionListener(new newL());
        file.add(jmi = new JMenuItem("Open",KeyEvent.VK_O));
        jmi.addActionListener(new openL());
        file.add(jmi = new JMenuItem("Save",KeyEvent.VK_S));
        jmi.addActionListener(new saveL());
        file.addSeparator();
        file.add(jmi = new JMenuItem("Exit",KeyEvent.VK_E));
        jmi.addActionListener(new exitL());

        edit.add(jmi = new JMenuItem("Copy",KeyEvent.VK_C));
        jmi.addActionListener(new copyL());
        edit.add(jmi = new JMenuItem("Cut",KeyEvent.VK_U));
        jmi.addActionListener(new cutL());
        edit.add(jmi = new JMenuItem("Paste",KeyEvent.VK_P));
        jmi.addActionListener(new pasteL());
        
        help.add(jmi = new JMenuItem("Database Parameter Files",KeyEvent.VK_D));
        jmi.addActionListener(new DBParam());
        help.add(jmi = new JMenuItem("Environment Variables File",KeyEvent.VK_V));
        jmi.addActionListener(new envVar());
        help.add(jmi = new JMenuItem("Okapi Command Interface",KeyEvent.VK_I));
        jmi.addActionListener(new commInterface());

        setLayout(new BorderLayout());
        jmb.add(file);
        jmb.add(edit);
        jmb.add(help);
        add(jmb, BorderLayout.NORTH);
        add(jsp, BorderLayout.CENTER);
    }

    public static void main(String args[])
    {
    JFrame f = new JFrame();
    Notepad applet = new Notepad();
    f.setTitle("Notepad");
    f.setBackground(Color.lightGray);
    f.getContentPane().add(applet, BorderLayout.CENTER);
    f.addWindowListener(new appCloseL());
        f.setSize(800, 500);
        f.setVisible(true);
    }

    protected static final class appCloseL extends WindowAdapter
    {
        public void windowClosing(WindowEvent e)
        {
            System.exit(0);
        }
    }
    public void openWebPage(String url){
      JFrame frame = new JFrame();
      JPanel panel = new JPanel();
      final JTextField urlField =  new JTextField(url);
      JButton webButton = new JButton("Go");
      webButton.addActionListener(new ActionListener() {
         public void actionPerformed(ActionEvent e) {            
            BrowserLauncher.openURL(urlField.getText().trim()); }
            } );
      frame.setTitle("Browser Launcher");
      frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
      panel.add(new JLabel("URL:"));
      panel.add(urlField);
      panel.add(webButton);
      frame.getContentPane().add(panel);
      frame.pack();
      frame.setVisible(true);
    }  
    public void openFile(String directory, String file)
    {
        File f = new File(directory, file);
        if(f.exists())
        {
            try
            {
                Reader in = new FileReader(f);
                char[] buff = new char[4096];
                int nch;
                while((nch = in.read(buff, 0, buff.length)) != -1)
                {
                    jta.setDocument(new PlainDocument());
                    jta.append(new String(buff, 0, nch));
                }
            }
            catch (IOException io)
            {
                    System.err.println("IOException: " + io.getMessage());
            }
        }else{
            System.err.println(directory + file + " does not exist.");
        }
    }
} 