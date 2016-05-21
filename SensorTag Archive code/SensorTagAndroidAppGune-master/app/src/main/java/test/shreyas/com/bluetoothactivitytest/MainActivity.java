package test.shreyas.com.bluetoothactivitytest;

//you're going to need all of these imports.

import android.app.Activity;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.util.SparseArray;
import android.view.Menu;
import android.view.MenuItem;
import android.view.Window;
import android.widget.TextView;
import android.widget.Toast;

import java.util.UUID;


public class MainActivity extends Activity implements BluetoothAdapter.LeScanCallback {

    private static final String TAG = "BluetoothGattActivity";

    private static final String DEVICE_NAME = "Shreyas's Sensor Tag"; //my device name

    /* Services Invoked to read - all of these are predefined in the firmware code for the SensorTag
     * Mostly everything that you've defined in your attribute table in your C code goes here */
    private static final UUID YOURSERVICE_SERVICE = UUID.fromString("the UUID");
    private static final UUID YOURSERVICE_DATA_CHAR = UUID.fromString("the UUID");
    private static final UUID YOURSERVICE_CONFIG_CHAR = UUID.fromString("the UUID");
    //configuration descriptor
    private static final UUID CONFIG_DESCRIPTOR = UUID.fromString("the UUID");


    private BluetoothAdapter mBluetoothAdapter; //declaring a variable for the Bluetooth Adapter
    private SparseArray<BluetoothDevice> mDevices; // an array that holds device list. <http://developer.android.com/reference/android/util/SparseArray.html>

    private BluetoothGatt mConnectedGatt; //to connect to the gatt server

    private TextView mData; // this holds the data read

