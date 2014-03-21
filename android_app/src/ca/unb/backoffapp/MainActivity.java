package ca.unb.backoffapp;

import java.io.BufferedWriter;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.util.Scanner;
import java.util.Set;
import java.util.UUID;
import java.util.Vector;

import ca.unb.backoffapp.R;

import android.os.Bundle;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.view.Menu;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.TextView;

public class MainActivity extends Activity {
	
	private static final int REQUEST_ENABLE_BT = 1;
	private static final String MY_UUID = "00001101-0000-1000-8000-00805F9B34FB";
	private static final String DEVICE_ADDRESS = "00:01:95:0B:CE:1F";
	static final String EXTRA_MSG = "ca.unb.backoffapp.message";
	private BluetoothSocket sockPuppet;
	private BluetoothDevice  arduino = null;
	static Context c;
	
	BluetoothAdapter btAdapter;
	@Override
	protected void onCreate(Bundle savedInstanceState) {

		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		c = getApplicationContext();
		
		
		btAdapter = BluetoothAdapter.getDefaultAdapter();
		//TextView msgField = (TextView) findViewById(R.id.btStatusTxt);
		if (btAdapter == null) {
			System.err.println("No Bluetooth Adapter Available");
		}
		if (!btAdapter.isEnabled()) {
			System.err.println("Requesting Bluetooth Enable");
			Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
		    startActivityForResult(enableBtIntent, REQUEST_ENABLE_BT);
		} else {
			System.err.println("Bluetooth enabled.");
		}

		Button b = (Button) findViewById(R.id.launchButton);
		b.setEnabled(false);
		
		//query the set of paired devices
		Set<BluetoothDevice> devices =  btAdapter.getBondedDevices();
		for (BluetoothDevice dev : devices) {
			System.err.println(dev + " " + dev.getName());
			if (dev.getAddress().equals(DEVICE_ADDRESS)) {
				arduino = dev;
			}
		}
		
		if (arduino != null) {
			deviceFound();
		}
		//DEBUG
		deviceFound();
		//DEBUG
		IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
		IntentFilter finishFilter = new IntentFilter(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
		registerReceiver(mReceiver, filter); // Don't forget to unregister during onDestroy
		registerReceiver(mReceiver, finishFilter);
		/*
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
		        	//throw new IOException();
		        } catch (IOException connectException) {
		            // Unable to connect; close the socket and get out
		            try {
		            	connectException.printStackTrace();
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
		*/
	}
	
	private void deviceFound() {
		ImageButton ib = (ImageButton) findViewById(R.id.pairButton);
		TextView tv = (TextView) findViewById(R.id.pairButtonLabel);
		ib.setImageResource(R.drawable.ic_green_check);
		tv.setText(R.string.pairstatus_found);
		Button b = (Button) findViewById(R.id.launchButton);
		b.setEnabled(true);
	}
	
	private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
	    public void onReceive(Context context, Intent intent) {
	        String action = intent.getAction();
	        // When discovery finds a device
	        if (BluetoothDevice.ACTION_FOUND.equals(action)) {
	            // Get the BluetoothDevice object from the Intent
	            BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
	            if (device.getAddress().equals(DEVICE_ADDRESS)) {
	            	arduino = device;
	            	deviceFound();
	            	BluetoothAdapter.getDefaultAdapter().cancelDiscovery();
	            }
	        }
	        if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)) {
	        	if (arduino == null) {
	        		ImageButton ib = (ImageButton) findViewById(R.id.pairButton);
	        		TextView tv = (TextView) findViewById(R.id.pairButtonLabel);
	        		ib.setImageResource(R.drawable.ic_red_x);
	        		ib.setEnabled(true);
	        		tv.setText(R.string.pairstatus_bad);
	        	}
	        }
	    }
	};
	
	public void attemptPairing(View v) {
		arduino = null;
		ImageButton ib = (ImageButton) findViewById(R.id.pairButton);
		TextView tv = (TextView) findViewById(R.id.pairButtonLabel);
		ib.setImageResource(R.drawable.ic_yellow_qmark);
		ib.setEnabled(false);
		tv.setText(R.string.pairstatus_search);
		BluetoothAdapter.getDefaultAdapter().startDiscovery();
		Button b = (Button) findViewById(R.id.launchButton);
		b.setEnabled(false);
	}
	
	public void launchManagementScreen(View v) {
		Intent i = new Intent(this, DeviceManagerActivity.class);
		i.putExtra(EXTRA_MSG, "MANAGE");
		startActivity(i);
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}
	
    public static Vector<BackoffMessage> LoadMessages() {
		Vector<BackoffMessage> rv = new Vector<BackoffMessage>();
		try {
			FileInputStream msgFile = c.openFileInput("profiles.txt");
			Scanner sc = new Scanner(msgFile);
			while (sc.hasNext()) {
				rv.add(new BackoffMessage(sc.next(), sc.next(), sc.nextDouble()));
			}
			sc.close();
			rv.add(new BackoffMessage("Create new profile...", "Back Off!", 1.0));
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}  
		return rv;
	}
    
    public static void saveMessages(Vector<BackoffMessage> msgs) {
    	try {
			FileOutputStream msgFile = c.openFileOutput("profiles.txt", MODE_PRIVATE);
			PrintWriter pr = new PrintWriter(msgFile);
			for (BackoffMessage b : msgs) {
				pr.write(b.toString());
			}
			msgFile.close();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    }
	
	/*
	public void sendMessage(View v) {
		try {
			TextView tv = (TextView) findViewById(R.id.msgField);
			CharSequence msg = tv.getText();
			String sMsg  = msg.toString();

        	OutputStream os = sockPuppet.getOutputStream();
        	os.write(sMsg.getBytes());
        	os.write('\r');
        	os.write('\n');
        	
        	tv.setText("");
		} catch (Exception err) {
			err.printStackTrace();
		}
		
	}
	*/
	/*
	private class ReadThread implements Runnable {
		
		private final InputStream istream;
		private final EditText txt;
		
		public ReadThread (BluetoothSocket s) {
			txt  = (EditText) findViewById(R.id.recieveField);
			InputStream tmp = null;
			try {
				tmp = s.getInputStream();
			} catch (IOException err) {
				update("Couldn't open input stream!");
			}
			istream = tmp;
		}
		
		private void update(final String s) {
			txt.post(new Runnable() {

				@Override
				public void run() {
					// TODO Auto-generated method stub
					txt.append(s);
				}
				
			});
		}
		
		@Override
		public void run() {
			update("begin\n");
			System.err.println("readthread in");
			if (istream != null) {
				char in;
				try {
					while ((in = (char) istream.read()) != -1) {
						update(in + "");
					}
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
					update("\nThere was an error reading the input stream");
				}
				update("\nINPUT TERMINATED");
			}
		}
	}
	*/

	@Override
	protected void onDestroy() {
		super.onDestroy();
		unregisterReceiver(mReceiver);
	}
}
