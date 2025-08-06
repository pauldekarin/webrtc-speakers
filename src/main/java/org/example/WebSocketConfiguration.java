package org.example;

import com.fasterxml.jackson.core.type.TypeReference;
import com.fasterxml.jackson.databind.ObjectMapper;
import lombok.*;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.boot.web.server.WebServer;
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
import java.util.HashMap;
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
        private WebSocketSession nativeSession = null;

        private final ObjectMapper mapper = new ObjectMapper();

        @Override
        public void afterConnectionEstablished(WebSocketSession session) throws Exception {
            logger.info("New connection {} on port {}", session.getId(), session.getUri().getPort());
            sessions.put(session.getId(), session);
        }

        @Override
        protected void handleTextMessage(WebSocketSession session, TextMessage message) throws Exception {
            logger.info("Received message: {}\nFrom: {} on port {}", message.getPayload(), session.getId(), session.getUri().getPort());

            Map<String, String> data = mapper.readValue(message.getPayload(), new TypeReference<Map<String, String>>() {});

            if(!data.containsKey("type")){
                logger.error("Undefined type of message. Skip it");

                return;
            }


            if(data.get("type").equals("register_native")){
                nativeSession = session;

                return;
            }

            if(nativeSession == null){
                logger.error("Native session is undefined. Skip external message");

                return;
            }

            if(session == nativeSession){
                if(!data.containsKey("client_id")){
                    logger.error("Undefined `client_id`, so destination is undefined. Skip this message from native");

                    throw new IllegalArgumentException("Undefined `client_id`, so destination is undefined. Skip this message from native");
                }

                WebSocketSession sessionSendTo = sessions.get(data.get("client_id"));

                if(sessionSendTo != null){
                    sessionSendTo.sendMessage(new TextMessage(message.getPayload()));
                }
            }else{
                data.put("client_id", session.getId());

                nativeSession.sendMessage(new TextMessage(mapper.writeValueAsString(data)));
            }
        }

        @Override
        public void afterConnectionClosed(WebSocketSession session, CloseStatus status) throws Exception {
            logger.info("Disconnected {} on port {}", session.getId(), session.getUri().getPort());
            sessions.remove(session.getId());
        }
    }
}