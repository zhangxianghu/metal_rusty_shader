# metal_rusty_shader

This project is going to implement different metallic and rusted metal rendering along with the time changing.
	
	1. The project will use Phong shading to implement the lighting which is the sum of ambient, diffuse, and specular light. 
	For different materials, the parameters of Ka, Kd, Ks are various. The website http://devernay.free.fr/cours/opengl/
	materials.html gives a selection of different material settings designed to mimic various materials, including some metal such as chrome, gold and silver.
	
	2. Next, create the representation of a metal’s surrounding environment and add them to the color which we have in the above step. 
	This is what we learn in the cube mapping.

	3. Now we can generate noise and use a time variable to simulate the rusting process along with the time changing.

	4. Finally, re-calculate the color for the rusty area.