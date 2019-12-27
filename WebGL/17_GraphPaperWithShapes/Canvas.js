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

var vao_lines;
var vbo_position_lines;
var vbo_color_lines;

var vao_points;
var vbo_position_points;
var vbo_color_points;

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
        "gl_PointSize=1.0f;"+
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

    vao_lines = gl.createVertexArray();
    gl.bindVertexArray(vao_lines);

    vbo_position_lines = gl.createBuffer();

    //Bind Generated Buffer
    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_position_lines);
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
    vbo_color_lines = gl.createBuffer();

    //Bind Generated Buffer
    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_color_lines);
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



    vao_points = gl.createVertexArray();
    gl.bindVertexArray(vao_points);

    vbo_position_points = gl.createBuffer();

    //Bind Generated Buffer
    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_position_points);
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
    vbo_color_points = gl.createBuffer();

    //Bind Generated Buffer
    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_color_points);
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

    // gl.vertexAttrib3f(WebGLMicros.AMC_ATTRIBUTE_COLOR,
    //     1.0, 1.0, 0.0);


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

var lineVertices = new Float32Array([
    0.0, 0.0,
    0.0, 0.0,
    0.0, 0.0
]);

var lineColor = new Float32Array([
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0
]);


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

    drawGraph(0.05);

    drawTriangle(0.5);

    drawInCircle(0.0, 0.5, 0.0,
        -0.5, -0.5, 0.0,
        0.5, -0.5, 0.0);

    drawRectangle(1.0, 1.0);

	drawCircleAroundRectangle(1.0, 1.0);

    gl.useProgram(null);

    requestAnimationFrame(draw, canvas);
}

function UnInitialize() {

    if (vao_lines) {
        gl.deleteVertexArrays(vao_lines);
        vao_lines = null;
    }
    if (vbo_position_lines) {
        gl.deleteBuffers(vbo_position_lines);
        vbo_position_lines = null;
    }
    if (vbo_color_lines) {
        gl.deleteBuffers(vbo_color_lines);
        vbo_color_lines = null;
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


function drawGraph(offset) {

    lineColor[0] = 0.0;
    lineColor[1] = 0.0;
    lineColor[2] = 1.0;
    lineColor[3] = 0.0;
    lineColor[4] = 0.0;
    lineColor[5] = 1.0;

    gl.bindVertexArray(vao_lines);

    //Bind with textures if any

    for (var count = -1; count <= 1.01; count = count + offset) {

        lineVertices[0] = count;
        lineVertices[1] = -1.0;
        lineVertices[2] = 0.0;
        lineVertices[3] = count;
        lineVertices[4] = 1.0;
        lineVertices[5] = 0.0;

        gl.bindBuffer(gl.ARRAY_BUFFER,
            vbo_position_lines);
        //Fill Buffer
        gl.bufferData(gl.ARRAY_BUFFER,
            lineVertices,
            gl.DYNAMIC_DRAW);
        gl.bindBuffer(gl.ARRAY_BUFFER,
            null);

        gl.bindBuffer(gl.ARRAY_BUFFER,
            vbo_color_lines);
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
            vbo_position_lines);
        //Fill Buffer
        gl.bufferData(gl.ARRAY_BUFFER,
            lineVertices,
            gl.DYNAMIC_DRAW);
        gl.bindBuffer(gl.ARRAY_BUFFER,
            null);

        gl.bindBuffer(gl.ARRAY_BUFFER,
            vbo_color_lines);
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

}


function drawTriangle(offset) {

    gl.bindVertexArray(vao_lines);


    lineVertices[0] = 0.0;
    lineVertices[1] = offset;
    lineVertices[2] = 0.0;
    lineVertices[3] = -offset;
    lineVertices[4] = -offset;
    lineVertices[5] = 0.0;

    lineColor[0] = 1.0;
    lineColor[1] = 1.0;
    lineColor[2] = 0.0;
    lineColor[3] = 1.0;
    lineColor[4] = 1.0;
    lineColor[5] = 0.0;

    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_position_lines);
    //Fill Buffer
    gl.bufferData(gl.ARRAY_BUFFER,
        lineVertices,
        gl.DYNAMIC_DRAW);
    gl.bindBuffer(gl.ARRAY_BUFFER,
        null);

    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_color_lines);
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

    lineVertices[0] = -offset;
    lineVertices[1] = -offset;
    lineVertices[2] = 0.0;
    lineVertices[3] = offset;
    lineVertices[4] = -offset;
    lineVertices[5] = 0.0;
    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_position_lines);
    //Fill Buffer
    gl.bufferData(gl.ARRAY_BUFFER,
        lineVertices,
        gl.DYNAMIC_DRAW);
    gl.bindBuffer(gl.ARRAY_BUFFER,
        null);

    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_color_lines);
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



    lineVertices[0] = offset;
    lineVertices[1] = -offset;
    lineVertices[2] = 0.0;
    lineVertices[3] = 0.0;
    lineVertices[4] = offset;
    lineVertices[5] = 0.0;
    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_position_lines);
    //Fill Buffer
    gl.bufferData(gl.ARRAY_BUFFER,
        lineVertices,
        gl.DYNAMIC_DRAW);
    gl.bindBuffer(gl.ARRAY_BUFFER,
        null);

    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_color_lines);
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


}

