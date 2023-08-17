#version 330 core
layout (lines) in;
layout(triangle_strip, max_vertices = 8) out;//triangle_strip

uniform float u_thickness ;
uniform   vec2    u_viewportSize ;
uniform   bool    u_scale_width_by_zoom ;

// in gl_PerVertex
// {
//   vec4 gl_Position;
// } gl_in[];

vec4 rotate(vec4 p, vec4 axis,float angle  )
{
    vec3 q;
    q[0] = p[0] * (axis.x*axis.x * (1.0 - cos(angle)) + cos(angle))
        + p[1] * (axis.x*axis.y * (1.0 - cos(angle)) + axis.z * sin(angle))
        + p[2] * (axis.x*axis.z * (1.0 - cos(angle)) - axis.y * sin(angle));

    q[1] = p[0] * (axis.y*axis.x * (1.0 - cos(angle)) - axis.z * sin(angle))
        + p[1]* (axis.y*axis.y * (1.0 - cos(angle)) + cos(angle))
        + p[2] * (axis.y*axis.z * (1.0 - cos(angle)) + axis.x * sin(angle));

    q[2] = p[0] * (axis.z*axis.x * (1.0 - cos(angle)) + axis.y * sin(angle))
        + p[1] * (axis.z*axis.y * (1.0 - cos(angle)) - axis.x * sin(angle))
        + p[2] * (axis.z*axis.z * (1.0 - cos(angle)) + cos(angle));

   return vec4(q, 0.0);

}


void main() {   


    vec4 p1 = gl_in[0].gl_Position;
    vec4 p2 = gl_in[1].gl_Position;

   //https://stackoverflow.com/questions/54686818/glsl-geometry-shader-to-replace-gllinewidth



    vec2 dir = normalize((p2.xy / p2.w - p1.xy / p1.w) * u_viewportSize);
    vec2 offset = vec2(-dir.y, dir.x) * u_thickness / u_viewportSize;

    float scale_0 = p1.w;
    float scale_1 = p2.w;


    if(u_scale_width_by_zoom){
     scale_0 = 3;
     scale_1 = 3;
    }

    gl_Position = p1 - vec4(offset.xy * scale_0, 0.0, 0.0);
    EmitVertex();
    gl_Position = p1 + vec4(offset.xy * scale_0, 0.0, 0.0);
    EmitVertex();
    gl_Position = p2 - vec4(offset.xy * scale_1, 0.0, 0.0);
    EmitVertex();
    gl_Position = p2 + vec4(offset.xy * scale_1, 0.0, 0.0);
    EmitVertex();



//
//    //tube
//    // Specify the axis to rotate about:
//    vec4 axis =  p2-p1;
//   // axis = normalize(axis);
//     vec4 perp = vec4( cross(vec3(axis.x,axis.y,axis.z),vec3(0.1,1,0)) ,0);
//     	perp = normalize(perp);
//    //float length = hypotf(axis[0], hypotf(axis[1], axis[2]));
////    float length = sqrt((axis[0]*axis[0]) + sqrt(axis[1]*axis[1]+ axis[2]*axis[2]));
////	float dir_scalar = (axis[0] > 0.0) ? length : -length;
////	float xt = axis[0] + dir_scalar;
////	float dot = -axis[1] / (dir_scalar * xt);
////
////    vec3 perp_0 = vec3(dot * xt, 1.0f + dot * axis.y, dot * axis.z);
////	perp_0 = normalize(perp_0);
//     perp = perp*u_thickness*0.01;
//
//    //side0
//    vec4 perp_rot_1=rotate(perp,axis,6.24/3*0);
//    vec4 p1_1 = p1+perp_rot_1;
//    vec4 p1_2 = p2+perp_rot_1;
//
//    vec4 perp_rot_2=rotate(perp,axis,6.24/3);
//    vec4 p2_1 = p1+perp_rot_2;
//    vec4 p2_2 = p2+perp_rot_2;
//
//
//    vec4 perp_rot_3=rotate(perp,axis, 6.24/3*2);
//    vec4 p3_1 = p1+perp_rot_3;
//    vec4 p3_2 = p2+perp_rot_3;
//
//    gl_Position = p1_1;
//    EmitVertex();
//    gl_Position = p1_2;
//    EmitVertex();
//    gl_Position = p2_1;
//    EmitVertex();
//    gl_Position = p2_2;
//    EmitVertex();
//    gl_Position = p3_1;
//    EmitVertex();
//    gl_Position = p3_2;
//    EmitVertex();
//    gl_Position = p1_1;
//    EmitVertex();
//    gl_Position = p1_2;
//    EmitVertex();
//
//
//
//    EndPrimitive();
}