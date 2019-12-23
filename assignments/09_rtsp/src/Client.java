// ------------------
// Client
// Usage: java Client [Server hostname] [Server RTSP listening port] [Video file requested]
// ----------------------

import java.io.*;
import java.net.*;
import java.util.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.Timer;

public class Client {

    // GUI
    JFrame f = new JFrame("Client");
    JButton setupButton = new JButton("Setup");
    JButton playButton = new JButton("Play");
    JButton pauseButton = new JButton("Pause");
    JButton tearButton = new JButton("Teardown");
    JPanel mainPanel = new JPanel();
    JPanel buttonPanel = new JPanel();
    JLabel iconLabel = new JLabel();
    ImageIcon icon;

    // RTP variables
    DatagramPacket rcvdp;                       // UDP packet received from the server
    DatagramSocket RTPsocket;                   // Socket to be used to send and receive UDP packets
    static int RTP_RCV_PORT = 25000;            // Port where the client will receive the RTP packets

    Timer timer;                                // Timer used to receive data from the UDP socket
    byte[] buf;                                 // Buffer used to store data received from the server

    // RTSP states
    final static int INIT = 0;
    final static int READY = 1;
    final static int PLAYING = 2;
    static int state;                           // RTSP state == INIT or READY or PLAYING

    Socket RTSPsocket;                          // Socket used to send/receive RTSP messages

    // Input and output stream filters
    static BufferedReader RTSPBufferedReader;
    static BufferedWriter RTSPBufferedWriter;
    static String VideoFileName;                // Video file to request to the server
    int RTSPSeqNb = 0;                          // Sequence number of RTSP messages within the session
    int RTSPid = 0;                             // ID of the RTSP session (given by the RTSP Server)

    final static String CRLF = "\r\n";

    // Video constants
    static int MJPEG_TYPE = 26;                 // RTP payload type for MJPEG video

