#include"SceneRTR.h"


int SceneRTR::InitScene()
{
	/*Write Vertex Shader code*/
	const GLchar* vertexShaderSourceCode =
	{
		"#version 430 core"\
		"\n"\
		"layout(location = 0)in vec4 position;"\
		"layout(location = 2)in vec3 normal;"\

		"out VS_OUT"\
		"{"\
			"vec3 N;"\
			"vec3 L;"\
			"vec3 V;"\
			"flat int material_index;"\
		"}vs_out;"\

		//position of light
		"uniform vec3 light_pos = vec3(0.0, 0.0, 0.0);"\
		"layout(binding = 0, std140)uniform TRANSFORM_BLOCK"\
		"{"\
			"mat4 mat_proj;"\
			"mat4 mat_view;"\
			"mat4 mat_model[1];"\
		"}transforms;"\

		"void main(void)"\
		"{"\
			"mat4 mat_mv = transforms.mat_view * transforms.mat_model[gl_InstanceID];"\
			"vec4 P = mat_mv * position;"\

			"vs_out.N = mat3(mat_mv) * normal;"\
			"vs_out.L = light_pos - P.xyz;"\
			"vs_out.V = -P.xyz;"\
			"vs_out.material_index = gl_InstanceID;"

			"gl_Position = transforms.mat_proj * P;"\
		"}"
	};

	//Write Fragment shader code
	const GLchar* fragmentShaderSourceCode=
	{
		"#version 430 core"\
		"\n"\
		"layout(location = 0)out vec4 color0;"\
		"layout(location = 1)out vec4 color1;"\

		"in VS_OUT"\
		"{"\
			"vec3 N;"\
			"vec3 L;"\
			"vec3 V;"\
			"flat int material_index;"\
		"}fs_in;"\

		//Material Properties
		"uniform float bloom_thresh_min = 0.8;"\
		"uniform float bloom_thresh_max = 1.2;"\

		"struct material_t"\
		"{"\
			"vec3 diffuse_color;"\
			"vec3 specular_color;"\
			"float specular_power;"\
			"vec3 ambient_color;"\
		"};"\

		"layout(binding = 1, std140)uniform MATERIAL_BLOCK"\
		"{"\
			"material_t material[1];"\
		"}materials;"\

		"void main(void)"\
		"{"\
			"vec3 N = normalize(fs_in.N);"\
			"vec3 L = normalize(fs_in.L);"\
			"vec3 V = normalize(fs_in.V);"\

			"vec3 R = reflect(-L, N);"\

			"material_t m = materials.material[fs_in.material_index];"\

			"vec3 diffuse = max(dot(N, L), 0.0) * m.diffuse_color;"\
			"vec3 specular = pow(max(dot(R, V), 0.0), m.specular_power) * m.specular_color;"\
			"vec3 ambient = m.ambient_color;"\

			"vec3 color = ambient + diffuse + specular;"\

			"color0 = vec4(color, 1.0);"\
			"float Y = dot(color, vec3(0.299, 0.587, 0.144));"\

			"color = color * 4.0 * smoothstep(bloom_thresh_min, bloom_thresh_max, Y);"\
			"color1 = vec4(color, 1.0);"\
		"}"
	};

	Shader objShader_RTRDisplay(true, vertexShaderSourceCode, fragmentShaderSourceCode);

	shaders.push_back(objShader_RTRDisplay);

	/*################# FILTER SHADER ##########################*/

	/*Write Vertex Shader code*/
	const GLchar* FiltervertexShaderSourceCode =
	{
		"#version 430 core"\
		"\n"\
		"void main(void)"\
		"{"\
			"const vec4 vertices[] = vec4[](vec4(-1.0, -1.0, 0.5, 1.0),"\
											"vec4(1.0, -1.0, 0.5, 1.0),"\
											"vec4(-1.0, 1.0, 0.5, 1.0),"\
											"vec4(1.0, 1.0, 0.5, 1.0));"\

			"gl_Position = vertices[gl_VertexID];"\
		"}"
	};

	//Write Fragment shader code
	const GLchar* FilterfragmentShaderSourceCode =
	{
		"#version 430 core"\
		"\n"\
		"layout (binding = 0) uniform sampler2D hdr_image;"\

		"out vec4 color;"\

		"const float weights[] = float[](0.0024499299678342,"\
		"0.0043538453346397,"\
		"0.0073599963704157,"\
		"0.0118349786570722,"\
		"0.0181026699707781,"\
		"0.0263392293891488,"\
		"0.0364543006660986,"\
		"0.0479932050577658,"\
		"0.0601029809166942,"\
		"0.0715974486241365,"\
		"0.0811305381519717,"\
		"0.0874493212267511,"\
		"0.0896631113333857,"\
		"0.0874493212267511,"\
		"0.0811305381519717,"\
		"0.0715974486241365,"\
		"0.0601029809166942,"\
		"0.0479932050577658,"\
		"0.0364543006660986,"\
		"0.0263392293891488,"\
		"0.0181026699707781,"\
		"0.0118349786570722,"\
		"0.0073599963704157,"\
		"0.0043538453346397,"\
		"0.0024499299678342);"\
		"void main(void)"\
		"{"\
			"vec4 c = vec4(0.0);"\
			"ivec2 P = ivec2(gl_FragCoord.yx) - ivec2(0, weights.length() >> 1);"\
			"int i;"\

			"for(i = 0; i < weights.length(); i++)"\
			"{"\
				"c += texelFetch(hdr_image, P + ivec2(0, i), 0) * weights[i];"\
			"}"\

			"color = c;"\
		"}"\
	};

	Shader objShader_RTRFilter(true, FiltervertexShaderSourceCode, FilterfragmentShaderSourceCode);

	shaders.push_back(objShader_RTRFilter);

	/*############## RESOLVE SHADER ########################*/

	/*Write Vertex Shader code*/
	const GLchar* ResolvevertexShaderSourceCode=
	{
		"#version 430 core"\
		"\n"\
		"void main(void)"\
		"{"\
			"const vec4 vertices[] = vec4[] (vec4(-1.0, -1.0, 0.5, 1.0),"\
											"vec4(1.0, -1.0, 0.5, 1.0),"\
											"vec4(-1.0, 1.0, 0.5, 1.0),"\
											"vec4(1.0, 1.0, 0.5, 1.0));"\

			"gl_Position = vertices[gl_VertexID];"\
		"}"\
	};

	const GLchar* ResolvefragmentShaderSourceCode =
	{
		"#version 430 core"\
		"\n"\

		"layout (binding = 0) uniform sampler2D hdr_image;"\
		"layout (binding = 1) uniform sampler2D bloom_image;"\

		"uniform float exposure = 1.0;"\
		"uniform float bloom_factor = 1.0;"\
		"uniform float scene_factor = 1.0;"\

		"out vec4 color;"\

		"void main(void)"\
		"{"\
			"vec4 c = vec4(0.0);"\

			"c += texelFetch(hdr_image, ivec2(gl_FragCoord.xy), 0) * scene_factor;"\
			"c += texelFetch(bloom_image, ivec2(gl_FragCoord.xy), 0) * bloom_factor;"\

			"c.rgb = vec3(1.0) - exp(-c.rgb * exposure);"\
			"color = c;"\
		"}"\
	};

	Shader objShader_RTRResolve(true, ResolvevertexShaderSourceCode, ResolvefragmentShaderSourceCode);
	shaders.push_back(objShader_RTRResolve);


	/*Framebuffer facilities*/
	static const GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	static const GLfloat exposureLUT[20] = { 11.0f, 6.0f, 3.2f, 2.8f, 2.2f, 1.90f, 1.80f, 1.80f, 1.70f, 1.70f,  1.60f, 1.60f, 1.50f, 1.50f, 1.40f, 1.40f, 1.30f, 1.20f, 1.10f, 1.00f };

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenFramebuffers(1, &render_fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, render_fbo);

	glGenTextures(1, &tex_scene);
	glBindTexture(GL_TEXTURE_2D, tex_scene);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex_scene, 0);

	glGenTextures(1, &tex_brightpass);
	glBindTexture(GL_TEXTURE_2D, tex_brightpass);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, tex_brightpass, 0);

	glGenTextures(1, &tex_depth);
	glBindTexture(GL_TEXTURE_2D, tex_depth);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, MAX_SCENE_WIDTH, MAX_SCENE_HEIGHT);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, tex_depth, 0);
	glDrawBuffers(2, buffers);

	glGenFramebuffers(2, &filter_fbo[0]);
	glGenTextures(2, &tex_filter[0]);
	for (int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, filter_fbo[i]);
		glBindTexture(GL_TEXTURE_2D, tex_filter[i]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, i ? MAX_SCENE_WIDTH : MAX_SCENE_HEIGHT, i ? MAX_SCENE_HEIGHT : MAX_SCENE_WIDTH);
		//glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16F, i ? MAX_SCENE_WIDTH : MAX_SCENE_WIDTH, i ? MAX_SCENE_HEIGHT : MAX_SCENE_HEIGHT);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex_filter[i], 0);
		glDrawBuffers(1, buffers);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenTextures(1, &tex_lut);
	glBindTexture(GL_TEXTURE_1D, tex_lut);
	glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, 20);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, 20, GL_RED, GL_FLOAT, exposureLUT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	/*Create vao of sphere*/
	glGenVertexArrays(1, &vao_rtr_rectangle);
	glBindVertexArray(vao_rtr_rectangle);
	glGenBuffers(1, &vbo_rtr_rectangle_position);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_rtr_rectangle_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 3, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	/*normal Buffer*/
	glGenBuffers(1, &vbo_rtr_rectangle_normal);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_rtr_rectangle_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 3, NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &ubo_transform);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_transform);
	glBufferData(GL_UNIFORM_BUFFER, (2 + SPHERE_COUNT) * sizeof(mat4), NULL, GL_DYNAMIC_DRAW);

	struct material
	{
		vmath::vec3 diffuse_color;
		unsigned int : 32;//pad
		vmath::vec3 specular_color;
		float specular_power;
		vmath::vec3 ambient_color;
		unsigned int : 32; //pad
	};

	glGenBuffers(1, &ubo_material);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_material);
	glBufferData(GL_UNIFORM_BUFFER, SPHERE_COUNT * sizeof(material), NULL, GL_DYNAMIC_DRAW);

	material *m = (material*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, SPHERE_COUNT * sizeof(material), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	float ambient = 0.002f;
	for (int i = 0; i < SPHERE_COUNT; i++)
	{
		float fi = float(M_PI * (float)i / 8.0f);
		m[i].diffuse_color = vec3(0.0f, 1.0f, 0.0f);
		m[i].specular_color = vec3(0.0f, 1.0f, 0.0f);
		m[i].specular_power = 40.0f;
		m[i].ambient_color = vec3(1.0f, 1.0f, 1.0f);
		ambient *= 1.5f;
	}
	glUnmapBuffer(GL_UNIFORM_BUFFER);

	rectangleVertices[0] = -1.5f;
	rectangleVertices[1] = 1.5f;
	rectangleVertices[2] = 0.0f;
	rectangleVertices[3] = -1.5f;
	rectangleVertices[4] = 0.8f;
	rectangleVertices[5] = 0.0f;

	rectangleVertices[6] = -1.5f;
	rectangleVertices[7] = 1.5f;
	rectangleVertices[8] = 0.0f;
	rectangleVertices[9] = -1.3f;
	rectangleVertices[10] = 1.5f;
	rectangleVertices[11] = 0.0f;

	rectangleVertices[12] = -1.5f;
	rectangleVertices[13] = 1.15f;
	rectangleVertices[14] = 0.0f;
	rectangleVertices[15] = -1.3f;
	rectangleVertices[16] = 1.15f;
	rectangleVertices[17] = 0.0f;

	rectangleVertices[18] = -1.3f;
	rectangleVertices[19] = 1.5f;
	rectangleVertices[20] = 0.0f;
	rectangleVertices[21] = -1.3f;
	rectangleVertices[22] = 1.15f;
	rectangleVertices[23] = 0.0f;

	rectangleVertices[24] = -1.5f;
	rectangleVertices[25] = 1.15f;
	rectangleVertices[26] = 0.0f;
	rectangleVertices[27] = -1.3f;
	rectangleVertices[28] = 0.8f;
	rectangleVertices[29] = 0.0f;

	/*T*/
	rectangleVertices[30] = -1.0f;
	rectangleVertices[31] = 1.5f;
	rectangleVertices[32] = 0.0f;
	rectangleVertices[33] = -1.0f;
	rectangleVertices[34] = 0.8f;
	rectangleVertices[35] = 0.0f;

	rectangleVertices[36] = -1.15f;
	rectangleVertices[37] = 1.5f;
	rectangleVertices[38] = 0.0f;
	rectangleVertices[39] = -0.85f;
	rectangleVertices[40] = 1.5f;
	rectangleVertices[41] = 0.0f;

	/*R*/
	rectangleVertices[42] = -0.7f;
	rectangleVertices[43] = 1.5f;
	rectangleVertices[44] = 0.0f;
	rectangleVertices[45] = -0.7f;
	rectangleVertices[46] = 0.8f;
	rectangleVertices[47] = 0.0f;

	rectangleVertices[48] = -0.7f;
	rectangleVertices[49] = 1.5f;
	rectangleVertices[50] = 0.0f;
	rectangleVertices[51] = -0.5f;
	rectangleVertices[52] = 1.5f;
	rectangleVertices[53] = 0.0f;

	rectangleVertices[54] = -0.7f;
	rectangleVertices[55] = 1.15f;
	rectangleVertices[56] = 0.0f;
	rectangleVertices[57] = -0.5f;
	rectangleVertices[58] = 1.15f;
	rectangleVertices[59] = 0.0f;

	rectangleVertices[60] = -0.5f;
	rectangleVertices[61] = 1.5f;
	rectangleVertices[62] = 0.0f;
	rectangleVertices[63] = -0.5f;
	rectangleVertices[64] = 1.15f;
	rectangleVertices[65] = 0.0f;

	rectangleVertices[66] = -0.7f;
	rectangleVertices[67] = 1.15f;
	rectangleVertices[68] = 0.0f;
	rectangleVertices[69] = -0.5f;
	rectangleVertices[70] = 0.8f;
	rectangleVertices[71] = 0.0f;

	/*2*/
	rectangleVertices[72] = -0.3f;
	rectangleVertices[73] = 1.5f;
	rectangleVertices[74] = 0.0f;
	rectangleVertices[75] = -0.1f;
	rectangleVertices[76] = 1.5f;
	rectangleVertices[77] = 0.0f;

	rectangleVertices[78] = -0.1f;
	rectangleVertices[79] = 1.5f;
	rectangleVertices[80] = 0.0f;
	rectangleVertices[81] = -0.1f;
	rectangleVertices[82] = 1.15f;
	rectangleVertices[83] = 0.0f;

	rectangleVertices[84] = -0.1f;
	rectangleVertices[85] = 1.15f;
	rectangleVertices[86] = 0.0f;
	rectangleVertices[87] = -0.3f;
	rectangleVertices[88] = 1.15f;
	rectangleVertices[89] = 0.0f;

	rectangleVertices[90] = -0.3f;
	rectangleVertices[91] = 1.15f;
	rectangleVertices[92] = 0.0f;
	rectangleVertices[93] = -0.3f;
	rectangleVertices[94] = 0.8f;
	rectangleVertices[95] = 0.0f;

	rectangleVertices[96] = -0.3f;
	rectangleVertices[97] = 0.8f;
	rectangleVertices[98] = 0.0f;
	rectangleVertices[99] = -0.1f;
	rectangleVertices[100] = 0.8f;
	rectangleVertices[101] = 0.0f;

	/*0*/
	rectangleVertices[102] = 0.1f;
	rectangleVertices[103] = 1.5f;
	rectangleVertices[104] = 0.0f;
	rectangleVertices[105] = 0.1f;
	rectangleVertices[106] = 0.8f;
	rectangleVertices[107] = 0.0f;

	rectangleVertices[108] = 0.1f;
	rectangleVertices[109] = 1.5f;
	rectangleVertices[110] = 0.0f;
	rectangleVertices[111] = 0.3f;
	rectangleVertices[112] = 1.5f;
	rectangleVertices[113] = 0.0f;

	rectangleVertices[114] = 0.3f;
	rectangleVertices[115] = 1.5f;
	rectangleVertices[116] = 0.0f;
	rectangleVertices[117] = 0.3f;
	rectangleVertices[118] = 0.8f;
	rectangleVertices[119] = 0.0f;

	rectangleVertices[120] = 0.1f;
	rectangleVertices[121] = 0.8f;
	rectangleVertices[122] = 0.0f;
	rectangleVertices[123] = 0.3f;
	rectangleVertices[124] = 0.8f;
	rectangleVertices[125] = 0.0f;

	/*1*/
	rectangleVertices[126] = 0.5f;
	rectangleVertices[127] = 1.5f;
	rectangleVertices[128] = 0.0f;
	rectangleVertices[129] = 0.5f;
	rectangleVertices[130] = 0.8f;
	rectangleVertices[131] = 0.0f;

	/*8*/
	rectangleVertices[132] = 0.7f;
	rectangleVertices[133] = 1.5f;
	rectangleVertices[134] = 0.0f;
	rectangleVertices[135] = 0.7f;
	rectangleVertices[136] = 0.8f;
	rectangleVertices[137] = 0.0f;

	rectangleVertices[138] = 0.7f;
	rectangleVertices[139] = 1.5f;
	rectangleVertices[140] = 0.0f;
	rectangleVertices[141] = 0.9f;
	rectangleVertices[142] = 1.5f;
	rectangleVertices[143] = 0.0f;

	rectangleVertices[144] = 0.9f;
	rectangleVertices[145] = 1.5f;
	rectangleVertices[146] = 0.0f;
	rectangleVertices[147] = 0.9f;
	rectangleVertices[148] = 0.8f;
	rectangleVertices[149] = 0.0f;

	rectangleVertices[150] = 0.9f;
	rectangleVertices[151] = 0.8f;
	rectangleVertices[152] = 0.0f;
	rectangleVertices[153] = 0.7f;
	rectangleVertices[154] = 0.8f;
	rectangleVertices[155] = 0.0f;

	rectangleVertices[156] = 0.7f;
	rectangleVertices[157] = 1.15f;
	rectangleVertices[158] = 0.0f;
	rectangleVertices[159] = 0.9f;
	rectangleVertices[160] = 1.15f;
	rectangleVertices[161] = 0.0f;

	/*H*/
	rectangleVertices[162] = -2.7f;
	rectangleVertices[163] = 0.5f;
	rectangleVertices[164] = 0.0f;
	rectangleVertices[165] = -2.7f;
	rectangleVertices[166] = -0.2f;
	rectangleVertices[167] = 0.0f;

	rectangleVertices[168] = -2.4f;
	rectangleVertices[169] = 0.5f;
	rectangleVertices[170] = 0.0f;
	rectangleVertices[171] = -2.4f;
	rectangleVertices[172] = -0.2f;
	rectangleVertices[173] = 0.0f;

	rectangleVertices[174] = -2.7f;
	rectangleVertices[175] = 0.15f;
	rectangleVertices[176] = 0.0f;
	rectangleVertices[177] = -2.4f;
	rectangleVertices[178] = 0.15f;
	rectangleVertices[179] = 0.0f;

	/*U*/
	rectangleVertices[180] = -2.2f;
	rectangleVertices[181] = 0.5f;
	rectangleVertices[182] = 0.0f;
	rectangleVertices[183] = -2.2f;
	rectangleVertices[184] = -0.2f;
	rectangleVertices[185] = 0.0f;

	rectangleVertices[186] = -2.2f;
	rectangleVertices[187] = -0.19f;
	rectangleVertices[188] = 0.0f;
	rectangleVertices[189] = -1.9f;
	rectangleVertices[190] = -0.19f;
	rectangleVertices[191] = 0.0f;

	rectangleVertices[192] = -1.9f;
	rectangleVertices[193] = 0.5f;
	rectangleVertices[194] = 0.0f;
	rectangleVertices[195] = -1.9f;
	rectangleVertices[196] = -0.2f;
	rectangleVertices[197] = 0.0f;

	/*L*/
	rectangleVertices[198] = -1.7f;
	rectangleVertices[199] = 0.5f;
	rectangleVertices[200] = 0.0f;
	rectangleVertices[201] = -1.7f;
	rectangleVertices[202] = -0.2f;
	rectangleVertices[203] = 0.0f;

	rectangleVertices[204] = -1.7f;
	rectangleVertices[205] = -0.2f;
	rectangleVertices[206] = 0.0f;
	rectangleVertices[207] = -1.5f;
	rectangleVertices[208] = -0.2f;
	rectangleVertices[209] = 0.0f;

	/*L*/
	rectangleVertices[210] = -1.3f;
	rectangleVertices[211] = 0.5f;
	rectangleVertices[212] = 0.0f;
	rectangleVertices[213] = -1.3f;
	rectangleVertices[214] = -0.2f;
	rectangleVertices[215] = 0.0f;

	rectangleVertices[216] = -1.3f;
	rectangleVertices[217] = -0.2f;
	rectangleVertices[218] = 0.0f;
	rectangleVertices[219] = -1.1f;
	rectangleVertices[220] = -0.2f;
	rectangleVertices[221] = 0.0f;

	/*S*/
	rectangleVertices[222] = -0.8f;
	rectangleVertices[223] = 0.5f;
	rectangleVertices[224] = 0.0f;
	rectangleVertices[225] = -0.6f;
	rectangleVertices[226] = 0.5f;
	rectangleVertices[227] = 0.0f;

	rectangleVertices[228] = -0.8f;
	rectangleVertices[229] = 0.5f;
	rectangleVertices[230] = 0.0f;
	rectangleVertices[231] = -0.8f;
	rectangleVertices[232] = 0.15f;
	rectangleVertices[233] = 0.0f;

	rectangleVertices[234] = -0.6f;
	rectangleVertices[235] = 0.15f;
	rectangleVertices[236] = 0.0f;
	rectangleVertices[237] = -0.8f;
	rectangleVertices[238] = 0.15f;
	rectangleVertices[239] = 0.0f;

	rectangleVertices[240] = -0.6f;
	rectangleVertices[241] = 0.15f;
	rectangleVertices[242] = 0.0f;
	rectangleVertices[243] = -0.6f;
	rectangleVertices[244] = -0.2f;
	rectangleVertices[245] = 0.0f;

	rectangleVertices[246] = -0.6f;
	rectangleVertices[247] = -0.2f;
	rectangleVertices[248] = 0.0f;
	rectangleVertices[249] = -0.8f;
	rectangleVertices[250] = -0.2f;
	rectangleVertices[251] = 0.0f;

	/*H*/
	rectangleVertices[252] = -0.4f;
	rectangleVertices[253] = 0.5f;
	rectangleVertices[254] = 0.0f;
	rectangleVertices[255] = -0.4f;
	rectangleVertices[256] = -0.2f;
	rectangleVertices[257] = 0.0f;

	rectangleVertices[258] = -0.1f;
	rectangleVertices[259] = 0.5f;
	rectangleVertices[260] = 0.0f;
	rectangleVertices[261] = -0.1f;
	rectangleVertices[262] = -0.2f;
	rectangleVertices[263] = 0.0f;

	rectangleVertices[264] = -0.1f;
	rectangleVertices[265] = 0.15f;
	rectangleVertices[266] = 0.0f;
	rectangleVertices[267] = -0.4f;
	rectangleVertices[268] = 0.15f;
	rectangleVertices[269] = 0.0f;

	/*A*/
	rectangleVertices[270] = 0.1f;
	rectangleVertices[271] = -0.2f;
	rectangleVertices[272] = 0.0f;
	rectangleVertices[273] = 0.1f;
	rectangleVertices[274] = 0.5f;
	rectangleVertices[275] = 0.0f;

	rectangleVertices[276] = 0.4f;
	rectangleVertices[277] = 0.5f;
	rectangleVertices[278] = 0.0f;
	rectangleVertices[279] = 0.4f;
	rectangleVertices[280] = -0.2f;
	rectangleVertices[281] = 0.0f;

	rectangleVertices[282] = 0.1f;
	rectangleVertices[283] = 0.5f;
	rectangleVertices[284] = 0.0f;
	rectangleVertices[285] = 0.4f;
	rectangleVertices[286] = 0.5f;
	rectangleVertices[287] = 0.0f;

	rectangleVertices[288] = 0.1f;
	rectangleVertices[289] = 0.15f;
	rectangleVertices[290] = 0.0f;
	rectangleVertices[291] = 0.4f;
	rectangleVertices[292] = 0.15f;
	rectangleVertices[293] = 0.0f;

	/*D*/
	rectangleVertices[294] = 0.7f;
	rectangleVertices[295] = 0.5f;
	rectangleVertices[296] = 0.0f;
	rectangleVertices[297] = 0.7f;
	rectangleVertices[298] = -0.2f;
	rectangleVertices[299] = 0.0f;

	rectangleVertices[300] = 1.0f;
	rectangleVertices[301] = 0.5f;
	rectangleVertices[302] = 0.0f;
	rectangleVertices[303] = 1.0f;
	rectangleVertices[304] = -0.2f;
	rectangleVertices[305] = 0.0f;

	rectangleVertices[306] = 0.6f;
	rectangleVertices[307] = 0.5f;
	rectangleVertices[308] = 0.0f;
	rectangleVertices[309] = 1.0f;
	rectangleVertices[310] = 0.5f;
	rectangleVertices[311] = 0.0f;

	rectangleVertices[312] = 0.6f;
	rectangleVertices[313] = -0.2f;
	rectangleVertices[314] = 0.0f;
	rectangleVertices[315] = 1.0f;
	rectangleVertices[316] = -0.2f;
	rectangleVertices[317] = 0.0f;

	/*E*/
	rectangleVertices[318] = 1.2f;
	rectangleVertices[319] = 0.5f;
	rectangleVertices[320] = 0.0f;
	rectangleVertices[321] = 1.2f;
	rectangleVertices[322] = -0.2f;
	rectangleVertices[323] = 0.0f;

	rectangleVertices[324] = 1.2f;
	rectangleVertices[325] = 0.5f;
	rectangleVertices[326] = 0.0f;
	rectangleVertices[327] = 1.5f;
	rectangleVertices[328] = 0.5f;
	rectangleVertices[329] = 0.0f;

	rectangleVertices[330] = 1.2f;
	rectangleVertices[331] = 0.15f;
	rectangleVertices[332] = 0.0f;
	rectangleVertices[333] = 1.5f;
	rectangleVertices[334] = 0.15f;
	rectangleVertices[335] = 0.0f;

	rectangleVertices[336] = 1.2f;
	rectangleVertices[337] = -0.2f;
	rectangleVertices[338] = 0.0f;
	rectangleVertices[339] = 1.5f;
	rectangleVertices[340] = -0.2f;
	rectangleVertices[341] = 0.0f;

	/*R*/
	rectangleVertices[342] = 1.7f;
	rectangleVertices[343] = 0.5f;
	rectangleVertices[344] = 0.0f;
	rectangleVertices[345] = 1.7f;
	rectangleVertices[346] = -0.2f;
	rectangleVertices[347] = 0.0f;

	rectangleVertices[348] = 1.7f;
	rectangleVertices[349] = 0.5f;
	rectangleVertices[350] = 0.0f;
	rectangleVertices[351] = 2.0f;
	rectangleVertices[352] = 0.5f;
	rectangleVertices[353] = 0.0f;

	rectangleVertices[354] = 2.0f;
	rectangleVertices[355] = 0.5f;
	rectangleVertices[356] = 0.0f;
	rectangleVertices[357] = 2.0f;
	rectangleVertices[358] = 0.15f;
	rectangleVertices[359] = 0.0f;

	rectangleVertices[360] = 2.0f;
	rectangleVertices[361] = 0.15f;
	rectangleVertices[362] = 0.0f;
	rectangleVertices[363] = 1.7f;
	rectangleVertices[364] = 0.15f;
	rectangleVertices[365] = 0.0f;

	rectangleVertices[366] = 1.7f;
	rectangleVertices[367] = 0.15f;
	rectangleVertices[368] = 0.0f;
	rectangleVertices[369] = 2.0f;
	rectangleVertices[370] = -0.2f;
	rectangleVertices[371] = 0.0f;

	rectangleNormal[0] = 0.0f;
	rectangleNormal[1] = 0.0f;
	rectangleNormal[2] = 1.0f;
	rectangleNormal[3] = 0.0f;
	rectangleNormal[4] = 0.0f;
	rectangleNormal[5] = 1.0f;

	rectangleNormal[6] = 0.0f;
	rectangleNormal[7] = 0.0f;
	rectangleNormal[8] = 1.0f;
	rectangleNormal[9] = 0.0f;
	rectangleNormal[10] = 0.0f;
	rectangleNormal[11] = 1.0f;

	rectangleNormal[12] = 0.0f;
	rectangleNormal[13] = 0.0f;
	rectangleNormal[14] = 1.0f;
	rectangleNormal[15] = 0.0f;
	rectangleNormal[16] = 0.0f;
	rectangleNormal[17] = 1.0f;

	rectangleNormal[18] = 0.0f;
	rectangleNormal[19] = 0.0f;
	rectangleNormal[20] = 1.0f;
	rectangleNormal[21] = 0.0f;
	rectangleNormal[22] = 0.0f;
	rectangleNormal[23] = 1.0f;

	rectangleNormal[24] = 0.0f;
	rectangleNormal[25] = 0.0f;
	rectangleNormal[26] = 1.0f;
	rectangleNormal[27] = 0.0f;
	rectangleNormal[28] = 0.0f;
	rectangleNormal[29] = 1.0f;

	rectangleNormal[30] = 0.0f;
	rectangleNormal[31] = 0.0f;
	rectangleNormal[32] = 1.0f;
	rectangleNormal[33] = 0.0f;
	rectangleNormal[34] = 0.0f;
	rectangleNormal[35] = 1.0f;

	rectangleNormal[36] = 0.0f;
	rectangleNormal[37] = 0.0f;
	rectangleNormal[38] = 1.0f;
	rectangleNormal[39] = 0.0f;
	rectangleNormal[40] = 0.0f;
	rectangleNormal[41] = 1.0f;

	rectangleNormal[42] = 0.0f;
	rectangleNormal[43] = 0.0f;
	rectangleNormal[44] = 1.0f;
	rectangleNormal[45] = 0.0f;
	rectangleNormal[46] = 0.0f;
	rectangleNormal[47] = 1.0f;

	rectangleNormal[48] = 0.0f;
	rectangleNormal[49] = 0.0f;
	rectangleNormal[50] = 1.0f;
	rectangleNormal[51] = 0.0f;
	rectangleNormal[52] = 0.0f;
	rectangleNormal[53] = 1.0f;

	rectangleNormal[54] = 0.0f;
	rectangleNormal[55] = 0.0f;
	rectangleNormal[56] = 1.0f;
	rectangleNormal[57] = 0.0f;
	rectangleNormal[58] = 0.0f;
	rectangleNormal[59] = 1.0f;

	rectangleNormal[60] = 0.0f;
	rectangleNormal[61] = 0.0f;
	rectangleNormal[62] = 1.0f;
	rectangleNormal[63] = 0.0f;
	rectangleNormal[64] = 0.0f;
	rectangleNormal[65] = 1.0f;

	rectangleNormal[66] = 0.0f;
	rectangleNormal[67] = 0.0f;
	rectangleNormal[68] = 1.0f;
	rectangleNormal[69] = 0.0f;
	rectangleNormal[70] = 0.0f;
	rectangleNormal[71] = 1.0f;

	rectangleNormal[72] = 0.0f;
	rectangleNormal[73] = 0.0f;
	rectangleNormal[74] = 1.0f;
	rectangleNormal[75] = 0.0f;
	rectangleNormal[76] = 0.0f;
	rectangleNormal[77] = 1.0f;

	rectangleNormal[78] = 0.0f;
	rectangleNormal[79] = 0.0f;
	rectangleNormal[80] = 1.0f;
	rectangleNormal[81] = 0.0f;
	rectangleNormal[82] = 0.0f;
	rectangleNormal[83] = 1.0f;

	rectangleNormal[84] = 0.0f;
	rectangleNormal[85] = 0.0f;
	rectangleNormal[86] = 1.0f;
	rectangleNormal[87] = 0.0f;
	rectangleNormal[88] = 0.0f;
	rectangleNormal[89] = 1.0f;

	rectangleNormal[90] = 0.0f;
	rectangleNormal[91] = 0.0f;
	rectangleNormal[92] = 1.0f;
	rectangleNormal[93] = 0.0f;
	rectangleNormal[94] = 0.0f;
	rectangleNormal[95] = 1.0f;

	rectangleNormal[96] = 0.0f;
	rectangleNormal[97] = 0.0f;
	rectangleNormal[98] = 1.0f;
	rectangleNormal[99] = 0.0f;
	rectangleNormal[100] = 0.0f;
	rectangleNormal[101] = 1.0f;

	rectangleNormal[102] = 0.0f;
	rectangleNormal[103] = 0.0f;
	rectangleNormal[104] = 1.0f;
	rectangleNormal[105] = 0.0f;
	rectangleNormal[106] = 0.0f;
	rectangleNormal[107] = 1.0f;

	rectangleNormal[108] = 0.0f;
	rectangleNormal[109] = 0.0f;
	rectangleNormal[110] = 1.0f;
	rectangleNormal[111] = 0.0f;
	rectangleNormal[112] = 0.0f;
	rectangleNormal[113] = 1.0f;

	rectangleNormal[114] = 0.0f;
	rectangleNormal[115] = 0.0f;
	rectangleNormal[116] = 1.0f;
	rectangleNormal[117] = 0.0f;
	rectangleNormal[118] = 0.0f;
	rectangleNormal[119] = 1.0f;

	rectangleNormal[120] = 0.0f;
	rectangleNormal[121] = 0.0f;
	rectangleNormal[122] = 1.0f;
	rectangleNormal[123] = 0.0f;
	rectangleNormal[124] = 0.0f;
	rectangleNormal[125] = 1.0f;

	rectangleNormal[126] = 0.0f;
	rectangleNormal[127] = 0.0f;
	rectangleNormal[128] = 1.0f;
	rectangleNormal[129] = 0.0f;
	rectangleNormal[130] = 0.0f;
	rectangleNormal[131] = 1.0f;

	rectangleNormal[132] = 0.0f;
	rectangleNormal[133] = 0.0f;
	rectangleNormal[134] = 1.0f;
	rectangleNormal[135] = 0.0f;
	rectangleNormal[136] = 0.0f;
	rectangleNormal[137] = 1.0f;

	rectangleNormal[138] = 0.0f;
	rectangleNormal[139] = 0.0f;
	rectangleNormal[140] = 1.0f;
	rectangleNormal[141] = 0.0f;
	rectangleNormal[142] = 0.0f;
	rectangleNormal[143] = 1.0f;

	rectangleNormal[144] = 0.0f;
	rectangleNormal[145] = 0.0f;
	rectangleNormal[146] = 1.0f;
	rectangleNormal[147] = 0.0f;
	rectangleNormal[148] = 0.0f;
	rectangleNormal[149] = 1.0f;

	rectangleNormal[150] = 0.0f;
	rectangleNormal[151] = 0.0f;
	rectangleNormal[152] = 1.0f;
	rectangleNormal[153] = 0.0f;
	rectangleNormal[154] = 0.0f;
	rectangleNormal[155] = 1.0f;

	rectangleNormal[156] = 0.0f;
	rectangleNormal[157] = 0.0f;
	rectangleNormal[158] = 1.0f;
	rectangleNormal[159] = 0.0f;
	rectangleNormal[160] = 0.0f;
	rectangleNormal[161] = 1.0f;

	rectangleNormal[162] = 0.0f;
	rectangleNormal[163] = 0.0f;
	rectangleNormal[164] = 1.0f;
	rectangleNormal[165] = 0.0f;
	rectangleNormal[166] = 0.0f;
	rectangleNormal[167] = 1.0f;

	rectangleNormal[168] = 0.0f;
	rectangleNormal[169] = 0.0f;
	rectangleNormal[170] = 1.0f;
	rectangleNormal[171] = 0.0f;
	rectangleNormal[172] = 0.0f;
	rectangleNormal[173] = 1.0f;

	rectangleNormal[174] = 0.0f;
	rectangleNormal[175] = 0.0f;
	rectangleNormal[176] = 1.0f;
	rectangleNormal[177] = 0.0f;
	rectangleNormal[178] = 0.0f;
	rectangleNormal[179] = 1.0f;

	rectangleNormal[180] = 0.0f;
	rectangleNormal[181] = 0.0f;
	rectangleNormal[182] = 1.0f;
	rectangleNormal[183] = 0.0f;
	rectangleNormal[184] = 0.0f;
	rectangleNormal[185] = 1.0f;

	rectangleNormal[186] = 0.0f;
	rectangleNormal[187] = 0.0f;
	rectangleNormal[188] = 1.0f;
	rectangleNormal[189] = 0.0f;
	rectangleNormal[190] = 0.0f;
	rectangleNormal[191] = 1.0f;

	rectangleNormal[192] = 0.0f;
	rectangleNormal[193] = 0.0f;
	rectangleNormal[194] = 1.0f;
	rectangleNormal[195] = 0.0f;
	rectangleNormal[196] = 0.0f;
	rectangleNormal[197] = 1.0f;

	rectangleNormal[198] = 0.0f;
	rectangleNormal[199] = 0.0f;
	rectangleNormal[200] = 1.0f;
	rectangleNormal[201] = 0.0f;
	rectangleNormal[202] = 0.0f;
	rectangleNormal[203] = 1.0f;

	rectangleNormal[204] = 0.0f;
	rectangleNormal[205] = 0.0f;
	rectangleNormal[206] = 1.0f;
	rectangleNormal[207] = 0.0f;
	rectangleNormal[208] = 0.0f;
	rectangleNormal[209] = 1.0f;

	rectangleNormal[210] = 0.0f;
	rectangleNormal[211] = 0.0f;
	rectangleNormal[212] = 1.0f;
	rectangleNormal[213] = 0.0f;
	rectangleNormal[214] = 0.0f;
	rectangleNormal[215] = 1.0f;

	rectangleNormal[216] = 0.0f;
	rectangleNormal[217] = 0.0f;
	rectangleNormal[218] = 1.0f;
	rectangleNormal[219] = 0.0f;
	rectangleNormal[220] = 0.0f;
	rectangleNormal[221] = 1.0f;

	rectangleNormal[222] = 0.0f;
	rectangleNormal[223] = 0.0f;
	rectangleNormal[224] = 1.0f;
	rectangleNormal[225] = 0.0f;
	rectangleNormal[226] = 0.0f;
	rectangleNormal[227] = 1.0f;

	rectangleNormal[228] = 0.0f;
	rectangleNormal[229] = 0.0f;
	rectangleNormal[230] = 1.0f;
	rectangleNormal[231] = 0.0f;
	rectangleNormal[232] = 0.0f;
	rectangleNormal[233] = 1.0f;

	rectangleNormal[234] = 0.0f;
	rectangleNormal[235] = 0.0f;
	rectangleNormal[236] = 1.0f;
	rectangleNormal[237] = 0.0f;
	rectangleNormal[238] = 0.0f;
	rectangleNormal[239] = 1.0f;

	rectangleNormal[240] = 0.0f;
	rectangleNormal[241] = 0.0f;
	rectangleNormal[242] = 1.0f;
	rectangleNormal[243] = 0.0f;
	rectangleNormal[244] = 0.0f;
	rectangleNormal[245] = 1.0f;

	rectangleNormal[246] = 0.0f;
	rectangleNormal[247] = 0.0f;
	rectangleNormal[248] = 1.0f;
	rectangleNormal[249] = 0.0f;
	rectangleNormal[250] = 0.0f;
	rectangleNormal[251] = 1.0f;

	rectangleNormal[252] = 0.0f;
	rectangleNormal[253] = 0.0f;
	rectangleNormal[254] = 1.0f;
	rectangleNormal[255] = 0.0f;
	rectangleNormal[256] = 0.0f;
	rectangleNormal[257] = 1.0f;

	rectangleNormal[258] = 0.0f;
	rectangleNormal[259] = 0.0f;
	rectangleNormal[260] = 1.0f;
	rectangleNormal[261] = 0.0f;
	rectangleNormal[262] = 0.0f;
	rectangleNormal[263] = 1.0f;

	rectangleNormal[264] = 0.0f;
	rectangleNormal[265] = 0.0f;
	rectangleNormal[266] = 1.0f;
	rectangleNormal[267] = 0.0f;
	rectangleNormal[268] = 0.0f;
	rectangleNormal[269] = 1.0f;

	rectangleNormal[270] = 0.0f;
	rectangleNormal[271] = 0.0f;
	rectangleNormal[272] = 1.0f;
	rectangleNormal[273] = 0.0f;
	rectangleNormal[274] = 0.0f;
	rectangleNormal[275] = 1.0f;

	rectangleNormal[276] = 0.0f;
	rectangleNormal[277] = 0.0f;
	rectangleNormal[278] = 1.0f;
	rectangleNormal[279] = 0.0f;
	rectangleNormal[280] = 0.0f;
	rectangleNormal[281] = 1.0f;

	rectangleNormal[282] = 0.0f;
	rectangleNormal[283] = 0.0f;
	rectangleNormal[284] = 1.0f;
	rectangleNormal[285] = 0.0f;
	rectangleNormal[286] = 0.0f;
	rectangleNormal[287] = 1.0f;

	rectangleNormal[288] = 0.0f;
	rectangleNormal[289] = 0.0f;
	rectangleNormal[290] = 1.0f;
	rectangleNormal[291] = 0.0f;
	rectangleNormal[292] = 0.0f;
	rectangleNormal[293] = 1.0f;

	rectangleNormal[294] = 0.0f;
	rectangleNormal[295] = 0.0f;
	rectangleNormal[296] = 1.0f;
	rectangleNormal[297] = 0.0f;
	rectangleNormal[298] = 0.0f;
	rectangleNormal[299] = 1.0f;

	rectangleNormal[300] = 0.0f;
	rectangleNormal[301] = 0.0f;
	rectangleNormal[302] = 1.0f;
	rectangleNormal[303] = 0.0f;
	rectangleNormal[304] = 0.0f;
	rectangleNormal[305] = 1.0f;

	rectangleNormal[306] = 0.0f;
	rectangleNormal[307] = 0.0f;
	rectangleNormal[308] = 1.0f;
	rectangleNormal[309] = 0.0f;
	rectangleNormal[310] = 0.0f;
	rectangleNormal[311] = 1.0f;

	rectangleNormal[312] = 0.0f;
	rectangleNormal[313] = 0.0f;
	rectangleNormal[314] = 1.0f;
	rectangleNormal[315] = 0.0f;
	rectangleNormal[316] = 0.0f;
	rectangleNormal[317] = 1.0f;

	rectangleNormal[318] = 0.0f;
	rectangleNormal[319] = 0.0f;
	rectangleNormal[320] = 1.0f;
	rectangleNormal[321] = 0.0f;
	rectangleNormal[322] = 0.0f;
	rectangleNormal[323] = 1.0f;

	rectangleNormal[324] = 0.0f;
	rectangleNormal[325] = 0.0f;
	rectangleNormal[326] = 1.0f;
	rectangleNormal[327] = 0.0f;
	rectangleNormal[328] = 0.0f;
	rectangleNormal[329] = 1.0f;

	rectangleNormal[330] = 0.0f;
	rectangleNormal[331] = 0.0f;
	rectangleNormal[332] = 1.0f;
	rectangleNormal[333] = 0.0f;
	rectangleNormal[334] = 0.0f;
	rectangleNormal[335] = 1.0f;

	rectangleNormal[336] = 0.0f;
	rectangleNormal[337] = 0.0f;
	rectangleNormal[338] = 1.0f;
	rectangleNormal[339] = 0.0f;
	rectangleNormal[340] = 0.0f;
	rectangleNormal[341] = 1.0f;

	rectangleNormal[342] = 0.0f;
	rectangleNormal[343] = 0.0f;
	rectangleNormal[344] = 1.0f;
	rectangleNormal[345] = 0.0f;
	rectangleNormal[346] = 0.0f;
	rectangleNormal[347] = 1.0f;

	rectangleNormal[348] = 0.0f;
	rectangleNormal[349] = 0.0f;
	rectangleNormal[350] = 1.0f;
	rectangleNormal[351] = 0.0f;
	rectangleNormal[352] = 0.0f;
	rectangleNormal[353] = 1.0f;

	rectangleNormal[354] = 0.0f;
	rectangleNormal[355] = 0.0f;
	rectangleNormal[356] = 1.0f;
	rectangleNormal[357] = 0.0f;
	rectangleNormal[358] = 0.0f;
	rectangleNormal[359] = 1.0f;

	rectangleNormal[360] = 0.0f;
	rectangleNormal[361] = 0.0f;
	rectangleNormal[362] = 1.0f;
	rectangleNormal[363] = 0.0f;
	rectangleNormal[364] = 0.0f;
	rectangleNormal[365] = 1.0f;

	rectangleNormal[366] = 0.0f;
	rectangleNormal[367] = 0.0f;
	rectangleNormal[368] = 1.0f;
	rectangleNormal[369] = 0.0f;
	rectangleNormal[370] = 0.0f;
	rectangleNormal[371] = 1.0f;
	

	return 1;
}

