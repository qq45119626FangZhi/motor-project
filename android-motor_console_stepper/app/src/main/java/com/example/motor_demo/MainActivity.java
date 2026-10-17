package com.example.motor_demo;

import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;
import org.json.JSONException;
import org.json.JSONObject;

public class MainActivity extends AppCompatActivity implements MqttManager.Callback {

    EditText etHost, etPort, etDist, etSpeed;
    TextView tvConnStatus, tvRunDist, tvMotorState;
    Button btnConnect, btnDisconnect, btnStartMotor;

    MqttManager mqttManager;
    // 主题定义
    final String PUB_TOPIC = "console_to_gateway";
    final String SUB_TOPIC = "gateway_to_console";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        etHost = findViewById(R.id.etHost);
        etPort = findViewById(R.id.etPort);
        etDist = findViewById(R.id.etDist);
        etSpeed = findViewById(R.id.etSpeed);
        tvConnStatus = findViewById(R.id.tvConnStatus);
        tvRunDist = findViewById(R.id.tvRunDist);
        tvMotorState = findViewById(R.id.tvMotorState);
        btnConnect = findViewById(R.id.btnConnect);
        btnDisconnect = findViewById(R.id.btnDisconnect);
        btnStartMotor = findViewById(R.id.btnStartMotor);

        mqttManager = new MqttManager();
        mqttManager.setCallback(this);

        btnConnect.setOnClickListener(v -> {
            String host = etHost.getText().toString().trim();
            int port = Integer.parseInt(etPort.getText().toString().trim());

            new Thread(()->{
                mqttManager.connect(host, port, SUB_TOPIC);
            }).start();


        });

        btnDisconnect.setOnClickListener(v -> {
            mqttManager.disconnect();
            tvConnStatus.setText("已断开连接");
            tvConnStatus.setTextColor(0xFFFF0000);
        });

        btnStartMotor.setOnClickListener(v -> {
            if(!mqttManager.isConnected()){
                Toast.makeText(MainActivity.this,"请先连接MQTT",Toast.LENGTH_SHORT).show();
                return;
            }
            String dist = etDist.getText().toString().trim();
            String speed = etSpeed.getText().toString().trim();
            try {
                JSONObject json = new JSONObject();
                json.put("cmd","run");
                json.put("distance", dist);
                json.put("speed", speed);
                mqttManager.publish(PUB_TOPIC, json.toString());
            } catch (JSONException e) {
                e.printStackTrace();
            }
        });
    }

    // ========= MQTT回调 =========
    @Override
    public void onConnected() {
        tvConnStatus.setText("已连接");
        tvConnStatus.setTextColor(0xFF008800);
        Toast.makeText(this,"MQTT连接成功",Toast.LENGTH_SHORT).show();
    }

    @Override
    public void onDisconnected(String reason) {
        tvConnStatus.setText("未连接："+reason);
        tvConnStatus.setTextColor(0xFFD92121);
    }

    @Override
    public void onMessage(String topic, String payload) {
        // 网关上报消息，解析JSON更新UI
        try {
            Log.e("fangzhi", "onMessage:" + topic + " " + payload);
            JSONObject obj = new JSONObject(payload);
            if(obj.has("cur_distance")){
                tvRunDist.setText("已行进距离："+obj.getString("cur_distance")+" mm");
            }
            if(obj.has("motor_status")){
                tvMotorState.setText("当前状态："+obj.getString("motor_status"));
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void onError(String error) {
        tvConnStatus.setText("连接失败："+error);
        tvConnStatus.setTextColor(0xFFD92121);
        Toast.makeText(this,"MQTT错误："+error,Toast.LENGTH_LONG).show();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mqttManager.disconnect();
    }
}
