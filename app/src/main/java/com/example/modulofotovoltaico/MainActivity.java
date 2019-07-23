package com.example.modulofotovoltaico;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.GridView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;

public class MainActivity extends AppCompatActivity implements View.OnClickListener{

    Button bConnect;
    TextView lblData;
    private GridView gridData;
    private GridAdapter gridAdapter;
    ArrayList<Sensores> dataSensores;
    private final int POS_TEMPERATURA = 0;
    private final int POS_HUMEDAD = 1;
    private final int POS_LUZ_UV = 2;
    private final int POS_LUZ_IR = 3;
    private final int POS_VOLTAJE = 4;
    private final int POS_CORRIENTE = 5;
    private final int POS_POTENCIA  = 6;

    public String strData[] = {"NaN","NaN","NaN","NaN","NaN","NaN","NaN"};



    @SuppressLint("HandlerLeak")
    Handler mHandler = new Handler(){
      @Override
        public void handleMessage(Message msg){
          super.handleMessage(msg);
          switch (msg.what){
              case Bluetooth.SUCCESS_CONNECT:
                  Bluetooth.connectedThread = new Bluetooth.ConnectedThread((BluetoothSocket)msg.obj);
                  Toast.makeText(getApplicationContext(),"Conectado",Toast.LENGTH_SHORT).show();
                  Bluetooth.connectedThread.start();
                  break;
              case Bluetooth.MESSAGE_READ:
                  byte[] readBuf = (byte[]) msg.obj;

                  String strIncom = new String(readBuf,0,msg.arg1);

                  Log.d("strIncom", strIncom);
                  lblData.setText(strIncom);
                  strData = strIncom.split(",");

                  actualizarSensores();
                  break;
          }
      }

        public boolean isFloatNumber(String num){
            try{
                Double.parseDouble(num);
            }catch (NumberFormatException nfe){
                return false;
            }
            return true;
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,WindowManager.LayoutParams.FLAG_FULLSCREEN);

        try
        {
            this.getSupportActionBar().hide();
        }
        catch (NullPointerException e){}


        setContentView(R.layout.activity_main);

        init();
        buttonInit();

    }

    void init(){
        Bluetooth.gethandler(mHandler);

        dataSensores = prepareDataSet();
        gridData = (GridView)findViewById(R.id.gridData);

        gridAdapter = new GridAdapter(this,dataSensores);

        gridData.setAdapter(gridAdapter);

    }

    private ArrayList<Sensores> prepareDataSet(){
        ArrayList<Sensores> sensores = new ArrayList<>();
        Sensores sensor;

        // Temperatura
        sensor = new Sensores("Temperatura");
        sensor.setMedida(strData[POS_TEMPERATURA]);
        sensor.setColor("#2146C6");
        sensores.add(sensor);


        // Humedad
        sensor = new Sensores("Humedad");
        sensor.setMedida(strData[POS_HUMEDAD]);
        sensores.add(sensor);

        // Luz UV
        sensor = new Sensores("Luz UV");
        sensor.setMedida(strData[POS_LUZ_UV]);
        sensores.add(sensor);

        // Luz IR
        sensor = new Sensores("Luz IR");
        sensor.setMedida(strData[POS_LUZ_IR]);
        sensores.add(sensor);

        // Voltaje
        sensor = new Sensores("Voltaje");
        sensor.setMedida(strData[POS_VOLTAJE]);
        sensores.add(sensor);

        // Corriente
        sensor = new Sensores("Corriente");
        sensor.setMedida(strData[POS_CORRIENTE]);
        sensores.add(sensor);

        // Potencia
        sensor = new Sensores("Potencia");
        sensor.setMedida(strData[POS_POTENCIA]);
        sensores.add(sensor);

        return sensores;
    }

    public void actualizarSensores(){
        // Se actualiza cada elemento del grid con cada dato nuevo
        for(int i = 0;i<strData.length;i++){
            gridAdapter.setMedida(i,strData[i]);
        }

        gridData.setAdapter(gridAdapter);


    }

    void buttonInit(){
        bConnect = (Button)findViewById(R.id.bConnect);
        bConnect.setOnClickListener(this);

        lblData = (TextView)findViewById(R.id.lblData);
        lblData.setText("Conectado");
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.bConnect:
                startActivity(new Intent("android.intent.action.BT1"));
            break;
        }
    }
}
