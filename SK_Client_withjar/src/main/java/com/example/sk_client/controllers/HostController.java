package com.example.sk_client.controllers;

import com.example.sk_client.Klient;
import com.example.sk_client.controllers.GuestController;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.*;

import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;

public class HostController extends GuestController {
    @FXML Button buttonWyrzuc;
    @FXML TextField wyrzuc;
    Alert popup2 = new Alert(Alert.AlertType.WARNING, "Nie podano adresu ip uzytkownika.",ButtonType.OK);
    Alert popup3 = new Alert(Alert.AlertType.WARNING, "Nie mozesz wyrzucic samego siebie!",ButtonType.OK);

    @Override
    public void setNazwa(String nazwa) throws IOException {
        this.IP = String.valueOf(InetAddress.getLocalHost());
        this.IP = IP.substring(IP.indexOf("/")+1);
        this.nazwa = nazwa;
        this.tytul.setText("Pokoj " + nazwa);
        this.klient = new Klient(IP, czat);
        this.writer = new PrintWriter(new OutputStreamWriter(klient.getSocket().getOutputStream()), true);
        String inputText = "create "+nazwa;
        writer.println(inputText);
    }

    @FXML protected void wyrzucUzytkownika(ActionEvent event){
        if(wyrzuc.getText() == null || wyrzuc.getText().trim().isEmpty()){
            popup2.showAndWait();
        }
        else if(wyrzuc.getText().equals(String.valueOf(IP))){
            popup3.showAndWait();
        }
        else{
            String inputText = this.wyrzuc.getText();
            inputText = "kick "+inputText;
            this.writer.println(inputText);
            this.wyrzuc.clear();
        }
    }
}
