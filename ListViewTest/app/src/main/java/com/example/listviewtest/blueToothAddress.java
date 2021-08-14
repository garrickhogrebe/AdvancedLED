package com.example.listviewtest;

public class blueToothAddress {

    public static String address;

    private static blueToothAddress instance = new blueToothAddress();

    private blueToothAddress(){}

    public static blueToothAddress getInstance(){
        return instance;
    }


}
