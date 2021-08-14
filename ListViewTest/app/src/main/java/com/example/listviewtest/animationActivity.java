package com.example.listviewtest;

import androidx.appcompat.app.AppCompatActivity;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Toast;

import java.io.IOException;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.UUID;


public class animationActivity extends AppCompatActivity {

    //Member Fields
    private BluetoothAdapter btAdapter = null;
    private BluetoothSocket btSocket = null;
    private OutputStream outStream = null;

    // UUID service - This is the type of Bluetooth device that the BT module is
    // It is very likely yours will be the same, if not google UUID for your manufacturer
    private static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    // MAC-address of Bluetooth module
    public String newAddress = null;

    public static final String EXTRA_MESSAGE = "Yeah";
    ListView animationListView;
    ArrayAdapter<String> adapter;
    boolean deleteMode = false;
    SingleLoadedList loadedList;

    @Override
    protected void onResume() {
        super.onResume();
        adapter.notifyDataSetChanged();
        newAddress = blueToothAddress.address;

        // Set up a pointer to the remote device using its address.
        BluetoothDevice device = btAdapter.getRemoteDevice(newAddress);

        //Attempt to create a bluetooth socket for comms
        try {
            btSocket = device.createRfcommSocketToServiceRecord(MY_UUID);
        } catch (IOException e1) {
            Toast.makeText(getBaseContext(), "ERROR - Could not create Bluetooth socket", Toast.LENGTH_SHORT).show();
        }

        // Establish the connection.
        try {
            btSocket.connect();
        } catch (IOException e) {
            try {
                btSocket.close();        //If IO exception occurs attempt to close socket
            } catch (IOException e2) {
                Toast.makeText(getBaseContext(), "ERROR - Could not close Bluetooth socket", Toast.LENGTH_SHORT).show();
            }
        }

        // Create a data stream so we can talk to the device
        try {
            outStream = btSocket.getOutputStream();
        } catch (IOException e) {
            Toast.makeText(getBaseContext(), "ERROR - Could not create bluetooth outstream", Toast.LENGTH_SHORT).show();
        }
        //When activity is resumed, attempt to send a piece of junk data ('x') so that it will fail if not connected
        // i.e don't wait for a user to press button to recognise connection failure
        sendData("x");
    }

    @Override
    public void onPause() {
        super.onPause();
        //Pausing can be the end of an app if the device kills it or the user doesn't open it again
        //close all connections so resources are not wasted

        //Close BT socket to device
        try     {
            btSocket.close();
        } catch (IOException e2) {
            Toast.makeText(getBaseContext(), "ERROR - Failed to close Bluetooth socket", Toast.LENGTH_SHORT).show();
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_animation);
        animationListView = findViewById(R.id.animationListy);

        loadedList = SingleLoadedList.getInstance();

        //animationNames.add("yeet");
        adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, loadedList.animationNames);
        //animationNames.add("yort");

        animationListView.setAdapter(adapter);

        animationListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                if(deleteMode){
                    loadedList.remove(i);
                    adapter.notifyDataSetChanged();
                }else{
                    editAnimation(i);
                }
            }
        });

        //Bluetooth Section
        //getting the bluetooth adapter value and calling checkBTstate function
        btAdapter = BluetoothAdapter.getDefaultAdapter();
        checkBTState();
    }

    //takes the UUID and creates a comms socket
    private BluetoothSocket createBluetoothSocket(BluetoothDevice device) throws IOException {

        return  device.createRfcommSocketToServiceRecord(MY_UUID);
    }

    //same as in device list activity
    private void checkBTState() {
        // Check device has Bluetooth and that it is turned on
        if(btAdapter==null) {
            Toast.makeText(getBaseContext(), "ERROR - Device does not support bluetooth", Toast.LENGTH_SHORT).show();
            finish();
        } else {
            if (btAdapter.isEnabled()) {
            } else {
                //Prompt user to turn on Bluetooth
                Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                startActivityForResult(enableBtIntent, 1);
            }
        }
    }

    // Method to send data
    private void sendData(String message) {
        byte[] msgBuffer = message.getBytes();

        try {
            //attempt to place data on the outstream to the BT device
            outStream.write(msgBuffer);
        } catch (IOException e) {
            //if the sending fails this is most likely because device is no longer there
            Toast.makeText(getBaseContext(), "ERROR - Device not found", Toast.LENGTH_SHORT).show();
            finish();
        }
    }

    public void addAnimation(View v){
        Intent intent = new Intent(this, addAnimationActivity.class);
        startActivity(intent);
    }

    public void deleteToggle(View v){

        if (deleteMode){
            deleteMode = false;
            v.setBackgroundColor(Color.BLUE);
        }else{
            deleteMode = true;
            v.setBackgroundColor(Color.RED);
        }
    }

    public void editAnimation(int index){
        Intent intent = new Intent(this, editAnimationActivity.class);
        intent.putExtra(EXTRA_MESSAGE, index);
        startActivity(intent);
    }

    public void upload(View v){
        //Tell controller in upload mode
        sendData("u");
        //For every loaded animation
        for(animationInfo a : SingleLoadedList.animations){
            //Send ID
            sendData(String.valueOf(a.id) + "/");
            //Send all inputs
            for (int x = 0; x < a.num_inputs - 2; x++){
                sendData(String.valueOf(a.input_values[x]) + "/");
            }

            //Tell controller inputs are over
            sendData("o");

            //Send layer
            sendData(String.valueOf(a.input_values[a.input_values.length-2]) + "/");

            //Send write type
            sendData(String.valueOf(a.input_values[a.input_values.length-1]) + "/");
        }

        //Tell controller thats it
        sendData("e");

    }
}