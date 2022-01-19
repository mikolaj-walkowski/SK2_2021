package com.example.sk_client;

import com.example.sk_client.actions.WaitForMessage;
import javafx.scene.control.Alert;
import javafx.scene.control.ButtonType;
import javafx.scene.control.TextArea;

import java.io.IOException;
import java.net.Socket;

public class Klient {
    static Klient instance;
    Integer port = 4200;
    String IP;
    WaitForMessage waitForMessage;
    public Socket clientSocket = null;
    Thread thread;
    Alert popup = new Alert(Alert.AlertType.ERROR,"Serwer nie odpowiada lub jest wylaczony!", ButtonType.OK);

    public Klient() {
        this.IP = "150.254.32.85";
        try {
            clientSocket = new Socket(IP, port);
        } catch (IOException e) {
            popup.showAndWait();
        }
    }

    static public Klient getInstance(){
        if(instance == null) {
            instance = new Klient();
        }
        return instance;
    }

    public void stopListening(){
        thread.interrupt();
    }

    public void closeSocket() throws IOException {
        thread.interrupt();
        if (!clientSocket.isClosed()) {
            clientSocket.close();
        }
    }

    public void activate(TextArea ta){
        try {
            waitForMessage = new WaitForMessage(clientSocket, ta);
            thread = new Thread(waitForMessage);
            thread.start();
        } catch (IOException e) {
            popup.showAndWait();
        }
    }
}