    // --------------------------
    // Constructor
    // --------------------------
    public Client() {

        // Build GUI

        // Frame
        f.addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) {
            System.exit(0);
            }
        });

        // Buttons
        buttonPanel.setLayout(new GridLayout(1,0));
        buttonPanel.add(setupButton);
        buttonPanel.add(playButton);
        buttonPanel.add(pauseButton);
        buttonPanel.add(tearButton);
        setupButton.addActionListener(new setupButtonListener());
        playButton.addActionListener(new playButtonListener());
        pauseButton.addActionListener(new pauseButtonListener());
        tearButton.addActionListener(new tearButtonListener());

        // Image display label
        iconLabel.setIcon(null);

        // Frame layout
        mainPanel.setLayout(null);
        mainPanel.add(iconLabel);
        mainPanel.add(buttonPanel);
        iconLabel.setBounds(0,0,380,280);
        buttonPanel.setBounds(0,280,430,50);

        f.getContentPane().add(mainPanel, BorderLayout.CENTER);
        f.setSize(new Dimension(440,362));
        f.setVisible(true);

        // Init timer
        // --------------------------
        timer = new Timer(20, new timerListener());
        timer.setInitialDelay(0);
        timer.setCoalesce(true);

        // Allocate enough memory for the buffer used to receive data from the server
        buf = new byte[15000];
    }

    // ------------------------------------
    // Main
    // ------------------------------------
    public static void main(String argv[]) throws Exception {

        if (argv.length < 3) {
            System.out.println("Usage: java Client [Server hostname] [Server RTSP listening port] [Video file requested]");
            System.exit(0);
        }

        // Create a Client object
        Client theClient = new Client();

        // Get server RTSP port and IP address from the command line
        int RTSP_server_port = Integer.parseInt(argv[1]);

        System.out.println("cli args: " + Arrays.toString(argv));

        String ServerHost = argv[0];
        InetAddress ServerIPAddr = InetAddress.getByName(ServerHost);

        // Get video filename to request:
        VideoFileName = argv[2];

        // Establish a TCP connection with the server to exchange RTSP messages
        theClient.RTSPsocket = new Socket(ServerIPAddr, RTSP_server_port);

        // Set input and output stream filters:
        RTSPBufferedReader = new BufferedReader(new InputStreamReader(theClient.RTSPsocket.getInputStream()));
        RTSPBufferedWriter = new BufferedWriter(new OutputStreamWriter(theClient.RTSPsocket.getOutputStream()));

        // Init RTSP state:
        state = INIT;
    }

    // ------------------------------------
    // Handler for buttons
    // ------------------------------------

    // -----------------------
    // Handler for Setup button
    // -----------------------
    class setupButtonListener implements ActionListener {

        public void actionPerformed(ActionEvent e) {
            System.out.println("Setup Button pressed!");

            if (state == INIT) {
                // Init non-blocking RTPsocket that will be used to receive data
                try {
                    // Construct a new DatagramSocket to receive RTP packets from the server, on port RTP_RCV_PORT
                    RTPsocket = new DatagramSocket(RTP_RCV_PORT);

                    // Set TimeOut value of the socket to 5msec.
                    RTPsocket.setSoTimeout(5);
                } catch (SocketException se) {
                    System.out.println("Socket exception: " + se);
                    System.exit(0);
                }

                // Init RTSP sequence number
                RTSPSeqNb = 1;

                // Send SETUP message to the server
                send_RTSP_request("SETUP");

                // Wait for the response
                if (parse_server_response() != 200) {
                    System.out.println("Invalid Server Response");
                }
                else {
                    // -----------------------
                    state = READY;
                    System.out.println("New RTSP state: READY");
                }
            } // else if state != INIT then do nothing
        }
    }

    // -----------------------
    // Handler for Play button
    // -----------------------
    class playButtonListener implements ActionListener {

        public void actionPerformed(ActionEvent e){

            System.out.println("Play Button pressed!");

            if (state == READY) {
                // Increase RTSP sequence number
                RTSPSeqNb++;

                // Send PLAY message to the server
                send_RTSP_request("PLAY");

                // Wait for the response
                if (parse_server_response() != 200) {
                    System.out.println("Invalid Server Response");
                }
                else {
                    // Change RTSP state and print out new state
                    state = PLAYING;
                    System.out.println("New RTSP state: PLAYING");

                    // Start the timer
                    timer.start();
                }
            } // else if state != READY then do nothing
        }
    }

    // -----------------------
    // Handler for Pause button
    // -----------------------
    class pauseButtonListener implements ActionListener {

        public void actionPerformed(ActionEvent e){

            System.out.println("Pause Button pressed!");

            if (state == PLAYING) {
                // Increase RTSP sequence number
                RTSPSeqNb++;

                //Send PAUSE message to the server
                send_RTSP_request("PAUSE");

                //Wait for the response
                if (parse_server_response() != 200) {
                    System.out.println("Invalid Server Response");
                }
                else {
                    // Change RTSP state and print out new state
                    state = READY;
                    System.out.println("New RTSP state: READY");

                    //stop the timer
                    timer.stop();
                }
            } // else if state != PLAYING then do nothing
        }
    }

    // -----------------------
    // Handler for Teardown button
    // -----------------------
    class tearButtonListener implements ActionListener {

        public void actionPerformed(ActionEvent e){

            System.out.println("Teardown Button pressed!");

            // Increase RTSP sequence number
            RTSPSeqNb++;

            //Send TEARDOWN message to the server
            send_RTSP_request("TEARDOWN");

            //Wait for the response
            if (parse_server_response() != 200) {
                System.out.println("Invalid Server Response");
            }
            else {
                // Change RTSP state and print out new state
                state = INIT;

                System.out.println("New RTSP state: INIT");

                // Stop the timer
                timer.stop();

                // Exit
                System.exit(0);
            }
        }
    }

    // ------------------------------------
    //  Handler for timer
    // ------------------------------------
    class timerListener implements ActionListener {

        public void actionPerformed(ActionEvent e) {

            // Construct a DatagramPacket to receive data from the UDP socket
            rcvdp = new DatagramPacket(buf, buf.length);

            try {
                // Receive the DP from the socket:
                RTPsocket.receive(rcvdp);

                // Create an RTPpacket object from the DP
                RTPpacket rtp_packet = new RTPpacket(rcvdp.getData(), rcvdp.getLength());

                // Print important header fields of the RTP packet received:
                System.out.println("Got RTP packet with SeqNum # " + rtp_packet.getsequencenumber() + " TimeStamp " + rtp_packet.gettimestamp() + " ms, of type " + rtp_packet.getpayloadtype());

                // Print header bitstream:
                rtp_packet.printheader();

                // Get the payload bitstream from the RTPpacket object
                int payload_length = rtp_packet.getpayload_length();
                byte [] payload = new byte[payload_length];
                rtp_packet.getpayload(payload);

                // Get an Image object from the payload bitstream
                Toolkit toolkit = Toolkit.getDefaultToolkit();
                Image image = toolkit.createImage(payload, 0, payload_length);

                // Display the image as an ImageIcon object
                icon = new ImageIcon(image);
                iconLabel.setIcon(icon);
            }
            catch (InterruptedIOException iioe) {
                System.out.println("Nothing to read");
            }
            catch (IOException ioe) {
                System.out.println("Exception caught: " + ioe);
            }
        }
    }

    // ------------------------------------
    // Parse Server Response
    // ------------------------------------
    private int parse_server_response() {

        int reply_code = 0;

        try {
            // Parse status line and extract the reply_code:
            String StatusLine = RTSPBufferedReader.readLine();
            System.out.println("RTSP Client - Received from Server:");
            System.out.println("  " + StatusLine);

            StringTokenizer tokens = new StringTokenizer(StatusLine);
            tokens.nextToken(); //skip over the RTSP version
            reply_code = Integer.parseInt(tokens.nextToken());

            // If reply code is OK get and print the 2 other lines
            if (reply_code == 200) {
                String SeqNumLine = RTSPBufferedReader.readLine();
                System.out.println("  " + SeqNumLine);

                String SessionLine = RTSPBufferedReader.readLine();
                System.out.println("  " + SessionLine);

                // If state == INIT gets the Session Id from the SessionLine
                tokens = new StringTokenizer(SessionLine);
                tokens.nextToken(); //skip over the Session:
                RTSPid = Integer.parseInt(tokens.nextToken());
            }
        }
        catch(Exception ex) {
            System.out.println("Exception caught: " + ex);
            System.exit(0);
        }

        return(reply_code);
    }

    // ------------------------------------
    // Send RTSP Request
    // ------------------------------------
    private void send_RTSP_request(String request_type) {

        try {
            // Write the request line
            RTSPBufferedWriter.write(request_type + " movie.mjpeg RTSP/1.0\r\n");

            // Write the CSeq line
            RTSPBufferedWriter.write("Cseq: " + RTSPSeqNb + "\r\n");

            // Check if request_type is equal to "SETUP" and in this case write the Transport: line
            // advertising to the server the port used to receive the RTP packets RTP_RCV_PORT

            if (request_type == "SETUP") {
                RTSPBufferedWriter.write("Transport: RTP/UDP;unicast;client_port=" + RTP_RCV_PORT + "\r\n");
            }
            else {
                RTSPBufferedWriter.write("Session: " + RTSPid + "\r\n");
            }

            RTSPBufferedWriter.flush();
        }
        catch(Exception ex) {
            System.out.println("Exception caught: " + ex);
            System.exit(0);
        }
    }
} // End of Class Client