var ArrInCirclePos = new Float32Array(((2 * 3.1415) / 0.01) * 3);
var ArrInCircleColor = new Float32Array(((2 * 3.1415) / 0.01) * 3);
var isArrInCircleInit = 0;


function drawInCircle(x1, y1, z1,
    x2, y2, z2,
    x3, y3, z3
) {
    gl.bindVertexArray(vao_points);

    if (isArrInCircleInit == 0) {

        var angle = 0;

        var distP1P2 = Math.sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z2 - z1) * (z2 - z1));
        var distP2P3 = Math.sqrt((x3 - x2) * (x3 - x2) + (y3 - y2) * (y3 - y2) + (z3 - z2) * (z3 - z2));
        var distP3P1 = Math.sqrt((x1 - x3) * (x1 - x3) + (y1 - y3) * (y1 - y3) + (z1 - z3) * (z1 - z3));

        var semiPerimeter = (distP1P2 + distP2P3 + distP3P1) / 2;

        var radius = Math.sqrt((semiPerimeter - distP1P2) * (semiPerimeter - distP2P3) * (semiPerimeter - distP3P1) / semiPerimeter);

        var Ox = (x3 * distP1P2 + x1 * distP2P3 + x2 * distP3P1) / (semiPerimeter * 2);
        var Oy = (y3 * distP1P2 + y1 * distP2P3 + y2 * distP3P1) / (semiPerimeter * 2);
        var Oz = (z3 * distP1P2 + z1 * distP2P3 + z2 * distP3P1) / (semiPerimeter * 2);

        var indexPos = 0;
        var indexColor = 0;

        for (var angle = 0.0; angle < 2 * 3.14159265; angle = angle + 0.01) {
            ArrInCirclePos[indexPos] = Math.cos(angle) * radius + Ox;
            indexPos = indexPos + 1;
            ArrInCirclePos[indexPos] = Math.sin(angle) * radius + Oy;
            indexPos = indexPos + 1;
            ArrInCirclePos[indexPos] = 0.0 + Oz;
            indexPos = indexPos + 1;

            ArrInCircleColor[indexColor] = 1.0;
            indexColor = indexColor + 1;
            ArrInCircleColor[indexColor] = 1.0;
            indexColor = indexColor + 1;
            ArrInCircleColor[indexColor] = 0.0;
            indexColor = indexColor + 1;

        }
        isArrInCircleInit = 1;

        console.log(ArrInCirclePos);
        console.log(ArrInCircleColor);

    }

    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_position_points);
    //Fill Buffer
    gl.bufferData(gl.ARRAY_BUFFER,
        ArrInCirclePos,
        gl.DYNAMIC_DRAW);
    gl.bindBuffer(gl.ARRAY_BUFFER,
        null);

    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_color_points);
    //Fill Buffer
    gl.bufferData(gl.ARRAY_BUFFER,
        ArrInCircleColor,
        gl.DYNAMIC_DRAW);
    gl.bindBuffer(gl.ARRAY_BUFFER,
        null);

    //Draw

    gl.drawArrays(gl.POINTS,
        0,
        ((2 * 3.1415) / 0.01));

    gl.bindVertexArray(null);

}


