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

var gVertexShaderObject;
var gFragementShaderObject;
var gShaderProgramObject;

var keyPress=0;
var angleRotation = 0.0;

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
        case 88:
            keyPress = 1;
			lightPosition[0] = 0.0;
			lightPosition[1] = 0.0;
			lightPosition[2] = 0.0;

			angleRotation = 0.0;
            break;
            case 89:
                keyPress = 2;
                lightPosition[0] = 0.0;
                lightPosition[1] = 0.0;
                lightPosition[2] = 0.0;
    
                angleRotation = 0.0;
                break;
                case 90:
                    keyPress = 3;
                    lightPosition[0] = 0.0;
                    lightPosition[1] = 0.0;
                    lightPosition[2] = 0.0;
        
                    angleRotation = 0.0;
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
  
    gl.depthFunc(gl.LEQUAL);
    gl.enable(gl.DEPTH_TEST);
    
    sphere=new Mesh();
    //Sun.setColor(1.0,1.0,1.0);
    makeSphere(sphere,0.5, 30, 30,0.0,0.0,0.0);

	lightAmbient[0] = 0.0;
	lightAmbient[1] = 0.0;
	lightAmbient[2] = 0.0;
	lightAmbient[3] = 1.0;
	lightDiffuse[0] = 1.0;
	lightDiffuse[1] = 1.0;
	lightDiffuse[2] = 1.0;
	lightDiffuse[3] = 1.0;
	lightSpecular[0] = 0.0;
	lightSpecular[1] = 0.0;
	lightSpecular[2] = 0.0;
	lightSpecular[3] = 1.0;
	lightPosition[0] = 0.0;
	lightPosition[1] = 0.0;
	lightPosition[2] = 0.0;
	lightPosition[3] = 1.0;

    //gl.clearDepth(1.0);
    gl.clearColor(0.25, 0.25, 0.25, 1.0);

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
    var modelMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();
    var viewMatrix = mat4.create();

	// ***** 1st sphere on 1st column, emerald *****
    materialAmbient[0] = 0.0215; // r
	materialAmbient[1] = 0.1745; // g
	materialAmbient[2] = 0.0215; // b
	materialAmbient[3] = 1.0;   // a
	 
	// diffuse material
	materialDiffuse[0] = 0.07568; // r
	materialDiffuse[1] = 0.61424; // g
	materialDiffuse[2] = 0.07568; // b
	materialDiffuse[3] = 1.0;    // a
	 
	// specular material
	materialSpecular[0] = 0.633;    // r
	materialSpecular[1] = 0.727811; // g
	materialSpecular[2] = 0.633;    // b
	materialSpecular[3] = 1.0;     // a
	 
	// shininess
	materialShininess = 0.6 * 128;
	
    mat4.identity(modelMatrix);
    mat4.identity(viewMatrix);
    mat4.identity(modelViewProjectionMatrix);

    //Initialize matrices
    //Transformation
	gl.viewport(parseFloat(canvas.width)*(-0.45), parseFloat(canvas.height)*(0.35), parseFloat(canvas.width) , parseFloat(canvas.height));

    mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -12.0]);
 
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




	// ambient material
	materialAmbient[0] = 0.135;  // r
	materialAmbient[1] = 0.2225; // g
	materialAmbient[2] = 0.1575; // b
	materialAmbient[3] = 1.0;   // a
	
	// diffuse material
	materialDiffuse[0] = 0.54; // r
	materialDiffuse[1] = 0.89; // g
	materialDiffuse[2] = 0.63; // b
	materialDiffuse[3] = 1.0; // a
	 
	// specular material
	materialSpecular[0] = 0.316228; // r
	materialSpecular[1] = 0.316228; // g
	materialSpecular[2] = 0.316228; // b
	materialSpecular[3] = 1.0;     // a
	 
	// shininess
	materialShininess = 0.1 * 128;

	
    mat4.identity(modelMatrix);
    mat4.identity(viewMatrix);
    mat4.identity(modelViewProjectionMatrix);

    //Initialize matrices
    //Transformation
	gl.viewport(parseFloat(canvas.width)*(-0.45), parseFloat(canvas.height)*(0.20), parseFloat(canvas.width), parseFloat(canvas.height));

    mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -12.0]);
 
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



	materialAmbient[0] = 0.05375; // r
	materialAmbient[1] = 0.05;    // g
	materialAmbient[2] = 0.06625; // b
	materialAmbient[3] = 1.0;    // a
	 
	// diffuse material
	materialDiffuse[0] = 0.18275; // r
	materialDiffuse[1] = 0.17;    // g
	materialDiffuse[2] = 0.22525; // b
	materialDiffuse[3] = 1.0;    // a
	 
	// specular material
	materialSpecular[0] = 0.332741; // r
	materialSpecular[1] = 0.328634; // g
	materialSpecular[2] = 0.346435; // b
	materialSpecular[3] = 1.0;     // a
	 
	// shininess
	materialShininess = 0.3 * 128;

