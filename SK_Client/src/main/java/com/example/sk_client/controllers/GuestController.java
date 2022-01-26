package com.example.sk_client.controllers;

import com.example.sk_client.Klient;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.*;
import javafx.stage.Stage;

import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.nio.charset.StandardCharsets;
import java.util.Objects;

public class GuestController {
    @FXML Button buttonWyslij;
    @FXML Button buttonWyjdz;
    @FXML TextField wiadomosc;
    @FXML TextArea czat;
    @FXML Label tytul;
    Klient klient;
    String nazwa, IP;
    PrintWriter writer;

    public void setNazwa(String nazwa) throws IOException {
        this.IP = "192.168.55.104";
        //this.IP = IP.substring(IP.indexOf("/")+1);
        this.nazwa = nazwa;
        this.tytul.setText("Pokoj " + nazwa);
        this.klient = Klient.getInstance();
//        this.czat = klient.getTextArea();
        this.klient.activate(czat);
        String inputText = "join "+nazwa+"\0";
        klient.clientSocket.getOutputStream().write(inputText.getBytes(StandardCharsets.US_ASCII));
    }

    @FXML protected void wyslijWiadomosc(){
        if(!(wiadomosc.getText() == null) && !wiadomosc.getText().trim().isEmpty()) {
            String inputText = "msg "+this.wiadomosc.getText() + "\0";
            try {
                klient.clientSocket.getOutputStream().write(inputText.getBytes(StandardCharsets.US_ASCII));
            }catch (Exception e){
                e.printStackTrace();
            }
            this.wiadomosc.clear();
        }
    }

    @FXML protected void toMain(ActionEvent event) throws IOException{
        try {
            klient.clientSocket.getOutputStream().write("leave\0".getBytes(StandardCharsets.US_ASCII));
        }catch (Exception e){
            e.printStackTrace();
        }
        klient.stopListening();
        Parent parent = FXMLLoader.load(Objects.requireNonNull(this.getClass().getResource("/com/example/sk_client/main_page.fxml")));
        Scene scene = new Scene(parent);
        Stage stage = (Stage) ((Node)event.getSource()).getScene().getWindow();
        stage.setScene(scene);
        stage.show();
    }
}
