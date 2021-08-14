package com.example.listviewtest;

import java.io.Serializable;
import java.util.ArrayList;

public class animationInfo implements Serializable {
    int num_inputs;
    int id;
    String name;
    String inputString;
    String[] input_names;
    int[] input_values;

    animationInfo(int new_id, String new_name, String variable_names){
        id = new_id;
        name = new_name ;
        inputString = variable_names;
        input_names = (inputString + "/Layer/Write Type").split("/");
        num_inputs = input_names.length;
        input_values = new int[num_inputs];

        if(input_names[0].matches("")){
            num_inputs = 0;
        }
        for (int i = 0; i < num_inputs; i++){
            input_values[i] = 0;
        }
    }

}
