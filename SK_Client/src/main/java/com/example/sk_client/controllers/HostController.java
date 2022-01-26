package com.example.sk_client.controllers;

import com.example.sk_client.Klient;
import javafx.fxml.FXML;
import javafx.scene.control.*;

import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.nio.charset.StandardCharsets;

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
        this.klient = Klient.getInstance();
        this.klient.activate(czat);
        //this.writer = new PrintWriter(new OutputStreamWriter(klient.clientSocket.getOutputStream()), true);
        String inputText = "create "+nazwa+"\0";
        klient.clientSocket.getOutputStream().write(inputText.getBytes(StandardCharsets.US_ASCII));
        //writer.write(inputText);
    }

    @FXML protected void wyrzucUzytkownika(){
        if(wyrzuc.getText() == null || wyrzuc.getText().trim().isEmpty()){
            popup2.showAndWait();
        }
        else if(wyrzuc.getText().equals(klient.clientSocket.getLocalSocketAddress().toString().substring(1,klient.clientSocket.getLocalSocketAddress().toString().indexOf(':')))){
            popup3.showAndWait();
        }
        else{
            String inputText = this.wyrzuc.getText();
            inputText = "kick "+inputText+"\0";
            try {
                klient.clientSocket.getOutputStream().write(inputText.getBytes(StandardCharsets.US_ASCII));
            }catch (Exception  e){
                e.printStackTrace();
            }
//            this.writer.println(inputText);
//            this.wyrzuc.clear();
        }
    }
}
