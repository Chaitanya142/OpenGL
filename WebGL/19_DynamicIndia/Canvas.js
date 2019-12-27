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

var stage = 1;
var I_1_XPosition = -2.25;
var A_XPosition = 2.25;
var N_YPosition = 2.25;
var I_2_YPosition = -2.25;
var Plane1_Y_Position = 0.00;
var Plane1_Z_Rotation_Angle = -90.0;

var Plane1_X_Position = -2.5;
var angle = 0.0;
var Line_X_Position = -1.28;

var sphere=null;
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
    gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_POSITION,
        3,
        gl.FLOAT,
        false,
        0,
        0);
    //Enable Vertex Attrib Array
    gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_POSITION);
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
    gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_COLOR,
        3,
        gl.FLOAT,
        false,
        0,
        0);
    //Enable Vertex Attrib Array
    gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_COLOR);
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
    gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_POSITION,
        3,
        gl.FLOAT,
        false,
        0,
        0);
    //Enable Vertex Attrib Array
    gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_POSITION);
    //Unbind Buffer
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    // gl.vertexAttrib3f(WebGLMacros.AMC_ATTRIBUTE_COLOR,
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
    gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_COLOR,
        3,
        gl.FLOAT,
        false,
        0,
        0);
    //Enable Vertex Attrib Array
    gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_COLOR);
    //Unbind Buffer
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //Unbind array
    gl.bindVertexArray(null);



    vao_point = gl.createVertexArray();
    gl.bindVertexArray(vao_point);

    vbo_position_point = gl.createBuffer();

    //Bind Generated Buffer
    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_position_point);
    //Fill Buffer
    gl.bufferData(gl.ARRAY_BUFFER,
        null,
        gl.DYNAMIC_DRAW);
    //Set Vertex Attrib Pointer
    gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_POSITION,
        3,
        gl.FLOAT,
        false,
        0,
        0);
    //Enable Vertex Attrib Array
    gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_POSITION);
    //Unbind Buffer
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    vbo_color_point = gl.createBuffer();

    //Bind Generated Buffer
    gl.bindBuffer(gl.ARRAY_BUFFER,
        vbo_color_point);
    //Fill Buffer
    gl.bufferData(gl.ARRAY_BUFFER,
        null,
        gl.DYNAMIC_DRAW);
    //Set Vertex Attrib Pointer
    gl.vertexAttribPointer(WebGLMacros.AMC_ATTRIBUTE_COLOR,
        3,
        gl.FLOAT,
        false,
        0,
        0);
    //Enable Vertex Attrib Array
    gl.enableVertexAttribArray(WebGLMacros.AMC_ATTRIBUTE_COLOR);
    //Unbind Buffer
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //Unbind array
    gl.bindVertexArray(null);


    sphere=new Mesh();
    setCenterSphere(0.21,0.0);
    makeSphere(sphere,0.07, 30, 30);

    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);
    //gl.enable(gl.CULL_FACE);
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
        //DrawLetter('T');

        //Unbind vao_quad

    }

    if (stage == 8) {
        Plane1_Y_Position = ((Plane1_X_Position - 1.3) * (Plane1_X_Position - 1.3) / (4 * 0.15)) - 0.0;

        mat4.identity(modelViewMatrix);
        mat4.identity(modelViewProjectionMatrix);

        //Transformation
        mat4.translate(modelViewMatrix, modelViewMatrix, [Plane1_X_Position, Plane1_Y_Position, -3.0]);
        mat4.rotateZ(modelViewMatrix, modelViewMatrix, degToRad(Plane1_Z_Rotation_Angle));

        mat4.multiply(
            modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix
        );

        //Send necessary matrices to shader in resp. Uniforms

        gl.uniformMatrix4fv(mvpUniform,
            false,
            modelViewProjectionMatrix);

        //Bind with vao_quad

        //Bind with textures if any

        DrawLetter('P');

        //Unbind vao_quad


        //Initialize matrices
        mat4.identity(modelViewMatrix);
        mat4.identity(modelViewProjectionMatrix);

        //Transformation
        mat4.translate(modelViewMatrix, modelViewMatrix, [Plane1_X_Position, 0.0, -3.0]);
        //mat4.rotateZ(modelViewMatrix, modelViewMatrix,degToRad(Plane1_Z_Rotation_Angle));     

        mat4.multiply(
            modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix
        );

        //Send necessary matrices to shader in resp. Uniforms

        gl.uniformMatrix4fv(mvpUniform,
            false,
            modelViewProjectionMatrix);

        //Bind with vao_quad

        //Bind with textures if any

        DrawLetter('P');

        //Unbind vao_quad


        //Initialize matrices
        mat4.identity(modelViewMatrix);
        mat4.identity(modelViewProjectionMatrix);

        //Transformation
        mat4.translate(modelViewMatrix, modelViewMatrix, [Plane1_X_Position, -Plane1_Y_Position, -3.0]);
        mat4.rotateZ(modelViewMatrix, modelViewMatrix, degToRad(-Plane1_Z_Rotation_Angle));

        mat4.multiply(
            modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix
        );

        //Send necessary matrices to shader in resp. Uniforms

        gl.uniformMatrix4fv(mvpUniform,
            false,
            modelViewProjectionMatrix);


        //Bind with vao_quad

        //Bind with textures if any

        DrawLetter('P');

        //Unbind vao_quad
    }

    if (stage == 7) {
        //Initialize matrices
        mat4.identity(modelViewMatrix);
        mat4.identity(modelViewProjectionMatrix);

        //Transformation
        mat4.translate(modelViewMatrix, modelViewMatrix, [Plane1_X_Position, 0.0, -3.0]);
        mat4.rotateZ(modelViewMatrix, modelViewMatrix, degToRad(-Plane1_Z_Rotation_Angle));

        mat4.multiply(
            modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix
        );

        //Send necessary matrices to shader in resp. Uniforms

        gl.uniformMatrix4fv(mvpUniform,
            false,
            modelViewProjectionMatrix);

        //Bind with vao_quad

        //Bind with textures if any

        DrawLetter('P');

        //Unbind vao_quad

    }

    if ((stage == 7 || stage == 8) && Plane1_X_Position > -1.02 && Plane1_X_Position < 1.40) {
        //Initialize matrices
        mat4.identity(modelViewMatrix);
        mat4.identity(modelViewProjectionMatrix);

        //Transformation
        mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -3.0]);
        //mat4.rotateZ(modelViewMatrix, modelViewMatrix, degToRad(-Plane1_Z_Rotation_Angle));

        mat4.multiply(
            modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix
        );

        //Send necessary matrices to shader in resp. Uniforms

        gl.uniformMatrix4fv(mvpUniform,
            false,
            modelViewProjectionMatrix);

        //Bind with vao_quad

        //Bind with textures if any

        DrawLetter('L');

        //Unbind vao_quad

    }

    if (stage > 7 && Plane1_X_Position > 1.40) {
        //Initialize matrices
        mat4.identity(modelViewMatrix);
        mat4.identity(modelViewProjectionMatrix);

        //Transformation
        mat4.translate(modelViewMatrix, modelViewMatrix, [A_XPosition, 0.0, -3.0]);
        //mat4.rotateZ(modelViewMatrix, modelViewMatrix, degToRad(-Plane1_Z_Rotation_Angle));

        mat4.multiply(
            modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix
        );

        //Send necessary matrices to shader in resp. Uniforms

        gl.uniformMatrix4fv(mvpUniform,
            false,
            modelViewProjectionMatrix);

        //Bind with vao_quad

        //Bind with textures if any

        DrawLetter('T');

        //Unbind vao_quad
    }

    if (stage > 7 && Plane1_X_Position > 1.40 && Line_X_Position <= 1.10) {
        Line_X_Position = Line_X_Position + 0.006;

        if (Line_White_R > 0.0)
            Line_White_R = Line_White_R - Line_White_R / 256;
        if (Line_White_G > 0.0)
            Line_White_G = Line_White_G - Line_White_G / 256;
        if (Line_White_B > 0.0)
            Line_White_B = Line_White_B - Line_White_B / 256;

        if (Line_Orange_R > 0.0)
            Line_Orange_R = Line_Orange_R - Line_Orange_R / 256;
        if (Line_Orange_G > 0.0)
            Line_Orange_G = Line_Orange_G - Line_Orange_G / 256;
        if (Line_Orange_B > 0.0)
            Line_Orange_B = Line_Orange_B - Line_Orange_B / 256;

        if (Line_Green_R > 0.0)
            Line_Green_R = Line_Green_R - Line_Green_R / 256;
        if (Line_Green_G > 0.0)
            Line_Green_G = Line_Green_G - Line_Green_G / 256;
        if (Line_Green_B > 0.0)
            Line_Green_B = Line_Green_B - Line_Green_B / 256;

        //Initialize matrices
        mat4.identity(modelViewMatrix);
        mat4.identity(modelViewProjectionMatrix);

        //Transformation
        mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -3.0]);
        //mat4.rotateZ(modelViewMatrix, modelViewMatrix, degToRad(-Plane1_Z_Rotation_Angle));

        mat4.multiply(
            modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix
        );

        //Send necessary matrices to shader in resp. Uniforms

        gl.uniformMatrix4fv(mvpUniform,
            false,
            modelViewProjectionMatrix);

        //Bind with vao_quad

        //Bind with textures if any

        DrawLetter('L');

        //Unbind vao_quad

    }

    if (stage > 7 && Plane1_X_Position > 1.40) {
        //Initialize matrices
        mat4.identity(modelViewMatrix);
        mat4.identity(modelViewProjectionMatrix);

        //Transformation
        mat4.translate(modelViewMatrix, modelViewMatrix, [A_XPosition, 0.0, -3.0]);
        //mat4.rotateZ(modelViewMatrix, modelViewMatrix, degToRad(-Plane1_Z_Rotation_Angle));

        mat4.multiply(
            modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix
        );

        //Send necessary matrices to shader in resp. Uniforms

        gl.uniformMatrix4fv(mvpUniform,
            false,
            modelViewProjectionMatrix);

        //Bind with vao_quad

        //Bind with textures if any

        DrawLetter('T');

        //Unbind vao_quad

    }


    if (stage == 6) {
        Plane1_Y_Position = ((Plane1_X_Position + 1.3) * (Plane1_X_Position + 1.3) / (4 * 0.15)) - 0.0;

        //Initialize matrices
        mat4.identity(modelViewMatrix);
        mat4.identity(modelViewProjectionMatrix);

        //Transformation
        mat4.translate(modelViewMatrix, modelViewMatrix, [Plane1_X_Position, Plane1_Y_Position, -3.0]);
        mat4.rotateZ(modelViewMatrix, modelViewMatrix, degToRad(Plane1_Z_Rotation_Angle));

        mat4.multiply(
            modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix
        );

        //Send necessary matrices to shader in resp. Uniforms

        gl.uniformMatrix4fv(mvpUniform,
            false,
            modelViewProjectionMatrix);

        //Bind with vao_quad

        //Bind with textures if any

        DrawLetter('P');

        //Unbind vao_quad

        //Initialize matrices
        mat4.identity(modelViewMatrix);
        mat4.identity(modelViewProjectionMatrix);

        //Transformation
        mat4.translate(modelViewMatrix, modelViewMatrix, [Plane1_X_Position, 0.0, -3.0]);
        //mat4.rotateZ(modelViewMatrix, modelViewMatrix, degToRad(Plane1_Z_Rotation_Angle));

        mat4.multiply(
            modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix
        );

        //Send necessary matrices to shader in resp. Uniforms

        gl.uniformMatrix4fv(mvpUniform,
            false,
            modelViewProjectionMatrix);

        //Bind with vao_quad

        //Bind with textures if any

        DrawLetter('P');

        //Unbind vao_quad

        //Initialize matrices
        mat4.identity(modelViewMatrix);
        mat4.identity(modelViewProjectionMatrix);

        //Transformation
        mat4.translate(modelViewMatrix, modelViewMatrix, [Plane1_X_Position, -1 * (Plane1_Y_Position), -3.0]);
        mat4.rotateZ(modelViewMatrix, modelViewMatrix, degToRad(-Plane1_Z_Rotation_Angle));

        mat4.multiply(
            modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix
        );

        //Send necessary matrices to shader in resp. Uniforms

        gl.uniformMatrix4fv(mvpUniform,
            false,
            modelViewProjectionMatrix);

        //Bind with vao_quad

        //Bind with textures if any

        DrawLetter('P');

        //Unbind vao_quad

    }

    gl.useProgram(null);

    requestAnimationFrame(draw, canvas);

    Update();

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



