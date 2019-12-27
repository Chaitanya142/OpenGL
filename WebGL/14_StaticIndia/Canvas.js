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

var vao_triangle;
var vbo_position_triangle;
var vbo_color_triangle;

var vao_quad;
var vbo_position_quad;
var vbo_color_quad;

var vao_point;
var vbo_position_point;
var vbo_color_point;

var mvpUniform;
var perspectiveProjectionMatrix;

var gVertexShaderObject;
var gFragementShaderObject;
var gShaderProgramObject;


var D_R_OrangeColor = 1.0;
var D_G_OrangeColor = 0.6;
var D_B_OrangeColor = 0.2;
var D_R_GreenColor = 0.074;
var D_G_GreenColor = 0.533;
var D_B_GreenColor = 0.2;

var Line_Orange_R = 1.0;
var Line_Orange_G = 0.6;
var Line_Orange_B = 0.2;
var Line_White_R = 1.00;
var Line_White_G = 1.00;
var Line_White_B = 1.00;
var Line_Green_R = 0.074;
var Line_Green_G = 0.533;
var Line_Green_B = 0.2;

var stage = 5;
// var I_1_XPosition = -2.25;
// var A_XPosition = 2.25;
// var N_YPosition = 2.25;
// var I_2_YPosition = -2.25;
// var Plane1_Y_Position = 0.00;
// var Plane1_Z_Rotation_Angle = -90.0;

var I_1_XPosition = -1.5; //-2.25;
var A_XPosition = 0.75;//2.25;
var N_YPosition = 0.00;//2.25;
var I_2_YPosition = 0.00;//-2.25;


// var Plane1_X_Position = -2.5;
// var angle = 0.0;
// var Line_X_Position = -1.28;

//Sphere planeFrontSphere;

// var angleTriangle = 0;
// var anglequad = 0;

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

    vao_triangle = gl.createVertexArray();
    gl.bindVertexArray(vao_triangle);

    vbo_position_triangle = gl.createBuffer();

    //Bind Generated Buffer
    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_position_triangle);
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


    vbo_color_triangle = gl.createBuffer();

    //Bind Generated Buffer
    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_color_triangle);
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


    vao_quad = gl.createVertexArray();
    gl.bindVertexArray(vao_quad);

    vbo_position_quad = gl.createBuffer();

    //Bind Generated Buffer
    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_position_quad);
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

    vbo_color_quad = gl.createBuffer();

    //Bind Generated Buffer
    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_color_quad);
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

    gl.enable(gl.DEPTH_TEST);
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

    if (stage > 0) {
        //Initialize matrices

        mat4.identity(modelViewMatrix);
        mat4.identity(modelViewProjectionMatrix);

        //Transformation

        mat4.translate(modelViewMatrix, modelViewMatrix, [I_1_XPosition, 0.0, -3.0]);
        //mat4.rotateX(modelViewMatrix, modelViewMatrix,degToRad(anglequad));     

        mat4.multiply(
            modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix
        );

        //Send necessary matrices to shader in resp. Uniforms

        gl.uniformMatrix4fv(mvpUniform,
            false,
            modelViewProjectionMatrix);

        //Bind with vao_quad

        //Bind with textures if any

        DrawLetter('I');

        //Unbind vao_quad

    }

    if (stage > 2) {
        //Initialize matrices

        mat4.identity(modelViewMatrix);
        mat4.identity(modelViewProjectionMatrix);

        //Transformation

        mat4.translate(modelViewMatrix, modelViewMatrix, [-1.0, N_YPosition, -3.0]);
        //mat4.rotateX(modelViewMatrix, modelViewMatrix,degToRad(anglequad));     

        mat4.multiply(
            modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix
        );

        //Send necessary matrices to shader in resp. Uniforms

        gl.uniformMatrix4fv(mvpUniform,
            false,
            modelViewProjectionMatrix);

        //Bind with vao_quad

        //Bind with textures if any

        DrawLetter('N');

        //Unbind vao_quad

    }

    if (stage > 4) {
        //Initialize matrices

        mat4.identity(modelViewMatrix);
        mat4.identity(modelViewProjectionMatrix);

        //Transformation

        mat4.translate(modelViewMatrix, modelViewMatrix, [-0.25, 0.0, -3.0]);
        //mat4.rotateX(modelViewMatrix, modelViewMatrix,degToRad(anglequad));     

        mat4.multiply(
            modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix
        );

        //Send necessary matrices to shader in resp. Uniforms

        gl.uniformMatrix4fv(mvpUniform,
            false,
            modelViewProjectionMatrix);

        //Bind with vao_quad

        //Bind with textures if any

        DrawLetter('D');

        //Unbind vao_quad

    }

    if (stage > 3) {

        //Initialize matrices

        mat4.identity(modelViewMatrix);
        mat4.identity(modelViewProjectionMatrix);

        //Transformation
        mat4.translate(modelViewMatrix, modelViewMatrix, [0.25, I_2_YPosition, -3.0]);
        //mat4.rotateX(modelViewMatrix, modelViewMatrix,degToRad(anglequad));     

        mat4.multiply(
            modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix
        );

        //Send necessary matrices to shader in resp. Uniforms

        gl.uniformMatrix4fv(mvpUniform,
            false,
            modelViewProjectionMatrix);


        //Bind with vao_quad

        //Bind with textures if any

        DrawLetter('I');

        //Unbind vao_quad

    }
    if (stage > 1) {

        //Initialize matrices

        mat4.identity(modelViewMatrix);
        mat4.identity(modelViewProjectionMatrix);

        //Transformation
        mat4.translate(modelViewMatrix, modelViewMatrix, [A_XPosition, 0.0, -3.0]);
        //mat4.rotateX(modelViewMatrix, modelViewMatrix,degToRad(anglequad));     

        mat4.multiply(
            modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix
        );

        //Send necessary matrices to shader in resp. Uniforms

        gl.uniformMatrix4fv(mvpUniform,
            false,
            modelViewProjectionMatrix);

        //Bind with vao_quad

        //Bind with textures if any

        DrawLetter('A');
        DrawLetter('T');

        //Unbind vao_quad

    }

    gl.useProgram(null);

    requestAnimationFrame(draw, canvas);

    //Update();

}

