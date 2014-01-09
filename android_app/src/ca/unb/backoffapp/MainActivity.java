package ca.unb.backoffapp;

import android.os.Bundle;
import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.view.Menu;
import android.widget.TextView;

public class MainActivity extends Activity {
	
	private static final int REQUEST_ENABLE_BT = 1;
	
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
			msgField.append("Bluetooth enabled.");
		}
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

}
