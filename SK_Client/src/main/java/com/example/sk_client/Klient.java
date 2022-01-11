package com.example.sk_client;

import com.example.sk_client.actions.WaitForMessage;
import javafx.scene.control.Alert;
import javafx.scene.control.ButtonType;
import javafx.scene.control.TextArea;

import java.io.IOException;
import java.net.Socket;

public class Klient {
    Integer port = 4200;
    String IP;
    WaitForMessage waitForMessage;
    TextArea receive;
    Socket clientSocket = null;
    Alert popup = new Alert(Alert.AlertType.ERROR,"Serwer nie odpowiada lub jest wylaczony!", ButtonType.OK);

    public Klient(String host, TextArea receive) {
        this.IP = host;
        this.receive = receive;
        activate();
    }

    public Socket getSocket(){
        if(clientSocket == null) {
            try {
                clientSocket = new Socket(IP, port);
            } catch (IOException e) {
                popup.showAndWait();
            }
        }
        return clientSocket;
    }

    public void activate(){
        try {
            Socket help = getSocket();
            waitForMessage = new WaitForMessage(help, receive, IP);
            Thread thread = new Thread(waitForMessage);
            thread.start();
        } catch (IOException ex) {
            popup.showAndWait();
        }
    }
}
