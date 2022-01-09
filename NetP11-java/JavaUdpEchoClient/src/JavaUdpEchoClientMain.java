// Client.Java Java Chatting Client 의 Nicknam, IP, Port 번호 입력하고 접속하는 부분

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.net.UnknownHostException;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.border.EmptyBorder;
import javax.swing.SwingConstants;

public class JavaUdpEchoClientMain extends JFrame {

	private JPanel contentPane;
	private JTextField tf_ID; // ID를 입력받을곳
	private JTextField tf_IP; // IP를 입력받을곳
	private JTextField tf_PORT; //PORT를 입력받을곳


	public static void main(String[] args) {
		JavaUdpEchoClientMain client = new JavaUdpEchoClientMain();
		client.setVisible(true);
	}
	

	public JavaUdpEchoClientMain() // 화면 구성
	{
		setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		setBounds(100, 100, 288, 392);
		contentPane = new JPanel();
		contentPane.setBorder(new EmptyBorder(5, 5, 5, 5));
		setContentPane(contentPane);
		contentPane.setLayout(null);

		JLabel lblNewLabel = new JLabel("UserName");
		lblNewLabel.setHorizontalAlignment(SwingConstants.RIGHT);
		lblNewLabel.setBounds(12, 57, 68, 34);
		contentPane.add(lblNewLabel);
		

		tf_ID = new JTextField();
		tf_ID.setHorizontalAlignment(SwingConstants.CENTER);
		tf_ID.setBounds(92, 64, 150, 21);
		contentPane.add(tf_ID);
		tf_ID.setColumns(10);
	

		JLabel lblServerIp = new JLabel("Server IP");
		lblServerIp.setHorizontalAlignment(SwingConstants.RIGHT);
		lblServerIp.setBounds(22, 111, 58, 34);
		contentPane.add(lblServerIp);

		tf_IP = new JTextField();
		tf_IP.setText("127.0.0.1");
		tf_IP.setHorizontalAlignment(SwingConstants.CENTER);
		tf_IP.setColumns(10);
		tf_IP.setBounds(92, 118, 150, 21);
		contentPane.add(tf_IP);

		JLabel lblPort = new JLabel("Port");
		lblPort.setHorizontalAlignment(SwingConstants.RIGHT);
		lblPort.setBounds(48, 178, 32, 34);
		contentPane.add(lblPort);

		tf_PORT = new JTextField();
		tf_PORT.setText("30000");
		tf_PORT.setHorizontalAlignment(SwingConstants.CENTER);
		tf_PORT.setColumns(10);
		tf_PORT.setBounds(92, 185, 150, 21);
		contentPane.add(tf_PORT);
		
		JButton btnNewButton = new JButton("Connect");
		btnNewButton.setBounds(36, 266, 206, 52);
		contentPane.add(btnNewButton);
		
		ConnectAction action = new ConnectAction();
		btnNewButton.addActionListener(action);
		tf_PORT.addActionListener(action);
		tf_ID.addActionListener(action);
		tf_IP.addActionListener(action);	
	}
	class ConnectAction implements ActionListener {
		@Override
		public void actionPerformed(ActionEvent arg0) {			
			String _id = tf_ID.getText().trim(); // 공백이 있지 모르니 공백 제거 trim() 사용
			String _ip= tf_IP.getText().trim(); // 공백이 있을지 모르므로 공백제거
			int _port=Integer.parseInt(tf_PORT.getText().trim()); // 공백을 제거한 후 int형으로 변환 			
			JavaUdpEchoClientView view = new JavaUdpEchoClientView(_id,_ip,_port);
			setVisible(false);						
		}
	}
}
