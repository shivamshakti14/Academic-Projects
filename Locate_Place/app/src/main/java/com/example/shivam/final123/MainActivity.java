package com.example.shivam.final123;

import java.util.*;
import java.lang.*;
import java.io.*;

import android.content.Context;
import android.content.res.AssetManager;
import android.location.Location;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.location.LocationServices;
import com.google.android.gms.location.places.ui.PlacePicker;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

public class MainActivity extends AppCompatActivity implements
        GoogleApiClient.ConnectionCallbacks,
        GoogleApiClient.OnConnectionFailedListener{

    private Location mLastLocation;
    private GoogleApiClient mGoogleApiClient;
    private TextView llll;
    private TextView llll1;
    private boolean mRequestLocationUpdates = false;

    double mLatitudeText;
    double mLongitudeText;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main); //Creates layout
        llll = (TextView) findViewById(R.id.textView); // Creates textview
        llll1 = (TextView) findViewById(R.id.textView2);

        //Check if mGoogleapiClient is null then create new
        if (mGoogleApiClient == null) {
            mGoogleApiClient = new GoogleApiClient.Builder(this)
                    .addConnectionCallbacks(this)
                    .addOnConnectionFailedListener(this)
                    .addApi(LocationServices.API)
                    .build();
        }
    }

//After creation of mGoogleApiClient it is connected using this function
    protected void onStart() {
        mGoogleApiClient.connect();
        super.onStart();
    }

    protected void onStop() {
        mGoogleApiClient.disconnect();
        super.onStop();
    }

    //When  mGoogleApiClient is connected , button is created, ,rest of the fuctions for calculation of distance is called here
    @Override
    public void onConnected(Bundle connectionHint) {
        Button pickerButton = (Button) findViewById(R.id.button);
        pickerButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String Loca="The places near you : \n\n";
                String Loca1="";

                mLastLocation = LocationServices.FusedLocationApi.getLastLocation(
                        mGoogleApiClient);
                if (mLastLocation != null) {
                    mLatitudeText=mLastLocation.getLatitude();
                    mLongitudeText=mLastLocation.getLongitude();
                    Loca1 ="Latitude : "+ Double.toString(mLatitudeText)  + "\n" +"Longitude : "+ Double.toString(mLongitudeText)+"\n";
                  /*  Toast.makeText(getApplicationContext(), Loca1, Toast.LENGTH_SHORT)
                            .show();*/
                    String datab ="";
                    BufferedReader reader = null;
                    try {
                        reader = new BufferedReader(
                                new InputStreamReader(getAssets().open("file1.txt")));
                        // do reading, usually loop until end of file reading
                        String sCurrentLine;
                        while ((sCurrentLine = reader.readLine()) != null) {
                            double lat2=Double.parseDouble(sCurrentLine);
                            sCurrentLine = reader.readLine();
                            double lon2=Double.parseDouble(sCurrentLine);
                            sCurrentLine = reader.readLine();
                            double q = distance(mLatitudeText,mLongitudeText,lat2,lon2);
                            if(q*1000<=100){
                                double q11=q*1000;
                                float k = (float) Math.round(q11 * 100) / 100;
                                datab = datab+sCurrentLine+"\n"+"Distance = "+String.valueOf(k)+"meters"+"\n\n";
                            }

                        }
                    } catch (IOException e) {
                    } finally {
                        if (reader != null) {
                            try {
                                reader.close();
                            } catch (IOException e) {
                            }
                        }
                    }

                    Loca = Loca + "  " + datab ;
                    llll1.setText(Loca1);
                    llll.setText(Loca);
                   /* Toast.makeText(getApplicationContext(), "Done", Toast.LENGTH_SHORT)
                            .show();*/
                }else{
                    Toast.makeText(getApplicationContext(), "Not Connected please check location settings", Toast.LENGTH_SHORT)
                            .show();

                }
            }
        });

    }

   //Function to calculate distance between two places
    private static  double distance(double lat1, double lon1, double lat2, double lon2) {
        double theta = lon1 - lon2;
        double dist = Math.sin(deg2rad(lat1)) * Math.sin(deg2rad(lat2)) + Math.cos(deg2rad(lat1)) * Math.cos(deg2rad(lat2)) * Math.cos(deg2rad(theta));
        dist = Math.acos(dist);
        dist = rad2deg(dist);
        dist = dist * 60 * 1.1515;
        dist = dist * 1.609344;
        return (dist);
    }
    //convert degree to radian
    private static double deg2rad(double deg) {
        return (deg * Math.PI / 180.0);
    }
    //convert radian to degree
    private static double rad2deg(double rad) {
        return (rad * 180 / Math.PI);
    }
    @Override
    public void onConnectionSuspended(int i) {
        mGoogleApiClient.connect();
    }


    @Override
    // Called when   mGoogleApiClient is unable to connect
    public void onConnectionFailed(@NonNull ConnectionResult connectionResult) {
        Toast.makeText(getApplicationContext(), "Connection failed", Toast.LENGTH_SHORT)
                .show();
    }
}

