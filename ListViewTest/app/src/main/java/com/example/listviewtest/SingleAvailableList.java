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
        append(new animationInfo(9, "Beat Bar", "Band/Start/End/Color/Locality"));
        append(new animationInfo(10,"Beat Detector", "Band/Start/End/Color"));
        append(new animationInfo(11, "Zoomer", "Start/End/Color/Period"));
        append(new animationInfo(12,"Tetris","Start/End/Color/Color Shift/Period/Bar Width"));
        append(new animationInfo(13, "Music Color", "Start/End/Color Start/Color End"));
        append(new animationInfo(14, "Music Color Volume", "Start/End/Color Start/Color End/Locality"));
        append(new animationInfo(15,"Dueling Bars", "Upper Start/Upper Size/Upper Color/Lower Start/Lower Size/ Lower Color/Period/Send Size"));
        append(new animationInfo(16, "Dueling Bar Sender", "Destination Index/Start/Direction/Period/Size"));
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
