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


var vao_cube;
var vbo_cube_position;
var vbo_cube_color;
var vbo_cube_texture;

var perspectiveProjectionMatrix;
var uniform_sampler;

var angle_rotate = 0.0;
var cube_texture;

var fbo;
var texturefbo;
var depthRenderBuffer;
var backingWidth, backing_height;
var fboShaderProgramObject;
var fboSamplerUniform;
var fbomvpUniform;

var gWidth, gHeight;

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
	var fboVertexShaderObject;
    var fboFragementShaderObject;

    gl = canvas.getContext("webgl2");
    if(gl == null)
    {
        console.log("Failed to get rendering context for WebGL!");
        return;
    }
    gl.viewportWidth = canvas.width;
    gl.viewportHeight = canvas.height;

    var vertexShaderSourceCode = 
    "#version 300 es"+
    "\n"+
    "in vec4 vPosition;"+
    "in vec2 vTexCoord;"+
    "uniform mat4 u_mvp_matrix;"+
    "out vec2 out_TexCoord;"+
    "void main(void)"+
    "{"+
        "out_TexCoord = vTexCoord;" +
        "gl_Position = u_mvp_matrix * vPosition;"+
    "}";
    fboVertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
    gl.shaderSource(fboVertexShaderObject, vertexShaderSourceCode);
    gl.compileShader(fboVertexShaderObject);
    if(gl.getShaderParameter(fboVertexShaderObject, gl.COMPILE_STATUS) == false)
    {
        var error = gl.getShaderInfoLog(fboVertexShaderObject);
        if(error.length > 0)
        {
            alert(error);
            console.log(error);
            UnInitialize();
        }
    }

    var fragmentShaderSourceCode = 
    "#version 300 es"+
    "\n"+
    "precision highp float;" +
    "in vec2 out_TexCoord;" +
    "uniform highp sampler2D u_sampler;"+
    "out vec4 FragColor;"+
    "void main(void)"+
    "{"+
        "FragColor = texture(u_sampler, out_TexCoord);" +
    "}";

    fboFragementShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
    gl.shaderSource(fboFragementShaderObject, fragmentShaderSourceCode);
    gl.compileShader(fboFragementShaderObject);
    if(gl.getShaderParameter(fboFragementShaderObject, gl.COMPILE_STATUS) == false);
    {
        var error = gl.getShaderInfoLog(fboFragementShaderObject);
        if(error.length > 0)
        {
            alert(error);
            UnInitialize();
        }
    }

    fboShaderProgramObject = gl.createProgram();
	gl.attachShader(fboShaderProgramObject, fboVertexShaderObject);
	gl.attachShader(fboShaderProgramObject, fboFragementShaderObject);
	
	gl.bindAttribLocation(fboShaderProgramObject, WebGLMicros.AMC_ATTRIBUTE_POSITION, "vPosition");
	gl.bindAttribLocation(fboShaderProgramObject, WebGLMicros.AMC_ATTRIBUTE_TEXCOORD0, "vTexCoord");

	gl.linkProgram(fboShaderProgramObject);
	if(!gl.getProgramParameter(fboShaderProgramObject, gl.LINK_STATUS))
	{
		var error = gl.getProgramInfoLog(fboShaderProgramObject);
		if(error.length > 0)
		{
			alert(error);
			UnInitialize();
		}
	}
    
	fbomvpUniform = gl.getUniformLocation(fboShaderProgramObject, "u_mvp_matrix");
    fboSamplerUniform = gl.getUniformLocation(fboShaderProgramObject, "u_sampler");
    
    cube_texture = gl.createTexture();
	cube_texture.image = new Image();
	cube_texture.image.src = "Vijay_Kundali.png";
	cube_texture.image.onload = function()
	{
	    gl.bindTexture(gl.TEXTURE_2D, cube_texture);
	    gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
	    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, cube_texture.image);
	    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
	    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
	    gl.bindTexture(gl.TEXTURE_2D, null);
    }
    
    var CubeVertices = new Float32Array([1.0, 1.0, -1.0, 
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


var CubeTexCoord = new Float32Array([0.0, 1.0,
									0.0, 0.0,
									1.0, 0.0,
									1.0, 1.0,

									1.0, 1.0,
									0.0, 1.0,
									0.0, 0.0,
									1.0, 0.0,

									0.0, 0.0,
									1.0, 0.0,
									1.0, 1.0,
									0.0, 1.0,

									0.0, 0.0,
									0.0, 1.0,
									1.0, 1.0,
									1.0, 0.0,

									1.0, 0.0,
									1.0, 1.0,
									0.0, 1.0,
									0.0, 0.0,

									0.0, 0.0,
									1.0, 0.0,
									1.0, 1.0,
									0.0, 1.0
									]);

	vao_cube = gl.createVertexArray();
	gl.bindVertexArray(vao_cube);


    
	vbo_cube_position = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, vbo_cube_position);
	gl.bufferData(gl.ARRAY_BUFFER, CubeVertices, gl.STATIC_DRAW);
	gl.vertexAttribPointer(WebGLMicros.AMC_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0 );
	gl.enableVertexAttribArray(WebGLMicros.AMC_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    vbo_cube_texture = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_cube_texture);
    gl.bufferData(gl.ARRAY_BUFFER,CubeTexCoord , gl.STATIC_DRAW);
    gl.vertexAttribPointer(WebGLMicros.AMC_ATTRIBUTE_TEXCOORD0, 2, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(WebGLMicros.AMC_ATTRIBUTE_TEXCOORD0);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

	gl.bindVertexArray(null);
    
    frameBufferObject = gl.createFramebuffer();
    gl.bindFramebuffer(gl.FRAMEBUFFER, frameBufferObject);
    textureFrameBuffer = gl.createTexture();
    gl.bindTexture(gl.TEXTURE_2D, textureFrameBuffer);
    gl.texStorage2D(gl.TEXTURE_2D, 1, gl.RGBA8, 1024, 1024);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_LINEAR);
    gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, textureFrameBuffer, 0);
    
    depthRenderBuffer = gl.createRenderbuffer();
    gl.bindRenderbuffer(gl.RENDERBUFFER, depthRenderBuffer);
    gl.renderbufferStorage(gl.RENDERBUFFER, gl.DEPTH_COMPONENT16, 1024, 1024);
    gl.framebufferRenderbuffer(gl.FRAMEBUFFER, gl.DEPTH_ATTACHMENT, gl.RENDERBUFFER, depthRenderBuffer);
    gl.bindTexture(gl.TEXTURE_2D, null);
    gl.bindRenderbuffer(gl.RENDERBUFFER, null);

    gl.bindFramebuffer(gl.FRAMEBUFFER, null);

    if(gl.checkFramebufferStatus(gl.FRAMEBUFFER) == gl.FRAMEBUFFER_COMPLETE)
    {
        console.log("Done with FBOfacilities");
    }
    else
    {
        console.log("FBO creation failed!!!\n");
    }
	gl.clearDepth(1.0);

	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);

    gl.clearColor(0.0, 0.0, 0.0, 1.0); 
	perspectiveProjectionMatrix = mat4.create();

}


