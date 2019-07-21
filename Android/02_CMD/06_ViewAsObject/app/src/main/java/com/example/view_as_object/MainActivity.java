package com.example.view_as_object;

//import android.support.v7.app.AppCompatActivity;
import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;

import android.view.Window;
import android.view.WindowManager;
import android.content.pm.ActivityInfo;
import android.graphics.Color;

import androidx.appcompat.widget.AppCompatTextView;
import android.view.View;
import android.view.Gravity;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //setContentView(R.layout.activity_main);
		//this.requestWindowFeature(Window.FEATURE_NO_TITLE);
		this.supportRequestWindowFeature(Window.FEATURE_NO_TITLE);
		this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,WindowManager.LayoutParams.FLAG_FULLSCREEN);
		this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		this.getWindow().getDecorView().setBackgroundColor(Color.BLACK);
		this.getWindow().getDecorView().setSystemUiVisibility(View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);
		
		AppCompatTextView myView = new AppCompatTextView(this);
		myView.setTextColor(Color.rgb(0,255,0));
		myView.setTextSize(60);
		myView.setGravity(Gravity.CENTER);
		myView.setText("Hello World!!!");
		
		setContentView(myView);
    }
	
	@Override
	protected void onPause(){
		super.onPause();
	}
	
	@Override
	protected void onResume(){
		super.onResume();
	}
}
