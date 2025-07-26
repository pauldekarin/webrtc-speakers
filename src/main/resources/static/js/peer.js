import {SignalingChannel} from "/js/signaling_channel.js";

const localVideo = document.querySelector("#localVideo");
const remoteVideo = document.querySelector("#remoteVideo");

let localStream;
let peerConnection;
let audioContext;
let localAudioSource;
let remoteAudioSource;

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

    peerConnection.ontrack = event => {
        console.log("Received remote track:", event.track.kind);

        // Set remote video stream
        remoteVideo.srcObject = event.streams[0];

        // Handle audio tracks specifically
        if (event.track.kind === 'audio') {
            // Create a MediaStreamAudioSourceNode from the remote stream
            const remoteStream = event.streams[0];
            remoteAudioSource = audioContext.createMediaStreamSource(remoteStream);

            // Create an AnalyserNode to process remote audio data
            const analyser = audioContext.createAnalyser();
            analyser.fftSize = 2048;

            // Connect the audio source to the analyser
            remoteAudioSource.connect(analyser);
            // Connect to the audio context destination to hear the audio
            analyser.connect(audioContext.destination);

            // Process remote audio data
            const processRemoteAudio = () => {
                const bufferLength = analyser.frequencyBinCount;
                const dataArray = new Float32Array(bufferLength);
                analyser.getFloatFrequencyData(dataArray);

                // Example: Log remote audio levels (modify as needed)
                const average = dataArray.reduce((sum, value) => sum + value, 0) / bufferLength;
                console.log("Remote audio level:", average);

                // Continue processing audio in a loop
                requestAnimationFrame(processRemoteAudio);
            };

            // Start processing remote audio
            processRemoteAudio();
        }
    }

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

    // Initialize Web Audio API context
    audioContext = new (window.AudioContext || window.webkitAudioContext)();

    // Create a MediaStreamAudioSourceNode for local stream
    localAudioSource = audioContext.createMediaStreamSource(localStream);

    // Create an AnalyserNode to process local audio data
    const analyser = audioContext.createAnalyser();
    analyser.fftSize = 2048;

    // Connect the local audio source to the analyser
    localAudioSource.connect(analyser);
    // Connect to the audio context destination to hear the audio locally
    analyser.connect(audioContext.destination);

    // Process local audio data
    const processLocalAudio = () => {
        const bufferLength = analyser.frequencyBinCount;
        const dataArray = new Float32Array(bufferLength);
        analyser.getFloatFrequencyData(dataArray);

        // Example: Log local audio levels (modify as needed)
        const average = dataArray.reduce((sum, value) => sum + value, 0) / bufferLength;

        // Continue processing audio in a loop
        requestAnimationFrame(processLocalAudio);
    };

    // Start processing local audio
    processLocalAudio();

    call();
}

const constraints = window.constraints = {
    audio: true,
    video: true
}

document.addEventListener("DOMContentLoaded", () => {
    start();
});