void SceneRTR::DisplayScene()
{
	if (firstDisplay == 1) {
		glClearColor(0.0f,0.0f,0.0f,0.0f);
	}

//	float t = current_time;

	static const GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	static const GLfloat one = 1.0f;

	glViewport(0, 0, (GLsizei)gWidth, (GLsizei)gHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, render_fbo);
	glClearBufferfv(GL_COLOR, 0, black);
	glClearBufferfv(GL_COLOR, 1, black);
	glClearBufferfv(GL_DEPTH, 0, &one);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	projection = perspective(camera.Zoom, (float)gWidth / (float)gHeight, 0.1f, 100.0f);//radians
	view = camera.GetViewMatrix();

	shaders[Shader_RTRDisplay].use();

	glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo_transform);

	struct transforms_t
	{
		mat4 mat_proj;
		mat4 mat_view;
		mat4 mat_model[SPHERE_COUNT];
	};

	transforms_t *transforms = (transforms_t *)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(transforms_t), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	transforms->mat_proj = projection;
	//transforms->mat_view = vmath::translate(0.0f, 0.0f, -3.0f);
	transforms->mat_view = view;

	translationMatrix = translate(0.2f, 0.0f, -3.0f);

	for (int i = 0; i < SPHERE_COUNT; i++)
	{
		float fi = float(M_PI * (float)i / 16.0f);
		float r = (i & 2) ? 0.6f : 1.5f;
		transforms->mat_model[i] = translationMatrix;
	}

	glUnmapBuffer(GL_UNIFORM_BUFFER);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_material);

	modelMatrix = translationMatrix * rotationMatrix;
	
	
	shaders[Shader_RTRDisplay].setMat4("projection", projection);
	shaders[Shader_RTRDisplay].setMat4("view", view);
	shaders[Shader_RTRDisplay].setMat4("model", modelMatrix);

	shaders[Shader_RTRDisplay].setFloat("bloom_thresh_min", bloom_thresh_min);
	shaders[Shader_RTRDisplay].setFloat("bloom_thresh_max", bloom_thresh_max);



	glLineWidth(5.0f);
	glBindVertexArray(vao_rtr_rectangle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_rtr_rectangle_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleNormal), rectangleNormal, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_rtr_rectangle_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_DYNAMIC_DRAW);
	glDrawArrays(GL_LINES, 0, sizeof(rectangleVertices));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDisable(GL_DEPTH_TEST);

	shaders[Shader_RTRFilter].use();

	glBindVertexArray(vao);
	glBindFramebuffer(GL_FRAMEBUFFER, filter_fbo[0]);
	glBindTexture(GL_TEXTURE_2D, tex_brightpass);
	//glViewport(0, 0, gWidth, gHeight);
	glViewport(0, 0, 2048, 2048);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindFramebuffer(GL_FRAMEBUFFER, filter_fbo[1]);
	glBindTexture(GL_TEXTURE_2D, tex_filter[0]);
	//glViewport(0, 0, gWidth, gHeight);
	glViewport(0, 0, 2048, 2048);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	shaders[Shader_RTRResolve].use();

	shaders[Shader_RTRResolve].setInt("exposure", 1);
	
	shaders[Shader_RTRResolve].setFloat("bloom_factor", bloom_factor);
	shaders[Shader_RTRResolve].setFloat("scene_factor", 1.0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex_filter[1]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_scene);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

}

void SceneRTR::UpdateScene()
{
}

void SceneRTR::UninitializeScene()
{
	//ToDo: free framebuffer : render_fbo

	// Delete Vertex buffers : vbo's 
	DeleteVertexBuffer(&vbo_rtr_rectangle_position);
	DeleteVertexBuffer(&vbo_rtr_rectangle_normal);
	DeleteVertexBuffer(&ubo_transform);
	DeleteVertexBuffer(&ubo_material);
	DeleteVertexBuffer(&filter_fbo[0]);
	DeleteVertexBuffer(&filter_fbo[1]);

	// Delete Vertex Arrays : vao's
	DeleteVertexArray(&vao_rtr_rectangle);
	DeleteVertexArray(&vao);
}
