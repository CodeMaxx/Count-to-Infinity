package com.example.harshith.chatsockets;

import android.util.Log;

import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

/**
 * Created by harshith on 4/26/17.
 */



public class Utils {
    public static char escapeCharacter = '\\';

    static String escape_special_chars(String msg) {
        StringBuilder stringBuilder = new StringBuilder(msg);
        for(int i = 0; i != stringBuilder.length(); i++) {
            if(stringBuilder.charAt(i) == ':' || stringBuilder.charAt(i) == '#' || stringBuilder.charAt(i) == '~' || stringBuilder.charAt(i) == escapeCharacter) {
                stringBuilder.insert(i, escapeCharacter);
                i++;
            }
        }
        return stringBuilder.toString();
    }

    public static String list2string(ArrayList<String> v) { // converts login of strings to a string that
        StringBuilder ret = new StringBuilder("/");                              // fits our protocol
        for (String str : v) {
            ret.append( escape_special_chars(str) + ':');
        }
        ret.deleteCharAt(ret.length() - 1);
        ret.append('#');
        return ret.toString();
    }

    public static ArrayList<String> string2list(String msg) {   // converts string of our protocol to a login of strings
        System.out.println("MessageData: " + msg);
        StringBuilder stringBuilder = new StringBuilder(msg);
        Log.d("stringBuilder before", stringBuilder.toString());
        if(stringBuilder.charAt(0) == '/') {                                     // for easy access
            stringBuilder.deleteCharAt(0);
        }
        if(stringBuilder.charAt(stringBuilder.length() - 1) == '#') {
            stringBuilder.deleteCharAt(stringBuilder.length() - 1);
        }

        for(int i = 0; i != stringBuilder.length(); i++) {
            if(stringBuilder.charAt(i) == escapeCharacter) {
                if(stringBuilder.charAt(i + 1) == '#' || stringBuilder.charAt(i + 1) == '~' || stringBuilder.charAt(i + 1) == escapeCharacter) {
                    stringBuilder.deleteCharAt(i);
                    i++;
                }
            }
        }
        Log.d("stringBuilder after", stringBuilder.toString());

        String[] array = stringBuilder.toString().split(":");

        ArrayList<String> seglist = new ArrayList<String>();
        StringBuilder buffer = new StringBuilder("");

        for (String part : array) {
            System.out.println(part);
            buffer.append(part);
            if (buffer.charAt(buffer.length() - 1) == escapeCharacter) {
                buffer.deleteCharAt(buffer.length() - 1);
                buffer.append(':');
            }
            else {
                System.out.println(buffer);
                seglist.add(buffer.toString());
                buffer.delete(0, buffer.length());
            }
        }


        return seglist;
    }
}