mat4.identity(modelMatrix);
mat4.identity(viewMatrix);
mat4.identity(modelViewProjectionMatrix);

//Initialize matrices
//Transformation
gl.viewport(parseFloat(canvas.width)*(-0.45), parseFloat(canvas.height)*(0.05), parseFloat(canvas.width), parseFloat(canvas.height));

mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -12.0]);

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



	// ambient material
	materialAmbient[0] = 0.25;    // r
	materialAmbient[1] = 0.20725; // g
	materialAmbient[2] = 0.20725; // b
	materialAmbient[3] = 1.0;    // a
	 
	// diffuse material
	materialDiffuse[0] = 1.0;   // r
	materialDiffuse[1] = 0.829; // g
	materialDiffuse[2] = 0.829; // b
	materialDiffuse[3] = 1.0;  // a
	 
	// specular material
	materialSpecular[0] = 0.296648; // r
	materialSpecular[1] = 0.296648; // g
	materialSpecular[2] = 0.296648; // b
	materialSpecular[3] = 1.0;     // a
	 
	// shininess
	materialShininess = 0.088 * 128;
	 
	 
mat4.identity(modelMatrix);
mat4.identity(viewMatrix);
mat4.identity(modelViewProjectionMatrix);

//Initialize matrices
//Transformation
gl.viewport(parseFloat(canvas.width)*(-0.45), parseFloat(canvas.height)*(-0.10), parseFloat(canvas.width), parseFloat(canvas.height));

mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -12.0]);

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




// ambient material
materialAmbient[0] = 0.1745;  // r
materialAmbient[1] = 0.01175; // g
materialAmbient[2] = 0.01175; // b
materialAmbient[3] = 1.0;    // a
 
// diffuse material
materialDiffuse[0] = 0.61424; // r
materialDiffuse[1] = 0.04136; // g
materialDiffuse[2] = 0.04136; // b
materialDiffuse[3] = 1.0;    // a
 
// specular material
materialSpecular[0] = 0.727811; // r
materialSpecular[1] = 0.626959; // g
materialSpecular[2] = 0.626959; // b
materialSpecular[3] = 1.0;     // a
 
// shininess
materialShininess = 0.6 * 128;
 
	 
mat4.identity(modelMatrix);
mat4.identity(viewMatrix);
mat4.identity(modelViewProjectionMatrix);

//Initialize matrices
//Transformation
gl.viewport(parseFloat(canvas.width)*(-0.45), parseFloat(canvas.height)*(-0.25), parseFloat(canvas.width), parseFloat(canvas.height));


mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -12.0]);

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




// ambient material
materialAmbient[0] = 0.1;     // r
materialAmbient[1] = 0.18725; // g
materialAmbient[2] = 0.1745;  // b
materialAmbient[3] = 1.0;    // a
 
// diffuse material
materialDiffuse[0] = 0.396;   // r
materialDiffuse[1] = 0.74151; // g
materialDiffuse[2] = 0.69102; // b
materialDiffuse[3] = 1.0;    // a
 
// specular material
materialSpecular[0] = 0.297254; // r
materialSpecular[1] = 0.30829;  // g
materialSpecular[2] = 0.306678; // b
materialSpecular[3] = 1.0;     // a
 
