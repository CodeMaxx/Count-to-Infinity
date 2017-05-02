package com.example.harshith.chatsockets;

/**
 * Created by harshith on 4/30/17.
 */

public class MessageData {
    public String message;
    public boolean yours;

    public MessageData() {

    }
    public MessageData(String message, boolean yours) {
        this.message = message;
        this.yours = yours;
    }
}
