package org.example;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.web.socket.client.standard.StandardWebSocketClient;

import javax.net.ssl.*;
import java.io.FileInputStream;
import java.security.KeyStore;
import java.security.SecureRandom;
import java.security.cert.X509Certificate;

@Configuration
public class WebSocketConfig {
    @Bean
    public StandardWebSocketClient webSocketClient() throws Exception {
        return new StandardWebSocketClient();
    }
}