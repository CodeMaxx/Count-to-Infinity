package com.example.harshith.chatsockets;

/**
 * Created by harshith on 4/27/17.
 */

import android.os.AsyncTask;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.ArrayList;

/**
 * Represents an asynchronous login/registration task used to authenticate
 * the user.
 */
public class UserLoginTask extends AsyncTask<Void, Void, Boolean> {

    final String mEmail;
    final String mPassword;
    private final String mIpAddr;
    private final int mPort;
    String data;

    ArrayList<String> login, online_users, all_users, friends;


    UserLoginTask(String email, String password, String ip_addr, int port) {
        mEmail = email;
        mPassword = password;
        mIpAddr = ip_addr;
        mPort = port;
        login = new ArrayList<String>();
    }

    @Override
    protected Boolean doInBackground(Void... params) {

        try {
            // Simulate network access.
            InetAddress inetAddress = Inet4Address.getByName(mIpAddr);
            if(SocketHandler.getSocket() == null) {
                SocketHandler.setSocket(new Socket(inetAddress, mPort));
            }
            Log.d("isKeepAlive", SocketHandler.getSocket().getKeepAlive() + "");
            InputStream inputStream = SocketHandler.getSocket().getInputStream();
            OutputStream outputStream = SocketHandler.getSocket().getOutputStream();

            ArrayList<String> messageVector = new ArrayList<String>() {
                {
                    add("login");
                    add(mEmail);
                    add(mPassword);
                }
            };

            byte[] message = (Utils.list2string(messageVector)).getBytes();
            outputStream.write(message);

            byte[] buffer = new byte[256];
            int endPos = 0;
            StringBuilder stringBuilder = new StringBuilder();

            // reading login ack
            int bytes = inputStream.read(buffer);
            if(bytes == -1) {
                return false;
            }
            stringBuilder.append(new String(buffer).substring(0, bytes));
            Log.d("stream", stringBuilder.toString());

            while((endPos = stringBuilder.indexOf("#")) < 0) {
                bytes = inputStream.read(buffer);
                if (bytes == -1) {
                    return false;
                } else {
                    stringBuilder.append(new String(buffer).substring(0, bytes));
                    Log.d("stream", stringBuilder.toString());
                }
            }
            Log.d("Meg", stringBuilder.toString());
            Log.d("End Pos", endPos + "");

            login = Utils.string2list(stringBuilder.substring(0, endPos + 1));
            System.out.println(login);
            if(login.get(0).equals("wrong")) {
                return false;
            }
            stringBuilder.delete(0, endPos + 1);

            // reading online users and users
            bytes = inputStream.read(buffer);
            stringBuilder.append(new String(buffer).substring(0, bytes));
            Log.d("stream", stringBuilder.toString());
            int counter = 0;

            while(true) {
                if(counter < 3) {
                    if ((endPos = stringBuilder.indexOf("#")) >= 0) {
                        if(counter == 0) {
                            online_users = Utils.string2list(stringBuilder.substring(0, endPos + 1));
                            System.out.println(online_users);
                        }
                        else if (counter == 1) {
                            friends = Utils.string2list(stringBuilder.substring(0, endPos + 1));
                            System.out.println(friends);
                        }
                        else if (counter == 2) {
                            all_users = Utils.string2list(stringBuilder.substring(0, endPos + 1));
                            System.out.println(all_users);
                        }
                        stringBuilder.delete(0, endPos + 1);
                        counter++;
                    } else {
                        bytes = inputStream.read(buffer);
                        if (bytes == -1) {
                            return false;
                        } else {
                            stringBuilder.append(new String(buffer).substring(0, bytes));
                            Log.d("stream", stringBuilder.toString());
                        }
                    }
                }
                else {
                    break;
                }
            }
            Log.d("Meg", stringBuilder.toString());

            stringBuilder.delete(0, endPos + 1);

            if(stringBuilder.length() > 0) {
                data = stringBuilder.toString();
            }
            else {
                data = "";
            }

        }  catch (UnknownHostException e) {
            return false;
        } catch (IOException e) {
            return false;
        }


        // TODO: register the new account here.
        return true;
    }


}
