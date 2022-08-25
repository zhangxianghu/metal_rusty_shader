attribute  vec4 vPosition;
attribute  vec3 vNormal;
varying vec4 color;

varying vec3 fN;
varying vec3 fL;
varying vec3 fE;


uniform mat4 ModelView;
uniform mat4 Projection;
uniform vec4 LightPosition;


void main(){
    vec3 pos = (ModelView * vPosition).xyz;
    //gl_Position = (Projection * ModelView *vPosition);
    fN = (ModelView*vec4(vNormal, 0.0)).xyz;
    fL = (LightPosition.xyz - pos);
    fE = -pos;
    /*fN = (ModelView*vec4(vNormal, 0.0)).xyz;
    fL = LightPosition.xyz;
    fE = vPosition.xyz;*/
    if (LightPosition.w != 0.0){
        fL = LightPosition.xyz - vPosition.xyz;
    }
     gl_Position = (Projection * ModelView *vPosition);
   
}



