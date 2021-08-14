package com.example.listviewtest;

import java.util.ArrayList;

public class SingleLoadedList {

    public static ArrayList<animationInfo> animations = new ArrayList<>();
    public static ArrayList<String> animationNames = new ArrayList<>();

    private static SingleLoadedList instance = new SingleLoadedList();

    private SingleLoadedList(){}

    public static SingleLoadedList getInstance(){
        return instance;
    }

    public static void append(animationInfo new_animation){
        animations.add(new_animation);
        animationNames.add(new_animation.name);
    }

    public void remove(int index){
        animations.remove(index);
        animationNames.remove(index);
    }

}
