package org.example;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Configuration;
import org.springframework.web.socket.CloseStatus;
import org.springframework.web.socket.TextMessage;
import org.springframework.web.socket.WebSocketSession;
import org.springframework.web.socket.config.annotation.EnableWebSocket;
import org.springframework.web.socket.config.annotation.WebSocketConfigurer;
import org.springframework.web.socket.config.annotation.WebSocketHandlerRegistry;
import org.springframework.web.socket.handler.TextWebSocketHandler;
import org.springframework.web.socket.server.standard.TomcatRequestUpgradeStrategy;
import org.springframework.web.socket.server.support.DefaultHandshakeHandler;

import java.io.IOException;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

@Configuration
@EnableWebSocket
public class WebSocketConfiguration implements WebSocketConfigurer {

    private static final Logger logger = LoggerFactory.getLogger(WebSocketConfiguration.class);

    @Override
    public void registerWebSocketHandlers(WebSocketHandlerRegistry registry) {
        registry.addHandler(new SignalingHandler(), "/signaling")
                .setAllowedOriginPatterns("*")
                .setHandshakeHandler(new DefaultHandshakeHandler(new TomcatRequestUpgradeStrategy()));
    }

    public static class SignalingHandler extends TextWebSocketHandler {
        private static final Logger logger = LoggerFactory.getLogger(SignalingHandler.class);
        private final Map<String, WebSocketSession> sessions = new ConcurrentHashMap<>();

        @Override
        public void afterConnectionEstablished(WebSocketSession session) throws Exception {
            logger.info("New connection {} on port {}", session.getId(), session.getUri().getPort());
            sessions.put(session.getId(), session);
            session.sendMessage(new TextMessage("Hello world!"));
        }

        @Override
        protected void handleTextMessage(WebSocketSession session, TextMessage message) throws Exception {
            logger.info("Received message: {}\nFrom: {} on port {}", message.getPayload(), session.getId(), session.getUri().getPort());
            for (WebSocketSession webSocketSession : sessions.values()) {
                if (!webSocketSession.equals(session) && webSocketSession.isOpen()) {
                    try {
                        TextMessage textMessage = new TextMessage(message.getPayload());
                        webSocketSession.sendMessage(textMessage);

                        logger.info("Sending to {}. Message: \n{}\n", session.getId(), textMessage.getPayload());
                    } catch (IOException e) {
                        logger.error("Failed to send message to session {}", webSocketSession.getId(), e);
                    }
                }
            }
        }

        @Override
        public void afterConnectionClosed(WebSocketSession session, CloseStatus status) throws Exception {
            logger.info("Disconnected {} on port {}", session.getId(), session.getUri().getPort());
            sessions.remove(session.getId());
        }
    }
}