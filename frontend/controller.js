const Http = new XMLHttpRequest();
const url = "http://192.168.43.153";//"http://192.168.43.153"; //192.168.43.86, 192.168.50.106    http://192.168.43.153
const throttle_time = 0.15; //1,10,25,50,200
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


let isAuto = false;
let isAutoEl;
let leftBtnEl;
let rightBtnEl;
let frontBtnEl;
let backBtnEl;



let is_same_command = (cur_cmd, past_cmd) => {
    // return false;
    // console.log(`${cur_cmd} vs ${past_cmd}`)
    return cur_cmd.dir == past_cmd.dir && cur_cmd.level == past_cmd.level;
}

let form_url = (url_list) => {
    return url_list.join('/');
}

let get_request = (url) => {
    latestUrl = url;
    console.log(`url: ${url}`)
    Http.open("GET", url);
    Http.send();
}


let forwardHandler = (level) => {
    let cur_cmd = {
        dir:F,
        level: level
    }//"forward"+level;
    // if (is_same_command(cur_cmd, cur_front_back)) return;
    cur_front_back = cur_cmd;
    let this_url = form_url([url, forward_path,level]);//url + "forward";
    get_request(this_url)
}

let backwardHandler = (level) => {
    let cur_cmd = {
        dir:B,
        level: level
    }//"forward"+level;
    // if (is_same_command(cur_cmd, cur_front_back)) return;
    cur_front_back = cur_cmd;
    let this_url = form_url([url, backward_path,level]);//url + "forward";
    get_request(this_url)
}

let stopHandler = ()=>{
    // let cur_cmd = "stop";
    let cur_cmd = {
        dir:STOP,
        level: 0
    }
    // if (is_same_command(cur_cmd, cur_front_back)) return;
    cur_front_back = cur_cmd;
    let this_url = form_url([url, stop_path]);
    get_request(this_url)
}

let leftHandler = (level) => {
    // let cur_cmd = "left"+level;
    let cur_cmd = {
        dir:L,
        level: level
    }
    // if (is_same_command(cur_cmd, cur_left_right)) return;
    cur_left_right = cur_cmd;
    let this_url = form_url([url, left_path,level]);
    get_request(this_url)
}

let rightHandler = (level) => {
    // let cur_cmd = "right"+level;
    let cur_cmd = {
        dir:R,
        level: level
    }
    // if (is_same_command(cur_cmd, cur_left_right)) return;
    cur_left_right = cur_cmd;
    let this_url = form_url([url, right_path,level]);
    get_request(this_url)
}

let straightHandler = () => {
    // let cur_cmd = "straight";
    let cur_cmd = {
        dir:STRAIGHT,
        level: 0
    }
    // if (is_same_command(cur_cmd, cur_left_right)) return;
    cur_left_right = cur_cmd;
    let this_url = form_url([url, straight_path]);
    get_request(this_url)
}


t_straight = _.throttle(straightHandler, throttle_time);
t_right = _.throttle(rightHandler, throttle_time);
t_left = _.throttle(leftHandler, throttle_time);
t_stop = _.throttle(stopHandler, throttle_time);
t_forward = _.throttle(forwardHandler, throttle_time);
t_backward = _.throttle(backwardHandler, throttle_time);

// setInterval(()=>{
//     console.log(`cur_front_back.dir: ${cur_front_back.dir}, cur_left_right.dir: ${cur_left_right.dir}`)
//     if(cur_front_back.dir == STOP && cur_left_right.dir == STRAIGHT){
//         console.log("SEND STOP")
//         stopHandler();
//         straightHandler();
//     }
// }, 500)


let setToggleStyle = () => {
    let auto_class = "auto";
    if(isAuto){
        isAutoEl.classList.add(auto_class)
    } else {
        isAutoEl.classList.remove(auto_class)
    }
}

let toggleAutoHandler = () => {
    let this_url = "";
    console.log(`isAuto: ${isAuto}`)
    if (isAuto){
        this_url = form_url([url, notAuto_path]);
    } else {
        this_url = form_url([url, isAuto_path]);
    }
    get_request(this_url);
    isAuto = !isAuto;
    setToggleStyle();
}


//https://developer.mozilla.org/en-US/docs/Web/API/XMLHttpRequest/readyState
Http.onload = (e) => {   //onreadystatechange
    // console.log(Http.response);
}

Http.onerror = (e) => {
    console.log("ERROR. now timeout");
    // console.log(e);
    setTimeout(()=>{
        console.log("WAIT");
        get_request(latestUrl);
    }, 120) // maybe have a wait state that disables buttons
    
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
    /*
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
    });*/
    // var Joy2= new JoyStick('front-back', {}, function(stickData) {
    //     // console.log(stickData)
    //     let y_pos = parseFloat(stickData.y)//.yPosition - 100
    //     let isBack = false;
    //     if (y_pos < 0){
    //         isBack = true;
    //         y_pos = -y_pos;
    //     }
    //     let level = Math.ceil(y_pos / 20);
    //     if (level > 0){
    //         if (isBack){
    //             // t_backward(level);

    //             backwardHandler(level);

    //         } else {
    //             // t_forward(level);;
    //             forwardHandler(level);
    //         }
    //     } else {
    //         // t_stop();
    //         stopHandler();
    //     }
    // });



    isAutoEl = document.getElementById('isAuto');
    isAutoEl.onclick = ((e)=>{
        toggleAutoHandler();
    })


    frontBtnEl = document.getElementById('frontBtn');
    backBtnEl = document.getElementById('backBtn');

    leftBtnEl = document.getElementById('leftBtn');
    rightBtnEl = document.getElementById('rightBtn');

    frontBtnEl.onmousedown = ((e)=>{
        forwardHandler(5);
    })
    backBtnEl.onmousedown = ((e)=>{
        backwardHandler(5);
    })
    leftBtnEl.onmousedown = ((e)=>{
        leftHandler(5);
    })
    rightBtnEl.onmousedown = ((e)=>{
        // console.log(e);
        rightHandler(5);
    })


    frontBtnEl.ontouchstart = ((e)=>{
        forwardHandler(5);
    })
    backBtnEl.ontouchstart = ((e)=>{
        backwardHandler(5);
    })
    leftBtnEl.ontouchstart = ((e)=>{
        // console.log("LEFT BTN KEYDOWN");
        // console.log(e);
        leftHandler(5);
    })
    rightBtnEl.ontouchstart = ((e)=>{
        // console.log(e);
        rightHandler(5);
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

})