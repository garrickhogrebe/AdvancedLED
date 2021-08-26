package com.example.listviewtest;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Toast;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;

public class PresetActivity extends AppCompatActivity {

    EditText newPreset;
    Button savePresetButton;
    Button deleteButton;
    ListView presetsListView;
    ArrayList<String> presetNames = new ArrayList<>();
    ArrayAdapter<String> adapter;
    ArrayList<PresetClass> presetList = new ArrayList<>();
    SingleLoadedList loadedList;
    PresetClass preset = new PresetClass();
    boolean delete = false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_preset);
        loadedList = SingleLoadedList.getInstance();
        preset.animations = SingleLoadedList.animations;
        preset.animationNames = SingleLoadedList.animationNames;

        newPreset = findViewById(R.id.editTextPresetName);
        savePresetButton = findViewById(R.id.savePresetButton);
        deleteButton = findViewById(R.id.deletePresetButton);


        //List set up
        presetsListView = findViewById(R.id.presetListy);
        adapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1, presetNames);
        presetsListView.setAdapter(adapter);
        loadAllPresets();

        presetsListView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                //Select a preset
                if(delete){
                    deletePresetFromFile(i);
                }
                else{
                    SingleLoadedList.animations = presetList.get(i).animations;
                    SingleLoadedList.animationNames = presetList.get(i).animationNames;
                }

            }
        });


    }

    public void deleteToggle(View v){
        if(delete){
            delete = false;
            v.setBackgroundColor(Color.BLUE);
        }else{
            delete = true;
            v.setBackgroundColor(Color.RED);
        }
    }

    public void addPresetToFile(View v){
        try{
            preset.name = newPreset.getText().toString();
            FileOutputStream fileOutputStream = openFileOutput("Presets.txt", MODE_PRIVATE);
            ObjectOutputStream objectOut = new ObjectOutputStream(fileOutputStream);
            presetList.add(preset);
            objectOut.writeObject(presetList);
            Toast.makeText(PresetActivity.this, preset.name, Toast.LENGTH_SHORT).show();

        } catch (FileNotFoundException e){
            Toast.makeText(PresetActivity.this, "AHHH", Toast.LENGTH_SHORT).show();
            e.printStackTrace();
        } catch (IOException e){
            Toast.makeText(PresetActivity.this, "AHHH2", Toast.LENGTH_SHORT).show();
            e.printStackTrace();
        }
        loadAllPresets();
    }

    public void deletePresetFromFile(int index){

        try{
            presetList.remove(index);
            FileOutputStream fileOutputStream = openFileOutput("Presets.txt", MODE_PRIVATE);
            ObjectOutputStream objectOut = new ObjectOutputStream(fileOutputStream);
            objectOut.writeObject(presetList);
            //Toast.makeText(PresetActivity.this, preset.name, Toast.LENGTH_SHORT).show();

        } catch (FileNotFoundException e){
            Toast.makeText(PresetActivity.this, "AHHH", Toast.LENGTH_SHORT).show();
            e.printStackTrace();
        } catch (IOException e){
            Toast.makeText(PresetActivity.this, "AHHH2", Toast.LENGTH_SHORT).show();
            e.printStackTrace();
        }

        loadAllPresets();
    }

    public void loadAllPresets(){
        try {
            //Toast.makeText(PresetActivity.this, "1", Toast.LENGTH_SHORT).show();
            FileInputStream fileInputStream = openFileInput("Presets.txt");
            //Toast.makeText(PresetActivity.this, "2", Toast.LENGTH_SHORT).show();
            ObjectInputStream ois = new ObjectInputStream(fileInputStream);
            //Toast.makeText(PresetActivity.this, "3", Toast.LENGTH_SHORT).show();
            //Object obj = ois.readObject();
            presetList = (ArrayList<PresetClass>) ois.readObject();
            //Toast.makeText(PresetActivity.this, "4", Toast.LENGTH_SHORT).show();
            if (presetList != null){
                //Toast.makeText(PresetActivity.this, "5", Toast.LENGTH_SHORT).show();
            }
            else{
                //Toast.makeText(PresetActivity.this, "6", Toast.LENGTH_SHORT).show();
                return;

            }
            //Toast.makeText(PresetActivity.this, "7", Toast.LENGTH_SHORT).show();
            presetNames.clear();
            for(PresetClass x : presetList){
                presetNames.add(x.name);
            }
            //Toast.makeText(PresetActivity.this, "8", Toast.LENGTH_SHORT).show();
            adapter.notifyDataSetChanged();

        }catch(Exception ex) {
            Toast.makeText(PresetActivity.this, "bro", Toast.LENGTH_SHORT).show();
            ex.printStackTrace();
        }
    }

}