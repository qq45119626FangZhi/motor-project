package com.example.motor_demo;

import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.eclipse.paho.client.mqttv3.persist.MemoryPersistence;
import java.util.UUID;

public class MqttManager {
    private static final String TAG = "MqttManager";
    private MqttClient client;
    private final Handler mainHandler = new Handler(Looper.getMainLooper());

    public interface Callback {
        void onConnected();
        void onDisconnected(String reason);
        void onMessage(String topic, String payload);
        void onError(String error);
    }
    private Callback callback;
    public void setCallback(Callback cb) { callback = cb; }

    public void connect(String host, int port, String subTopic) {
        try {
            String broker = "tcp://" + host + ":" + port;
            String clientId = "android_" + UUID.randomUUID().toString().substring(0,8);
            client = new MqttClient(broker, clientId, new MemoryPersistence());
            MqttConnectOptions opt = new MqttConnectOptions();
            opt.setCleanSession(true);
            opt.setConnectionTimeout(5);
            opt.setKeepAliveInterval(30);
            opt.setAutomaticReconnect(true);

            client.setCallback(new MqttCallback() {
                @Override
                public void connectionLost(Throwable cause) {
                    post(()->{
                        String msg = cause==null?"断开连接":cause.getMessage();
                        if(callback!=null) callback.onDisconnected(msg);
                    });
                }
                @Override
                public void messageArrived(String topic, MqttMessage message) {
                    String payload = new String(message.getPayload());
                    post(()->{ if(callback!=null) callback.onMessage(topic, payload); });
                }
                @Override
                public void deliveryComplete(IMqttDeliveryToken token) {}
            });
            client.connect(opt);
            if(subTopic!=null && !subTopic.isEmpty()){
                client.subscribe(subTopic,1);
            }
            post(()->{ if(callback!=null) callback.onConnected(); });
            Log.d(TAG,"MQTT connected");
        }catch (Exception e){
            Log.e(TAG,"connect fail",e);
            post(()->{ if(callback!=null) callback.onError(e.getMessage()); });
        }
    }

    public void publish(String topic, String payload){
        try{
            if(client!=null && client.isConnected()){
                MqttMessage msg = new MqttMessage(payload.getBytes());
                msg.setQos(1);
                client.publish(topic,msg);
            }
        }catch (Exception e){
            Log.e(TAG,"publish err",e);
        }
    }

    public void disconnect(){
        try{
            if(client!=null && client.isConnected()) client.disconnect();
            if(client!=null) client.close();
        }catch (Exception e){
            e.printStackTrace();
        }
    }

    public boolean isConnected(){
        return client!=null && client.isConnected();
    }

    private void post(Runnable r){
        mainHandler.post(r);
    }
}