package com.example.harshith.chatsockets;

import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import java.util.ArrayList;

/**
 * Created by harshith on 4/28/17.
 */

public class MyAdapter extends RecyclerView.Adapter<MyAdapter.ViewHolder> {
    private ArrayList<Person> mDataset;

    // Provide a reference to the views for each data item
    // Complex data items may need more than one view per item, and
    // you provide access to all the views for a data item in a view holder
    public static class ViewHolder extends RecyclerView.ViewHolder {
        // each data item is just a string in this case
        public TextView name, lastSeen, username;
        public ViewHolder(View v) {
            super(v);
            name = (TextView) v.findViewById(R.id.name);
            lastSeen = (TextView) v.findViewById(R.id.last_seen);
            username = (TextView) v.findViewById(R.id.username);
        }
    }

    // Provide a suitable constructor (depends on the kind of dataset)
    public MyAdapter(ArrayList<Person> myDataset) {
        mDataset = myDataset;
    }

    // Create new views (invoked by the layout manager)
    @Override
    public MyAdapter.ViewHolder onCreateViewHolder(ViewGroup parent,
                                                   int viewType) {
        // create a new view
        View itemView = LayoutInflater.from(parent.getContext())
                .inflate(R.layout.person_list_row, parent, false);
        // set the view's size, margins, paddings and layout parameters

        ViewHolder vh = new ViewHolder(itemView);
        return vh;
    }

    // Replace the contents of a view (invoked by the layout manager)
    @Override
    public void onBindViewHolder(ViewHolder holder, int position) {
        // - get element from your dataset at this position
        // - replace the contents of the view with that element
        String datetime = mDataset.get(position).getLastOnline();
        if(datetime != null) {
            if (!datetime.equals("")) {
                String[] strings = datetime.split(" ");
                datetime = "";
                for (int i = 0; i != strings.length; i++) {
                    if (i < 2) {
                        datetime += strings[i] + "/";
                    } else if (i == 2) {
                        datetime += strings[i] + " ";
                    } else if (i < 4) {
                        datetime += strings[i] + ":";
                    }
                    else {
                        datetime += strings[i];
                    }
                }
            }
        }
        else {
            datetime = new String();
        }
        holder.name.setText(mDataset.get(position).getName());
        holder.lastSeen.setText(datetime);
        holder.username.setText("@" + mDataset.get(position).getUsername());
    }

    // Return the size of your dataset (invoked by the layout manager)
    @Override
    public int getItemCount() {
        return mDataset.size();
    }
}