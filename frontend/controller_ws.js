//192.168.50.141
const url = "192.168.43.195:9300"//"192.168.50.250:9300";//"http://192.168.43.153"; //192.168.43.86, 192.168.50.106    http://192.168.43.153
var gateway = `ws://${url}/ws`;

const isMobile = window.matchMedia("only screen and (max-width: 1080px)").matches;
console.log(`isMobile: ${isMobile}`)

const throttle_time = 0.3; //1,10,25,50,200
let lastClientRequestTime = 0;
// Flag that sets when data 

// Two methods
// Create a queue for the commands HERE. The queue will only have stuff iff client commands arrive before clitnet request 
// throttle time. Or, when connection refused, attempt to reconnect and resend that command.

// Encoding: Either i stick to my current one by one, or i reserve 4 bits leftright, 4 bits frontback, and after a client  request 
// timeout period, i send whatever is in that variable. Timer starts when a client command issued. Any addtional commands during this
// period will save to the variable but not send direct. When timeout completes, send the command.  


const cmdQ = [];

const F = 1;
const B = 2;
const STOP = 3;
const L = 4;
const R = 5;
const STRAIGHT = 6;

let cur_front_back = {
    dir:STOP,
    level: 0
};
let cur_left_right = {
    dir:STRAIGHT,
    level: 0
};

let latestUrl = "";

const left_path = "l";
const right_path = "r";
const straight_path = "s2";
const stop_path = "s1";
const forward_path = "f";
const backward_path = "b";
const isAuto_path = 'isAuto';
const notAuto_path = 'notAuto';
const isStart_path = 'startTrial';
const isEnd_path = 'endTrial';

let isAutoEl;
let leftBtnEl;
let rightBtnEl;
let frontBtnEl;
let backBtnEl;
let toggleTrialBtnEl;
let time_div;

let timer_interval = null;
// let endBtnEl;



let isAuto = false;
let isStartTrial = false;
let isEndTrial = false;

let active_class = "activeState";

function initWebSocket() {
    console.log("Trying to open a WebSocket connection...");
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage; // <-- add this line
}
function onOpen(event) {
    console.log("Connection opened");
}
function onClose(event) {
    console.log("Connection closed");
    setTimeout(initWebSocket, 2000);
}
function onMessage(event) {
    console.log("MESSAGE RECEIVED");
    console.log(event);
    var state;
    if (event.data == "1") {
        state = "ON";
    } else {
        state = "OFF";
    }
}

let is_same_command = (cur_cmd, past_cmd) => {
    return cur_cmd.dir == past_cmd.dir && cur_cmd.level == past_cmd.level;
}

let form_url = (url_list) => {
    return url_list.join('/');
}

let get_request = (url) => {
    websocket.send(url);
}


let forwardHandler = (level) => {
    let cur_cmd = {
        dir:F,
        level: level
    }
    if (is_same_command(cur_cmd, cur_front_back)) return;
    cur_front_back = cur_cmd;
    
    let this_url = form_url([forward_path,level]);//url + "forward";
    get_request(this_url)
}

let backwardHandler = (level) => {
    let cur_cmd = {
        dir:B,
        level: level
    }
    if (is_same_command(cur_cmd, cur_front_back)) return;
    cur_front_back = cur_cmd;
    let this_url = form_url([backward_path,level]);//url + "forward";
    get_request(this_url)
}

let stopHandler = ()=>{
    let cur_cmd = {
        dir:STOP,
        level: 0
    }
    if (is_same_command(cur_cmd, cur_front_back)) return;
    cur_front_back = cur_cmd;
    let this_url = form_url([stop_path]);
    get_request(this_url)
}

let leftHandler = (level) => {
    let cur_cmd = {
        dir:L,
        level: level
    }
    if (is_same_command(cur_cmd, cur_left_right)) return;
    cur_left_right = cur_cmd;
    let this_url = form_url([left_path,level]);
    get_request(this_url)
}

let rightHandler = (level) => {
    let cur_cmd = {
        dir:R,
        level: level
    }
    if (is_same_command(cur_cmd, cur_left_right)) return;
    cur_left_right = cur_cmd;
    let this_url = form_url([right_path,level]);
    get_request(this_url)
}

let straightHandler = () => {
    let cur_cmd = {
        dir:STRAIGHT,
        level: 0
    }
    if (is_same_command(cur_cmd, cur_left_right)) return;
    cur_left_right = cur_cmd;
    let this_url = form_url([straight_path]);
    get_request(this_url)
}


t_straight = _.throttle(straightHandler, throttle_time);
t_right = _.throttle(rightHandler, throttle_time);
t_left = _.throttle(leftHandler, throttle_time);
t_stop = _.throttle(stopHandler, throttle_time);
t_forward = _.throttle(forwardHandler, throttle_time);
t_backward = _.throttle(backwardHandler, throttle_time);


