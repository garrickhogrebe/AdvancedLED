package com.example.listviewtest;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;

public class addAnimationActivity extends AppCompatActivity {

    ListView listView;
    ListView editListView;
    SingleLoadedList loadedList;
    SingleAvailableList availableList;
    ArrayList<String> variableNames = new ArrayList<>();
    ArrayAdapter<String> editAdapter;
    animationInfo current_animation = null;
    int input_index = 0;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_add_animation);

        listView = findViewById(R.id.selectableListy);
        editListView = findViewById((R.id.editListy));

        loadedList = SingleLoadedList.getInstance();
        availableList = SingleAvailableList.getInstance();

        ArrayAdapter adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, availableList.animationNames);
        editAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, variableNames);
        listView.setAdapter(adapter);
        editListView.setAdapter(editAdapter);

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                View text = findViewById(R.id.variableNameText);
                TextView textview = (TextView) text;
                textview.setText(" ");
                beginEdit(i);
                input_index = 0;
                text = findViewById(R.id.variableNameText);
                textview = (TextView) text;
                textview.setText(current_animation.input_names[0]);
            }
        });

        editListView.setOnItemClickListener((new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                input_index = i;
                View text = findViewById(R.id.variableNameText);
                TextView textview = (TextView) text;
                textview.setText(current_animation.input_names[i]);
            }
        }));
    }

    public void beginEdit(int index){
        int num_inputs = availableList.animations.get(index).num_inputs;
        current_animation = availableList.animations.get(index);
        variableNames.clear();
        View text = findViewById(R.id.animationNameText);
        TextView textview = (TextView) text;
        textview.setText(current_animation.name);
        for (int x = 0; x < num_inputs; x++){
            variableNames.add(current_animation.input_names[x] + " : " + current_animation.input_values[x]);
        }
        editAdapter.notifyDataSetChanged();

    }

    public void setVariable(View view){

        if(current_animation == null){
            Toast.makeText(addAnimationActivity.this, "Select an animation", Toast.LENGTH_SHORT).show();
            return;
        }

        int num_inputs = current_animation.num_inputs;
        EditText editText = (EditText) findViewById(R.id.editTextTextVariableValue);
        String message = editText.getText().toString();
        if (message.matches("")){
            Toast.makeText(addAnimationActivity.this, "Fuck u", Toast.LENGTH_SHORT).show();
            return;
        }
        int value = Integer.parseInt(message);
        current_animation.input_values[input_index] = value;
        variableNames.clear();
        for (int x = 0; x < num_inputs; x++){
            variableNames.add(current_animation.input_names[x] + " : " + current_animation.input_values[x]);
        }
        editAdapter.notifyDataSetChanged();
    }

    public void addAnimation(View view){

        //copy data
        String inputString = current_animation.inputString;
        String name = current_animation.name;
        int id = current_animation.id;
        animationInfo new_animation = new animationInfo(id, name, inputString);
        for(int x = 0; x < current_animation.num_inputs; x++){
            new_animation.input_values[x] = current_animation.input_values[x];
        }
        //append new

        SingleLoadedList.append(new_animation);
        Toast.makeText(addAnimationActivity.this, name + " Added", Toast.LENGTH_SHORT).show();
    }
}