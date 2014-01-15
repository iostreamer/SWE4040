package ca.unb.backoffapp;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Set;
import java.util.UUID;

import android.os.Bundle;
import android.os.Handler;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.view.Menu;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

public class MainActivity extends Activity {
	
	private static final int REQUEST_ENABLE_BT = 1;
	private static final String MY_UUID = "00001101-0000-1000-8000-00805F9B34FB";
	private BluetoothSocket sockPuppet;
	private BluetoothDevice  arduino = null;
	
	BluetoothAdapter btAdapter;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		btAdapter = BluetoothAdapter.getDefaultAdapter();
		TextView msgField = (TextView) findViewById(R.id.btStatusTxt);
		if (btAdapter == null) {
			msgField.append("No Bluetooth Adapter Available");
		}
		if (!btAdapter.isEnabled()) {
			msgField.append("Requesting Bluetooth Enable");
			Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
		    startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
		} else {
			msgField.append("Bluetooth enabled.\n");
		}
		
		//query the set of paired devices
		Set<BluetoothDevice> devices =  btAdapter.getBondedDevices();
		for (BluetoothDevice dev : devices) {
			msgField.append(dev + " " + dev.getName() + "\n");
			if (dev.getName().equals("Balls of Steel")) {
				arduino = dev;
			}
		}
		
		if (arduino != null) {
			try {
				sockPuppet = arduino.createRfcommSocketToServiceRecord(UUID.fromString(MY_UUID));
			} catch (Exception err) {
				msgField.append("Failed to open socket\n");
			}
			if (sockPuppet != null) {
		        try {
		            // Connect the device through the socket. This will block
		            // until it succeeds or throws an exception
		        	sockPuppet.connect();
		        	Thread t = new Thread(new ReadThread(sockPuppet));
		        	t.start();
		        } catch (IOException connectException) {
		            // Unable to connect; close the socket and get out
		            try {
		            	sockPuppet.close();
		            	msgField.append("Unable to connect\n");
		            } catch (IOException closeException) { }
		            return;
		        }
			}
		} else {
			msgField.append("Could not find the specified device!\n");	
			findViewById(R.id.sendButton).setEnabled(false);
		}
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
	public void sendMessage(View v) {
		try {
			TextView tv = (TextView) findViewById(R.id.msgField);
			CharSequence msg = tv.getText();
			String sMsg  = msg.toString();

        	OutputStream os = sockPuppet.getOutputStream();
        	os.write(sMsg.getBytes());
        	os.write('\n');
        	
        	tv.setText("");
		} catch (Exception err) {
			err.printStackTrace();
		}
		
	}
	
	private class ReadThread implements Runnable {
		
		private final InputStream istream;
		private final EditText txt;
		
		public ReadThread (BluetoothSocket s) {
			txt  = (EditText) findViewById(R.id.recieveField);
			InputStream tmp = null;
			try {
				tmp = s.getInputStream();
			} catch (IOException err) {
				txt.append("Couldn't open input stream!");
			}
			istream = tmp;
		}
		
		private void update(String s) {
			txt.post(new Runnable() {

				@Override
				public void run() {
					// TODO Auto-generated method stub
					
				}
				
			});
		}
		
		@Override
		public void run() {
			if (istream != null) {
				char in;
				try {
					while ((in = (char) istream.read()) != -1) {
						txt.append(in + "");
					}
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
					txt.append("\nThere was an error reading the input stream");
					
				}
				txt.append("\nINPUT TERMINATED");
			}
		}
	}

}
