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

var mUniform;
var vUniform;
var pUniform;
var lKeyIsPressedUniform;
var ldUniform;
var kdUniform;
var laUniform;
var kaUniform;
var lsUniform;
var ksUniform;
var lightPositionUniform;
var shininessUniform;

var gb_Lighting=0;

var perspectiveProjectionMatrix;

var gVertexShaderObject_frag;
var gFragementShaderObject_frag;
var gShaderProgramObject_frag;

var gVertexShaderObject_vert;
var gFragementShaderObject_vert;
var gShaderProgramObject_vert;

var perVertexOrFragement=0;

var lightAmbient=new Float32Array(4);
var lightDiffuse=new Float32Array(4);
var lightSpecular=new Float32Array(4);
var lightPosition=new Float32Array(4);

var materialAmbient=new Float32Array(4);
var materialDiffuse=new Float32Array(4);
var materialSpecular=new Float32Array(4);
var materialShininess;

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
        // case 70:
        //     toggleFullScreen();
        //     //drawText("Hello World!!!");
        //     break;
        case 27://Escape
            // UnInitialize();
            // window.close();
            toggleFullScreen();

            break;
     
        case 76:
            if (gb_Lighting == 0)
				gb_Lighting = 1;
			else
				gb_Lighting = 0;
            //l
            break;
        case 86:
            perVertexOrFragement=0;
            getUniformLocation(gShaderProgramObject_vert);
            break;
        case 70:
            perVertexOrFragement=1;
            getUniformLocation(gShaderProgramObject_frag);
            break;
        case 69:
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
    gVertexShaderObject_frag = gl.createShader(gl.VERTEX_SHADER);

    //Vertex Shader Code
    var vertextShaderSourceCode_frag =
        "#version 300 es " +
        "\n " +
        "precision mediump int;"+
        "in vec4 vPosition;" +
		"in vec3 vNormal;" +
		"uniform mat4 u_m_matrix;" +
		"uniform mat4 u_v_matrix;" +
		"uniform mat4 u_p_matrix;" +
		"uniform int u_lkeyispressed;" +
		"uniform vec4 u_lightposition;" +
		"out vec3 tnorm;"+
		"out vec3 lightDirection;"+
		"out vec3 reflectionVector;"+
		"out vec3 viewerVector;"+
		"void main(void)" +
		"{" +
		"gl_Position=u_p_matrix*u_v_matrix*u_m_matrix * vPosition;" +
		"if(u_lkeyispressed==1)"+
		"{"+
		"vec4 eyecoordinates=u_v_matrix*u_m_matrix*vPosition;"+
		"tnorm=mat3(u_v_matrix*u_m_matrix)*vNormal;"+
		"lightDirection=vec3(u_lightposition-eyecoordinates);"+
		"reflectionVector=reflect(-lightDirection,tnorm);"+
		"viewerVector=vec3(-eyecoordinates);"+
		"}"+
		"}";
    
    //Shader object and source code mapping
    gl.shaderSource(gVertexShaderObject_frag, vertextShaderSourceCode_frag);

    //Compile Shader
    gl.compileShader(gVertexShaderObject_frag);

    //Error Checking
    if (gl.getShaderParameter(gVertexShaderObject_frag, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(gVertexShaderObject_frag);
        if (error.length > 0) {
            alert(error+" VertexShaderObject");
            UnInitialize();
        }
    }

    //Fragement Shader
    //Define Shader Object
    gFragementShaderObject_frag = gl.createShader(gl.FRAGMENT_SHADER);

    //Fragement Shader Code
    var fragementShaderSourceCode_frag =
        "#version 300 es " +
        "\n " +
        "precision highp float;" +
        "precision mediump int;"+
        "out vec4 fragColor;"+
		"uniform vec3 u_ld;" +
		"uniform vec3 u_kd;" +
		"uniform vec3 u_la;" +
		"uniform vec3 u_ka;" +
		"uniform vec3 u_ls;" +
		"uniform vec3 u_ks;" +
		"uniform float u_shininess;" +
		"uniform int u_lkeyispressed;" +
		"in vec3 tnorm;"+
		"in vec3 lightDirection;"+
		"in vec3 reflectionVector;"+
		"in vec3 viewerVector;"+
		"void main(void)" +
		"{" +
		"vec3 phongADSLight;"+
		"if(u_lkeyispressed==1)"+
		"{"+
		"vec3 tnorm_normalize=normalize(tnorm);"+
		"vec3 lightDirection_normalize=normalize(lightDirection);"+
		"vec3 reflectionVector_normalize=normalize(reflectionVector);"+
		"vec3 viewerVector_normalize=normalize(viewerVector);"+
		"float tn_dot_ld=max(dot(lightDirection_normalize,tnorm_normalize),0.0);"+
		"vec3 ambient=u_la*u_ka;"+
		"vec3 diffuse=u_ld*u_kd*tn_dot_ld;"+
		"vec3 specular=u_ls*u_ks*pow(max(dot(reflectionVector_normalize,viewerVector_normalize),0.0),u_shininess);"+
		"phongADSLight=ambient+diffuse+specular;"+
		"}"+
		"else"+
		"{"+
		"phongADSLight=vec3(1.0f,1.0f,1.0f);"+
		"}"+
		"fragColor=vec4(phongADSLight,1.0f);"+
		"} ";

    //Shader object and source code mapping
    gl.shaderSource(gFragementShaderObject_frag, fragementShaderSourceCode_frag);

    //Compile Shader
    gl.compileShader(gFragementShaderObject_frag);

    //Error Checking
    if (gl.getShaderParameter(gFragementShaderObject_frag, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(gFragementShaderObject_frag);
        if (error.length > 0) {
            alert(error+" FragementShaderObject");
            UnInitialize();
        }
    }


    //Program Object
    gShaderProgramObject_frag = gl.createProgram();
    //Attach Shaders
    gl.attachShader(gShaderProgramObject_frag, gVertexShaderObject_frag);
    gl.attachShader(gShaderProgramObject_frag, gFragementShaderObject_frag);

    //Prelinking
    //Binding to attribute
    gl.bindAttribLocation(gShaderProgramObject_frag,
        WebGLMacros.AMC_ATTRIBUTE_POSITION,
        "vPosition");
    gl.bindAttribLocation(gShaderProgramObject_frag,
        WebGLMacros.AMC_ATTRIBUTE_NORMAL,
        "vNormal");



    //Link Shader Program
    gl.linkProgram(gShaderProgramObject_frag);

    if (!gl.getProgramParameter(gShaderProgramObject_frag, gl.LINK_STATUS)) {
        var error = gl.getProgramInfoLog(gShaderProgramObject_frag);
        if (error.length > 0) {
            alert(error);
            UnInitialize();
        }
    }

    if(perVertexOrFragement==1){
        getUniformLocation(gShaderProgramObject_frag);
    }

       //Vertex Shader
    //Define Shader Object
    gVertexShaderObject_vert = gl.createShader(gl.VERTEX_SHADER);

    //Vertex Shader Code
    var vertextShaderSourceCode_vert =
        "#version 300 es " +
        "\n " +
        "precision mediump int;"+
        "in vec4 vPosition;" +
		"in vec3 vNormal;" +
		"uniform mat4 u_m_matrix;" +
		"uniform mat4 u_v_matrix;" +
		"uniform mat4 u_p_matrix;" +
		"uniform int u_lkeyispressed;" +
		"uniform vec3 u_ld;" +
		"uniform vec3 u_kd;" +
		"uniform vec3 u_la;" +
		"uniform vec3 u_ka;" +
		"uniform vec3 u_ls;" +
		"uniform vec3 u_ks;" +
		"uniform float u_shininess;" +
		"uniform vec4 u_lightposition;" +
		"out vec3 phongADSLight;" +
		"void main(void)" +
		"{" +
		 "gl_Position=u_p_matrix * u_v_matrix * u_m_matrix * vPosition;" +
		 "if(u_lkeyispressed==1)"+
		 "{"+
		 "vec4 eyecoordinates=u_v_matrix*u_m_matrix*vPosition;"+
		 "vec3 tnorm=normalize(mat3(u_v_matrix*u_m_matrix)*vNormal);"+
		 "vec3 lightDirection=normalize(vec3(u_lightposition-eyecoordinates));"+
		 "float tn_dot_ld=max(dot(lightDirection,tnorm),0.0);"+
		 "vec3 reflectionVector=reflect(-lightDirection,tnorm);"+
		 "vec3 viewerVector=normalize(vec3(-eyecoordinates));"+
		 "vec3 ambient=u_la*u_ka;"+
		 "vec3 diffuse = u_ld *u_kd * tn_dot_ld;"+
		 "vec3 specular=u_ls*u_ks*pow(max(dot(reflectionVector,viewerVector),0.0),u_shininess);"+
		 "phongADSLight=ambient+diffuse+specular;"+
		 "}"+
		 "else"+
		 "{"+
		 "phongADSLight=vec3(1.0f,1.0f,1.0f);"+
		 "}"+
		"}";

    //Shader object and source code mapping
    gl.shaderSource(gVertexShaderObject_vert, vertextShaderSourceCode_vert);

    //Compile Shader
    gl.compileShader(gVertexShaderObject_vert);

    //Error Checking
    if (gl.getShaderParameter(gVertexShaderObject_vert, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(gVertexShaderObject_vert);
        if (error.length > 0) {
            alert(error+" VertexShaderObject");
            UnInitialize();
        }
    }

    //Fragement Shader
    //Define Shader Object
    gFragementShaderObject_vert = gl.createShader(gl.FRAGMENT_SHADER);

    //Fragement Shader Code
    var fragementShaderSourceCode_vert =
        "#version 300 es " +
        "\n " +
        "precision highp float;" +
        "precision mediump int;"+
        "out vec4 fragColor;"+
		"in vec3 phongADSLight;" +
		"void main(void)" +
		"{" +
		"fragColor=vec4(phongADSLight,1.0f);"+
		"} ";
    //Shader object and source code mapping
    gl.shaderSource(gFragementShaderObject_vert, fragementShaderSourceCode_vert);

    //Compile Shader
    gl.compileShader(gFragementShaderObject_vert);

    //Error Checking
    if (gl.getShaderParameter(gFragementShaderObject_vert, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(gFragementShaderObject_vert);
        if (error.length > 0) {
            alert(error+" FragementShaderObject");
            UnInitialize();
        }
    }


    //Program Object
    gShaderProgramObject_vert = gl.createProgram();
    //Attach Shaders
    gl.attachShader(gShaderProgramObject_vert, gVertexShaderObject_vert);
    gl.attachShader(gShaderProgramObject_vert, gFragementShaderObject_vert);

    //Prelinking
    //Binding to attribute
    gl.bindAttribLocation(gShaderProgramObject_vert,
        WebGLMacros.AMC_ATTRIBUTE_POSITION,
        "vPosition");
    gl.bindAttribLocation(gShaderProgramObject_vert,
        WebGLMacros.AMC_ATTRIBUTE_NORMAL,
        "vNormal");

    //Link Shader Program
    gl.linkProgram(gShaderProgramObject_vert);

    if (!gl.getProgramParameter(gShaderProgramObject_vert, gl.LINK_STATUS)) {
        var error = gl.getProgramInfoLog(gShaderProgramObject_vert);
        if (error.length > 0) {
            alert(error);
            UnInitialize();
        }
    }

    if(perVertexOrFragement==0){
        getUniformLocation(gShaderProgramObject_vert);
    }

    gl.depthFunc(gl.LEQUAL);
    gl.enable(gl.DEPTH_TEST);
    
    sphere=new Mesh();
    //Sun.setColor(1.0,1.0,1.0);
    makeSphere(sphere,1.0, 30, 30,0.0,0.0,0.0);

	lightAmbient[0] = 0.0;
	lightAmbient[1] = 0.0;
	lightAmbient[2] = 0.0;
	lightAmbient[3] = 0.0;
	lightDiffuse[0] = 1.0;
	lightDiffuse[1] = 1.0;
	lightDiffuse[2] = 1.0;
	lightDiffuse[3] = 1.0;
	lightSpecular[0] = 1.0;
	lightSpecular[1] = 1.0;
	lightSpecular[2] = 1.0;
	lightSpecular[3] = 1.0;
	lightPosition[0] = 100.0;
	lightPosition[1] = 100.0;
	lightPosition[2] = 100.0;
	lightPosition[3] = 100.0;
	materialAmbient[0] = 0.0;
	materialAmbient[1] = 0.0;
	materialAmbient[2] = 0.0;
	materialAmbient[3] = 0.0;
	materialDiffuse[0] = 1.0;
	materialDiffuse[1] = 1.0;
	materialDiffuse[2] = 1.0;
	materialDiffuse[3] = 1.0;
	materialSpecular[0] = 1.0;
	materialSpecular[1] = 1.0;
	materialSpecular[2] = 1.0;
	materialSpecular[3] = 1.0;
	materialShininess = 50.0;

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

    if(perVertexOrFragement==1)
    gl.useProgram(gShaderProgramObject_frag);
    else
    gl.useProgram(gShaderProgramObject_vert);

    //Declaration of Matrices
    var modelMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();
    var viewMatrix = mat4.create();


    mat4.identity(modelMatrix);
    mat4.identity(viewMatrix);

    mat4.identity(modelViewProjectionMatrix);

    //Initialize matrices
    //Transformation

    mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -3.0]);
    // mat4.rotateX(modelMatrix, modelMatrix,degToRad(angleCube));
    // mat4.rotateY(modelMatrix, modelMatrix,degToRad(angleCube));
    // mat4.rotateZ(modelMatrix, modelMatrix,degToRad(angleCube));
    // mat4.scale(modelMatrix, modelMatrix, [0.75, 0.75, 0.75]);

    //mat4.multiply(modelMatrix,modelMatrix,1.0)

    sphere.draw();

    gl.uniformMatrix4fv(mUniform,
        false,
        modelMatrix);

    gl.uniformMatrix4fv(vUniform,
        false,
        viewMatrix);

    gl.uniformMatrix4fv(pUniform,
            false,
            perspectiveProjectionMatrix);
    
    if (gb_Lighting) {
                // gl.uniform1i(lKeyIsPressedUniform, gb_Lighting);
                // gl.uniform3f(ldUniform, 1.0, 1.0, 1.0);
                // gl.uniform3f(kdUniform, 0.5, 0.5, 0.5);
                // gl.uniform4f(lightPositionUniform, 0.0, 0.0, 2.0, 1.0);

                gl.uniform1i(lKeyIsPressedUniform, gb_Lighting);

                gl.uniform3fv(ldUniform, new Float32Array([lightDiffuse[0]
                                                            ,lightDiffuse[1]
                                                            ,lightDiffuse[2]]));
                gl.uniform3fv(laUniform, new Float32Array([lightAmbient[0],lightAmbient[1],lightAmbient[2]]));
                gl.uniform3fv(lsUniform, new Float32Array([lightSpecular[0],lightSpecular[1],lightSpecular[2]]));
        
                gl.uniform3fv(kaUniform, new Float32Array([materialAmbient[0],materialAmbient[1],materialAmbient[2]]));
                gl.uniform3fv(kdUniform, new Float32Array([materialDiffuse[0],materialDiffuse[1],materialDiffuse[2]]));
                gl.uniform3fv(ksUniform, new Float32Array([materialSpecular[0],materialSpecular[1],materialSpecular[2]]));
        
                gl.uniform1f(shininessUniform, materialShininess);
        
                gl.uniform4fv(lightPositionUniform, lightPosition);
        
    }
    else {
                gl.uniform1i(lKeyIsPressedUniform, gb_Lighting);
    }

    sphere.draw();  

    gl.useProgram(null);

    requestAnimationFrame(draw, canvas);
}

function UnInitialize() {
  

    if (gShaderProgramObject_frag) {
        if (gFragementShaderObject_frag) {
            gl.detachShader(gShaderProgramObject_frag, gFragementShaderObject_frag);
            gl.deleteShader(gFragementShaderObject_frag);
            gFragementShaderObject_frag = null;
        }
        if (gVertexShaderObject_frag) {
            gl.detachShader(gShaderProgramObject_frag, gVertexShaderObject_frag);
            gl.deleteShader(gVertexShaderObject_frag);
            gVertexShaderObject_frag = null;
        }
        gl.deleteProgram(gShaderProgramObject_frag);
        gShaderProgramObject_frag = null;
    }
}


function degToRad(degrees) {
    return (degrees * Math.PI / 180)
}


function getUniformLocation(gShaderProgramObject){
    mUniform = gl.getUniformLocation(gShaderProgramObject,
		"u_m_matrix");
	vUniform = gl.getUniformLocation(gShaderProgramObject,
		"u_v_matrix");
	pUniform = gl.getUniformLocation(gShaderProgramObject,
		"u_p_matrix");
	lKeyIsPressedUniform = gl.getUniformLocation(gShaderProgramObject,
		"u_lkeyispressed");
	ldUniform = gl.getUniformLocation(gShaderProgramObject,
		"u_ld");
	kdUniform = gl.getUniformLocation(gShaderProgramObject,
		"u_kd");
	laUniform = gl.getUniformLocation(gShaderProgramObject,
		"u_la");
	kaUniform = gl.getUniformLocation(gShaderProgramObject,
		"u_ka");
	lsUniform = gl.getUniformLocation(gShaderProgramObject,
		"u_ls");
	ksUniform = gl.getUniformLocation(gShaderProgramObject,
		"u_ks");
	lightPositionUniform = gl.getUniformLocation(gShaderProgramObject,
		"u_lightposition");
	shininessUniform = gl.getUniformLocation(gShaderProgramObject,
		"u_shininess");
}