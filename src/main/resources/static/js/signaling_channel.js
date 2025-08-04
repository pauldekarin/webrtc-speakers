export class SignalingChannel {
    constructor(data){
        this.uri = data.uri;

        this.ws = new WebSocket(this.uri);

        this.ws.onopen = () => {
            if(data.onopen){
                data.onopen();
            }
        };
        this.ws.onmessage = event => {
            if(data.onmessage){
                data.onmessage(event);
            }
        };
    }

    send(message){
        this.ws.send(JSON.stringify(message));
    }
}

