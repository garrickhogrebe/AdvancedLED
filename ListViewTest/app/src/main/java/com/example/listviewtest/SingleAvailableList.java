package com.example.listviewtest;

import java.util.ArrayList;

public class SingleAvailableList {

    public ArrayList<animationInfo> animations = new ArrayList<>();
    public ArrayList<String> animationNames = new ArrayList<>();

    //create an object
    private static SingleAvailableList instance = new SingleAvailableList();

    private SingleAvailableList(){
        append(new animationInfo(0,"Fill Solid", "Start/End/Color/Brightness"));
        append(new animationInfo(1, "Sinelon", "Start/End/Color/BPM"));
        append(new animationInfo(2, "Display Band Values", ""));
        append(new animationInfo(3,"Display Band Peak Values",""));
        append(new animationInfo(4,"Display Average Band Values", ""));
        append(new animationInfo(5,"Band Lerp","Band/Start/End/Color"));
        append(new animationInfo(6,"Music Fire", "Start/End/Base Color/Color Shift"));
        append(new animationInfo(7,"Fade Block", "Start/End/Color/Fade Rate"));
        append(new animationInfo(8, "Gravity Ball", "Start/End/Color/Gravity"));

        //append(new animationInfo(1,"Gravity Ball", "Start/End/Gravity/Color"));
        //append(new animationInfo(2,"Beat Bar", "Start/End/Bands/Color"));
        //append(new animationInfo(3,"Sinelon", "Start/End/Color/BPM"));
        //append(new animationInfo(4,"Firework", "Start/End/Speed/Color"));
        //append(new animationInfo(5,"Print Band Values", ""));
    }

    private void append(animationInfo new_animation){
        animations.add(new_animation);
        animationNames.add(new_animation.name);
    }

    public static SingleAvailableList getInstance(){
        return instance;
    }

}
