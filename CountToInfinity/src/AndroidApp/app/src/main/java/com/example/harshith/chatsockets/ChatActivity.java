package com.example.harshith.chatsockets;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.DefaultItemAnimator;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import java.util.ArrayList;

public class ChatActivity extends AppCompatActivity {

    RecyclerView recyclerView;
    LinearLayoutManager mLayoutManager;
    ChatAdapter mAdapter;
    String username;
    ArrayList<MessageData> messageList;
    Person person;
    BroadcastReceiver broadcastReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            if (intent.getStringExtra(Constants.BROADCAST).equals(Constants.CHAT)) {
                mAdapter.notifyDataSetChanged();
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_chat);

        username = getIntent().getStringExtra(Constants.USERNAME);
        System.out.println(username + " opened");
        person = Database.getPerson(username);
        if (person.getName() != null && getSupportActionBar()!= null) {
            getSupportActionBar().setTitle(person.getName() + "(@" + username + ")");
        }
    }
    @Override
    protected void onStart() {
        super.onStart();
        ArrayList<String> messages = new ArrayList<String>() {
            {
                add("getmessages");
                add(username);
            }
        };
        Intent intent = new Intent(getApplicationContext(), NetworkService.class);
        intent.putExtra(Constants.REQUEST, messages);
        startService(intent);

    }

    @Override
    protected void onResume() {
        super.onResume();
        registerReceiver(broadcastReceiver, new IntentFilter(Constants.BROADCAST_BASE));

        Button sendReq = (Button) findViewById(R.id.send_req);
        final EditText editText = (EditText) findViewById(R.id.chatBox);
        messageList = person.getMessageDatas();
        String control = null;
        if(person.getFriendIndication() == 3) {
            sendReq.setText("Send Friend Request");
            control = "friend";
        }
        else if(person.getFriendIndication() == 1) {
            sendReq.setText("The other person has not yet accepted your friend request");
        }
        else if(person.getFriendIndication() == -1) {
            sendReq.setText("Accept Friend Request");
            control = "accept";
        }
        else if(person.getFriendIndication() == 2) {
            sendReq.setText("Unblock");
            control = "unblock";
        }
        else if(person.getFriendIndication() == -2) {
            sendReq.setText("You are blocked");
        }
        else if(person.getFriendIndication() == 0) {
            Button button = (Button) findViewById(R.id.send);
            sendReq.setVisibility(View.GONE);
            editText.setFreezesText(true);
            button.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    final String message = editText.getText().toString();
                    ArrayList<String> messageList = new ArrayList<String>() {
                        {
                            add("message");
                            add(username);
                            add(message);
                        }
                    };
                    Intent intent = new Intent(getApplicationContext(), NetworkService.class);
                    intent.putExtra(Constants.REQUEST, messageList);
                    startService(intent);
                }
            });
        }

        if((person.getFriendIndication() == 3 || person.getFriendIndication() == -1 || person.getFriendIndication() == 2) && control != null ){
            final String finalControl = control;
            sendReq.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    ArrayList<String> messageList = new ArrayList<String>() {
                        {
                            add(finalControl);
                            add(username);
                        }
                    };
                    Intent intent = new Intent(getApplicationContext(), NetworkService.class);
                    intent.putExtra(Constants.REQUEST, messageList);
                    startService(intent);
                }
            });
        }

        recyclerView = (RecyclerView) findViewById(R.id.chat);

//        // use this setting to improve performance if you know that changes
//        // in content do not change the layout size of the RecyclerView
//        recyclerView.setHasFixedSize(true);

        // use a linear layout manager
        mLayoutManager = new LinearLayoutManager(getApplicationContext());
        recyclerView.setLayoutManager(mLayoutManager);

        recyclerView.setItemAnimator(new DefaultItemAnimator());

        // specify an adapter (see also next example)

        mAdapter = new ChatAdapter(messageList);
        recyclerView.setAdapter(mAdapter);
    }

    @Override
    protected void onPause() {
        super.onPause();
        registerReceiver(broadcastReceiver, new IntentFilter(Constants.BROADCAST_BASE));
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
        this.finish();
    }
}