function drawRectangle(width, height) {

    gl.bindVertexArray(vao_lines);

    lineVertices[0] = width / 2;
    lineVertices[1] = height / 2;
    lineVertices[2] = 0.0;
    lineVertices[3] = -width / 2;
    lineVertices[4] = height / 2;
    lineVertices[5] = 0.0;

    lineColor[0] = 1.0;
    lineColor[1] = 1.0;
    lineColor[2] = 0.0;
    lineColor[3] = 1.0;
    lineColor[4] = 1.0;
    lineColor[5] = 0.0;

    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_position_lines);
    //Fill Buffer
    gl.bufferData(gl.ARRAY_BUFFER,
        lineVertices,
        gl.DYNAMIC_DRAW);
    gl.bindBuffer(gl.ARRAY_BUFFER,
        null);

    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_color_lines);
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

    lineVertices[0] = -width / 2;
    lineVertices[1] = height / 2;
    lineVertices[2] = 0.0;
    lineVertices[3] = -width / 2;
    lineVertices[4] = -height / 2;
    lineVertices[5] = 0.0;

    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_position_lines);
    //Fill Buffer
    gl.bufferData(gl.ARRAY_BUFFER,
        lineVertices,
        gl.DYNAMIC_DRAW);
    gl.bindBuffer(gl.ARRAY_BUFFER,
        null);

    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_color_lines);
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

    lineVertices[0] = -width / 2;
    lineVertices[1] = -height / 2;
    lineVertices[2] = 0.0;
    lineVertices[3] = width / 2;
    lineVertices[4] = -height / 2;
    lineVertices[5] = 0.0;


    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_position_lines);
    //Fill Buffer
    gl.bufferData(gl.ARRAY_BUFFER,
        lineVertices,
        gl.DYNAMIC_DRAW);
    gl.bindBuffer(gl.ARRAY_BUFFER,
        null);

    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_color_lines);
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

    lineVertices[0] = width / 2;
    lineVertices[1] = -height / 2;
    lineVertices[2] = 0.0;
    lineVertices[3] = width / 2;
    lineVertices[4] = height / 2;
    lineVertices[5] = 0.0;

    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_position_lines);
    //Fill Buffer
    gl.bufferData(gl.ARRAY_BUFFER,
        lineVertices,
        gl.DYNAMIC_DRAW);
    gl.bindBuffer(gl.ARRAY_BUFFER,
        null);

    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_color_lines);
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


}


function drawCircleAroundRectangle(width,height) {

	var radius = Math.sqrt((width / 2) * (width / 2 )+ (height / 2) * (height / 2));
	drawCircle(radius);

}


var ArrCirclePos = new Float32Array(((2 * 3.1415) / 0.01) * 3);
var ArrCircleColor = new Float32Array(((2 * 3.1415) / 0.01) * 3);

var isArrCircleInit=0;
function drawCircle(radius) {
    gl.bindVertexArray(vao_points);

    if (isArrCircleInit == 0) {

        var Ox = 0;
        var Oy = 0;
        var Oz = 0;

        var indexPos = 0;
        var indexColor = 0;

        for (var angle = 0.0; angle < 2 * 3.14159265; angle = angle + 0.01) {
            ArrCirclePos[indexPos] = Math.cos(angle) * radius + Ox;
            indexPos = indexPos + 1;
            ArrCirclePos[indexPos] = Math.sin(angle) * radius + Oy;
            indexPos = indexPos + 1;
            ArrCirclePos[indexPos] = 0.0 + Oz;
            indexPos = indexPos + 1;

            ArrCircleColor[indexColor] = 1.0;
            indexColor = indexColor + 1;
            ArrCircleColor[indexColor] = 1.0;
            indexColor = indexColor + 1;
            ArrCircleColor[indexColor] = 0.0;
            indexColor = indexColor + 1;

        }
        isArrCircleInit = 1;

        console.log(ArrCirclePos);
        console.log(ArrCircleColor);

    }

    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_position_points);
    //Fill Buffer
    gl.bufferData(gl.ARRAY_BUFFER,
        ArrCirclePos,
        gl.DYNAMIC_DRAW);
    gl.bindBuffer(gl.ARRAY_BUFFER,
        null);

    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_color_points);
    //Fill Buffer
    gl.bufferData(gl.ARRAY_BUFFER,
        ArrCircleColor,
        gl.DYNAMIC_DRAW);
    gl.bindBuffer(gl.ARRAY_BUFFER,
        null);

    //Draw

    gl.drawArrays(gl.POINTS,
        0,
        ((2 * 3.1415) / 0.01));

    gl.bindVertexArray(null);

}
