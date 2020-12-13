__kernel void simple_vbo_kernel(__global float4* pos, unsigned int width, unsigned int height, float time)
{
	unsigned int x = get_group_id(0) * get_local_size(0) + get_local_id(0);
	unsigned int y = get_group_id(1) * get_local_size(1) + get_local_id(1);

	int grid_width = get_num_groups(0) * get_local_size(0);
	int index = y * grid_width + x;

	float u = x / (float)width;
	float v = y / (float)height;
	
	u = u * 2.0f - 1.0f;
	v = v * 2.0f - 1.0f;

	float freq = 4.0f;

	float w = sin(u * freq + time) * cos(v * freq + time) * 0.5f;
	float4 result;
	result.x = u;
	result.y = w;
	result.z = v;
	result.w = 1.0;

	pos[y * width + x] = result;
}
