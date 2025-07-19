package org.example;

import org.apache.catalina.connector.Connector;
import org.apache.tomcat.util.net.SSLHostConfig;
import org.apache.tomcat.util.net.SSLHostConfigCertificate;
import org.springframework.boot.web.embedded.tomcat.TomcatServletWebServerFactory;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
public class TomcatConfig {

    @Bean
    public TomcatServletWebServerFactory servletContainer() {
        TomcatServletWebServerFactory tomcat = new TomcatServletWebServerFactory();
        tomcat.addAdditionalTomcatConnectors(createWebSocketConnector());
//        tomcat.addAdditionalTomcatConnectors(createTLSWebSocketConnector());
        return tomcat;
    }

    private Connector createWebSocketConnector() {
        Connector connector = new Connector("org.apache.coyote.http11.Http11NioProtocol");
        connector.setScheme("http"); // Or "https" if using SSL for WebSockets
        connector.setPort(8080); // Specify the desired port for WebSockets
        return connector;
    }

    private Connector createTLSWebSocketConnector() {
        Connector connector = new Connector("org.apache.coyote.http11.Http11NioProtocol");
        connector.setScheme("https");
        connector.setSecure(true);
        connector.setPort(8443);
        connector.setProperty("SSLEnabled", "true");

        SSLHostConfig sslHostConfig = new SSLHostConfig();
        SSLHostConfigCertificate certificate = new SSLHostConfigCertificate(sslHostConfig, SSLHostConfigCertificate.Type.RSA);
        certificate.setCertificateKeystoreType("PKCS12");
        certificate.setCertificateKeystoreFile("/home/bimba/Projects/webrtc-speakers/src/main/resources/keystore/.keystore");
        certificate.setCertificateKeyPassword("password");
        certificate.setCertificateKeyAlias("signaling");
        sslHostConfig.addCertificate(certificate);

        connector.addSslHostConfig(sslHostConfig);
        return connector;
    }
}
