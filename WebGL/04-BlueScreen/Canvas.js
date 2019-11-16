var canvas = null;
var gl = null;//context=null;
var bFullScreen = false;
var canvas_original_width;
var canvas_original_height;

var requestAnimationFrame =
    window.requestAnimationFrame ||
    window.webkitRequestAnimationFrame ||
    window.mozRequestAnimationFrame ||
    window.oRequestAnimationFrame ||
    window.msRequestAnimationFrame;

var cancelAnimationFrame =
    window.cancelAnimationFrame ||
    window.webkitCancelRequestAnimationFrame || window.webkitCancelAnimationFrame ||
    window.mozCancelRequestAnimationFrame || window.mozCancelAnimationFrame ||
    window.oCancelRequestAnimationFrame || window.oCancelAnimationFrame ||
    window.msCancelRequestAnimationFrame || window.msCancelAnimationFrame;



function main() {
    canvas = document.getElementById("RTR");
    if (!canvas) {
        console.log("Obtaining Canvas Failed\n");
    }
    else {
        console.log("Obtaining Canvas Succeeded\n");
    }
    console.log("Canvas Width : " + canvas.width + " And Canvas Height : " + canvas.height);

    canvas_original_height = canvas.height;
    canvas_original_width = canvas.width;

    window.addEventListener("keydown", keyDown, false);
    window.addEventListener("click", mouseDown, false);
    window.addEventListener("resize", resize, false);

    init();

    resize();

    draw();

}

function drawText(text) {
    context.textAlign = "center";
    context.textBaseline = "middle";

    context.font = "48px sans-serif";

    context.fillStyle = "white";

    context.fillText(text, canvas.width / 2, canvas.height / 2);

}

function keyDown(event) {
    switch (event.keyCode) {
        case 70:
            toggleFullScreen();
            //drawText("Hello World!!!");
            break;
    }
}

function mouseDown() {
}


function toggleFullScreen() {
    var fullscreen_element =
        document.fullscreenElement ||
        document.webkitFullscreenElement ||
        document.mozFullScreenElement ||
        document.msFullscreenElement ||
        null

    if (fullscreen_element == null) {
        if (canvas.requestFullscreen)
            canvas.requestFullscreen();
        else if (canvas.mozRequestFullScreen)
            canvas.mozRequestFullScreen();
        else if (canvas.webkitRequestFullscreen)
            canvas.webkitRequestFullscreen();
        else if (canvas.msRequestFullScreen)
            canvas.msRequestFullScreen();

        bFullScreen = true;
    }
    else {
        if (document.exitFullscreen)
            document.exitFullscreen();
        else if (document.mozCancelFullScreen)
            document.mozCancelFullScreen();
        else if (document.webkitExitFullscreen)
            document.webkitExitFullscreen();
        else if (document.msExitFullscreen)
            document.msExitFullscreen();

        bFullScreen = false;
    }
}


function init() {
    gl = canvas.getContext("webgl2");
    if (gl == null) {
        console.log("Failed to get the rendering context for WebGL");
        return;
    }
    gl.viewportWidth = canvas.width;
    gl.viewportHeight = canvas.height;

    gl.clearColor(0.0, 0.0, 1.0, 1.0);
}


function resize() {
    if (bFullScreen == true) {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
    }
    else {
        canvas.width = canvas_original_width;
        canvas.height = canvas_original_height;
    }
    gl.viewport(0, 0, canvas.width, canvas.height);
}

function draw(){
    gl.clear(gl.COLOR_BUFFER_BIT);

    requestAnimationFrame(draw,canvas);
}

