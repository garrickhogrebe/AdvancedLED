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

public class editAnimationActivity extends AppCompatActivity {

    ListView variablesListView;
    ArrayList<String> displayStrings = new ArrayList<>();
    ArrayAdapter<String> adapter;
    animationInfo current_animation = null;
    int input_index;
    int num_inputs;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_edit_animation);
        Intent intent = getIntent();

        int animationInfo_index = intent.getIntExtra(animationActivity.EXTRA_MESSAGE, 0);
        current_animation = SingleLoadedList.animations.get(animationInfo_index);
        num_inputs = current_animation.num_inputs;
        variablesListView = findViewById(R.id.editAnimationVariablesListy);

        adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, displayStrings);
        variablesListView.setAdapter(adapter);
        updateDisplayStrings();

        variablesListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                View text = findViewById(R.id.textInputName);
                TextView textview = (TextView) text;
                textview.setText(current_animation.input_names[i]);
                input_index = i;
            }
        });

    }

    public void setInput (View v){
        EditText editText = (EditText) findViewById(R.id.editTextInputValue);
        String message = editText.getText().toString();
        if (message.matches("")){
            Toast.makeText(editAnimationActivity.this, "Fuck u", Toast.LENGTH_SHORT).show();
            return;
        }
        current_animation.input_values[input_index] = Integer.parseInt(message);
        updateDisplayStrings();
    }

    public void updateDisplayStrings(){
        displayStrings.clear();
        String variableName;
        String variableValue;
        for(int x = 0; x < num_inputs; x++){
            variableName = current_animation.input_names[x];
            variableValue = String.valueOf(current_animation.input_values[x]);

            displayStrings.add(variableName + " : " + variableValue);
        }
        //displayStrings.add("yort");
        adapter.notifyDataSetChanged();
    }
}