// shininess
materialShininess = 0.1 * 128;
 
 
	 
mat4.identity(modelMatrix);
mat4.identity(viewMatrix);
mat4.identity(modelViewProjectionMatrix);

//Initialize matrices
//Transformation
gl.viewport(parseFloat(canvas.width)*(-0.45), parseFloat(canvas.height)*(-0.40), parseFloat(canvas.width), parseFloat(canvas.height));

mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -12.0]);

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

gl.viewport(0, 0, parseFloat(canvas.width), parseFloat(canvas.height));


	// ***** 1st sphere on 2nd column, brass *****

// ambient material
materialAmbient[0] = 0.329412; // r
materialAmbient[1] = 0.223529; // g
materialAmbient[2] = 0.027451; // b
materialAmbient[3] = 1.0;     // a
 
// diffuse material
materialDiffuse[0] = 0.780392; // r
materialDiffuse[1] = 0.568627; // g
materialDiffuse[2] = 0.113725; // b
materialDiffuse[3] = 1.0;     // a
 
// specular material
materialSpecular[0] = 0.992157; // r
materialSpecular[1] = 0.941176; // g
materialSpecular[2] = 0.807843; // b
materialSpecular[3] = 1.0;     // a
 
// shininess
materialShininess = 0.21794872 * 128; 
 
	 
mat4.identity(modelMatrix);
mat4.identity(viewMatrix);
mat4.identity(modelViewProjectionMatrix);

//Initialize matrices
//Transformation
gl.viewport(parseFloat(canvas.width)*(-0.15), parseFloat(canvas.height)*(0.35), parseFloat(canvas.width), parseFloat(canvas.height));

mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -12.0]);

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


// ambient material
materialAmbient[0] = 0.2125; // r
materialAmbient[1] = 0.1275; // g
materialAmbient[2] = 0.054;  // b
materialAmbient[3] = 1.0;   // a
 
// diffuse material
materialDiffuse[0] = 0.714;   // r
materialDiffuse[1] = 0.4284;  // g
materialDiffuse[2] = 0.18144; // b
materialDiffuse[3] = 1.0;    // a
 
// specular material
materialSpecular[0] = 0.393548; // r
materialSpecular[1] = 0.271906; // g
materialSpecular[2] = 0.166721; // b
materialSpecular[3] = 1.0;     // a
 
// shininess
materialShininess = 0.2 * 128;
 

mat4.identity(modelMatrix);
mat4.identity(viewMatrix);
mat4.identity(modelViewProjectionMatrix);

//Initialize matrices
//Transformation

gl.viewport(parseFloat(canvas.width)*(-0.15), parseFloat(canvas.height)*(0.20), parseFloat(canvas.width), parseFloat(canvas.height));


mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -12.0]);

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



// ambient material
materialAmbient[0] = 0.25; // r
materialAmbient[1] = 0.25; // g
materialAmbient[2] = 0.25; // b
materialAmbient[3] = 1.0; // a
 
// diffuse material
materialDiffuse[0] = 0.4;  // r
materialDiffuse[1] = 0.4;  // g
materialDiffuse[2] = 0.4;  // b
materialDiffuse[3] = 1.0; // a
 
// specular material
materialSpecular[0] = 0.774597; // r
materialSpecular[1] = 0.774597; // g
materialSpecular[2] = 0.774597; // b
materialSpecular[3] = 1.0;     // a
 
// shininess
materialShininess = 0.6 * 128;
    
mat4.identity(modelMatrix);
mat4.identity(viewMatrix);
mat4.identity(modelViewProjectionMatrix);

//Initialize matrices
//Transformation
gl.viewport(parseFloat(canvas.width)*(-0.15), parseFloat(canvas.height)*(0.05), parseFloat(canvas.width), parseFloat(canvas.height));


mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -12.0]);

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




	// ambient material
	materialAmbient[0] = 0.19125; // r
	materialAmbient[1] = 0.0735;  // g
	materialAmbient[2] = 0.0225;  // b
	materialAmbient[3] = 1.0;    // a
	 
	// diffuse material
	materialDiffuse[0] = 0.7038;  // r
	materialDiffuse[1] = 0.27048; // g
	materialDiffuse[2] = 0.0828;  // b
	materialDiffuse[3] = 1.0;    // a
	 
	// specular material
	materialSpecular[0] = 0.256777; // r
	materialSpecular[1] = 0.137622; // g
	materialSpecular[2] = 0.086014; // b
	materialSpecular[3] = 1.0;     // a
	 
	// shininess
	materialShininess = 0.1 * 128;
    
mat4.identity(modelMatrix);
mat4.identity(viewMatrix);
mat4.identity(modelViewProjectionMatrix);

//Initialize matrices
//Transformation
gl.viewport(parseFloat(canvas.width)*(-0.15), parseFloat(canvas.height)*(-0.1), parseFloat(canvas.width), parseFloat(canvas.height));


mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -12.0]);

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



// ambient material
materialAmbient[0] = 0.24725; // r
materialAmbient[1] = 0.1995;  // g
materialAmbient[2] = 0.0745;  // b
materialAmbient[3] = 1.0;    // a
 
// diffuse material
materialDiffuse[0] = 0.75164; // r
materialDiffuse[1] = 0.60648; // g
materialDiffuse[2] = 0.22648; // b
materialDiffuse[3] = 1.0;    // a
 
// specular material
materialSpecular[0] = 0.628281; // r
materialSpecular[1] = 0.555802; // g
materialSpecular[2] = 0.366065; // b
materialSpecular[3] = 1.0;     // a
 
// shininess
materialShininess = 0.4 * 128;
 
	 
mat4.identity(modelMatrix);
mat4.identity(viewMatrix);
mat4.identity(modelViewProjectionMatrix);

//Initialize matrices
//Transformation
gl.viewport(parseFloat(canvas.width)*(-0.15), parseFloat(canvas.height)*(-0.25), parseFloat(canvas.width), parseFloat(canvas.height));


mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -12.0]);

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



// ambient material
materialAmbient[0] = 0.19225; // r
materialAmbient[1] = 0.19225; // g
materialAmbient[2] = 0.19225; // b
materialAmbient[3] = 1.0;    // a
 
// diffuse material
materialDiffuse[0] = 0.50754; // r
materialDiffuse[1] = 0.50754; // g
materialDiffuse[2] = 0.50754; // b
materialDiffuse[3] = 1.0;    // a
 
// specular material
materialSpecular[0] = 0.508273; // r
materialSpecular[1] = 0.508273; // g
materialSpecular[2] = 0.508273; // b
materialSpecular[3] = 1.0;     // a
 
// shininess
materialShininess = 0.4 * 128;
	 
mat4.identity(modelMatrix);
mat4.identity(viewMatrix);
mat4.identity(modelViewProjectionMatrix);

//Initialize matrices
//Transformation
gl.viewport(parseFloat(canvas.width)*(-0.15), parseFloat(canvas.height)*(-0.40), parseFloat(canvas.width), parseFloat(canvas.height));

mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -12.0]);

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

// ***** 1st sphere on 3rd column, black *****
	

    materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.0;  // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0; // a
	 
	// diffuse material
	materialDiffuse[0] = 0.1; // r
	materialDiffuse[1] = 0.1; // g
	materialDiffuse[2] = 0.1; // b
	materialDiffuse[3] = 1.0; // a
	 
	// specular material
	materialSpecular[0] = 0.50; // r
	materialSpecular[1] = 0.50; // g
	materialSpecular[2] = 0.50; // b
	materialSpecular[3] = 1.0; // a
	 
	// shininess
	materialShininess = 0.25 * 128;	 

mat4.identity(modelMatrix);
mat4.identity(viewMatrix);
mat4.identity(modelViewProjectionMatrix);

//Initialize matrices
//Transformation

gl.viewport(parseFloat(canvas.width)*(0.15), parseFloat(canvas.height)*(0.35), parseFloat(canvas.width), parseFloat(canvas.height));

mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -12.0]);

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


	// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.1;  // g
	materialAmbient[2] = 0.06; // b
	materialAmbient[3] = 1.0; // a
	 
	// diffuse material
	materialDiffuse[0] = 0.0;        // r
	materialDiffuse[1] = 0.50980392; // g
	materialDiffuse[2] = 0.50980392; // b
	materialDiffuse[3] = 1.0;       // a
	 
	// specular material
	materialSpecular[0] = 0.50196078; // r
	materialSpecular[1] = 0.50196078; // g
	materialSpecular[2] = 0.50196078; // b
	materialSpecular[3] = 1.0;       // a
	 
	// shininess
	materialShininess = 0.25 * 128;
	 
 

mat4.identity(modelMatrix);
mat4.identity(viewMatrix);
mat4.identity(modelViewProjectionMatrix);

//Initialize matrices
//Transformation

gl.viewport(parseFloat(canvas.width)*(0.15), parseFloat(canvas.height)*(0.20), parseFloat(canvas.width), parseFloat(canvas.height));

mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -12.0]);

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



// ambient material
materialAmbient[0] = 0.0;  // r
materialAmbient[1] = 0.0;  // g
materialAmbient[2] = 0.0;  // b
materialAmbient[3] = 1.0; // a
 
// diffuse material
materialDiffuse[0] = 0.1;  // r
materialDiffuse[1] = 0.35; // g
materialDiffuse[2] = 0.1;  // b
materialDiffuse[3] = 1.0; // a
 
// specular material
materialSpecular[0] = 0.45; // r
materialSpecular[1] = 0.55; // g
materialSpecular[2] = 0.45; // b
materialSpecular[3] = 1.0; // a
 
// shininess
materialShininess = 0.25 * 128;
 
    
mat4.identity(modelMatrix);
mat4.identity(viewMatrix);
mat4.identity(modelViewProjectionMatrix);

//Initialize matrices
//Transformation

gl.viewport(parseFloat(canvas.width)*(0.15), parseFloat(canvas.height)*(0.05), parseFloat(canvas.width), parseFloat(canvas.height));


mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -12.0]);

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




	// ambient material
	materialAmbient[0] = 0.0;  // r
	materialAmbient[1] = 0.0;  // g
	materialAmbient[2] = 0.0;  // b
	materialAmbient[3] = 1.0; // a
	 
	// diffuse material
	materialDiffuse[0] = 0.5;  // r
	materialDiffuse[1] = 0.0;  // g
	materialDiffuse[2] = 0.0;  // b
	materialDiffuse[3] = 1.0; // a
	 
	// specular material
	materialSpecular[0] = 0.7;  // r
	materialSpecular[1] = 0.6;  // g
	materialSpecular[2] = 0.6;  // b
	materialSpecular[3] = 1.0; // a
	 
	// shininess
	materialShininess = 0.25 * 128;
	 
mat4.identity(modelMatrix);
mat4.identity(viewMatrix);
mat4.identity(modelViewProjectionMatrix);

//Initialize matrices
//Transformation

gl.viewport(parseFloat(canvas.width)*(0.15), parseFloat(canvas.height)*(-0.10), parseFloat(canvas.width), parseFloat(canvas.height));


mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -12.0]);

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


// ambient material
materialAmbient[0] = 0.0;  // r
materialAmbient[1] = 0.0;  // g
materialAmbient[2] = 0.0;  // b
materialAmbient[3] = 1.0; // a
 
// diffuse material
materialDiffuse[0] = 0.55; // r
materialDiffuse[1] = 0.55; // g
materialDiffuse[2] = 0.55; // b
materialDiffuse[3] = 1.0; // a
 
// specular material
materialSpecular[0] = 0.70; // r
materialSpecular[1] = 0.70; // g
materialSpecular[2] = 0.70; // b
materialSpecular[3] = 1.0; // a
 
// shininess
materialShininess = 0.25 * 128;
 
	 
mat4.identity(modelMatrix);
mat4.identity(viewMatrix);
mat4.identity(modelViewProjectionMatrix);

//Initialize matrices
//Transformation
gl.viewport(parseFloat(canvas.width)*(0.15), parseFloat(canvas.height)*(-0.25), parseFloat(canvas.width), parseFloat(canvas.height));


mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -12.0]);

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


