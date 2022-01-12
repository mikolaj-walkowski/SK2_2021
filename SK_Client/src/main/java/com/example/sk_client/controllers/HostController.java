package com.example.sk_client.controllers;

import com.example.sk_client.Klient;
import com.example.sk_client.controllers.GuestController;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.scene.control.*;

import java.io.PrintWriter;

public class HostController extends GuestController {
    @FXML Button buttonWyrzuc;
    @FXML TextField wyrzuc;
    Alert popup1 = new Alert(Alert.AlertType.CONFIRMATION, "Uzytkownik zostal wyrzucony z pokoju.", ButtonType.OK);
    Alert popup2 = new Alert(Alert.AlertType.WARNING, "Nie podano adresu ip uzytkownika.",ButtonType.OK);
    Alert popup3 = new Alert(Alert.AlertType.WARNING, "Nie mozesz wyrzucic samego siebie!",ButtonType.OK);

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
            popup1.showAndWait();
        }
    }
}
