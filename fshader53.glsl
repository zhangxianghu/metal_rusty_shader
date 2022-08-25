varying  vec3 fN;
varying  vec3 fL;
varying  vec3 fE;


uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform mat4 ModelView;
uniform vec4 LightPosition;
uniform float Shininess;
uniform int selectionColorR;
uniform int selectionColorG;
uniform int selectionColorB;
uniform int selectionColorA;
uniform int flag;
uniform int shader_flag;
vec4 color;

void main(){
    vec3 N = normalize(fN);
    vec3 E = normalize(fE);
    vec3 L = normalize(fL);
    vec3 H = normalize( L + E );
    vec3 T;
    float LT, VT, a, b, BRDF, BRDF_d;
    vec4 ambient = AmbientProduct;
    float Kd = max(dot(L,N), 0.0);
    vec4 diffuse;
    
    //float ln;
    

    
    float Ks = pow(max(dot(H,N),0.0), Shininess);
    vec4 specular;
    
    
    
    if(flag != 1){
        if (shader_flag != 1){
        
            T = normalize( cross(N, E) );
            LT = dot( L, T );
            VT = dot( E, T );
            a = sqrt( 1.0 - pow( LT, 2.0 ) ) * sqrt( 1.0 - pow( VT, 2.0 ) ) - LT * VT;
            BRDF = pow( a, Shininess );
            b = sqrt(1.0 - pow(LT, 2.0));
            BRDF_d = pow (b, Shininess);
            specular = Ks*BRDF*SpecularProduct;
            diffuse = Kd*BRDF*DiffuseProduct;
            color = diffuse + specular +ambient;
            color.a = 1.0;
            
         }else{
            diffuse = Kd*DiffuseProduct;
            specular = Ks*SpecularProduct;
         }
     color = ambient + diffuse + specular;
     color.a = 1.0;
     }else {
        color.r = float(selectionColorR)/float(255);
        color.g = float(selectionColorG)/float(255);
		color.b = float(selectionColorB)/float(255);
		color.a = float(selectionColorA)/float(255);
	}    
    gl_FragColor = color;
}



/*void main() {
    // Normalize the input lighting vectors
    vec3 N = normalize(fN);
    vec3 E = normalize(fE);
    vec3 L = normalize(fL);
    vec3 H = normalize( L + E );
    //float sil = max(dot(E,N),0.0);
    vec4 ambient = AmbientProduct;
    
    float Kd = max(dot(L,N), 0.0);
    vec4 diffuse = Kd*DiffuseProduct;
    
    float sil = max(dot(N,E),0.0);
        
    float Ks = pow(max(dot(N,E),0.0), Shininess);
    vec4 specular = Ks*SpecularProduct;
    
    // discard the specular highlight if the light’s behind the vertex
    if( dot(L, N) < 0.0 ) {
        specular = vec4(0.0, 0.0, 0.0, 1.0);
    }
    
    if(flag != 1){
        color = ambient + diffuse + specular;
        color.a = 1.0;  

        //sil = max(dot(N,E),0.0);
        if (shader_flag != 1){
            if(sil < 0.1){
                    //color = vec4(0.0, 0.0, 0.0, 1.0);
            }
            if (shader_flag == 4){   
                if (1.0> Kd && Kd >= 0.95 ){
                    color = ambient + 0.95*SpecularProduct + specular;  
                }else if (Kd>= 0.75){
                    color = ambient + 0.75*SpecularProduct + specular;
                }else if(Kd >= 0.5){
                    color = ambient + 0.5*SpecularProduct + specular;
                }else{
                    color = ambient + 0.2*SpecularProduct + specular;
                }
                
             }else if (shader_flag == 3){
                if (1.0> Kd && Kd >= 0.8 ){
                    color = ambient + 0.95*SpecularProduct + specular;  
                }else if (Kd>= 0.4){
                    color = ambient + 0.5*SpecularProduct + specular;
                }else{
                    color = ambient + 0.2*SpecularProduct + specular;
                }    
              }else if (shader_flag == 2){
                if (1.0> Kd && Kd >= 0.5 ){
                    color = ambient + 0.95*SpecularProduct + specular;  
                }else{
                    color = ambient + 0.2*SpecularProduct + specular;
                }
             } 
        } 
	} else {
        color.r = float(selectionColorR)/float(255);
        color.g = float(selectionColorG)/float(255);
		color.b = float(selectionColorB)/float(255);
		color.a = float(selectionColorA)/float(255);
	}
    color.a = 1.0;
    gl_FragColor = color;
}*/



