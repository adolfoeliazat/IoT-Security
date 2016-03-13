package org.bluetooth.guneSec;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

/* this activity's purpose is to show how to use particular type of devices in easy and fast way */
public class SecurityActivity extends Activity {

	Button EnterKey, decryptIt, SEEKey;
	TextView Mykey, MyDecrypted, MyStatus;
	String m_Text = "";
	double keyRCVD,keyRSA,decryptor;
	//int secret = 72;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);

		EnterKey = (Button) findViewById(R.id.keyEnter);
		decryptIt = (Button) findViewById(R.id.decryptor);
		SEEKey = (Button) findViewById(R.id.keyDecrypted);
		Mykey = (TextView) findViewById(R.id.myKey);
		MyDecrypted = (TextView) findViewById(R.id.decryptedKey);
		MyStatus = (TextView) findViewById(R.id.oadStatus);




		EnterKey.setOnClickListener(new View.OnClickListener() {

			@Override
			public void onClick(View v) {
				AlertDialog.Builder builder = new AlertDialog.Builder(SecurityActivity.this);
				builder.setTitle("Enter The Key");

// Set up the input
				final EditText input = new EditText(SecurityActivity.this);
				builder.setView(input);

// Set up the buttons
				builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface dialog, int which) {
						m_Text = input.getText().toString();
						Mykey.setText(m_Text);
						keyRCVD = Integer.parseInt(m_Text);

					}
				});
				builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface dialog, int which) {
						dialog.cancel();
					}
				});

				builder.show();
			}
		});


		decryptIt.setOnClickListener(new View.OnClickListener() {

			@Override
			public void onClick(View v) {

				if(keyRCVD!= 0) {
					keyRSA = Math.abs((Math.pow(keyRCVD,7))%55);
					String apnaKey = String.valueOf(keyRSA);
					MyDecrypted.setText(apnaKey);
					Toast.makeText(getApplicationContext(),
							"Decrypted key is : " + apnaKey, Toast.LENGTH_LONG).show();
				}


			}
		});

		SEEKey.setOnClickListener(new View.OnClickListener() {

			@Override
			public void onClick(View v) {
				AlertDialog.Builder builder = new AlertDialog.Builder(SecurityActivity.this);
				builder.setTitle("Enter Decrypted Key");

// Set up the input
				final EditText input = new EditText(SecurityActivity.this);
				builder.setView(input);

// Set up the buttons
				builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface dialog, int which) {
						m_Text = input.getText().toString();
						decryptor = Double.parseDouble(m_Text);
						if (decryptor == 37) {
							MyStatus.setText("OAD IS NOW ACTIVE");
							MyStatus.setTextColor(Color.rgb(34, 139, 34));
						} else {
							MyStatus.setText("OAD IS NOW INACTIVE");
							MyStatus.setTextColor(Color.rgb(220, 20, 60));

						}
					}



				});
				builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
					@Override
					public void onClick(DialogInterface dialog, int which) {
						dialog.cancel();
					}
				});

				builder.show();
			}
		});


	}
}

	

