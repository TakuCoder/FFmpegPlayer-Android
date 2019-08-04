package com.willxing.ffmpegplayer;

import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;

import java.io.File;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    static {
        System.loadLibrary("ffmpegplayer-lib");
    }
    Button btnPlay,btnPasue,btnStop,btnReset;
    Surface surface;

    final String inputurl = "http://192.168.1.106:8000/Jupiter.mp4";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        btnPlay = findViewById(R.id.btnplay);
        btnPasue = findViewById(R.id.btnpause);
        btnStop = findViewById(R.id.btnstop);
        btnReset = findViewById(R.id.btnreset);

        btnPlay.setOnClickListener(this);
        btnPasue.setOnClickListener(this);
        btnStop.setOnClickListener(this);
        btnReset.setOnClickListener(this);
        initSufaceView();
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()){
            case R.id.btnplay:
                playUrl();
                break;
            case R.id.btnpause:
                pause();
                break;
            case R.id.btnstop:
                stop();
                break;
            case R.id.btnreset:
                reset(inputurl);
                break;
                default:
        }
    }

    public void playUrl() {

        new Thread(new Runnable() {
            @Override
            public void run() {
                play(inputurl, surface);
            }
        }).start();
    }

    public void initSufaceView(){
        SurfaceView surfaceView = findViewById(R.id.surfaceView);
        surfaceView.getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(final SurfaceHolder holder) {
                surface = holder.getSurface();
            }
            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            }
            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {
            }
        });
    }

    public native void play(String url, Surface surface);
    public native void pause();
    public native void stop();
    public native void reset(String url);
}