var triangleVertices = new Float32Array([

    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
    0.0, 0.0, 0.0,
]);

var triangleColorCoords = new Float32Array([
    0.729,
    0.886,
    0.933,
    0.729,
    0.886,
    0.933,
    0.729,
    0.886,
    0.933
]);


function Update() {
    switch (stage) {
        case 1:
            //Print I
            if (I_1_XPosition < -1.5)
                //I_1_XPosition = I_1_XPosition + 0.00022f;
                I_1_XPosition = I_1_XPosition + 0.005;
            else {
                I_1_XPosition = -1.5;
                stage++;
            }
            break;
        case 2:
            //Print A
            if (A_XPosition > 0.75)
                //A_XPosition = A_XPosition - 0.00022f;
                A_XPosition = A_XPosition - 0.005;
            else {
                A_XPosition = 0.75;
                stage++;
            }
            break;
        case 3:
            //Print N
            if (N_YPosition > 0.00)
                //N_YPosition = N_YPosition - 0.00022f;
                N_YPosition = N_YPosition - 0.005;
            else {
                N_YPosition = 0.00;
                stage++;
            }
            break;
        case 4:
            //Print I
            if (I_2_YPosition < 0.00)
                //I_2_YPosition = I_2_YPosition + 0.00022f;
                I_2_YPosition = I_2_YPosition + 0.005;
            else {
                I_2_YPosition = 0.00;
                stage++;
            }
            break;
        case 5:
            //Print D
            if (D_R_OrangeColor < 1.0)
                //D_R_OrangeColor = D_R_OrangeColor + 1.0f / 5050;
                D_R_OrangeColor = D_R_OrangeColor + 1.0 / 1000;
            if (D_G_OrangeColor < 0.6)
                D_G_OrangeColor = D_G_OrangeColor + 0.6 / 1000;
            if (D_B_OrangeColor < 0.2)
                D_B_OrangeColor = D_B_OrangeColor + 0.2 / 1000;

            if (D_R_GreenColor < 0.074)
                D_R_GreenColor = D_R_GreenColor + 0.074 / 1000;
            if (D_G_GreenColor < 0.533)
                D_G_GreenColor = D_G_GreenColor + 0.533 / 1000;
            if (D_B_GreenColor < 0.2)
                D_B_GreenColor = D_B_GreenColor + 0.2 / 1000;

            if (D_R_OrangeColor >= 1.0 && D_G_OrangeColor >= 0.6 && D_B_OrangeColor >= 0.2 && D_R_GreenColor >= 0.074 && D_G_GreenColor >= 0.533 && D_B_GreenColor >= 0.2)
                stage++;
            break;
        case 6:
            if (Plane1_X_Position < -1.3) {
                //Plane1_X_Position = Plane1_X_Position + 0.0008f;
                Plane1_X_Position = Plane1_X_Position + 0.001;
                //Plane1_Z_Rotation_Angle = Plane1_Z_Rotation_Angle + 90.0f / (1.2 / 0.0008);//0.015f
                Plane1_Z_Rotation_Angle = Plane1_Z_Rotation_Angle + 90.0 / (1.2 / 0.001);//0.015f
            }
            else
                stage++;
            break;
        case 7:
            if (Plane1_X_Position < 1.3)
                //Plane1_X_Position = Plane1_X_Position + 0.0008f;
                Plane1_X_Position = Plane1_X_Position + 0.005;
            else
                stage++;
            break;
        case 8:
            if (Plane1_X_Position < 2.5) {
                //Plane1_X_Position = Plane1_X_Position + 0.0008f;
                //Plane1_Z_Rotation_Angle = Plane1_Z_Rotation_Angle + 90.0f / (1.3 / 0.0008);
                Plane1_X_Position = Plane1_X_Position + 0.001;
                Plane1_Z_Rotation_Angle = Plane1_Z_Rotation_Angle + 90.0 / (1.3 / 0.001);
            }
            else
                stage++;
            break;
    }
}



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

            break;

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
        case 'L':
            gl.bindVertexArray(vao_quad);

            if (Plane1_X_Position < 1.40) {
                //Line Red
                rectangleVertices[0] = Plane1_X_Position - 0.27;
                rectangleVertices[1] = 0.06;
                rectangleVertices[2] = 0.0;
                rectangleVertices[3] = Line_X_Position;
                rectangleVertices[4] = 0.06;
                rectangleVertices[5] = 0.0;
                rectangleVertices[6] = Line_X_Position;
                rectangleVertices[7] = 0.02;
                rectangleVertices[8] = 0.0;
                rectangleVertices[9] = Plane1_X_Position - 0.27;
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

                // W
                rectangleVertices[0] = Plane1_X_Position - 0.27;
                rectangleVertices[1] = 0.02;
                rectangleVertices[2] = 0.0;
                rectangleVertices[3] = Line_X_Position;
                rectangleVertices[4] = 0.02;
                rectangleVertices[5] = 0.0;
                rectangleVertices[6] = Line_X_Position;
                rectangleVertices[7] = -0.02;
                rectangleVertices[8] = 0.0;
                rectangleVertices[9] = Plane1_X_Position - 0.27;
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


                // G		
                rectangleVertices[0] = Plane1_X_Position - 0.27;
                rectangleVertices[1] = -0.02;
                rectangleVertices[2] = 0.0;
                rectangleVertices[3] = Line_X_Position;
                rectangleVertices[4] = -0.02;
                rectangleVertices[5] = 0.0;
                rectangleVertices[6] = Line_X_Position;
                rectangleVertices[7] = -0.06;
                rectangleVertices[8] = 0.0;
                rectangleVertices[9] = Plane1_X_Position - 0.27;
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
            }
            else {

                //R
                rectangleVertices[0] = 1.1;
                rectangleVertices[1] = 0.06;
                rectangleVertices[2] = 0.0;
                rectangleVertices[3] = Line_X_Position;
                rectangleVertices[4] = 0.06;
                rectangleVertices[5] = 0.0;
                rectangleVertices[6] = Line_X_Position;
                rectangleVertices[7] = 0.02;
                rectangleVertices[8] = 0.0;
                rectangleVertices[9] = 1.1;
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
                // W
                rectangleVertices[0] = 1.1;
                rectangleVertices[1] = 0.02;
                rectangleVertices[2] = 0.0;
                rectangleVertices[3] = Line_X_Position;
                rectangleVertices[4] = 0.02;
                rectangleVertices[5] = 0.0;
                rectangleVertices[6] = Line_X_Position;
                rectangleVertices[7] = -0.02;
                rectangleVertices[8] = 0.0;
                rectangleVertices[9] = 1.1;
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
                // G		
                rectangleVertices[0] = 1.1;
                rectangleVertices[1] = -0.02;
                rectangleVertices[2] = 0.0;
                rectangleVertices[3] = Line_X_Position;
                rectangleVertices[4] = -0.02;
                rectangleVertices[5] = 0.0;
                rectangleVertices[6] = Line_X_Position;
                rectangleVertices[7] = -0.06;
                rectangleVertices[8] = 0.0;
                rectangleVertices[9] = 1.1;
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
            }
            gl.bindVertexArray(null);

            break;
        //Plane
        case 'P':
            gl.bindVertexArray(vao_quad);

            //Smoke R
            rectangleVertices[0] = -0.24;
            rectangleVertices[1] = 0.06;
            rectangleVertices[2] = 0.0;
            rectangleVertices[3] = -0.40;
            rectangleVertices[4] = 0.06;
            rectangleVertices[5] = 0.0;
            rectangleVertices[6] = -0.40;
            rectangleVertices[7] = -0.02;
            rectangleVertices[8] = 0.0;
            rectangleVertices[9] = -0.24;
            rectangleVertices[10] = -0.02;
            rectangleVertices[11] = 0.0;

            rectangleColorCoords[0] = 1.0;
            rectangleColorCoords[1] = 0.6;
            rectangleColorCoords[2] = 0.2;
            rectangleColorCoords[3] = 0.0;
            rectangleColorCoords[4] = 0.0;
            rectangleColorCoords[5] = 0.0;
            rectangleColorCoords[6] = 0.0;
            rectangleColorCoords[7] = 0.0;
            rectangleColorCoords[8] = 0.0;
            rectangleColorCoords[9] = 1.0;
            rectangleColorCoords[10] = 0.6;
            rectangleColorCoords[11] = 0.2;

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
            //Smoke W

            rectangleVertices[0] = -0.24;
            rectangleVertices[1] = 0.02;
            rectangleVertices[2] = 0.0;
            rectangleVertices[3] = -0.40;
            rectangleVertices[4] = 0.02;
            rectangleVertices[5] = 0.0;
            rectangleVertices[6] = -0.40;
            rectangleVertices[7] = -0.02;
            rectangleVertices[8] = 0.0;
            rectangleVertices[9] = -0.24;
            rectangleVertices[10] = -0.02;
            rectangleVertices[11] = 0.0;

            rectangleColorCoords[0] = 1.0;
            rectangleColorCoords[1] = 1.0;
            rectangleColorCoords[2] = 1.0;
            rectangleColorCoords[3] = 0.0;
            rectangleColorCoords[4] = 0.0;
            rectangleColorCoords[5] = 0.0;
            rectangleColorCoords[6] = 0.0;
            rectangleColorCoords[7] = 0.0;
            rectangleColorCoords[8] = 0.0;
            rectangleColorCoords[9] = 1.0;
            rectangleColorCoords[10] = 1.0;
            rectangleColorCoords[11] = 1.0;

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


            //Smoke G

            rectangleVertices[0] = -0.24;
            rectangleVertices[1] = -0.02;
            rectangleVertices[2] = 0.0;
            rectangleVertices[3] = -0.40;
            rectangleVertices[4] = -0.02;
            rectangleVertices[5] = 0.0;
            rectangleVertices[6] = -0.40;
            rectangleVertices[7] = -0.06;
            rectangleVertices[8] = 0.0;
            rectangleVertices[9] = -0.24;
            rectangleVertices[10] = -0.06;
            rectangleVertices[11] = 0.0;

            rectangleColorCoords[0] = 0.074;
            rectangleColorCoords[1] = 0.533;
            rectangleColorCoords[2] = 0.2;
            rectangleColorCoords[3] = 0.0;
            rectangleColorCoords[4] = 0.0;
            rectangleColorCoords[5] = 0.0;
            rectangleColorCoords[6] = 0.0;
            rectangleColorCoords[7] = 0.0;
            rectangleColorCoords[8] = 0.0;
            rectangleColorCoords[9] = 0.074;
            rectangleColorCoords[10] = 0.533;
            rectangleColorCoords[11] = 0.2;

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

            rectangleVertices[0] = 0.20;
            rectangleVertices[1] = 0.07;
            rectangleVertices[2] = 0.0;
            rectangleVertices[3] = -0.20;
            rectangleVertices[4] = 0.07;
            rectangleVertices[5] = 0.0;
            rectangleVertices[6] = -0.20;
            rectangleVertices[7] = -0.07;
            rectangleVertices[8] = 0.0;
            rectangleVertices[9] = 0.20;
            rectangleVertices[10] = -0.07;
            rectangleVertices[11] = 0.0;

            rectangleColorCoords[0] = 0.729;
            rectangleColorCoords[1] = 0.886;
            rectangleColorCoords[2] = 0.933;
            rectangleColorCoords[3] = 0.729;
            rectangleColorCoords[4] = 0.886;
            rectangleColorCoords[5] = 0.933;
            rectangleColorCoords[6] = 0.729;
            rectangleColorCoords[7] = 0.886;
            rectangleColorCoords[8] = 0.933;
            rectangleColorCoords[9] = 0.729;
            rectangleColorCoords[10] = 0.886;
            rectangleColorCoords[11] = 0.933;

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


            //Back Wings

            rectangleVertices[0] = -0.10;
            rectangleVertices[1] = 0.0;
            rectangleVertices[2] = 0.0;
            rectangleVertices[3] = -0.27;
            rectangleVertices[4] = 0.12;
            rectangleVertices[5] = 0.0;
            rectangleVertices[6] = -0.31;
            rectangleVertices[7] = 0.12;
            rectangleVertices[8] = 0.0;
            rectangleVertices[9] = -0.24;
            rectangleVertices[10] = 0.0;
            rectangleVertices[11] = 0.0;

            rectangleColorCoords[0] = 0.729;
            rectangleColorCoords[1] = 0.886;
            rectangleColorCoords[2] = 0.933;
            rectangleColorCoords[3] = 0.729;
            rectangleColorCoords[4] = 0.886;
            rectangleColorCoords[5] = 0.933;
            rectangleColorCoords[6] = 0.729;
            rectangleColorCoords[7] = 0.886;
            rectangleColorCoords[8] = 0.933;
            rectangleColorCoords[9] = 0.729;
            rectangleColorCoords[10] = 0.886;
            rectangleColorCoords[11] = 0.933;

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


            rectangleVertices[0] = -0.10;
            rectangleVertices[1] = 0.0;
            rectangleVertices[2] = 0.0;
            rectangleVertices[3] = -0.27;
            rectangleVertices[4] = -0.12;
            rectangleVertices[5] = 0.0;
            rectangleVertices[6] = -0.31;
            rectangleVertices[7] = -0.12;
            rectangleVertices[8] = 0.0;
            rectangleVertices[9] = -0.24;
            rectangleVertices[10] = 0.0;
            rectangleVertices[11] = 0.0;

            rectangleColorCoords[0] = 0.729;
            rectangleColorCoords[1] = 0.886;
            rectangleColorCoords[2] = 0.933;
            rectangleColorCoords[3] = 0.729;
            rectangleColorCoords[4] = 0.886;
            rectangleColorCoords[5] = 0.933;
            rectangleColorCoords[6] = 0.729;
            rectangleColorCoords[7] = 0.886;
            rectangleColorCoords[8] = 0.933;
            rectangleColorCoords[9] = 0.729;
            rectangleColorCoords[10] = 0.886;
            rectangleColorCoords[11] = 0.933;

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

            //Letter I
            rectangleVertices[0] = -0.05;
            rectangleVertices[1] = 0.05;
            rectangleVertices[2] = 0.0;
            rectangleVertices[3] = -0.07;
            rectangleVertices[4] = 0.05;
            rectangleVertices[5] = 0.0;
            rectangleVertices[6] = -0.07;
            rectangleVertices[7] = -0.05;
            rectangleVertices[8] = 0.0;
            rectangleVertices[9] = -0.05;
            rectangleVertices[10] = -0.05;
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


            //Letter A			
            rectangleVertices[0] = 0.00;
            rectangleVertices[1] = 0.05;
            rectangleVertices[2] = 0.0;
            rectangleVertices[3] = -0.02;
            rectangleVertices[4] = 0.05;
            rectangleVertices[5] = 0.0;
            rectangleVertices[6] = -0.04;
            rectangleVertices[7] = -0.05;
            rectangleVertices[8] = 0.0;
            rectangleVertices[9] = -0.02;
            rectangleVertices[10] = -0.05;
            rectangleVertices[11] = 0.0;

            rectangleColorCoords[0] = 0.0;
            rectangleColorCoords[1] = 0.0;
            rectangleColorCoords[2] = 0.530;
            rectangleColorCoords[3] = 0.0;
            rectangleColorCoords[4] = 0.0;
            rectangleColorCoords[5] = 0.530;
            rectangleColorCoords[6] = 0.0;
            rectangleColorCoords[7] = 0.0;
            rectangleColorCoords[8] = 0.530;
            rectangleColorCoords[9] = 0.0;
            rectangleColorCoords[10] = 0.0;
            rectangleColorCoords[11] = 0.530;

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

                rectangleVertices[0] = 0.00;
                rectangleVertices[1] = 0.05;
                rectangleVertices[2] = 0.0;
                rectangleVertices[3] = 0.02;
                rectangleVertices[4] = 0.05;
                rectangleVertices[5] = 0.0;
                rectangleVertices[6] = 0.04;
                rectangleVertices[7] = -0.05;
                rectangleVertices[8] = 0.0;
                rectangleVertices[9] = 0.02;
                rectangleVertices[10] = -0.05;
                rectangleVertices[11] = 0.0;
        
                rectangleColorCoords[0] = 0.0;
                rectangleColorCoords[1] = 0.0;
                rectangleColorCoords[2] = 0.530;
                rectangleColorCoords[3] = 0.0;
                rectangleColorCoords[4] = 0.0;
                rectangleColorCoords[5] = 0.530;
                rectangleColorCoords[6] = 0.0;
                rectangleColorCoords[7] = 0.0;
                rectangleColorCoords[8] = 0.530;
                rectangleColorCoords[9] = 0.0;
                rectangleColorCoords[10] = 0.0;
                rectangleColorCoords[11] = 0.530;

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

        
		rectangleVertices[0] = 0.02;
		rectangleVertices[1] = 0.01;
		rectangleVertices[2] = 0.0;
		rectangleVertices[3] = -0.02;
		rectangleVertices[4] = 0.01;
		rectangleVertices[5] = 0.0;
		rectangleVertices[6] = -0.02;
		rectangleVertices[7] = -0.01;
		rectangleVertices[8] = 0.0;
		rectangleVertices[9] = 0.02;
		rectangleVertices[10] = -0.01;
		rectangleVertices[11] = 0.0;

		rectangleColorCoords[0] = 0.0;
		rectangleColorCoords[1] = 0.0;
		rectangleColorCoords[2] = 0.530;
		rectangleColorCoords[3] = 0.0;
		rectangleColorCoords[4] = 0.0;
		rectangleColorCoords[5] = 0.530;
		rectangleColorCoords[6] = 0.0;
		rectangleColorCoords[7] = 0.0;
		rectangleColorCoords[8] = 0.530;
		rectangleColorCoords[9] = 0.0;
		rectangleColorCoords[10] = 0.0;
		rectangleColorCoords[11] = 0.530;


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


            //Letter F

            rectangleVertices[0] = 0.06;
            rectangleVertices[1] = 0.05;
            rectangleVertices[2] = 0.0;
            rectangleVertices[3] = 0.04;
            rectangleVertices[4] = 0.05;
            rectangleVertices[5] = 0.0;
            rectangleVertices[6] = 0.04;
            rectangleVertices[7] = -0.05;
            rectangleVertices[8] = 0.0;
            rectangleVertices[9] = 0.06;
            rectangleVertices[10] = -0.05;
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
            rectangleVertices[0] = 0.09;
            rectangleVertices[1] = 0.05;
            rectangleVertices[2] = 0.0;
            rectangleVertices[3] = 0.06;
            rectangleVertices[4] = 0.05;
            rectangleVertices[5] = 0.0;
            rectangleVertices[6] = 0.06;
            rectangleVertices[7] = 0.035;
            rectangleVertices[8] = 0.0;
            rectangleVertices[9] = 0.09;
            rectangleVertices[10] = 0.035;
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

            rectangleVertices[0] = 0.09;
            rectangleVertices[1] = 0.02;
            rectangleVertices[2] = 0.0;
            rectangleVertices[3] = 0.06;
            rectangleVertices[4] = 0.02;
            rectangleVertices[5] = 0.0;
            rectangleVertices[6] = 0.06;
            rectangleVertices[7] = 0.005;
            rectangleVertices[8] = 0.0;
            rectangleVertices[9] = 0.09;
            rectangleVertices[10] = 0.005;
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




            gl.bindVertexArray(vao_triangle);

            //Upper Wing
            triangleVertices[0] = -0.05;
            triangleVertices[1] = 0.07;
            triangleVertices[2] = 0.0;
            triangleVertices[3] = 0.05;
            triangleVertices[4] = 0.07;
            triangleVertices[5] = 0.0;
            triangleVertices[6] = -0.15;
            triangleVertices[7] = 0.22;
            triangleVertices[8] = 0.0;

            /*triangleColorCoords[0] = 0.729f;
            triangleColorCoords[1] = 0.886f;
            triangleColorCoords[2] = 0.933f;
            triangleColorCoords[3] = 1.0f;
            triangleColorCoords[4] = 0.6f;
            triangleColorCoords[5] = 0.2f;
            triangleColorCoords[6] = 0.074f;
            triangleColorCoords[7] = 0.533f;
            triangleColorCoords[8] = 0.2f;*/

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_position_triangle);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                triangleVertices,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_color_triangle);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                triangleColorCoords,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.drawArrays(gl.TRIANGLE_FAN,
                0,
                3);

            gl.bindVertexArray(null);



            gl.bindVertexArray(vao_triangle);

            triangleVertices[0] = -0.05;
            triangleVertices[1] = -0.07;
            triangleVertices[2] = 0.0;
            triangleVertices[3] = 0.05;
            triangleVertices[4] = -0.07;
            triangleVertices[5] = 0.0;
            triangleVertices[6] = -0.15;
            triangleVertices[7] = -0.22;
            triangleVertices[8] = 0.0;

            /*triangleColorCoords[0] = 0.729f;
            triangleColorCoords[1] = 0.886f;
            triangleColorCoords[2] = 0.933f;
            triangleColorCoords[3] = 1.0f;
            triangleColorCoords[4] = 0.6f;
            triangleColorCoords[5] = 0.2f;
            triangleColorCoords[6] = 0.074f;
            triangleColorCoords[7] = 0.533f;
            triangleColorCoords[8] = 0.2f;*/

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_position_triangle);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                triangleVertices,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_color_triangle);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                triangleColorCoords,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.drawArrays(gl.TRIANGLE_FAN,
                0,
                3);

            gl.bindVertexArray(null);


            //Lower Wing

            gl.bindVertexArray(vao_triangle);

            triangleVertices[0] = 0.05;
            triangleVertices[1] = 0.07;
            triangleVertices[2] = 0.0;
            triangleVertices[3] = -0.10;
            triangleVertices[4] = 0.22;
            triangleVertices[5] = 0.0;
            triangleVertices[6] = -0.15;
            triangleVertices[7] = 0.22;
            triangleVertices[8] = 0.0;

            /*triangleColorCoords[0] = 0.729f;
            triangleColorCoords[1] = 0.886f;
            triangleColorCoords[2] = 0.933f;
            triangleColorCoords[3] = 1.0f;
            triangleColorCoords[4] = 0.6f;
            triangleColorCoords[5] = 0.2f;
            triangleColorCoords[6] = 0.074f;
            triangleColorCoords[7] = 0.533f;
            triangleColorCoords[8] = 0.2f;*/

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_position_triangle);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                triangleVertices,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_color_triangle);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                triangleColorCoords,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.drawArrays(gl.TRIANGLE_FAN,
                0,
                3);

            gl.bindVertexArray(null);



            gl.bindVertexArray(vao_triangle);

            triangleVertices[0] = 0.05;
            triangleVertices[1] = -0.07;
            triangleVertices[2] = 0.0;
            triangleVertices[3] = -0.10;
            triangleVertices[4] = -0.22;
            triangleVertices[5] = 0.0;
            triangleVertices[6] = -0.15;
            triangleVertices[7] = -0.22;
            triangleVertices[8] = 0.0;

            /*triangleColorCoords[0] = 0.729f;
            triangleColorCoords[1] = 0.886f;
            triangleColorCoords[2] = 0.933f;
            triangleColorCoords[3] = 1.0f;
            triangleColorCoords[4] = 0.6f;
            triangleColorCoords[5] = 0.2f;
            triangleColorCoords[6] = 0.074f;
            triangleColorCoords[7] = 0.533f;
            triangleColorCoords[8] = 0.2f;*/

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_position_triangle);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                triangleVertices,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.bindBuffer(gl.ARRAY_BUFFER,
                vbo_color_triangle);
            //Fill Buffer
            gl.bufferData(gl.ARRAY_BUFFER,
                triangleColorCoords,
                gl.DYNAMIC_DRAW);
            gl.bindBuffer(gl.ARRAY_BUFFER,
                null);

            gl.drawArrays(gl.TRIANGLE_FAN,
                0,
                3);

            gl.bindVertexArray(null);




            //glBegin(GL_TRIANGLES);

            ////Upper Wing
            //glVertex3f(-0.05f, 0.07f, 0.0f);
            //glVertex3f(0.05f, 0.07f, 0.0f);
            //glVertex3f(-0.15f, 0.22f, 0.0f);

            //glVertex3f(-0.05f, -0.07f, 0.0f);
            //glVertex3f(0.05f, -0.07f, 0.0f);
            //glVertex3f(-0.15f, -0.22f, 0.0f);
            ////Lower Wing
            //glVertex3f(0.05f, 0.07f, 0.0f);
            //glVertex3f(-0.10f, 0.22f, 0.0f);
            //glVertex3f(-0.15f, 0.22f, 0.0f);

            //glVertex3f(0.05f, -0.07f, 0.0f);
            //glVertex3f(-0.10f, -0.22f, 0.0f);
            //glVertex3f(-0.15f, -0.22f, 0.0f);

            //glEnd();


            ////Front Circle

            //glBegin(GL_POINTS);

            ////glColor3f(1.0f, 1.0f, 1.0f);

            //for (float radius = 0.0f; radius <= 0.07f; radius = radius + 0.001f) {
            //	for (angle = 0.00f; angle < 2 * 3.14159265; angle = angle + 0.01f) {
            //		glVertex3f(cos(angle)*radius + 0.21f, sin(angle)*radius, 0.0f);
            //	}
            //}
            //glEnd();

            /*glColor3f(0.729f, 0.886f, 0.933f);
    
            if (!initCircle) {
                for (float radius = 0.0f; radius <= 0.07f; radius = radius + 0.001f) {
                    for (angle = 0.00f; angle < 2 * 3.14159265; angle = angle + 0.01f) {
                        glVertex3f(cos(angle)*radius + 0.21f, sin(angle)*radius, 0.0f);
                    }
                }
            }
    
            glBindVertexArray(vao_point);
    
            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_position_point);
            glBufferData(GL_ARRAY_BUFFER,
                3 * 3 * sizeof(float),
                triangleVertices,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
    
            glBindBuffer(GL_ARRAY_BUFFER,
                vbo_color_point);
            glBufferData(GL_ARRAY_BUFFER,
                3 * 3 * sizeof(float),
                triangleColorCoords,
                GL_DYNAMIC_DRAW);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
    
            glDrawArrays(GL_TRIANGLES,
                0,
                3);
    
            glBindVertexArray(0);
    */

            //           planeFrontSphere.drawWithColor();//uncomment

            sphere.draw();

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