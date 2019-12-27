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


var sphere=null;

var mvUniform;
var pUniform;
var lKeyIsPressedUniform;
var ldUniform;
var kdUniform;
var lightPositionUniform;


var gb_Lighting=0;

var perspectiveProjectionMatrix;

var gVertexShaderObject;
var gFragementShaderObject;
var gShaderProgramObject;

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
     
        case 76:
            if (gb_Lighting == 0)
				gb_Lighting = 1;
			else
				gb_Lighting = 0;
            //l
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
        "precision mediump int;"+
        "in vec4 vPosition;" +
		"in vec3 vNormal;" +
		"uniform mat4 u_mv_matrix;" +
		"uniform mat4 u_p_matrix;" +
		"uniform int u_lkeyispressed;" +
		"uniform vec3 u_ld;" +
		"uniform vec3 u_kd;" +
		"uniform vec4 u_lightposition;" +
		"out vec3 diffusecolor;" +
		"void main(void)" +
		"{" +
		"gl_Position=u_p_matrix*u_mv_matrix * vPosition;" +
		"if(u_lkeyispressed==1)"+
		"{"+
		"vec4 eyecoordinates=u_mv_matrix*vPosition;"+
		"mat3 normalmatrix=mat3(transpose(inverse(u_mv_matrix)));"+
		"vec3 tnorm=normalize(normalmatrix*vNormal);"+
		"vec3 s=normalize(vec3(u_lightposition)-eyecoordinates.xyz);"+
		"diffusecolor=u_ld*u_kd*max(dot(s,tnorm),0.0);"+
		"};"+
		"} ";

    //Shader object and source code mapping
    gl.shaderSource(gVertexShaderObject, vertextShaderSourceCode);

    //Compile Shader
    gl.compileShader(gVertexShaderObject);

    //Error Checking
    if (gl.getShaderParameter(gVertexShaderObject, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(gVertexShaderObject);
        if (error.length > 0) {
            alert(error+" VertexShaderObject");
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
        "precision mediump int;"+

        "out vec4 fragColor;"+
		"in vec3 diffusecolor;" +
		"uniform int u_lkeyispressed;" +
    
        "void main(void)" +
		"{" +
		"if(u_lkeyispressed==1)"+
            "{"+
            "fragColor=vec4(diffusecolor,1.0f);"+
            "}"+
		"else"+
            "{"+
            "fragColor=vec4(1.0f,1.0f,1.0f,1.0f);"+
            "}"+
		"} ";

    //Shader object and source code mapping
    gl.shaderSource(gFragementShaderObject, fragementShaderSourceCode);

    //Compile Shader
    gl.compileShader(gFragementShaderObject);

    //Error Checking
    if (gl.getShaderParameter(gFragementShaderObject, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(gFragementShaderObject);
        if (error.length > 0) {
            alert(error+" FragementShaderObject");
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
        WebGLMacros.AMC_ATTRIBUTE_NORMAL,
        "vNormal");



    //Link Shader Program
    gl.linkProgram(gShaderProgramObject);

    if (!gl.getProgramParameter(gShaderProgramObject, gl.LINK_STATUS)) {
        var error = gl.getProgramInfoLog(gShaderProgramObject);
        if (error.length > 0) {
            alert(error);
            UnInitialize();
        }
    }

    mvUniform = gl.getUniformLocation(gShaderProgramObject,
        "u_mv_matrix");
        pUniform = gl.getUniformLocation(gShaderProgramObject,
            "u_p_matrix");
            lKeyIsPressedUniform = gl.getUniformLocation(gShaderProgramObject,
                "u_lkeyispressed");
                ldUniform = gl.getUniformLocation(gShaderProgramObject,
                    "u_ld");
                    kdUniform = gl.getUniformLocation(gShaderProgramObject,
                        "u_kd");
                        lightPositionUniform = gl.getUniformLocation(gShaderProgramObject,
                            "u_lightposition");

  
    gl.depthFunc(gl.LEQUAL);
    gl.enable(gl.DEPTH_TEST);
    
    sphere=new Mesh();
    //Sun.setColor(1.0,1.0,1.0);
    makeSphere(sphere,0.75, 30, 30,0.0,0.0,0.0);

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

    mat4.translate(modelViewMatrix, modelViewMatrix, [0.0, 0.0, -3.0]);
    // mat4.rotateX(modelViewMatrix, modelViewMatrix,degToRad(angleCube));
    // mat4.rotateY(modelViewMatrix, modelViewMatrix,degToRad(angleCube));
    // mat4.rotateZ(modelViewMatrix, modelViewMatrix,degToRad(angleCube));
    // mat4.scale(modelViewMatrix, modelViewMatrix, [0.75, 0.75, 0.75]);

    //mat4.multiply(modelViewMatrix,modelViewMatrix,1.0)
    //Matrix Multiplication

    //Send necessary matrices to shader in resp. Uniforms

    gl.uniformMatrix4fv(mvUniform,
        false,
        modelViewMatrix);

    gl.uniformMatrix4fv(pUniform,
            false,
            perspectiveProjectionMatrix);
    
    if (gb_Lighting) {
                gl.uniform1i(lKeyIsPressedUniform, gb_Lighting);
                gl.uniform3f(ldUniform, 1.0, 1.0, 1.0);
                gl.uniform3f(kdUniform, 0.5, 0.5, 0.5);
                gl.uniform4f(lightPositionUniform, 0.0, 0.0, 2.0, 1.0);
    }
    else {
                gl.uniform1i(lKeyIsPressedUniform, gb_Lighting);
    }

    sphere.draw();
   

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