    private ProgressDialog mProgress; //this shows what is happening.


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS); //enables progress dialog
        setContentView(R.layout.activity_main);
        setProgressBarIndeterminate(true); //http://developer.android.com/reference/android/widget/ProgressBar.html

        //display our data in the text field we have defined
        mData = (TextView) findViewById(R.id.dataholder); //passes data to dataholder , that displays whats being read

        //Bluetooth Manager deals with all the devices. https://developer.android.com/reference/android/bluetooth/BluetoothManager.html
        BluetoothManager manager = (BluetoothManager) getSystemService(BLUETOOTH_SERVICE);
        mBluetoothAdapter = manager.getAdapter();

        mDevices = new SparseArray<BluetoothDevice>(); // all the devices show up in this array.

        //When the connection is in the process, you'll see this.
        mProgress = new ProgressDialog(this);
        mProgress.setIndeterminate(true);
        mProgress.setCancelable(false); // you can't really cancel the progress dialog box.

    }

    @Override
    protected  void onResume()
    {
        super.onResume(); //// Always call the superclass method first http://developer.android.com/training/basics/activity-lifecycle/pausing.html

        /*
        Making sure that Bluetooth is enabled first in your device.
         */

        if(mBluetoothAdapter == null || !mBluetoothAdapter.isEnabled())
        {
            //Bluetooth ain't working on your phone, man.
            Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE); //enables your bluetooth
            startActivity(enableBtIntent);
            finish();
            return;
        }

        /*
        Now, that the Bluetooth is enabled , check to see if the device has Bluetooth LE on it.
         */
        if(!getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE))
        {
            Toast.makeText(this,"No LE Support, Bro", Toast.LENGTH_SHORT).show();
            finish();
            return;
        }

        clearDisplayValues(); //this puts the dataholder thingy as "----"

    }



    @Override
    protected void onPause()
    {
        super.onPause();

        //hide the dialog box
        mProgress.dismiss();

        //cancel all scans in progress
        mHandler.removeCallbacks(mStopRunnable);
        mHandler.removeCallbacks(mStartRunnable);
        mBluetoothAdapter.stopLeScan(this);

    }



    @Override
    protected void onStop()
    {
        super.onStop();

        //disconnect connection
        if(mConnectedGatt !=null)
        {
            mConnectedGatt.disconnect();
            mConnectedGatt = null;
        }

    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.

        //Adding Scan option to the menu
        getMenuInflater().inflate(R.menu.menu_main, menu);

        for(int i =0; i <mDevices.size();i++)
        {
            BluetoothDevice device = mDevices.valueAt(i);
            menu.add(0, mDevices.keyAt(i), 0, device.getName());
        }
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        switch (item.getItemId()) {
            case R.id.action_scan:
                mDevices.clear();
                startScan();
                return true;

            default:
                //get the discovered device to connect with
                BluetoothDevice device = mDevices.get(item.getItemId());
                Log.i(TAG, "Connecting to"+device.getName());

                /*
                 * Make a connection with the device using the special LE-specific
                 * connectGatt() method, passing in a callback for GATT events
                 */
                mConnectedGatt = device.connectGatt(this, false, mGattCallback);
                //Display progress UI
                mHandler.sendMessage(Message.obtain(null, MSG_PROGRESS, "Connecting to " + device.getName() + "..."));
                return super.onOptionsItemSelected(item);

        }

    }


    private void clearDisplayValues()
    {
        mData.setText("----");
    }


    private Runnable mStopRunnable = new Runnable() {
        @Override
        public void run() {
            stopScan();
        }
    };

    private Runnable mStartRunnable = new Runnable() {
        @Override
        public void run() {
            startScan();
        }
    };

    private void startScan(){
        mBluetoothAdapter.startLeScan(this);
        setProgressBarIndeterminateVisibility(true);

        mHandler.postDelayed(mStopRunnable,2500);

    }

    private void stopScan(){
        mBluetoothAdapter.stopLeScan(this);
        setProgressBarIndeterminateVisibility(false);

    }


    //**********************************************************************
    /*
    Bluetooth LE Callback Functions
     */

    @Override
    public void onLeScan(BluetoothDevice device, int rssi, byte[] scanRecord)
    {
        Log.i(TAG,"New LE Device:" + device.getName()+"Address: "+device.getAddress()+"\n RSSI: "+rssi);

        //we are only interested in Texas Instruments SensorTag (CC2650). Validate it.

        if(DEVICE_NAME.equals(device.getName()))
        {
            mDevices.put(device.hashCode(),device);
            //update the overflow menu
            invalidateOptionsMenu();
        }

    }

    /*
    In the callback below, there is a state machine to enforce that only one characteristic be read at a time.
    Use this when you have multiple characteristics to be read or written.
     */

    private BluetoothGattCallback mGattCallback = new BluetoothGattCallback() {
        private int mState = 0;
        private void reset() { mState = 0; }
        private void advance() { mState++; }

        /*
         * Send an enable command to each sensor by writing a configuration
         * characteristic.  This is specific to the SensorTag to keep power
         * low by disabling sensors you aren't using.

        private void enableNextSensor(BluetoothGatt gatt)
        {
            BluetoothGattCharacteristic characteristic;
            switch (mState) {
                case 0:
                    Log.d(TAG, "Enabling YOURSERVICE ");
                    characteristic = gatt.getService(YOURSERVICE_SERVICE)
                            .getCharacteristic(YOURSERIVICE_CONFIG_CHAR);
                    characteristic.setValue(new byte[] {0x02});
                    break;

                default:
                    mHandler.sendEmptyMessage(MSG_DISMISS);
                    Log.i(TAG, "All Sensors Enabled");
                    return;
            }

            gatt.writeCharacteristic(characteristic);
        }
        */

        /*
         * Read the data characteristic's value for Service explicitly
         */
        private void readNextSensor(BluetoothGatt gatt) {
            BluetoothGattCharacteristic characteristic;
            switch (mState) {
                case 0:
                    Log.d(TAG, "Reading Value from Service");
                    characteristic = gatt.getService(YOURSERVICE_SERVICE)
                            .getCharacteristic(YOURSERVICE_DATA_CHAR);
                    break;

                default:
                    mHandler.sendEmptyMessage(MSG_DISMISS);
                    Log.i(TAG, "All Sensors Enabled");
                    return;
            }

            gatt.readCharacteristic(characteristic);
        }

        /*
         * Enable notification of changes on the data characteristic for each sensor
         * by writing the ENABLE_NOTIFICATION_VALUE flag to that characteristic's
         * configuration descriptor.

        private void setNotifyNextSensor(BluetoothGatt gatt) {
            BluetoothGattCharacteristic characteristic;
            switch (mState) {
                case 0:
                    Log.d(TAG, "Set notify YOURSERVICE");
                    characteristic = gatt.getService(YOURSERVICE_SERVICE)
                            .getCharacteristic(YOURSERVICE_DATA_CHAR);
                    break;

                default:
                    mHandler.sendEmptyMessage(MSG_DISMISS);
                    Log.i(TAG, "All Sensors Enabled");
                    return;
            }

            */

        //Enable local notifications
        //gatt.setCharacteristicNotification(characteristic, true);
        /*Enabled remote notifications
        BluetoothGattDescriptor desc = characteristic.getDescriptor(CONFIG_DESCRIPTOR);
        desc.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
        gatt.writeDescriptor(desc);
        */

        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            Log.d(TAG, "Connection State Change: "+status+" -> "+connectionState(newState));
            if (status == BluetoothGatt.GATT_SUCCESS && newState == BluetoothProfile.STATE_CONNECTED) {
                /*
                 * Once successfully connected, we must next discover all the services on the
                 * device before we can read and write their characteristics.
                 */
                gatt.discoverServices();
                mHandler.sendMessage(Message.obtain(null, MSG_PROGRESS, "Discovering Services..."));
            } else if (status == BluetoothGatt.GATT_SUCCESS && newState == BluetoothProfile.STATE_DISCONNECTED) {
                /*
                 * If at any point we disconnect, send a message to clear the weather values
                 * out of the UI
                 */
                mHandler.sendEmptyMessage(MSG_CLEAR);
            } else if (status != BluetoothGatt.GATT_SUCCESS) {
                /*
                 * If there is a failure at any stage, simply disconnect
                 */
                gatt.disconnect();
            }
        }


        //Discovering Services
        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            Log.d(TAG, "Services Discovered: "+status);
            mHandler.sendMessage(Message.obtain(null, MSG_PROGRESS, "Enabling Sensors..."));
            /*
             * With services discovered, we are going to reset our state machine and start
             * working through the sensors we need to enable
             */
            reset();
            //enableNextSensor(gatt);
        }


        @Override
        public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status)
        {
            //For each read, pass the data up to the UI thread to update the display
            if (YOURSERVICE_DATA_CHAR.equals(characteristic.getUuid())) {
                mHandler.sendMessage(Message.obtain(null, MSG_YOURSERVICE, characteristic));


                //After reading the initial value, next we enable notifications
                //setNotifyNextSensor(gatt);
            }
        }

        @Override
        public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status)
        {
            //After writing the enable flag, next we read the initial value
            readNextSensor(gatt);
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic)
        {
            /*
             * After notifications are enabled, all updates from the device on characteristic
             * value changes will be posted here.  Similar to read, we hand these up to the
             * UI thread to update the display.
             */
            if (YOURSERVICE_DATA_CHAR.equals(characteristic.getUuid())) {
                mHandler.sendMessage(Message.obtain(null, MSG_YOURSERVICE, characteristic));
            }

        }

        @Override
        public void onDescriptorWrite(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status)
        {
            //Once notifications are enabled, we move to the next sensor and start over with enable
            advance();
            //enableNextSensor(gatt);
        }

        @Override
        public void onReadRemoteRssi(BluetoothGatt gatt, int rssi, int status) {
            Log.d(TAG, "Remote RSSI: "+rssi);
        }


         private String connectionState(int status) {
            switch (status) {
                case BluetoothProfile.STATE_CONNECTED:
                    return "Connected";
                case BluetoothProfile.STATE_DISCONNECTED:
                    return "Disconnected";
                case BluetoothProfile.STATE_CONNECTING:
                    return "Connecting";
                case BluetoothProfile.STATE_DISCONNECTING:
                    return "Disconnecting";
                default:
                    return String.valueOf(status);
            }
        }
    };

    /*
     * We have a Handler to process event results on the main thread
     */
    private static final int MSG_YOURSERVICE = 101;

    private static final int MSG_PROGRESS = 201;
    private static final int MSG_DISMISS = 202;
    private static final int MSG_CLEAR = 301;
    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            BluetoothGattCharacteristic characteristic;
            switch (msg.what) {
                case MSG_YOURSERVICE:
                    characteristic = (BluetoothGattCharacteristic) msg.obj;
                    if (characteristic.getValue() == null) {
                        Log.w(TAG, "Error obtaining YourService value");
                        return;
                    }
                    updateYourServiceValues(characteristic);
                    break;

                case MSG_PROGRESS:
                    mProgress.setMessage((String) msg.obj);
                    if (!mProgress.isShowing()) {
                        mProgress.show();
                    }
                    break;
                case MSG_DISMISS:
                    mProgress.hide();
                    break;
                case MSG_CLEAR:
                    clearDisplayValues();
                    break;
            }
        }
    };

    /* Methods to extract sensor data and update the UI */

    private void  updateYourServiceValues(BluetoothGattCharacteristic characteristic) {
        double urData = SensorTagData.extractData(characteristic);

        mData.setText(String.format("%.0f%%", urData));
    }




}









