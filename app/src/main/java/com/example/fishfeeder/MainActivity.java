package com.example.fishfeeder;

import android.os.Bundle;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import com.google.android.material.textfield.TextInputEditText;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

import java.util.ArrayList;
import java.util.concurrent.TimeUnit;

public class MainActivity extends AppCompatActivity {

    FirebaseDatabase database;
    DatabaseReference feedTimeRef;
    DatabaseReference feedNowRef;
    DatabaseReference feedTimeChangedRef;
    DatabaseReference lastFeedingTimesRef;
    TextInputEditText feedTimeHoursEt;
    TextInputEditText feedTimeMinutesEt;
    TextView timeInHoursTv;
    TextView timeInMinutesTv;
    ListView feedingTimesLv;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        database = FirebaseDatabase.getInstance();
        feedTimeRef = database.getReference("feedTime");
        feedNowRef = database.getReference("feedNow");
        feedTimeChangedRef = database.getReference("feedTimeChanged");
        lastFeedingTimesRef = database.getReference("lastFeedingTime");
        feedTimeHoursEt = findViewById(R.id.feedTimeHoursEt);
        feedTimeMinutesEt = findViewById(R.id.feedTimeMinutesEt);
        timeInHoursTv = findViewById(R.id.timeInHours);
        timeInMinutesTv = findViewById(R.id.timeInMinutes);
        feedingTimesLv = findViewById(R.id.feedingTimesLv);
        final ArrayList<String> listItems = new ArrayList<>();
        final ArrayAdapter<String> adapter = new ArrayAdapter<>( this, android.R.layout.simple_list_item_1, listItems);
        feedingTimesLv.setAdapter(adapter);
        feedTimeRef.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                long feedTime = snapshot.getValue(Long.class);
                long hours = TimeUnit.MILLISECONDS.toHours(feedTime);
                long minutes = TimeUnit.MILLISECONDS.toMinutes(feedTime - (hours * 60 * 60 * 1000));
                timeInHoursTv.setText(hours + " hours and");
                timeInMinutesTv.setText(minutes + " minutes");
            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {

            }
        });
        lastFeedingTimesRef.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                listItems.clear();
                listItems.addAll((ArrayList<String>) snapshot.getValue());
                adapter.notifyDataSetChanged();
            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {

            }
        });
    }

    public void feedFish(View view) {
        feedNowRef.setValue(1);
    }

    public void changeFeedTime(View view) {
        long feedTimeHours = Integer.parseInt(feedTimeHoursEt.getEditableText().toString());
        long feedTimeMinutes = Integer.parseInt(feedTimeMinutesEt.getEditableText().toString());
        if(feedTimeHours < 0 || feedTimeHours > 48) {
            Toast.makeText(this, "Invalid hours value", Toast.LENGTH_SHORT).show();
        } else if(feedTimeMinutes < 0 || feedTimeMinutes > 59) {
            Toast.makeText(this, "Invalid minutes value", Toast.LENGTH_SHORT).show();
        } else {
            long feedTime = feedTimeHours * 1000 * 60 * 60 + feedTimeMinutes * 1000 * 60;
            feedTimeRef.setValue(feedTime);
            feedTimeChangedRef.setValue(1);
        }
    }
}