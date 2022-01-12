package com.example.sk_client.controllers;

import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Alert;
import javafx.scene.control.Button;
import javafx.scene.control.ButtonType;
import javafx.scene.control.TextField;
import javafx.stage.Stage;

import java.io.IOException;

public class MainController {
    @FXML Button buttonGuest;
    @FXML Button buttonHost;
    @FXML TextField adres;
    Alert popup = new Alert(Alert.AlertType.WARNING,"Nie podano nazwy pokoju.",ButtonType.OK);
    Alert popupOut = new Alert(Alert.AlertType.WARNING, "Wyrzucono Cie z pokoju.",ButtonType.OK);
    Alert popupTimeout = new Alert(Alert.AlertType.ERROR, "Nastapilo rozlaczenie z serwerem!", ButtonType.OK);
    Alert popupFailedJoin = new Alert(Alert.AlertType.ERROR, "Nie udalo sie dolaczyc do pokoju!",ButtonType.OK);
    Alert popupFailedCreate = new Alert(Alert.AlertType.ERROR, "Nie udalo sie stworzyc pokoju!",ButtonType.OK);


//    public void setEverything() {
//        try {
//            this.IP = String.valueOf(InetAddress.getLocalHost());
//            this.IP = IP.substring(IP.indexOf("/") + 1);
//            this.klient = new Klient(IP);
//            this.writer = new PrintWriter(new OutputStreamWriter(klient.getSocket().getOutputStream()), true);
//        } catch (UnknownHostException e) {
//            e.printStackTrace();
//        } catch (IOException e) {
//            e.printStackTrace();
//        }
//    }

    @FXML public void wyrzucenie(Integer co){
        switch (co) {
            case 1 -> popupOut.showAndWait();
            case 2 -> popupTimeout.showAndWait();
            case 3 -> popupFailedJoin.showAndWait();
            case 4 -> popupFailedCreate.showAndWait();
            default -> {
            }
        }
    }

    @FXML
    protected void toHost(ActionEvent event) throws IOException {
        if (adres.getText() == null || adres.getText().trim().isEmpty()) {
            popup.showAndWait();
        } else {
            try {
                FXMLLoader loader = new FXMLLoader(this.getClass().getResource("/com/example/sk_client/host_page.fxml"));
                Parent parent = (Parent) loader.load();
                HostController controller = (HostController) loader.getController();
                controller.setNazwa(adres.getText());
                Scene scene = new Scene(parent);
                Stage stage = (Stage) ((Node) event.getSource()).getScene().getWindow();
                stage.setScene(scene);
                stage.show();
            }catch (NullPointerException e){
            }
        }
    }

    @FXML
    protected void toGuest(ActionEvent event) throws IOException {
        if(adres.getText() == null || adres.getText().trim().isEmpty()){
            popup.showAndWait();
        }
        else {
            try {
                FXMLLoader loader = new FXMLLoader(this.getClass().getResource("/com/example/sk_client/guest_page.fxml"));
                Parent parent = (Parent) loader.load();
                GuestController controller = (GuestController) loader.getController();
                controller.setNazwa(adres.getText());
                Scene scene = new Scene(parent);
                Stage stage = (Stage) ((Node) event.getSource()).getScene().getWindow();
                stage.setScene(scene);
                stage.show();
            }catch (NullPointerException e){
            }
        }
    }
}