var rectangleVertices = new Float32Array([
    0.275, 1.0, 0.0,
    0.225, 1.0, 0.0,
    0.225, -1.0, 0.0,
    0.275, -1.0, 0.0
]);

var rectangleColorCoords = new Float32Array([
    1.0, 0.6, 0.2,
    1.0, 0.6, 0.2,
    0.074, 0.533, 0.2,
    0.074, 0.533, 0.2
]);


function DrawLetter(letter) {
    switch (letter) {
        case 'I':
            rectangleVertices[0] = 0.275;
            rectangleVertices[1] = 1.0;
            rectangleVertices[2] = 0.0;
            rectangleVertices[3] = 0.225;
            rectangleVertices[4] = 1.0;
            rectangleVertices[5] = 0.0;
            rectangleVertices[6] = 0.225;
            rectangleVertices[7] = -1.0;
            rectangleVertices[8] = 0.0;
            rectangleVertices[9] = 0.275;
            rectangleVertices[10] = -1.0;
            rectangleVertices[11] = 0.0;

            rectangleColorCoords[0] = 1.0;
            rectangleColorCoords[1] = 0.6;
            rectangleColorCoords[2] = 0.2;
            rectangleColorCoords[3] = 1.0;
            rectangleColorCoords[4] = 0.6;
            rectangleColorCoords[5] = 0.2;
            rectangleColorCoords[6] = 0.074;
            rectangleColorCoords[7] = 0.533;
            rectangleColorCoords[8] = 0.2;
            rectangleColorCoords[9] = 0.074;
            rectangleColorCoords[10] = 0.533;
            rectangleColorCoords[11] = 0.2;

            gl.bindVertexArray(vao_quad);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_position_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleVertices,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_color_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleColorCoords,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.drawArrays(gl.TRIANGLE_FAN,
                0,
                4);

            gl.bindVertexArray(null);

            break;

        case 'N':

            //Left side of N
            rectangleVertices[0] = 0.05;
            rectangleVertices[1] = 1.0;
            rectangleVertices[2] = 0.0;
            rectangleVertices[3] = 0.0;
            rectangleVertices[4] = 1.0;
            rectangleVertices[5] = 0.0;
            rectangleVertices[6] = 0.00;
            rectangleVertices[7] = -1.0;
            rectangleVertices[8] = 0.0;
            rectangleVertices[9] = 0.05;
            rectangleVertices[10] = -1.0;
            rectangleVertices[11] = 0.0;

            rectangleColorCoords[0] = 1.0;
            rectangleColorCoords[1] = 0.6;
            rectangleColorCoords[2] = 0.2;
            rectangleColorCoords[3] = 1.0;
            rectangleColorCoords[4] = 0.6;
            rectangleColorCoords[5] = 0.2;
            rectangleColorCoords[6] = 0.074;
            rectangleColorCoords[7] = 0.533;
            rectangleColorCoords[8] = 0.2;
            rectangleColorCoords[9] = 0.074;
            rectangleColorCoords[10] = 0.533;
            rectangleColorCoords[11] = 0.2;


            gl.bindVertexArray(vao_quad);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_position_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleVertices,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_color_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleColorCoords,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.drawArrays(gl.TRIANGLE_FAN,
                0,
                4);

            gl.bindVertexArray(null);


            //Middle part of N
            rectangleVertices[0] = 0.05;
            rectangleVertices[1] = 1.0;
            rectangleVertices[2] = 0.0;
            rectangleVertices[3] = 0.00;
            rectangleVertices[4] = 1.0;
            rectangleVertices[5] = 0.0;
            rectangleVertices[6] = 0.45;
            rectangleVertices[7] = -1.0;
            rectangleVertices[8] = 0.0;
            rectangleVertices[9] = 0.50;
            rectangleVertices[10] = -1.0;
            rectangleVertices[11] = 0.0;

            rectangleColorCoords[0] = 1.0;
            rectangleColorCoords[1] = 0.6;
            rectangleColorCoords[2] = 0.2;
            rectangleColorCoords[3] = 1.0;
            rectangleColorCoords[4] = 0.6;
            rectangleColorCoords[5] = 0.2;
            rectangleColorCoords[6] = 0.074;
            rectangleColorCoords[7] = 0.533;
            rectangleColorCoords[8] = 0.2;
            rectangleColorCoords[9] = 0.074;
            rectangleColorCoords[10] = 0.533;
            rectangleColorCoords[11] = 0.2;

            gl.bindVertexArray(vao_quad);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_position_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleVertices,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_color_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleColorCoords,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.drawArrays(gl.TRIANGLE_FAN,
                0,
                4);

            gl.bindVertexArray(null);


            //Right Side of N
            rectangleVertices[0] = 0.50;
            rectangleVertices[1] = 1.0;
            rectangleVertices[2] = 0.0;
            rectangleVertices[3] = 0.45;
            rectangleVertices[4] = 1.0;
            rectangleVertices[5] = 0.0;
            rectangleVertices[6] = 0.45;
            rectangleVertices[7] = -1.0;
            rectangleVertices[8] = 0.0;
            rectangleVertices[9] = 0.50;
            rectangleVertices[10] = -1.0;
            rectangleVertices[11] = 0.0;

            rectangleColorCoords[0] = 1.0;
            rectangleColorCoords[1] = 0.6;
            rectangleColorCoords[2] = 0.2;
            rectangleColorCoords[3] = 1.0;
            rectangleColorCoords[4] = 0.6;
            rectangleColorCoords[5] = 0.2;
            rectangleColorCoords[6] = 0.074;
            rectangleColorCoords[7] = 0.533;
            rectangleColorCoords[8] = 0.2;
            rectangleColorCoords[9] = 0.074;
            rectangleColorCoords[10] = 0.533;
            rectangleColorCoords[11] = 0.2;


            gl.bindVertexArray(vao_quad);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_position_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleVertices,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_color_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleColorCoords,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.drawArrays(gl.TRIANGLE_FAN,
                0,
                4);

            gl.bindVertexArray(null);



            break;

        case 'D':

            rectangleVertices[0] = 0.05;
            rectangleVertices[1] = 1.0;
            rectangleVertices[2] = 0.0;
            rectangleVertices[3] = 0.0;
            rectangleVertices[4] = 1.0;
            rectangleVertices[5] = 0.0;
            rectangleVertices[6] = 0.00;
            rectangleVertices[7] = -1.0;
            rectangleVertices[8] = 0.0;
            rectangleVertices[9] = 0.05;
            rectangleVertices[10] = -1.0;
            rectangleVertices[11] = 0.0;

            rectangleColorCoords[0] = D_R_OrangeColor;
            rectangleColorCoords[1] = D_G_OrangeColor;
            rectangleColorCoords[2] = D_B_OrangeColor;
            rectangleColorCoords[3] = D_R_OrangeColor;
            rectangleColorCoords[4] = D_G_OrangeColor;
            rectangleColorCoords[5] = D_B_OrangeColor;
            rectangleColorCoords[6] = D_R_GreenColor;
            rectangleColorCoords[7] = D_G_GreenColor;
            rectangleColorCoords[8] = D_B_GreenColor;
            rectangleColorCoords[9] = D_R_GreenColor;
            rectangleColorCoords[10] = D_G_GreenColor;
            rectangleColorCoords[11] = D_B_GreenColor;



            gl.bindVertexArray(vao_quad);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_position_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleVertices,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_color_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleColorCoords,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.drawArrays(gl.TRIANGLE_FAN,
                0,
                4);

            gl.bindVertexArray(null);



            rectangleVertices[0] = 0.5;
            rectangleVertices[1] = 1.0;
            rectangleVertices[2] = 0.0;
            rectangleVertices[3] = -0.01;
            rectangleVertices[4] = 1.0;
            rectangleVertices[5] = 0.0;
            rectangleVertices[6] = -0.01;
            rectangleVertices[7] = 0.95;
            rectangleVertices[8] = 0.0;
            rectangleVertices[9] = 0.5;
            rectangleVertices[10] = 0.95;
            rectangleVertices[11] = 0.0;

            rectangleColorCoords[0] = D_R_OrangeColor;
            rectangleColorCoords[1] = D_G_OrangeColor;
            rectangleColorCoords[2] = D_B_OrangeColor;
            rectangleColorCoords[3] = D_R_OrangeColor;
            rectangleColorCoords[4] = D_G_OrangeColor;
            rectangleColorCoords[5] = D_B_OrangeColor;
            rectangleColorCoords[6] = D_R_OrangeColor;
            rectangleColorCoords[7] = D_G_OrangeColor;
            rectangleColorCoords[8] = D_B_OrangeColor;
            rectangleColorCoords[9] = D_R_OrangeColor;
            rectangleColorCoords[10] = D_G_OrangeColor;
            rectangleColorCoords[11] = D_B_OrangeColor;


            gl.bindVertexArray(vao_quad);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_position_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleVertices,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_color_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleColorCoords,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.drawArrays(gl.TRIANGLE_FAN,
                0,
                4);

            gl.bindVertexArray(null);


            rectangleVertices[0] = 0.50;
            rectangleVertices[1] = 1.0;
            rectangleVertices[2] = 0.0;
            rectangleVertices[3] = 0.45;
            rectangleVertices[4] = 1.0;
            rectangleVertices[5] = 0.0;
            rectangleVertices[6] = 0.45;
            rectangleVertices[7] = -1.0;
            rectangleVertices[8] = 0.0;
            rectangleVertices[9] = 0.50;
            rectangleVertices[10] = -1.0;
            rectangleVertices[11] = 0.0;

            rectangleColorCoords[0] = D_R_OrangeColor;
            rectangleColorCoords[1] = D_G_OrangeColor;
            rectangleColorCoords[2] = D_B_OrangeColor;
            rectangleColorCoords[3] = D_R_OrangeColor;
            rectangleColorCoords[4] = D_G_OrangeColor;
            rectangleColorCoords[5] = D_B_OrangeColor;
            rectangleColorCoords[6] = D_R_GreenColor;
            rectangleColorCoords[7] = D_G_GreenColor;
            rectangleColorCoords[8] = D_B_GreenColor;
            rectangleColorCoords[9] = D_R_GreenColor;
            rectangleColorCoords[10] = D_G_GreenColor;
            rectangleColorCoords[11] = D_B_GreenColor;


            gl.bindVertexArray(vao_quad);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_position_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleVertices,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_color_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleColorCoords,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.drawArrays(gl.TRIANGLE_FAN,
                0,
                4);

            gl.bindVertexArray(null);


            rectangleVertices[0] = 0.50;
            rectangleVertices[1] = -0.95;
            rectangleVertices[2] = 0.0;
            rectangleVertices[3] = -0.01;
            rectangleVertices[4] = -0.95;
            rectangleVertices[5] = 0.0;
            rectangleVertices[6] = -0.01;
            rectangleVertices[7] = -1.0;
            rectangleVertices[8] = 0.0;
            rectangleVertices[9] = 0.50;
            rectangleVertices[10] = -1.0;
            rectangleVertices[11] = 0.0;

            rectangleColorCoords[0] = D_R_GreenColor;
            rectangleColorCoords[1] = D_G_GreenColor;
            rectangleColorCoords[2] = D_B_GreenColor;
            rectangleColorCoords[3] = D_R_GreenColor;
            rectangleColorCoords[4] = D_G_GreenColor;
            rectangleColorCoords[5] = D_B_GreenColor;
            rectangleColorCoords[6] = D_R_GreenColor;
            rectangleColorCoords[7] = D_G_GreenColor;
            rectangleColorCoords[8] = D_B_GreenColor;
            rectangleColorCoords[9] = D_R_GreenColor;
            rectangleColorCoords[10] = D_G_GreenColor;
            rectangleColorCoords[11] = D_B_GreenColor;

            gl.bindVertexArray(vao_quad);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_position_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleVertices,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_color_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleColorCoords,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.drawArrays(gl.TRIANGLE_FAN,
                0,
                4);

            gl.bindVertexArray(null);

            break;
        case 'A':

            rectangleVertices[0] = 0.275;
            rectangleVertices[1] = 1.0;
            rectangleVertices[2] = 0.0;
            rectangleVertices[3] = 0.225;
            rectangleVertices[4] = 1.0;
            rectangleVertices[5] = 0.0;
            rectangleVertices[6] = 0.00;
            rectangleVertices[7] = -1.0;
            rectangleVertices[8] = 0.0;
            rectangleVertices[9] = 0.05;
            rectangleVertices[10] = -1.0;
            rectangleVertices[11] = 0.0;

            rectangleColorCoords[0] = 1.0;
            rectangleColorCoords[1] = 0.6;
            rectangleColorCoords[2] = 0.2;
            rectangleColorCoords[3] = 1.0;
            rectangleColorCoords[4] = 0.6;
            rectangleColorCoords[5] = 0.2;
            rectangleColorCoords[6] = 0.074;
            rectangleColorCoords[7] = 0.533;
            rectangleColorCoords[8] = 0.2;
            rectangleColorCoords[9] = 0.074;
            rectangleColorCoords[10] = 0.533;
            rectangleColorCoords[11] = 0.2;

            gl.bindVertexArray(vao_quad);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_position_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleVertices,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_color_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleColorCoords,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.drawArrays(gl.TRIANGLE_FAN,
                0,
                4);

            gl.bindVertexArray(null);


            rectangleVertices[0] = 0.275;
            rectangleVertices[1] = 1.0;
            rectangleVertices[2] = 0.0;
            rectangleVertices[3] = 0.225;
            rectangleVertices[4] = 1.0;
            rectangleVertices[5] = 0.0;
            rectangleVertices[6] = 0.45;
            rectangleVertices[7] = -1.0;
            rectangleVertices[8] = 0.0;
            rectangleVertices[9] = 0.50;
            rectangleVertices[10] = -1.0;
            rectangleVertices[11] = 0.0;

            rectangleColorCoords[0] = 1.0;
            rectangleColorCoords[1] = 0.6;
            rectangleColorCoords[2] = 0.2;
            rectangleColorCoords[3] = 1.0;
            rectangleColorCoords[4] = 0.6;
            rectangleColorCoords[5] = 0.2;
            rectangleColorCoords[6] = 0.074;
            rectangleColorCoords[7] = 0.533;
            rectangleColorCoords[8] = 0.2;
            rectangleColorCoords[9] = 0.074;
            rectangleColorCoords[10] = 0.533;
            rectangleColorCoords[11] = 0.2;

            gl.bindVertexArray(vao_quad);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_position_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleVertices,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_color_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleColorCoords,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.drawArrays(gl.TRIANGLE_FAN,
                0,
                4);

            gl.bindVertexArray(null);


        case 'T':


            //Tricolor of A
            rectangleVertices[0] = 0.380;
            rectangleVertices[1] = 0.06;
            rectangleVertices[2] = 0.0;
            rectangleVertices[3] = 0.12;
            rectangleVertices[4] = 0.06;
            rectangleVertices[5] = 0.0;
            rectangleVertices[6] = 0.12;
            rectangleVertices[7] = 0.02;
            rectangleVertices[8] = 0.0;
            rectangleVertices[9] = 0.380;
            rectangleVertices[10] = 0.02;
            rectangleVertices[11] = 0.0;

            rectangleColorCoords[0] = 1.0;
            rectangleColorCoords[1] = 0.6;
            rectangleColorCoords[2] = 0.2;
            rectangleColorCoords[3] = 1.0;
            rectangleColorCoords[4] = 0.6;
            rectangleColorCoords[5] = 0.2;
            rectangleColorCoords[6] = 1.0;
            rectangleColorCoords[7] = 0.6;
            rectangleColorCoords[8] = 0.2;
            rectangleColorCoords[9] = 1.0;
            rectangleColorCoords[10] = 0.6;
            rectangleColorCoords[11] = 0.2;

            gl.bindVertexArray(vao_quad);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_position_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleVertices,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_color_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleColorCoords,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.drawArrays(gl.TRIANGLE_FAN,
                0,
                4);

            gl.bindVertexArray(null);

            rectangleVertices[0] = 0.380;
            rectangleVertices[1] = 0.02;
            rectangleVertices[2] = 0.0;
            rectangleVertices[3] = 0.12;
            rectangleVertices[4] = 0.02;
            rectangleVertices[5] = 0.0;
            rectangleVertices[6] = 0.12;
            rectangleVertices[7] = -0.02;
            rectangleVertices[8] = 0.0;
            rectangleVertices[9] = 0.380;
            rectangleVertices[10] = -0.02;
            rectangleVertices[11] = 0.0;

            rectangleColorCoords[0] = 1.0;
            rectangleColorCoords[1] = 1.0;
            rectangleColorCoords[2] = 1.0;
            rectangleColorCoords[3] = 1.0;
            rectangleColorCoords[4] = 1.0;
            rectangleColorCoords[5] = 1.0;
            rectangleColorCoords[6] = 1.0;
            rectangleColorCoords[7] = 1.0;
            rectangleColorCoords[8] = 1.0;
            rectangleColorCoords[9] = 1.0;
            rectangleColorCoords[10] = 1.0;
            rectangleColorCoords[11] = 1.0;

            gl.bindVertexArray(vao_quad);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_position_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleVertices,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_color_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleColorCoords,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.drawArrays(gl.TRIANGLE_FAN,
                0,
                4);

            gl.bindVertexArray(null);

            rectangleVertices[0] = 0.380;
            rectangleVertices[1] = -0.02;
            rectangleVertices[2] = 0.0;
            rectangleVertices[3] = 0.12;
            rectangleVertices[4] = -0.02;
            rectangleVertices[5] = 0.0;
            rectangleVertices[6] = 0.12;
            rectangleVertices[7] = -0.06;
            rectangleVertices[8] = 0.0;
            rectangleVertices[9] = 0.380;
            rectangleVertices[10] = -0.06;
            rectangleVertices[11] = 0.0;

            rectangleColorCoords[0] = 0.074;
            rectangleColorCoords[1] = 0.533;
            rectangleColorCoords[2] = 0.2;
            rectangleColorCoords[3] = 0.074;
            rectangleColorCoords[4] = 0.533;
            rectangleColorCoords[5] = 0.2;
            rectangleColorCoords[6] = 0.074;
            rectangleColorCoords[7] = 0.533;
            rectangleColorCoords[8] = 0.2;
            rectangleColorCoords[9] = 0.074;
            rectangleColorCoords[10] = 0.533;
            rectangleColorCoords[11] = 0.2;

            gl.bindVertexArray(vao_quad);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_position_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleVertices,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_color_quad);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                rectangleColorCoords,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.drawArrays(gl.TRIANGLE_FAN,
                0,
                4);

            gl.bindVertexArray(null);



            break;
    }
}



function UnInitialize() {
    if (vao_triangle) {
        gl.deleteVertexArrays(vao_triangle);
        vao_triangle = null;
    }
    if (vbo_position_triangle) {
        gl.deleteBuffers(vbo_position_triangle);
        vbo_position_triangle = null;
    }

    if (vbo_color_triangle) {
        gl.deleteVertexArrays(vbo_color_triangle);
        vbo_color_triangle = null;
    }
    if (vbo_color_quad) {
        gl.deleteBuffers(vbo_color_quad);
        vbo_color_quad = null;
    }

    if (vbo_position_quad) {
        gl.deleteVertexArrays(vbo_position_quad);
        vbo_position_quad = null;
    }
    if (vao_quad) {
        gl.deleteBuffers(vao_quad);
        vao_quad = null;
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