var canvas=null;
var context=null;

function main(){
    canvas=document.getElementById("RTR");
    if(!canvas){
        console.log("Obtaining Canvas Failed\n");
    }
    else{
        console.log("Obtaining Canvas Succeeded\n");
    }
    console.log("Canvas Width : "+canvas.width+ " And Canvas Height : "+canvas.height);

    context=canvas.getContext("2d");
    if(!context){
        console.log("Obtaining 2D Context Failed\n");
    }
    else{
        console.log("Obtaining 2D Context Succeeded\n");
    }

    context.fillStyle="black";
    context.fillRect(0,0,canvas.width,canvas.height);

    drawText("Hello World !!!");

    window.addEventListener("keydown",keyDown,false);
    window.addEventListener("click",mouseDown,false);    

}

function drawText(text)
{
    context.textAlign="center";
    context.textBaseline="middle";

    context.font="48px sans-serif";

    context.fillStyle="white";

    context.fillText(text,canvas.width/2,canvas.height/2);

}

function keyDown(event){
switch(event.keyCode){
    case 70:
        toggleFullScreen();
        //drawText("Hello World!!!");
        break;
}
}

function mouseDown(){
}


function toggleFullScreen()
{
var fullscreen_element=
document.fullscreenElement||
document.webkitFullscreenElement||
document.mozFullScreenElement||
document.msFullscreenElement||
null

if(fullscreen_element==null){
    if(canvas.requestFullscreen)
        canvas.requestFullscreen();
    else if(canvas.mozRequestFullScreen)
        canvas.mozRequestFullScreen();
    else if(canvas.webkitRequestFullscreen)
        canvas.webkitRequestFullscreen();
    else if(canvas.msRequestFullScreen)
        canvas.msRequestFullScreen();
}
else{
    if(document.exitFullscreen)
        document.exitFullscreen();
    else if(document.mozCancelFullScreen)
        document.mozCancelFullScreen();
    else if(document.webkitExitFullscreen)
        document.webkitExitFullscreen();
    else if(document.msExitFullscreen)
        document.msExitFullscreen();
}

}