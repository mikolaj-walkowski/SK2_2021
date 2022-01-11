module com.example.sk_client {
    requires javafx.controls;
    requires javafx.fxml;
    requires java.sql;


    opens com.example.sk_client to javafx.fxml;
    exports com.example.sk_client;
    exports com.example.sk_client.controllers;
    opens com.example.sk_client.controllers to javafx.fxml;
}