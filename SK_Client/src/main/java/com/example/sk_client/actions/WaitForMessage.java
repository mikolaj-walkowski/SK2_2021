package com.example.sk_client.actions;

import com.example.sk_client.Klient;
import com.example.sk_client.controllers.MainController;
import javafx.application.Platform;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Alert;
import javafx.scene.control.ButtonType;
import javafx.scene.control.TextArea;
import javafx.stage.Stage;

import java.io.IOException;
import java.io.InputStream;
import java.net.Socket;
import java.sql.Timestamp;
import java.text.SimpleDateFormat;

public class WaitForMessage implements Runnable {
    InputStream cos;
    Socket socket;
    private final TextArea chat;
    private final SimpleDateFormat format = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
    Alert popup1 = new Alert(Alert.AlertType.CONFIRMATION, "Uzytkownik zostal wyrzucony z pokoju.", ButtonType.OK);
    Alert popup2 = new Alert(Alert.AlertType.ERROR, "Uzytkownik nie zostal wyrzucony z pokoju.", ButtonType.OK);

    public WaitForMessage(Socket clientSocket, TextArea chat) throws IOException {
        socket = clientSocket;
        cos = clientSocket.getInputStream();
        this.chat = chat;
        while(cos.available()>0){cos.read();}
    }

    public void toMain(Integer co) throws IOException {
        FXMLLoader loader = new FXMLLoader(this.getClass().getResource("/com/example/sk_client/main_page.fxml"));
        Parent parent = loader.load();
        MainController controller = loader.getController();
        controller.wyrzucenie(co);
        Scene scene = new Scene(parent);
        //Optional<Window> stage = Stage.getWindows().stream().filter(Window::isShowing).findFirst();
        Stage stage = (Stage) chat.getScene().getWindow();
        stage.setScene(scene);
        stage.show();
        Klient.getInstance().stopListening();
    }

    public void run() {
        boolean loop = true;
        String serverMessage, help;
        char a;
        int co;
        while (loop && !Thread.currentThread().isInterrupted()) {
            serverMessage ="";
            try {
                help = chat.getText();
                while(cos.available()==0){}
                do{
                    a = (char)cos.read();
                    serverMessage += a;
                }
                while(cos.available()>0&&a!='\0');
                //serverMessage = this.cos.readLine();
                System.out.println(serverMessage);
                switch (serverMessage) {
                    case "terminate\0":
                        loop = false;
                        co = 2;
                        int finalCo = co;
                        socket.close();
                        Platform.runLater(() -> {
                            try {
                                toMain(finalCo);
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        });
                        break;
                    case "failed join\0":
                        loop = false;
                        co = 3;
                        finalCo = co;
                        Platform.runLater(() -> {
                            try {
                                toMain(finalCo);
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        });
                        break;
                    case "failed create\0":
                        loop = false;
                        co = 4;
                        finalCo = co;
                        Platform.runLater(() -> {
                            try {
                                toMain(finalCo);
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        });
                        break;
                    case "failed kick\0":
                        Platform.runLater(()->popup2.showAndWait());
                        break;
                    case "success kick\0":
                        Platform.runLater(()->popup1.showAndWait());
                        break;
                    case "kick\0":
                        loop = false;
                        co = 1;
                        finalCo = co;
                        Platform.runLater(() -> {
                            try {
                                toMain(finalCo);
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        });
                        break;
                    }
                    if (serverMessage.startsWith("msg ")) {
                        serverMessage = serverMessage.substring(4);
                        Timestamp timestamp = new Timestamp(System.currentTimeMillis());
                        help = help + format.format(timestamp) + " by " + serverMessage + "\n\n";
                        chat.setText(help);
                        chat.setScrollTop(Double.MAX_VALUE);
                    }
                } catch (IOException exc) {
                if(!exc.getLocalizedMessage().equals("Socket closed")){
                    exc.printStackTrace();
                }
            }
        }
    }
}
