export class SignalingChannel {
    constructor(uri){
        this.uri = uri;
        this.ws = new WebSocket(uri);
        this.ws.onopen = () => {
            if(this.onopen){
                this.onopen();
            }
        };
        this.ws.onmessage = event => {
            if(this.onmessage){
                this.onmessage(event);
            }
        };
    }

    send(message){
        this.ws.send(JSON.stringify(message));
    }

    set onmessage(callback){
        this._onmessage = callback;
    }

    get onmessage(){
        return this._onmessage;
    }

    set onopen(callback){
        this._onopen = callback;
    }

    get onopen(){
        return this._onopen;
    }
}