function resize() {

    if (bFullScreen == true) 
    {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
    }
    else 
    {
        canvas.width = canvas_original_width;
        canvas.height = canvas_original_height;
    }
    gl.viewport(0, 0, canvas.width, canvas.height);
    
    gWidth = canvas.width;
    gHeight = canvas.height;

    mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 100);

}

function draw() 
{
  
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	gl.useProgram(fboShaderProgramObject);
	var modelViewMatrix = mat4.create();
	var modelViewProjection = mat4.create();
	var translationMatrix = mat4.create();
	var rotationMatrix = mat4.create();
	var scaleMatrix = mat4.create();
    
	gl.bindFramebuffer(gl.FRAMEBUFFER, frameBufferObject);
    gl.viewport(0, 0, 1024, 1024);
    gl.clearColor(0.5, 0.5, 0.5, 1.0);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	
	mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(scaleMatrix, scaleMatrix, [0.5, 0.5, 0.5]);
	mat4.rotateX(rotationMatrix, rotationMatrix, angle_rotate);
	mat4.multiply(modelViewMatrix, translationMatrix, scaleMatrix);
	mat4.multiply(modelViewMatrix, modelViewMatrix, rotationMatrix);

	mat4.rotateY(rotationMatrix, rotationMatrix, angle_rotate);
	mat4.multiply(modelViewMatrix, translationMatrix, rotationMatrix);

	mat4.rotateZ(rotationMatrix, rotationMatrix, angle_rotate);
	mat4.multiply(modelViewMatrix, translationMatrix, rotationMatrix);

	mat4.multiply(modelViewProjection, perspectiveProjectionMatrix, modelViewMatrix);

	gl.uniformMatrix4fv(fbomvpUniform, false, modelViewProjection);

    gl.bindTexture(gl.TEXTURE_2D, cube_texture);	
    gl.bindVertexArray(vao_cube);
    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.bindVertexArray(null);

    gl.bindFramebuffer(gl.FRAMEBUFFER, null);
  
	gl.useProgram(null);
	
	gl.useProgram(fboShaderProgramObject);


	//Render
    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    gl.viewport(0, 0, gWidth, gHeight);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    mat4.identity(modelViewMatrix);
    mat4.identity(modelViewProjection);
	mat4.identity(translationMatrix);
	mat4.identity(rotationMatrix);
	mat4.identity(scaleMatrix);
    
    mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -6.0]);
	mat4.scale(scaleMatrix, scaleMatrix, [0.5, 0.5, 0.5]);
	mat4.rotateX(rotationMatrix, rotationMatrix, angle_rotate);
	mat4.multiply(modelViewMatrix, translationMatrix, scaleMatrix);
	mat4.multiply(modelViewMatrix, modelViewMatrix, rotationMatrix);

	mat4.rotateY(rotationMatrix, rotationMatrix, angle_rotate);
	mat4.multiply(modelViewMatrix, translationMatrix, rotationMatrix);

	mat4.rotateZ(rotationMatrix, rotationMatrix, angle_rotate);
	mat4.multiply(modelViewMatrix, translationMatrix, rotationMatrix);

	mat4.multiply(modelViewProjection, perspectiveProjectionMatrix, modelViewMatrix);
	gl.uniformMatrix4fv(fbomvpUniform, false, modelViewProjection);

    gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D, textureFrameBuffer);
	gl.uniform1i(uniform_sampler, 0);
    gl.bindVertexArray(vao_cube);
    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);
    gl.bindTexture(gl.TEXTURE_2D, null);
    gl.bindVertexArray(null);

    gl.useProgram(null);

    requestAnimationFrame(draw, canvas);    

	angle_rotate += 0.005;
    if (angle_rotate >= 360.0)
        angle_rotate = 0.0;


}

function UnInitialize() {
  
   	if(fboShaderProgramObject)
	{
		if(fboFragementShaderObject)
		{
			gl.detachShader(fboShaderProgramObject, fboFragementShaderObject);
			gl.deleteShader(fboFragementShaderObject);
			fboFragementShaderObject = null;
		}
		
		if(fboVertexShaderObject)
		{
			gl.detachShader(fboShaderProgramObject, fboVertexShaderObject);
			gl.deleteShader(fboVertexShaderObject);
			fboVertexShaderObject = null;
		}
		
		gl.deleteProgram(fboShaderProgramObject);
		fboShaderProgramObject = null;
	}
}

function degToRad(degrees) {
    return (degrees * Math.PI / 180)
}