package com.example.listviewtest;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {

    // ListView
    ListView listView;

    //Data for list
    String[] menus = {"Bluetooth Setup", "Select animations"};
    //Intent blueIntent = new Intent(this, blueActivity.class);
    //Intent animationsIntent = new Intent(this, animationActivity.class);
    //Intent blueIntent = new Intent(this, blueActivity.class);
    //Intent[] intents = {blueIntent, animationsIntent};

    public void menu_choice(int index){
        Intent intent;
        switch (index) {
            case 0:
                intent = new Intent(this, blueActivity.class);
                break;
            case 1:
                intent = new Intent(this, animationActivity.class);
                break;
            default:
                intent = new Intent(this, blueActivity.class);

        }
        startActivity(intent);

    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        listView = findViewById(R.id.listviewy);

        ArrayAdapter adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, menus);

        listView.setAdapter(adapter);

        // Click events in listview

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                //Toast.makeText(MainActivity.this, "Adding " + adapter.getItem(i), Toast.LENGTH_SHORT).show();
                menu_choice(i);
            }
        });

    }
}