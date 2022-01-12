package com.example.sk_client.actions;

import com.example.sk_client.controllers.GuestController;
import com.example.sk_client.controllers.MainController;
import javafx.application.Platform;
import javafx.event.ActionEvent;
import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.TextArea;
import javafx.stage.Stage;
import javafx.stage.Window;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.sql.Timestamp;
import java.text.SimpleDateFormat;
import java.util.List;
import java.util.Optional;

public class WaitForMessage implements Runnable {
    private final BufferedReader reader;
    private final TextArea chat;
    private final String IP;
    private final SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");

    public WaitForMessage(Socket clientSocket, TextArea chat, String ip) throws IOException {
        this.reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));
        this.chat = chat;
        this.IP = ip;
    }

    public void toMain(Integer co) throws IOException {
        FXMLLoader loader = new FXMLLoader(this.getClass().getResource("/com/example/sk_client/main_page.fxml"));
        Parent parent = (Parent) loader.load();
        MainController controller = (MainController) loader.getController();
        controller.wyrzucenie(co);
        Scene scene = new Scene(parent);
        //Optional<Window> stage = Stage.getWindows().stream().filter(Window::isShowing).findFirst();
        Stage stage = (Stage) chat.getScene().getWindow();
        stage.setScene(scene);
        stage.show();
    }

    public void run() {
        boolean loop = true;
        Integer co = null;
        String ohno = "kick "+IP;
        while (loop) {
            try {
                String help = chat.getText();
                String serverMessage = this.reader.readLine();
                if(serverMessage == null){
                    loop = false;
                    co = 2;
                    Integer finalCo = co;
                    Platform.runLater(()-> {
                        try {
                            toMain(finalCo);
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    });
                }
                else if(serverMessage.equals(ohno)){
                    loop = false;
                    co = 1;
                    Integer finalCo = co;
                    Platform.runLater(()-> {
                        try {
                            toMain(finalCo);
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    });
                }
                else {
                    Timestamp timestamp = new Timestamp(System.currentTimeMillis());
                    help = help + format.format(timestamp) + ": " + serverMessage + "\n";
                    chat.setText(help);
                }
            } catch (IOException exc) {
                System.out.println(exc);
                exc.printStackTrace();
            }
        }
    }
}
