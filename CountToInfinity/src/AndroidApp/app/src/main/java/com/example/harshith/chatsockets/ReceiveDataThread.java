package com.example.harshith.chatsockets;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;

import com.google.gson.Gson;

import java.io.IOException;
import java.io.InputStream;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.Socket;
import java.util.ArrayList;

/**
 * Created by harshith on 5/1/17.
 */

public class ReceiveDataThread extends Thread{
    private Socket socket;
    private InputStream inputStream;
    Context context;
    Handler handler;
    ArrayList<String> allUsers, friends, online;
    String ipAddr;
    int port;

    public ReceiveDataThread(Socket socket, Handler deliveryHandler, Context context, String ipAddr, int port) {
        this.socket = socket;
        this.context = context;
        this.handler = deliveryHandler;
        this.ipAddr = ipAddr;
        this.port = port;
    }

    @Override
    public void run() {
        super.run();
        try {
            socket = SocketHandler.getSocket();
            if(socket == null) {
                System.out.println("Socket null");
                InetAddress inetAddress = Inet4Address.getByName(ipAddr);
                SocketHandler.setSocket(new Socket(inetAddress, port));
                System.out.println("Sock connected");
            }
            else if (!socket.isConnected()) {
                System.out.println("Sock not connected");

                InetAddress inetAddress = Inet4Address.getByName(ipAddr);
                SocketHandler.setSocket(new Socket(inetAddress, port));
                System.out.println("Sock connected");

            }
            socket = SocketHandler.getSocket();
            inputStream = socket.getInputStream();
            System.out.println("got inputstream");
            if (inputStream != null) {
                System.out.println("input stream not null");

                byte[] buffer = new byte[256];
                int endPos = 0;
                ArrayList<String> data;
                StringBuilder stringBuilder = new StringBuilder();
                while(true) {
                    // reading login ack
                    int bytes = inputStream.read(buffer);
                    if(bytes == -1) {
                        // throw error
                        continue;
                    }

                    System.out.println("got some data");

                    stringBuilder.append(new String(buffer).substring(0, bytes));
                    Log.d("stream", stringBuilder.toString());

                    while((endPos = stringBuilder.indexOf("#")) >= 0) {
                        data = Utils.string2list(stringBuilder.substring(0, endPos + 1));
                        System.out.println(data);
                        Message message = handler.obtainMessage(Constants.RESPONSE);
                        Bundle bundle = new Bundle();
                        bundle.putStringArrayList(Constants.READ, data);
                        message.setData(bundle);
                        handler.sendMessage(message);
                        stringBuilder.delete(0, endPos + 1);
                    }
                    System.out.print("end of while loop , Continues? ");
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

    }
}