let setToggleStyle = (btnEl, state) => {
    console.log(btnEl)
    console.log(`state: ${state}`)
    if(state){
        btnEl.classList.add(active_class)
    } else {
        btnEl.classList.remove(active_class)
    }
}

let toggleAutoHandler = () => {
    let this_url = "";
    //console.log(`isAuto: ${isAuto}`)
    if (isAuto){
        this_url = form_url([notAuto_path]);
        stop_timer();
    } else {
        this_url = form_url([isAuto_path]);
        start_timer();
    }
    get_request(this_url);
    isAuto = !isAuto;
    setToggleStyle(isAutoEl,isAuto);
}

let toggleTrialHandler = () => {
    let this_url = "";
    //console.log(`isStartTrial: ${isStartTrial}`)
    //get_request(this_url);
    if (isStartTrial){
        this_url = form_url([isEnd_path]);
        toggleTrialBtnEl.innerHTML = "START";
        stop_timer();
    } else {
        this_url = form_url([isStart_path]);
        toggleTrialBtnEl.innerHTML = "END";
        start_timer();
    }
    get_request(this_url);
    isStartTrial = !isStartTrial;
    setToggleStyle(toggleTrialBtnEl, isStartTrial);
}

// let toggleEndHandler = () => {
//     let this_url = "";
//     this_url = form_url([isEnd_path])
//     /*if (isEndTrial){
//         this_url = form_url([notAuto_path]);
//     } else {
//         this_url = form_url([isAuto_path]);
//     }*/
//     //get_request(this_url);
//     //isEndTrial = !isEndTrial;
//     setToggleStyle(endBtnEl);
// }

const level_bin = (amt, levels) => {

}
let cur_time = 0;
const timer_count = (el) => {
    return () => {
        cur_time += 1;
        el.innerHTML= cur_time;
    }
}

const start_timer = () =>{
    reset_timer();
    timer_interval = setInterval(timer_count(time_div),1000) 
}

const stop_timer = () => {
    clearInterval(timer_interval);
}

const reset_timer = () => {
    cur_time = 0;
    time_div.innerHTML = cur_time;
}

let button_setup = () => {
    isAutoEl = document.getElementById('isAuto');
    isAutoEl.onclick = ((e)=>{
        toggleAutoHandler();
    })

    toggleTrialBtnEl = document.getElementById('toggleTrial');
    toggleTrialBtnEl.onclick = ((e)=>{
        toggleTrialHandler();
    })

    // endBtnEl = document.getElementById('endTrial');
    // isAutoEl.onclick = ((e)=>{
    //     toggleEndHandler();
    // })


    frontBtnEl = document.getElementById('frontBtn');
    backBtnEl = document.getElementById('backBtn');

    leftBtnEl = document.getElementById('leftBtn');
    rightBtnEl = document.getElementById('rightBtn');

    if(!isMobile){
        frontBtnEl.onmousedown = ((e)=>{
            forwardHandler(5);
            // t_forward(5);
        })
        backBtnEl.onmousedown = ((e)=>{
            backwardHandler(5);
            // t_backward(5);
        })
        leftBtnEl.onmousedown = ((e)=>{
            leftHandler(3); //5
            // t_left(5);
        })
        rightBtnEl.onmousedown = ((e)=>{
            // console.log(e);
            rightHandler(3); //5
            // t_right(5);
        })
        
        frontBtnEl.onmouseup = ((e)=>{
            console.log("LIFT FORWARD")
            stopHandler();
        })
        backBtnEl.onmouseup = ((e)=>{
            stopHandler();
        })
        leftBtnEl.onmouseup = ((e)=>{
            console.log("LIFT LEFT")
            straightHandler();
        })
        rightBtnEl.onmouseup = ((e)=>{
            straightHandler();
        })
    } else {
        frontBtnEl.ontouchstart = ((e)=>{
            forwardHandler(5);
            // t_forward(5);
        })
        backBtnEl.ontouchstart = ((e)=>{
            backwardHandler(5);
            // t_backward(5);
        })
        leftBtnEl.ontouchstart = ((e)=>{
            leftHandler(5);
            // t_left(5);
        })
        rightBtnEl.ontouchstart = ((e)=>{
            rightHandler(5);
            // t_right(5);
        })
    
    
    
        frontBtnEl.ontouchend = ((e)=>{
            stopHandler();
        })
        backBtnEl.ontouchend = ((e)=>{
            stopHandler();
        })  
        leftBtnEl.ontouchend = ((e)=>{
            straightHandler();
        })
        rightBtnEl.ontouchend = ((e)=>{
            straightHandler();
        })    
    }



    

}

window.onload = (()=>{
    //alert("LOADED");

    initWebSocket();

    time_div = document.getElementById("timer");

    button_setup();
    
})