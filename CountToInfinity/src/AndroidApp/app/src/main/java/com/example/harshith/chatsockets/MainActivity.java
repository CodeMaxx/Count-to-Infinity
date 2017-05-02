package com.example.harshith.chatsockets;

import android.app.DialogFragment;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.support.design.widget.TabLayout;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v4.util.ArraySet;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.DefaultItemAnimator;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.support.v7.widget.Toolbar;

import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;
import android.support.v4.view.ViewPager;
import android.os.Bundle;
import android.util.Log;
import android.view.GestureDetector;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;

import android.widget.EditText;

import com.google.gson.Gson;
import com.google.gson.reflect.TypeToken;

import java.util.ArrayList;
import java.util.Map;

public class MainActivity extends AppCompatActivity implements ServerDetailDialogFragment.NoticeDialogListener{

    /**
     * The {@link android.support.v4.view.PagerAdapter} that will provide
     * fragments for each of the sections. We use a
     * {@link FragmentPagerAdapter} derivative, which will keep every
     * loaded fragment in memory. If this becomes too memory intensive, it
     * may be best to switch to a
     * {@link android.support.v4.app.FragmentStatePagerAdapter}.
     */
    private SectionsPagerAdapter mSectionsPagerAdapter;

    /**
     * The {@link ViewPager} that will host the section contents.
     */
    private ViewPager mViewPager;

    SharedPreferences sharedPreferences;
    Map<String, ?> userData;
    String left_over;

    ArrayList<Person> allUserList, friends, onlineUsers, friendRequests;
    UserLoginTask userLoginTask;

    String ipAddr;
    int port;

    Gson gson;



    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        allUserList = Database.getAllPeople();
        friends = Database.getFriends();
        onlineUsers = Database.getOnlineUsers();
        friendRequests = Database.getFriendRequests();

//        friends = gson.fromJson((String) userData.get(Constants.FRIENDS), new TypeToken<ArrayList<Person>>(){}.getType());
//        onlineUsers = gson.fromJson((String) userData.get(Constants.ONLINE), new TypeToken<ArrayList<Person>>(){}.getType());
//        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
//        setSupportActionBar(toolbar);
        // Create the adapter that will return a fragment for each of the three
        // primary sections of the activity.
        mSectionsPagerAdapter = new SectionsPagerAdapter(getSupportFragmentManager(), allUserList, friends, onlineUsers, friendRequests);

        // Set up the ViewPager with the sections adapter.
        mViewPager = (ViewPager) findViewById(R.id.container);
        mViewPager.setAdapter(mSectionsPagerAdapter);