// ambient material
materialAmbient[0] = 0.0;  // r
materialAmbient[1] = 0.0;  // g
materialAmbient[2] = 0.0;  // b
materialAmbient[3] = 1.0; // a
 
// diffuse material
materialDiffuse[0] = 0.5;  // r
materialDiffuse[1] = 0.5;  // g
materialDiffuse[2] = 0.0;  // b
materialDiffuse[3] = 1.0; // a
 
// specular material
materialSpecular[0] = 0.60; // r
materialSpecular[1] = 0.60; // g
materialSpecular[2] = 0.50; // b
materialSpecular[3] = 1.0; // a
 
// shininess
materialShininess = 0.25 * 128;
	 
mat4.identity(modelMatrix);
mat4.identity(viewMatrix);
mat4.identity(modelViewProjectionMatrix);

//Initialize matrices
//Transformation

gl.viewport(parseFloat(canvas.width)*(0.15), parseFloat(canvas.height)*(-0.40), parseFloat(canvas.width), parseFloat(canvas.height));

mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -12.0]);

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


// ***** 1st sphere on 4th column, black *****

// ambient material
materialAmbient[0] = 0.02; // r
materialAmbient[1] = 0.02; // g
materialAmbient[2] = 0.02; // b
materialAmbient[3] = 1.0; // a
 
// diffuse material
materialDiffuse[0] = 0.1; // r
materialDiffuse[1] = 0.1; // g
materialDiffuse[2] = 0.1; // b
materialDiffuse[3] = 1.0; // a
 
// specular material
materialSpecular[0] = 0.4;  // r
materialSpecular[1] = 0.4;  // g
materialSpecular[2] = 0.4;  // b
materialSpecular[3] = 1.0; // a
 
// shininess
materialShininess = 0.078125 * 128;

mat4.identity(modelMatrix);
mat4.identity(viewMatrix);
mat4.identity(modelViewProjectionMatrix);

//Initialize matrices
//Transformation

gl.viewport(parseFloat(canvas.width)*(0.40), parseFloat(canvas.height)*(0.35), parseFloat(canvas.width), parseFloat(canvas.height));

mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -12.0]);

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

// ambient material
materialAmbient[0] = 0.0;  // r
materialAmbient[1] = 0.05; // g
materialAmbient[2] = 0.05; // b
materialAmbient[3] = 1.0; // a
 
// diffuse material
materialDiffuse[0] = 0.4;  // r
materialDiffuse[1] = 0.5;  // g
materialDiffuse[2] = 0.5;  // b
materialDiffuse[3] = 1.0; // a
 
// specular material
materialSpecular[0] = 0.04; // r
materialSpecular[1] = 0.7;  // g
materialSpecular[2] = 0.7;  // b
materialSpecular[3] = 1.0; // a
 
// shininess
materialShininess = 0.078125 * 128;
 
 


mat4.identity(modelMatrix);
mat4.identity(viewMatrix);
mat4.identity(modelViewProjectionMatrix);

//Initialize matrices
//Transformation

gl.viewport(parseFloat(canvas.width)*(0.40), parseFloat(canvas.height)*(0.20), parseFloat(canvas.width), parseFloat(canvas.height));

mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -12.0]);

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


// ambient material
materialAmbient[0] = 0.0;  // r
materialAmbient[1] = 0.05; // g
materialAmbient[2] = 0.0;  // b
materialAmbient[3] = 1.0; // a
 
// diffuse material
materialDiffuse[0] = 0.4;  // r
materialDiffuse[1] = 0.5;  // g
materialDiffuse[2] = 0.4;  // b
materialDiffuse[3] = 1.0; // a
 
// specular material
materialSpecular[0] = 0.04; // r
materialSpecular[1] = 0.7;  // g
materialSpecular[2] = 0.04; // b
materialSpecular[3] = 1.0; // a
 
// shininess
materialShininess = 0.078125 * 128;

mat4.identity(modelMatrix);
mat4.identity(viewMatrix);
mat4.identity(modelViewProjectionMatrix);

//Initialize matrices
//Transformation

