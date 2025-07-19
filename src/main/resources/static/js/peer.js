import {SignalingChannel} from "/js/signaling_channel.js";

const localVideo = document.querySelector("#localVideo");
const remoteVideo = document.querySelector("#remoteVideo");

let localStream;
let peerConnection;

const signaling = new SignalingChannel("wss://192.168.0.45:8444/signaling");

signaling.onmessage = event => {
    if(!localStream){
        console.error("Local stream is not ready yet, ignore messages!");
        return;
    }

    const data = JSON.parse(event.data);

    console.log("Received message: Type - ", data.type);

    switch (data.type){
        case 'offer':
            handleOffer(data);
            break;
        case 'answer':
            handleAnswer(data);
            break;
        case 'candidate':
            handleCandidate(data);
            break;
    }
}

async function createPeerConnection(){
    console.log("Create peer connection!");

    peerConnection = new RTCPeerConnection();
    peerConnection.onicecandidate = (event) => {
        const message = {
            "type": "candidate",
            candidate: null
        };

        if(event.candidate){
            message.candidate = event.candidate.candidate;
            message.sdpMid = event.candidate.sdpMid;
            message.sdpMLineIndex = event.candidate.sdpMLineIndex;
        }

        signaling.send(message);
    }

    peerConnection.ontrack = event => remoteVideo.srcObject = event.streams[0];

    localStream.getTracks().forEach(track => peerConnection.addTrack(track, localStream));
}

async function handleOffer(offer){
    if(!peerConnection){
        console.error("Peer connection is not ready!");
        return;
    }

    console.log("Handle offer. Offer:", offer);

    await createPeerConnection();
    await peerConnection.setRemoteDescription(offer);

    const answer = await peerConnection.createAnswer();
    signaling.send({
        "type": "answer",
        sdp: answer.sdp
    });
    await peerConnection.setLocalDescription(answer);
}

async function handleAnswer(answer){
    if(!peerConnection){
        console.error("Peer connection is not ready!");
        return;
    }

    console.log("Handle answer. Answer: ", answer);

    await peerConnection.setRemoteDescription(answer);
}

async function handleCandidate(candidate){
    if(!peerConnection){
        console.error("Peer connection is not ready!");
        return;
    }

    console.log("Handle candidate. Candidate: ", candidate);

    if(!candidate.candidate){
        await peerConnection.addIceCandidate(null);
    }else{
        await peerConnection.addIceCandidate(candidate);
    }
}

async function call(){
    await createPeerConnection();

    console.log("Call");

    const offer = await peerConnection.createOffer();
    signaling.send({
        type: "offer",
        sdp: offer.sdp
    });
    await peerConnection.setLocalDescription(offer);
}

async function start(){
    console.log("Start");

    localStream = await navigator.mediaDevices.getUserMedia(constraints);
    localVideo.srcObject = localStream;

    call();
}


const constraints = window.constraints = {
    audio: true,
    video: true
}

document.addEventListener("DOMContentLoaded", () => {
    start();
})
