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
import java.net.InetAddress;

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
        this.IP = String.valueOf(InetAddress.getLocalHost());
        this.IP = IP.substring(IP.indexOf("/")+1);
        this.nazwa = nazwa;
        this.tytul.setText("Pokoj " + nazwa);
        this.klient = new Klient(IP, czat);
        this.writer = new PrintWriter(new OutputStreamWriter(klient.getSocket().getOutputStream()), true);
        String inputText = "join "+nazwa;
        writer.println(inputText);
    }

    @FXML protected void wyslijWiadomosc(ActionEvent event){
        if(!(wiadomosc.getText() == null) && !wiadomosc.getText().trim().isEmpty()) {
            String inputText = this.wiadomosc.getText();
            String inputSize = "msg_size "+inputText.length();
            this.writer.println(inputSize);
            inputText = "msg "+inputText;
            this.writer.println(inputText);
            this.wiadomosc.clear();
        }
    }

    @FXML protected void toMain(ActionEvent event) throws IOException{
        writer.println("leave");
        klient.closeSocket();
        Parent parent = (Parent) FXMLLoader.load(this.getClass().getResource("/com/example/sk_client/main_page.fxml"));
        Scene scene = new Scene(parent);
        Stage stage = (Stage) ((Node)event.getSource()).getScene().getWindow();
        stage.setScene(scene);
        stage.show();
    }
}
