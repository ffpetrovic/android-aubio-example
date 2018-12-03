package ffpetrovic.anrdroid_aubio_example;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Handler;
import android.os.Looper;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    private int         sampleRate = 0;
    private int         bufferSize = 0;
    private int         readSize = 0;
    private int         amountRead = 0;
    private float[]     buffer = null;
    private short[]     intermediaryBuffer = null;

    /* These variables are used to store pointers to the C objects so JNI can keep track of them */
    public long ptr = 0;
    public long input = 0;
    public long pitch = 0;

    public boolean      isRecording = false;
    private AudioRecord audioRecord = null;
    Thread audioThread;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (ContextCompat.checkSelfPermission(this,
                Manifest.permission.RECORD_AUDIO) != PackageManager.PERMISSION_GRANTED) {

            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.RECORD_AUDIO},
                    11);


        } else {
            init();
            start();
        }


    }

    static {
        System.loadLibrary("aubio");
        System.loadLibrary("pitch");
    }

    private void init() {
        sampleRate = 44100;
        bufferSize = 4096;
        readSize = bufferSize / 4;
        buffer = new float[readSize];
        intermediaryBuffer = new short[readSize];
    }
//
    public void start() {
        if(!isRecording) {
            isRecording = true;
//        sampleRate = AudioUtils.getSampleRate();
//        bufferSize = AudioRecord.getMinBufferSize(sampleRate, AudioFormat.CHANNEL_IN_DEFAULT, AudioFormat.ENCODING_PCM_16BIT);
            initPitch(sampleRate, bufferSize);
            audioRecord = new AudioRecord(MediaRecorder.AudioSource.DEFAULT, sampleRate, AudioFormat.CHANNEL_IN_DEFAULT,
                    AudioFormat.ENCODING_PCM_16BIT, bufferSize);
            audioRecord.startRecording();
            audioThread = new Thread(new Runnable() {
                //Runs off the UI thread
                @Override
                public void run() {
                    findNote();
                }
            }, "Tuner Thread");
            audioThread.start();
        }
    }

    private void findNote() {
        while (isRecording) {
            amountRead = audioRecord.read(intermediaryBuffer, 0, readSize);
            buffer = shortArrayToFloatArray(intermediaryBuffer);
            final float frequency = getPitch(buffer);
            runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    ((TextView) findViewById(R.id.pitchView)).setText(String.valueOf(frequency));
                }
            });
        }
    }

    private float[] shortArrayToFloatArray(short[] array) {
        float[] fArray = new float[array.length];
        for (int i = 0; i < array.length; i++) {
            fArray[i] = (float) array[i];
        }
        return fArray;
    }

    private native float    getPitch(float[] input);
    private native void     initPitch(int sampleRate, int B);
    private native void     cleanupPitch();
}
