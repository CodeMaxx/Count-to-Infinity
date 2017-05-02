package com.example.harshith.chatsockets;

import java.util.ArrayList;

/**
 * Created by harshith on 4/28/17.
 */

public class Person {
    private String name, username, lastOnline;
    private int friendIndication;

    private boolean online;

    private ArrayList<MessageData> messageDatas;


    public Person() {
        messageDatas = new ArrayList<MessageData>();
    }

    public Person(String name, String username, String lastOnline, int friendIndication) {
        this.name = name;
        this.username = username;
        this.lastOnline = lastOnline;
        this.friendIndication = friendIndication;
        messageDatas = new ArrayList<MessageData>();
    }

    public void addMessage(String message, boolean you) {
        if(messageDatas != null) {
            messageDatas.add(new MessageData(message, you));
            // update ui
        }
    }

    public void setMessageDatas(ArrayList<MessageData> messageDatas) {
        this.messageDatas = messageDatas;
    }



    public ArrayList<MessageData> getMessageDatas() {
        return messageDatas;
    }

    public void print() {
        System.out.print(name + " " + username + " " + lastOnline);
    }

    public boolean isOnline() {
        return online;
    }

    public void setOnline(boolean online) {
        this.online = online;
    }

    public int getFriendIndication() {
        return friendIndication;
    }

    public void setFriendIndication(int friendIndication) {
        this.friendIndication = friendIndication;
    }

    static ArrayList<Person> getPersonArray(ArrayList<String> list) {
        if(list.get(0).equals("users")) {
            ArrayList<Person> arrayList = new ArrayList<Person>();
            for (int i = 1; i != list.size(); i++) {
                boolean b = true;
                Person person = new Person();
                person.setUsername(list.get(i)); i++;
                person.setName(list.get(i)); i++;
                person.setFriendIndication(Integer.parseInt(list.get(i)));
                if(person.getFriendIndication() == 0) {
                    i++;
                    person.setLastOnline(list.get(i));
                }
                else {
                    person.setLastOnline("");
                }
                arrayList.add(person);
            }
            return  arrayList;
        }
        else if(list.get(0).equals("olusers")) {
            ArrayList<Person> arrayList = new ArrayList<Person>();
            for(int i = 1; i != list.size(); i++) {
                Person person = new Person();
                person.setUsername(list.get(i)); i++;
                person.setName(list.get(i));
                person.setLastOnline("Online now");
                arrayList.add(person);
            }
            return arrayList;
        }
        else if(list.get(0).equals("urfriends")) {
            ArrayList<Person> arrayList = new ArrayList<Person>();
            for(int i = 1; i != list.size(); i++) {
                Person person = new Person();
                person.setUsername(list.get(i)); i++;
                person.setName(list.get(i));
                person.setLastOnline("");
                arrayList.add(person);
            }
            return arrayList;
        }
        else {
            System.out.println(list);
        }
        return null;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getUsername() {
        return username;
    }

    public void setUsername(String username) {
        this.username = username;
    }

    public String getLastOnline() {
        return lastOnline;
    }

    public void setLastOnline(String lastOnline) {
        this.lastOnline = lastOnline;
    }

}
