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

var vao_pyramid;
var vbo_position_pyramid;
var vbo_color_pyramid;

var vao_cube;
var vbo_position_cube;
var vbo_color_cube;

var mvpUniform;
var perspectiveProjectionMatrix;

var gVertexShaderObject;
var gFragementShaderObject;
var gShaderProgramObject;

var anglePyramid = 0;
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
    var pyramidVertices = new Float32Array([
        0.0, 1.0, 0.0,
		-1.0, -1.0, 1.0,
		1.0, -1.0, 1.0,
		0.0, 1.0, 0.0,
		1.0, -1.0, 1.0,
		1.0, -1.0, -1.0,
		0.0, 1.0, 0.0,
		1.0, -1.0, -1.0,
		-1.0, -1.0, -1.0,
		0.0, 1.0, 0.0,
		-1.0, -1.0, -1.0,
		-1.0, -1.0, 1.0
    ]);

    var pyramidColors = new Float32Array([
        1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		1.0, 0.0, 0.0,
		0.0, 0.0, 1.0,
		0.0, 1.0, 0.0,
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		1.0, 0.0, 0.0,
		0.0, 0.0, 1.0,
		0.0, 1.0, 0.0
    ]);

    var cubeVertices = new Float32Array([
        1.0, 1.0, -1.0,
		-1.0, 1.0, -1.0,
		-1.0, 1.0, 1.0,
		1.0, 1.0, 1.0,
		1.0, -1.0, -1.0,
		-1.0, -1.0, -1.0,
		-1.0, -1.0, 1.0,
		1.0, -1.0, 1.0,
		1.0, 1.0, 1.0,
		-1.0, 1.0, 1.0,
		-1.0, -1.0, 1.0,
		1.0, -1.0, 1.0,
		1.0, 1.0, -1.0,
		-1.0, 1.0, -1.0,
		-1.0, -1.0, -1.0,
		1.0, -1.0, -1.0,
		1.0, 1.0, -1.0,
		1.0, 1.0, 1.0,
		1.0, -1.0, 1.0,
		1.0, -1.0, -1.0,
		-1.0, 1.0, -1.0,
		-1.0, 1.0, 1.0,
		-1.0, -1.0, 1.0,
		-1.0, -1.0, -1.0
    ]);

    var cubeColors = new Float32Array ([
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		1.0, 0.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 1.0, 0.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 0.0, 1.0,
		0.0, 1.0, 1.0,
		0.0, 1.0, 1.0,
		0.0, 1.0, 1.0,
		0.0, 1.0, 1.0,
		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,
		1.0, 1.0, 0.0,
		1.0, 0.0, 1.0,
		1.0, 0.0, 1.0,
		1.0, 0.0, 1.0,
		1.0, 0.0, 1.0
    ]);

    vao_pyramid = gl.createVertexArray();
    gl.bindVertexArray(vao_pyramid);

    vbo_position_pyramid = gl.createBuffer();

    //Bind Generated Buffer
    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_position_pyramid);
    //Fill Buffer
    gl.bufferData(gl.ARRAY_BUFFER,
        pyramidVertices,
        gl.STATIC_DRAW);
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


    vbo_color_pyramid = gl.createBuffer();

    //Bind Generated Buffer
    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_color_pyramid);
    //Fill Buffer
    gl.bufferData(gl.ARRAY_BUFFER,
        pyramidColors,
        gl.STATIC_DRAW);
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




    vao_cube = gl.createVertexArray();
    gl.bindVertexArray(vao_cube);

    vbo_position_cube = gl.createBuffer();

    //Bind Generated Buffer
    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_position_cube);
    //Fill Buffer
    gl.bufferData(gl.ARRAY_BUFFER,
        cubeVertices,
        gl.STATIC_DRAW);
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
    vbo_color_cube = gl.createBuffer();

    //Bind Generated Buffer
    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_color_cube);
    //Fill Buffer
    gl.bufferData(gl.ARRAY_BUFFER,
        cubeColors,
        gl.STATIC_DRAW);
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

    //Initialize matrices
    //Transformation

    mat4.translate(modelViewMatrix, modelViewMatrix, [-1.5, 0.0, -5.0]);

    mat4.rotateY(modelViewMatrix, modelViewMatrix,degToRad(anglePyramid));

    //Matrix Multiplication

    mat4.multiply(
        modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix
    );

    //Send necessary matrices to shader in resp. Uniforms

    gl.uniformMatrix4fv(mvpUniform,
        false,
        modelViewProjectionMatrix);

    //Bind with vao_pyramid

    gl.bindVertexArray(vao_pyramid);

    //Bind with textures if any

    //Draw

    gl.drawArrays(gl.TRIANGLE_FAN,
        0,
        12);

    //Unbind vao_pyramid

    gl.bindVertexArray(null);


    mat4.identity(modelViewMatrix);
    mat4.identity(modelViewProjectionMatrix);

    //Initialize matrices
    //Transformation

    mat4.translate(modelViewMatrix, modelViewMatrix, [1.5, 0.0, -5.0]);
    mat4.rotateX(modelViewMatrix, modelViewMatrix,degToRad(angleCube));
    mat4.rotateY(modelViewMatrix, modelViewMatrix,degToRad(angleCube));
    mat4.rotateZ(modelViewMatrix, modelViewMatrix,degToRad(angleCube));
    mat4.scale(modelViewMatrix, modelViewMatrix, [0.75, 0.75, 0.75]);

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

    gl.bindVertexArray(vao_cube);

    //Bind with textures if any

    //Draw

        gl.drawArrays(gl.TRIANGLE_FAN,
            0,
            4);
        gl.drawArrays(gl.TRIANGLE_FAN,
            4,
            4);
        gl.drawArrays(gl.TRIANGLE_FAN,
            8,
            4);
        gl.drawArrays(gl.TRIANGLE_FAN,
            12,
            4);
        gl.drawArrays(gl.TRIANGLE_FAN,
            16,
            4);
        gl.drawArrays(gl.TRIANGLE_FAN,
            20,
            4);

    //Unbind vao_pyramid

    gl.bindVertexArray(null);

    gl.useProgram(null);

    anglePyramid = anglePyramid + 1.0;
    if (anglePyramid > 360.0)
        anglePyramid = 0.0;
    angleCube = angleCube - 1.0;
    if (angleCube < -360.0)
        angleCube = 0.0;

    requestAnimationFrame(draw, canvas);
}

function UnInitialize() {
    if (vao_pyramid) {
        gl.deleteVertexArrays(vao_pyramid);
        vao_pyramid = null;
    }
    if (vbo_position_pyramid) {
        gl.deleteBuffers(vbo_position_pyramid);
        vbo_position_pyramid = null;
    }
    if (vbo_color_pyramid) {
        gl.deleteBuffers(vbo_color_pyramid);
        vbo_color_pyramid = null;
    }

    if (vao_cube) {
        gl.deleteVertexArrays(vao_cube);
        vao_cube = null;
    }
    if (vbo_position_cube) {
        gl.deleteBuffers(vbo_position_cube);
        vbo_position_cube = null;
    }
    if (vbo_color_cube) {
        gl.deleteBuffers(vbo_color_cube);
        vbo_color_cube = null;
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