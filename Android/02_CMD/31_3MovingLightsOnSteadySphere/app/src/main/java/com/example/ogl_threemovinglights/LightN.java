package com.example.ogl_threemovinglights;
public class LightN {

	public float[] lightAmbient;
	public float[] lightDiffuse;
	public float[] lightSpecular;
	public float[] lightPosition;
	
	public LightN(){
		System.out.println("RTR : Test1");
		lightAmbient = new float[4];
		lightDiffuse = new float[4];
		lightSpecular = new float[4];
		lightPosition = new float[4];

	}
    
};