        TabLayout tabLayout = (TabLayout) findViewById(R.id.tabs);
        tabLayout.setupWithViewPager(mViewPager);

        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });

    }

    @Override
    protected void onResume() {
        super.onResume();

        if(SocketHandler.getSocket() == null) {
            allUserList = new ArrayList<Person>();

            gson = new Gson();

            sharedPreferences = getApplicationContext().getSharedPreferences(Constants.LOGIN_FILE_KEY, Context.MODE_PRIVATE);
            userData = sharedPreferences.getAll();

            allUserList = Database.getAllPeople();
            friends = Database.getFriends();
            onlineUsers = Database.getOnlineUsers();

            if(allUserList == null) {
                Log.d("Main create", "all null");
            }
            if (friends == null) {
                Log.d("Main create", "friends null");

            }
            if (onlineUsers == null) {
                Log.d("Main create", "online null");

            }

            Intent intent = new Intent(getApplicationContext(), NetworkService.class);
            ArrayList<String> messageVector = new ArrayList<String>() {
                {
                    add("login");
                    add(Database.getUsername());
                    add(Database.getPassword());
                }
            };
            intent.putExtra(Constants.REQUEST, messageVector);
            intent.putExtra(Constants.IP_ADDR, ipAddr);
            intent.putExtra(Constants.PORT, port);
            startService(intent);
        }
//            userLoginTask = new UserLoginTask((String) userData.get(Constants.USERNAME), (String) userData.get(Constants.PASSWORD),
//                    (String) userData.get(Constants.IP_ADDR), (Integer) userData.get(Constants.PORT)) {
//                @Override
//                protected void onPostExecute(final Boolean success) {
//                    userLoginTask = null;
//                    Log.d("post the request :", "Come in?");
//                    if (success) {
//                        if (login.size() > 0) {
//                            Gson gson = new Gson();
//                            final String allUsers = gson.toJson(Person.getPersonArray(all_users));
//                            String frnds = gson.toJson(Person.getPersonArray(friends));
//                            String online = gson.toJson(Person.getPersonArray(online_users));
//                            SharedPreferences sharedPreferences = getApplicationContext().getSharedPreferences(Constants.LOGIN_FILE_KEY, Context.MODE_PRIVATE);
//                            SharedPreferences.Editor editor = sharedPreferences.edit();
//                            editor.putBoolean(Constants.LOGGED_IN, true);
//                            editor.putString(Constants.USERNAME, mEmail);
//                            editor.putString(Constants.PASSWORD, mPassword);
//                            editor.putString(Constants.NAME, login.get(2));
//                            editor.putString(Constants.ALL_USERS, allUsers);
//                            editor.putString(Constants.ALL_USERS, allUsers);
//                            editor.putString(Constants.FRIENDS, frnds);
//                            editor.putString(Constants.ONLINE, online);
//                            editor.commit();
//                            Log.d("Main", "Stored user data");
//                            allUserList.addAll(Person.getPersonArray(all_users));
//
////                            if(mAllUsersAdapter != null) {
////                                mAllUsersAdapter.notifyDataSetChanged();
////                            }
//
//                            left_over = data;
//                        }
//                    } else {
//                        Log.d("First Frag", "into this");
//                        android.app.FragmentManager fragmentManager = getFragmentManager();
//                        android.app.FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
//
//                        ServerDetailDialogFragment serverDetailDialogFragment = new ServerDetailDialogFragment();
//                        Bundle bundle = new Bundle();
//                        bundle.putString(Constants.IP_ADDR, userData.get(Constants.IP_ADDR).toString());
//                        bundle.putString(Constants.PORT, userData.get(Constants.PORT).toString());
//                        serverDetailDialogFragment.setArguments(bundle);
//                        serverDetailDialogFragment.show(fragmentTransaction, "server_details");
//                    }
//                }
//
//                @Override
//                protected void onCancelled() {
//                    userLoginTask = null;
//                }
//            };

//            userLoginTask.execute((Void) null);

//            allUserList = gson.fromJson((String) userData.get(Constants.ALL_USERS), new TypeToken<ArrayList<Person>>(){}.getType());

    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    public void onDialogPositiveClick(DialogFragment dialog) {
        final EditText ip = (EditText) dialog.getDialog().findViewById(R.id.ip);
        final EditText port = (EditText) dialog.getDialog().findViewById(R.id.portno);

        Log.d("Dialog Positive", "Entered!");
        Log.d("IP", ip.getText().toString());
        Log.d("PORT", port.getText().toString());



//        userLoginTask = new UserLoginTask((String) userData.get(Constants.USERNAME), (String) userData.get(Constants.PASSWORD),
//                String.valueOf(ip.getText()), Integer.parseInt(String.valueOf(userData.get(port.getText())))) {
//            @Override
//            protected void onPostExecute(final Boolean success) {
//                userLoginTask = null;
//                if (success) {
//                    if (login.size() > 0) {
//                        Gson gson = new Gson();
//                        String allUsers = gson.toJson(Person.getPersonArray(all_users));
//                        String frnds = gson.toJson(Person.getPersonArray(friends));
//                        String online = gson.toJson(Person.getPersonArray(online_users));
//                        SharedPreferences sharedPreferences = getApplicationContext().getSharedPreferences(Constants.LOGIN_FILE_KEY, Context.MODE_PRIVATE);
//                        SharedPreferences.Editor editor = sharedPreferences.edit();
//                        editor.putBoolean(Constants.LOGGED_IN, true);
//                        editor.putString(Constants.USERNAME, mEmail);
//                        editor.putString(Constants.PASSWORD, mPassword);
//                        editor.putString(Constants.NAME, login.get(2));
//                        editor.putString(Constants.ALL_USERS, allUsers);
//                        editor.putString(Constants.FRIENDS, frnds);
//                        editor.putString(Constants.ONLINE, online);
//                        editor.commit();
//
//                        allUserList.addAll(Person.getPersonArray(all_users));
//
////                        mAllUsersAdapter.notifyDataSetChanged();
//
//                        left_over = data;
//                    }
//                } else {
//                    Log.d("Dialog Positive Frag", "into this");
//                    android.app.FragmentManager fragmentManager = getFragmentManager();
//                    android.app.FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
//
//                    ServerDetailDialogFragment serverDetailDialogFragment = new ServerDetailDialogFragment();
//                    Bundle bundle = new Bundle();
//                    bundle.putString(Constants.IP_ADDR, ip.getText().toString());
//                    bundle.putString(Constants.PORT, port.getText().toString());
//                    serverDetailDialogFragment.setArguments(bundle);
//                    serverDetailDialogFragment.show(fragmentTransaction, "server_details");
//                }
//            }
//
//            @Override
//            protected void onCancelled() {
//                userLoginTask = null;
//            }
//        };

//        userLoginTask.execute((Void) null);
    }

    @Override
    public void onDialogNegativeClick(DialogFragment dialog) {
        allUserList = gson.fromJson((String) userData.get(Constants.ALL_USERS), new TypeToken<ArrayList<Person>>(){}.getType());
        mSectionsPagerAdapter.notifyDataSetChanged();
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    /**
     * A placeholder fragment containing a simple view.
     */
    public static class PlaceholderFragment extends Fragment {
        /**
         * The fragment argument representing the section number for this
         * fragment.
         */

        private static final String ARG_SECTION_NUMBER = "section_number";

        private RecyclerView mUsersRecyclerView;
        private RecyclerView.Adapter mUsersAdapter;
        private RecyclerView.LayoutManager mLayoutManager;
        private int sectionNumber;
        private ArrayList<Person> userList;

        private Gson gson;
        private BroadcastReceiver broadcastReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context context, Intent intent) {
                if(intent.getStringExtra(Constants.BROADCAST).equals(Constants.USERS)) {
                    mUsersAdapter.notifyDataSetChanged();
                }
            }
        };


        public PlaceholderFragment() {

        }
        /**
         * Returns a new instance of this fragment for the given section
         * number.
         */
        public static PlaceholderFragment newInstance(int sectionNumber, ArrayList<Person> userList) {
            PlaceholderFragment fragment = new PlaceholderFragment();
            Bundle args = new Bundle();
            Gson gson = new Gson();
            args.putInt(ARG_SECTION_NUMBER, sectionNumber);
            args.putString(Constants.USERS, gson.toJson(userList));
            fragment.setArguments(args);
            return fragment;
        }

        @Override
        public View onCreateView(LayoutInflater inflater, ViewGroup container,
                                 Bundle savedInstanceState) {
            gson = new Gson();
//            SharedPreferences sharedPreferences = getContext().getSharedPreferences(Constants.LOGIN_FILE_KEY, Context.MODE_PRIVATE);
//            Map<String,?> userData = sharedPreferences.getAll();
//            if(sectionNumber )
//                userList = gson.fromJson((String) userData.get(Constants.ALL_USERS), new TypeToken<ArrayList<Person>>(){}.getType());
//            System.out.println("No. of online users: " + userData.size());
//            for (Person person : userData ) {
            View rootView = inflater.inflate(R.layout.fragment_main, container, false);
            return rootView;

        }

        @Override
        public void onStart() {
            super.onStart();

            Bundle bundle = getArguments();
            if (bundle != null) {
                sectionNumber = bundle.getInt(ARG_SECTION_NUMBER);
                userList = (ArrayList<Person>)gson.fromJson(bundle.getString(Constants.USERS), new TypeToken<ArrayList<Person>>(){}.getType());
            }
            else {
                Log.d("On TabFrag Create", "Bundle - NULL");
                userList = new ArrayList<Person>();
            }

            mUsersRecyclerView = (RecyclerView) getView().findViewById(R.id.user_list);

            // use this setting to improve performance if you know that changes
            // in content do not change the layout size of the RecyclerView
//            mUsersRecyclerView.setHasFixedSize(true);

            // use a linear layout manager
            mLayoutManager = new LinearLayoutManager(getContext());
            mUsersRecyclerView.setLayoutManager(mLayoutManager);

            mUsersRecyclerView.setItemAnimator(new DefaultItemAnimator());

            // specify an adapter (see also next example)


            mUsersAdapter = new MyAdapter(userList);
            mUsersRecyclerView.setAdapter(mUsersAdapter);

            Log.d("PlaceHolderFragment", "Set userList");

            mUsersRecyclerView.addOnItemTouchListener(new RecyclerTouchListener(getContext(), mUsersRecyclerView, new ClickListener() {
                @Override
                public void onClick(View view, int position) {
                    Person person = userList.get(position);
                    Intent intent = new Intent(getContext(), ChatActivity.class);
                    intent.putExtra(Constants.USERNAME, person.getUsername());
                    startActivity(intent);
                }

                @Override
                public void onLongClick(View view, int position) {

                }
            }));

        }

        public void updatedUserList() {

        }

        public interface ClickListener {
            void onClick(View view, int position);

            void onLongClick(View view, int position);
        }

        public static class RecyclerTouchListener implements RecyclerView.OnItemTouchListener {

            private GestureDetector gestureDetector;
            private PlaceholderFragment.ClickListener clickListener;

            public RecyclerTouchListener(Context context, final RecyclerView recyclerView, final PlaceholderFragment.ClickListener clickListener) {
                this.clickListener = clickListener;
                gestureDetector = new GestureDetector(context, new GestureDetector.SimpleOnGestureListener() {
                    @Override
                    public boolean onSingleTapUp(MotionEvent e) {
                        return true;
                    }

                    @Override
                    public void onLongPress(MotionEvent e) {
                        View child = recyclerView.findChildViewUnder(e.getX(), e.getY());
                        if (child != null && clickListener != null) {
                            clickListener.onLongClick(child, recyclerView.getChildPosition(child));
                        }
                    }
                });
            }

            @Override
            public boolean onInterceptTouchEvent(RecyclerView rv, MotionEvent e) {

                View child = rv.findChildViewUnder(e.getX(), e.getY());
                if (child != null && clickListener != null && gestureDetector.onTouchEvent(e)) {
                    clickListener.onClick(child, rv.getChildPosition(child));
                }
                return false;
            }

            @Override
            public void onTouchEvent(RecyclerView rv, MotionEvent e) {
            }

            @Override
            public void onRequestDisallowInterceptTouchEvent(boolean disallowIntercept) {

            }
        }
    }

    /**
     * A {@link FragmentPagerAdapter} that returns a fragment corresponding to
     * one of the sections/tabs/pages.
     */
    public class SectionsPagerAdapter extends FragmentPagerAdapter {

        public ArrayList<Person> allUsers, friends, onlineUsers, friendRequests;
        public SectionsPagerAdapter(FragmentManager fm, ArrayList<Person> allUsers, ArrayList<Person> friends, ArrayList<Person> onlineUsers, ArrayList<Person> friendRequests) {
            super(fm);
            this.allUsers = allUsers;
            this.friends = friends;
            this.onlineUsers = onlineUsers;
            this.friendRequests = friendRequests;
        }

        @Override
        public Fragment getItem(int position) {
            // getItem is called to instantiate the fragment for the given page.
            // Return a PlaceholderFragment (defined as a static inner class below).
            Log.d("Sections Adapter", "Create Placeholder instance");
            if(position == 0) {
                return PlaceholderFragment.newInstance(position + 1, onlineUsers);
            }
            else if (position == 1) {
                return PlaceholderFragment.newInstance(position + 1, friends);
            }
            else if (position == 2) {
                return PlaceholderFragment.newInstance(position + 1, allUsers);
            }
            else if (position == 3) {
                return PlaceholderFragment.newInstance(position + 1, friendRequests);
            }
            return null;
        }

        @Override
        public int getCount() {
            // Show 3 total pages.
            return 3;
        }

        @Override
        public CharSequence getPageTitle(int position) {
            switch (position) {
                case 0:
                    return "Online Users";
                case 1:
                    return "All Friends";
                case 2:
                    return "All Users";
                case 3:
                    return "Friend Requests";
            }
            return null;
        }
    }
}