gl.viewport(parseFloat(canvas.width)*(0.40), parseFloat(canvas.height)*(0.05), parseFloat(canvas.width), parseFloat(canvas.height));

mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -12.0]);

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




// ambient material
materialAmbient[0] = 0.05; // r
materialAmbient[1] = 0.0;  // g
materialAmbient[2] = 0.0;  // b
materialAmbient[3] = 1.0; // a
 
// diffuse material
materialDiffuse[0] = 0.5;  // r
materialDiffuse[1] = 0.4;  // g
materialDiffuse[2] = 0.4;  // b
materialDiffuse[3] = 1.0; // a
 
// specular material
materialSpecular[0] = 0.7;  // r
materialSpecular[1] = 0.04; // g
materialSpecular[2] = 0.04; // b
materialSpecular[3] = 1.0; // a
 
// shininess
materialShininess = 0.078125 * 128;

mat4.identity(modelMatrix);
mat4.identity(viewMatrix);
mat4.identity(modelViewProjectionMatrix);

//Initialize matrices
//Transformation

gl.viewport(parseFloat(canvas.width)*(0.40), parseFloat(canvas.height)*(-0.10), parseFloat(canvas.width), parseFloat(canvas.height));

mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -12.0]);

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

	// ambient material
	materialAmbient[0] = 0.05; // r
	materialAmbient[1] = 0.05; // g
	materialAmbient[2] = 0.05; // b
	materialAmbient[3] = 1.0; // a
	 
	// diffuse material
	materialDiffuse[0] = 0.5;  // r
	materialDiffuse[1] = 0.5;  // g
	materialDiffuse[2] = 0.5;  // b
	materialDiffuse[3] = 1.0; // a
	 
	// specular material
	materialSpecular[0] = 0.7;  // r
	materialSpecular[1] = 0.7;  // g
	materialSpecular[2] = 0.7;  // b
	materialSpecular[3] = 1.0; // a
	 
	// shininess
	materialShininess = 0.078125 * 128;
	 

 
mat4.identity(modelMatrix);
mat4.identity(viewMatrix);
mat4.identity(modelViewProjectionMatrix);

//Initialize matrices
//Transformation
gl.viewport(parseFloat(canvas.width)*(0.40), parseFloat(canvas.height)*(-0.25), parseFloat(canvas.width), parseFloat(canvas.height));

mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -12.0]);

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


// ambient material
materialAmbient[0] = 0.05; // r
materialAmbient[1] = 0.05; // g
materialAmbient[2] = 0.0;  // b
materialAmbient[3] = 1.0; // a
 
// diffuse material
materialDiffuse[0] = 0.5;  // r
materialDiffuse[1] = 0.5;  // g
materialDiffuse[2] = 0.4;  // b
materialDiffuse[3] = 1.0; // a
 
// specular material
materialSpecular[0] = 0.7;  // r
materialSpecular[1] = 0.7;  // g
materialSpecular[2] = 0.04; // b
materialSpecular[3] = 1.0; // a
 
// shininess
materialShininess = 0.078125 * 128;

mat4.identity(modelMatrix);
mat4.identity(viewMatrix);
mat4.identity(modelViewProjectionMatrix);

//Initialize matrices
//Transformation
gl.viewport(parseFloat(canvas.width)*(0.40), parseFloat(canvas.height)*(-0.40), parseFloat(canvas.width), parseFloat(canvas.height));

mat4.translate(modelMatrix, modelMatrix, [0.0, 0.0, -12.0]);

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

    Update();
}

function Update(){
	angleRotation = angleRotation + 0.01;
	if (angleRotation > 2 * 3.14159265)
		angleRotation = 0.0;

	if (keyPress == 1) {
		lightPosition[1] = 100 * Math.sin(angleRotation);
		lightPosition[2] = 100 * Math.cos(angleRotation);
	}
	if (keyPress == 2) {
		lightPosition[0] = 100 * Math.sin(angleRotation);
		lightPosition[2] = 100 * Math.cos(angleRotation);
	}
	if (keyPress == 3) {
		lightPosition[0] = 100 * Math.sin(angleRotation);
		lightPosition[1] = 100 * Math.cos(angleRotation);
	}
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