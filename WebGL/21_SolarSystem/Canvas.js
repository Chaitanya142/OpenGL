var canvas = null;
var gl = null;//context=null;
var bFullScreen = false;
var canvas_original_width;
var canvas_original_height;

const WebGLMacros = {
    AMC_ATTRIBUTE_POSITION: 0,
    AMC_ATTRIBUTE_COLOR: 1,
    AMC_ATTRIBUTE_NORMAL: 2,
    AMC_ATTRIBUTE_TEXCOORD0: 3
}

var mvpUniform;
var perspectiveProjectionMatrix;

var gVertexShaderObject;
var gFragementShaderObject;
var gShaderProgramObject;

var Moon=null;
var Earth=null;
var Sun=null;

var year=0;
var day=0;
var moon=0;

var stack = new Stack(); 

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
        case 27://Escape
            UnInitialize();
            window.close();
            break;
        case 89:
            year=(year+3)%360;
            break;
        case 68:
            day=(day+3)%360;
            break;
        case 77:
            moon=(moon+3)%360;
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

    //Vertex Shader
    //Define Shader Object
    gVertexShaderObject = gl.createShader(gl.VERTEX_SHADER);

    //Vertex Shader Code
    var vertextShaderSourceCode =
        "#version 300 es " +
        "\n " +
        "in vec4 vPosition;" +
        "in vec4 vColor;" +
        "uniform mat4 u_mvp_matrix;" +
        "out vec4 out_color;" +
        "void main(void)" +
        "{" +
        "gl_Position=u_mvp_matrix * vPosition;" +
        "out_color=vColor;" +
        "} ";


    //Shader object and source code mapping
    gl.shaderSource(gVertexShaderObject, vertextShaderSourceCode);

    //Compile Shader
    gl.compileShader(gVertexShaderObject);

    //Error Checking
    if (gl.getShaderParameter(gVertexShaderObject, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(gVertexShaderObject);
        if (error.length > 0) {
            alert(error);
            UnInitialize();
        }
    }

    //Fragement Shader
    //Define Shader Object
    gFragementShaderObject = gl.createShader(gl.FRAGMENT_SHADER);

    //Fragement Shader Code
    var fragementShaderSourceCode =
        "#version 300 es " +
        "\n " +
        "precision highp float;" +
        "out vec4 fragColor;" +
        "in vec4 out_color;" +
        "void main(void)" +
        "{" +
        "fragColor=out_color;" +
        "} ";

    //Shader object and source code mapping
    gl.shaderSource(gFragementShaderObject, fragementShaderSourceCode);

    //Compile Shader
    gl.compileShader(gFragementShaderObject);

    //Error Checking
    if (gl.getShaderParameter(gFragementShaderObject, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(gFragementShaderObject);
        if (error.length > 0) {
            alert(error);
            UnInitialize();
        }
    }


    //Program Object
    gShaderProgramObject = gl.createProgram();
    //Attach Shaders
    gl.attachShader(gShaderProgramObject, gVertexShaderObject);
    gl.attachShader(gShaderProgramObject, gFragementShaderObject);

    //Prelinking
    //Binding to attribute
    gl.bindAttribLocation(gShaderProgramObject,
        WebGLMacros.AMC_ATTRIBUTE_POSITION,
        "vPosition");
    gl.bindAttribLocation(gShaderProgramObject,
        WebGLMacros.AMC_ATTRIBUTE_COLOR,
        "vColor");



    //Link Shader Program
    gl.linkProgram(gShaderProgramObject);

    if (!gl.getProgramParameter(gShaderProgramObject, gl.LINK_STATUS)) {
        var error = gl.getProgramInfoLog(gShaderProgramObject);
        if (error.length > 0) {
            alert(error);
            UnInitialize();
        }
    }

    mvpUniform = gl.getUniformLocation(gShaderProgramObject,
        "u_mvp_matrix");


    Sun=new Mesh();
    //Sun.setColor(1.0,1.0,1.0);
    makeSphere(Sun,0.75, 30, 30,1.0,1.0,0.0);

    Earth=new Mesh();
    //Earth.setColor(0.4,0.9,1.0);
    makeSphere(Earth,0.2, 20, 20,0.4,0.9,1.0);
   
    Moon=new Mesh();
    //Moon.setColor(0.5,0.5,0.5);
    makeSphere(Moon,0.1,15,15,0.5,0.5,0.5);

    gl.depthFunc(gl.LEQUAL);
    gl.enable(gl.DEPTH_TEST);
    
    //gl.clearDepth(1.0);
    gl.clearColor(0.0, 0.0, 0.0, 1.0);

    perspectiveProjectionMatrix = mat4.create();
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

    mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 100)


}

function draw() {

    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    gl.useProgram(gShaderProgramObject);

    //Declaration of Matrices
    var modelViewMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();

    mat4.identity(modelViewMatrix);
    mat4.identity(modelViewProjectionMatrix);

    //Initialize matrices
    //Transformation

    mat4.lookAt(modelViewMatrix,
        [0.0,    0.0,    5.0],
        [0.0,    0.0,    0.0],
        [0.0,    1.0,    0.0]);

    stack.push(Object.assign({}, modelViewMatrix));
    
    mat4.rotateX(modelViewMatrix, modelViewMatrix,degToRad(90));

    mat4.multiply(
        modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix
    );

    gl.uniformMatrix4fv(mvpUniform,
        false,
        modelViewProjectionMatrix);

    Sun.draw();

    mat4.identity(modelViewMatrix);
    mat4.identity(modelViewProjectionMatrix);

    modelViewMatrix=stack.pop();

    stack.push(Object.assign({}, modelViewMatrix));

    mat4.rotateY(modelViewMatrix, modelViewMatrix,degToRad(year));

    mat4.translate(modelViewMatrix, modelViewMatrix, [2.0, 0.0, 0.0]);

    mat4.rotateX(modelViewMatrix, modelViewMatrix,degToRad(90));

    mat4.rotateZ(modelViewMatrix, modelViewMatrix,degToRad(day));

    mat4.multiply(
        modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix
    );

    gl.uniformMatrix4fv(mvpUniform,
        false,
        modelViewProjectionMatrix);


    Earth.drawLines();

    stack.push(Object.assign({}, modelViewMatrix));

    mat4.rotateZ(modelViewMatrix, modelViewMatrix,degToRad(moon));

    mat4.translate(modelViewMatrix, modelViewMatrix, [0.5, 0.0, 0.0]);

    mat4.multiply(
        modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix
    );

    gl.uniformMatrix4fv(mvpUniform,
        false,
        modelViewProjectionMatrix);

    Moon.draw();

    stack.pop();
    stack.pop();

    gl.useProgram(null);

    requestAnimationFrame(draw, canvas);
}

function UnInitialize() {

    if (gShaderProgramObject) {
        if (gFragementShaderObject) {
            gl.detachShader(gShaderProgramObject, gFragementShaderObject);
            gl.deleteShader(gFragementShaderObject);
            gFragementShaderObject = null;
        }
        if (gVertexShaderObject) {
            gl.detachShader(gShaderProgramObject, gVertexShaderObject);
            gl.deleteShader(gVertexShaderObject);
            gVertexShaderObject = null;
        }
        gl.deleteProgram(gShaderProgramObject);
        gShaderProgramObject = null;
    }
}


function degToRad(degrees) {
    return (degrees * Math.PI / 180)
}