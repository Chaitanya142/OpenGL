var canvas = null;
var gl = null;//context=null;
var bFullScreen = false;
var canvas_original_width;
var canvas_original_height;

const WebGLMicros = {
    AMC_ATTRIBUTE_POSITION: 0,
    AMC_ATTRIBUTE_COLOR: 1,
    AMC_ATTRIBUTE_NORMAL: 2,
    AMC_ATTRIBUTE_TEXCOORD0: 3
}

var vao;
var vbo_position;
var vbo_color;

var mvpUniform;
var perspectiveProjectionMatrix;

var gVertexShaderObject;
var gFragementShaderObject;
var gShaderProgramObject;

var angleCube = 0;

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
        WebGLMicros.AMC_ATTRIBUTE_POSITION,
        "vPosition");
    gl.bindAttribLocation(gShaderProgramObject,
        WebGLMicros.AMC_ATTRIBUTE_COLOR,
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

    //Vertices

    vao = gl.createVertexArray();
    gl.bindVertexArray(vao);

    vbo_position = gl.createBuffer();

    //Bind Generated Buffer
    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_position);
    //Fill Buffer
    gl.bufferData(gl.ARRAY_BUFFER,
        null,
        gl.DYNAMIC_DRAW);
    //Set Vertex Attrib Pointer
    gl.vertexAttribPointer(WebGLMicros.AMC_ATTRIBUTE_POSITION,
        3,
        gl.FLOAT,
        false,
        0,
        0);
    //Enable Vertex Attrib Array
    gl.enableVertexAttribArray(WebGLMicros.AMC_ATTRIBUTE_POSITION);
    //Unbind Buffer
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    // gl.vertexAttrib3f(WebGLMicros.AMC_ATTRIBUTE_COLOR,
    //     0.0, 0.0, 1.0);
    vbo_color = gl.createBuffer();

    //Bind Generated Buffer
    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_color);
    //Fill Buffer
    gl.bufferData(gl.ARRAY_BUFFER,
        null,
        gl.DYNAMIC_DRAW);
    //Set Vertex Attrib Pointer
    gl.vertexAttribPointer(WebGLMicros.AMC_ATTRIBUTE_COLOR,
        3,
        gl.FLOAT,
        false,
        0,
        0);
    //Enable Vertex Attrib Array
    gl.enableVertexAttribArray(WebGLMicros.AMC_ATTRIBUTE_COLOR);
    //Unbind Buffer
    gl.bindBuffer(gl.ARRAY_BUFFER, null);


    //Unbind array
    gl.bindVertexArray(null);

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

    var lineVertices = new Float32Array([
        0.0, 0.0,
        0.0, 0.0,
        0.0, 0.0
    ]);

    var lineColor = new Float32Array([
        0.0, 0.0, 1.0,
        0.0, 0.0, 1.0
    ]);


    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    gl.useProgram(gShaderProgramObject);

    //Declaration of Matrices
    var modelViewMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();

    mat4.identity(modelViewMatrix);
    mat4.identity(modelViewProjectionMatrix);

    //Initialize matrices
    //Transformation

    mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -3.0]);
    // mat4.rotateX(modelViewMatrix, modelViewMatrix,degToRad(angleCube));
    // mat4.rotateY(modelViewMatrix, modelViewMatrix,degToRad(angleCube));
    // mat4.rotateZ(modelViewMatrix, modelViewMatrix,degToRad(angleCube));
    // mat4.scale(modelViewMatrix, modelViewMatrix, [0.75, 0.75, 0.75]);

    //mat4.multiply(modelViewMatrix,modelViewMatrix,1.0)
    //Matrix Multiplication

    mat4.multiply(
        modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix
    );

    //Send necessary matrices to shader in resp. Uniforms

    gl.uniformMatrix4fv(mvpUniform,
        false,
        modelViewProjectionMatrix);

    //Bind with vao_pyramid

    gl.bindVertexArray(vao);

    //Bind with textures if any


    for (var count = -1; count <= 1.01; count = count + 0.05) {

        lineVertices[0] = count;
        lineVertices[1] = -1.0;
        lineVertices[2] = 0.0;
        lineVertices[3] = count;
        lineVertices[4] = 1.0;
        lineVertices[5] = 0.0;

        gl.bindBuffer(gl.ARRAY_BUFFER,
            vbo_position);
        //Fill Buffer
        gl.bufferData(gl.ARRAY_BUFFER,
            lineVertices,
            gl.DYNAMIC_DRAW);
        gl.bindBuffer(gl.ARRAY_BUFFER,
            null);

        gl.bindBuffer(gl.ARRAY_BUFFER,
            vbo_color);
        //Fill Buffer
        gl.bufferData(gl.ARRAY_BUFFER,
            lineColor,
            gl.DYNAMIC_DRAW);
        gl.bindBuffer(gl.ARRAY_BUFFER,
            null);

        //Draw

        gl.drawArrays(gl.LINES,
            0,
            2);

        lineVertices[0] = -1.0;
        lineVertices[1] = count;
        lineVertices[2] = 0.0;
        lineVertices[3] = 1.0;
        lineVertices[4] = count;
        lineVertices[5] = 0.0;

        gl.bindBuffer(gl.ARRAY_BUFFER,
            vbo_position);
        //Fill Buffer
        gl.bufferData(gl.ARRAY_BUFFER,
            lineVertices,
            gl.DYNAMIC_DRAW);
        gl.bindBuffer(gl.ARRAY_BUFFER,
            null);

        gl.bindBuffer(gl.ARRAY_BUFFER,
            vbo_color);
        //Fill Buffer
        gl.bufferData(gl.ARRAY_BUFFER,
            lineColor,
            gl.DYNAMIC_DRAW);
        gl.bindBuffer(gl.ARRAY_BUFFER,
            null);

        //Draw

        gl.drawArrays(gl.LINES,
            0,
            2);
    }

    gl.bindVertexArray(null);
    
    gl.bindVertexArray(vao);
    
    lineVertices[0] = -1.0;
    lineVertices[1] = 0.0;
    lineVertices[2] = 0.0;
    lineVertices[3] = 1.0;
    lineVertices[4] = 0.0;
    lineVertices[5] = 0.0;

    lineColor[0] = 1.0;
    lineColor[1] = 0.0;
    lineColor[2] = 0.0;
    lineColor[3] = 1.0;
    lineColor[4] = 0.0;
    lineColor[5] = 0.0;

    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_position);
    //Fill Buffer
    gl.bufferData(gl.ARRAY_BUFFER,
        lineVertices,
        gl.DYNAMIC_DRAW);
    gl.bindBuffer(gl.ARRAY_BUFFER,
        null);

    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_color);
    //Fill Buffer
    gl.bufferData(gl.ARRAY_BUFFER,
        lineColor,
        gl.DYNAMIC_DRAW);
    gl.bindBuffer(gl.ARRAY_BUFFER,
        null);

    //Draw

    gl.drawArrays(gl.LINES,
        0,
        2);

    lineVertices[0] = 0.0;
    lineVertices[1] = -1.0;
    lineVertices[2] = 0.0;
    lineVertices[3] = 0.0;
    lineVertices[4] = 1.0;
    lineVertices[5] = 0.0;

    lineColor[0] = 0.0;
    lineColor[1] = 1.0;
    lineColor[2] = 0.0;
    lineColor[3] = 0.0;
    lineColor[4] = 1.0;
    lineColor[5] = 0.0;

    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_position);
    //Fill Buffer
    gl.bufferData(gl.ARRAY_BUFFER,
        lineVertices,
        gl.DYNAMIC_DRAW);
    gl.bindBuffer(gl.ARRAY_BUFFER,
        null);

    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_color);
    //Fill Buffer
    gl.bufferData(gl.ARRAY_BUFFER,
        lineColor,
        gl.DYNAMIC_DRAW);
    gl.bindBuffer(gl.ARRAY_BUFFER,
        null);

    //Draw

    gl.drawArrays(gl.LINES,
        0,
        2);

    gl.bindVertexArray(null);

    gl.useProgram(null);

    requestAnimationFrame(draw, canvas);
}

function UnInitialize() {

    if (vao) {
        gl.deleteVertexArrays(vao);
        vao = null;
    }
    if (vbo_position) {
        gl.deleteBuffers(vbo_position);
        vbo_position = null;
    }
    if (vbo_color) {
        gl.deleteBuffers(vbo_color);
        vbo_color = null;
    }

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