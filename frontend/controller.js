const Http = new XMLHttpRequest();
const url = "http://192.168.50.106";
const throttle_time = 0.1; //1,10,25,50,200

let cur_front_back = "";
let cur_left_right = "";

let is_same_command = (cur_cmd, past_cmd) => {
    // return false;
    // console.log(`${cur_cmd} vs ${past_cmd}`)
    return cur_cmd == past_cmd;
}

let form_url = (url_list) => {
    return url_list.join('/');
}

let get_request = (url) => {
    Http.open("GET", url);
    Http.send();
}


let forwardHandler = (level) => {
    let cur_cmd = "forward"+level;
    if (is_same_command(cur_cmd, cur_front_back)) return;
    cur_front_back = cur_cmd;
    let this_url = form_url([url, "forward",level]);//url + "forward";
    get_request(this_url)
}

let backwardHandler = (level) => {
    let cur_cmd = "backward"+level;
    if (is_same_command(cur_cmd, cur_front_back)) return;
    cur_front_back = cur_cmd;
    let this_url = form_url([url, "backward",level]);//url + "forward";
    get_request(this_url)
}

let stopHandler = ()=>{
    let cur_cmd = "stop";
    if (is_same_command(cur_cmd, cur_front_back)) return;
    cur_front_back = cur_cmd;
    let this_url = form_url([url, "stop"]);
    get_request(this_url)
}

let leftHandler = (level) => {
    let cur_cmd = "left"+level;
    if (is_same_command(cur_cmd, cur_left_right)) return;
    cur_left_right = cur_cmd;
    let this_url = form_url([url, "left",level]);
    get_request(this_url)
}

let rightHandler = (level) => {
    let cur_cmd = "right"+level;
    if (is_same_command(cur_cmd, cur_left_right)) return;
    cur_left_right = cur_cmd;
    let this_url = form_url([url, "right",level]);
    get_request(this_url)
}

let straightHandler = () => {
    let cur_cmd = "straight";
    if (is_same_command(cur_cmd, cur_left_right)) return;
    cur_left_right = cur_cmd;
    let this_url = form_url([url, "straight"]);
    get_request(this_url)
}

t_straight = _.throttle(straightHandler, throttle_time);
t_right = _.throttle(rightHandler, throttle_time);
t_left = _.throttle(leftHandler, throttle_time);
t_stop = _.throttle(stopHandler, throttle_time);
t_forward = _.throttle(forwardHandler, throttle_time);
t_backward = _.throttle(backwardHandler, throttle_time);

//https://developer.mozilla.org/en-US/docs/Web/API/XMLHttpRequest/readyState
Http.onload = (e) => {   //onreadystatechange
    console.log(Http.response);
}

const level_bin = (amt, levels) => {

}
let cur_time = 0;
const timer_count = (el) => {
    return () => {
        cur_time += 1;
        el.innerHTML= cur_time;
    }
}


window.onload = (()=>{
    //alert("LOADED");
    let time_div = document.getElementById("timer");
    setInterval(timer_count(time_div),1000) 
    var Joy1 = new JoyStick('left-right', {}, function(stickData) {
        // console.log(stickData)
        let x_pos = parseFloat(stickData.x);
        let isLeft = false;
        if (x_pos < 0){
            isLeft = true;
            x_pos = -x_pos;
        }
        let level = Math.ceil(x_pos / 20);
        if (level == 0){
            t_straight();
        } else if (isLeft){
            t_left(level);
            //leftHandler();
        } else {
            t_right(level);
        }
        // joy1IinputPosX.value = stickData.xPosition;
        // joy1InputPosY.value = stickData.yPosition;
        // joy1Direzione.value = stickData.cardinalDirection;
        // joy1X.value = stickData.x;
        // joy1Y.value = stickData.y;
    });
    var Joy2= new JoyStick('front-back', {}, function(stickData) {
        // console.log(stickData)
        let y_pos = parseFloat(stickData.y)//.yPosition - 100
        let isBack = false;
        if (y_pos < 0){
            isBack = true;
            y_pos = -y_pos;
        }
        let level = Math.ceil(y_pos / 20);
        if (level > 0){
            if (isBack){
                t_backward(level);
            } else {
                t_forward(level);;
            }
        } else {
            t_stop();
        }
    });
})