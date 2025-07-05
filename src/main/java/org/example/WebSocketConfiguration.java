package org.example;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.context.annotation.Configuration;
import org.springframework.lang.NonNullApi;
import org.springframework.web.socket.CloseStatus;
import org.springframework.web.socket.TextMessage;
import org.springframework.web.socket.WebSocketSession;
import org.springframework.web.socket.config.annotation.EnableWebSocket;
import org.springframework.web.socket.config.annotation.WebSocketConfigurer;
import org.springframework.web.socket.config.annotation.WebSocketHandlerRegistry;
import org.springframework.web.socket.handler.TextWebSocketHandler;

import java.io.IOException;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

@Configuration
@EnableWebSocket
public class WebSocketConfiguration implements WebSocketConfigurer {
    private static final Logger logger = LoggerFactory.getLogger(WebSocketConfiguration.class);

    @Override
    public void registerWebSocketHandlers(WebSocketHandlerRegistry registry){
        registry.addHandler(new SignalingHandler(), "/signaling").setAllowedOriginPatterns("*");
    }

    public static class SignalingHandler extends TextWebSocketHandler {
        private static final Logger logger = LoggerFactory.getLogger(SignalingHandler.class);

        private final Map<String, WebSocketSession> sessions = new ConcurrentHashMap<>();

        @Override
        public void afterConnectionEstablished(WebSocketSession session) throws Exception {
            logger.info("New connection {}", session.getId());

            sessions.put(session.getId(), session);
        }

        @Override
        protected void handleTextMessage(WebSocketSession session, TextMessage message) throws Exception{
            logger.info("Received message: {}\nFrom: {}", message.getPayload(), session.getId());
            for (WebSocketSession webSocketSession : sessions.values()) {
                if(!webSocketSession.equals(session) && webSocketSession.isOpen()){
                    try {
                        webSocketSession.sendMessage(new TextMessage(message.getPayload()));
                    }catch (IOException e){
                    }
                }
            }
        }

        @Override
        public void afterConnectionClosed(WebSocketSession session, CloseStatus status) throws Exception{
            logger.info("Disconnected {}", session.getId());

            sessions.remove(session.getId());
        }
    }
}
