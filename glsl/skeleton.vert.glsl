#version 150
// ^ Change this to version 130 if you have compatibility issues

//This is a vertex shader. While it is called a "shader" due to outdated conventions, this file
//is used to apply matrix transformations to the arrays of vertex data passed to it.
//Since this code is run on your GPU, each vertex is transformed simultaneously.
//If it were run on your CPU, each vertex would have to be processed in a FOR loop, one at a time.
//This simultaneous transformation allows your program to run much faster, especially when rendering
//geometry with millions of vertices.

uniform mat4 u_Model;       // The matrix that defines the transformation of the
                            // object we're rendering. In this assignment,
                            // this will be the result of traversing your scene graph.

uniform mat4 u_ModelInvTr;  // The inverse transpose of the model matrix.
                            // This allows us to transform the object's normals properly
                            // if the object has been non-uniformly scaled.

uniform mat4 u_ViewProj;    // The matrix that defines the camera's transformation.
                            // We've written a static matrix for you to use for HW2,
                            // but in HW3 you'll have to generate one yourself

uniform mat4 u_bindMatrices[100]; // the uniform array of matrices that
                                  // each describe the bind matrix of a particular joint

uniform mat4 u_jointTrans[100]; // the uniform array of matrices that
                                // each describe the overall transformation of a particular joint

in vec4 vs_Pos;             // The array of vertex positions passed to the shader

in vec4 vs_Nor;             // The array of vertex normals passed to the shader

in vec4 vs_Col;             // The array of vertex colors passed to the shader.

in ivec2 vs_ids;            // The array of IDs of the joints

in vec2 vs_Inf;             // The array of joint influences on the vertex

out vec4 fs_Nor;            // The array of normals that has been transformed by u_ModelInvTr. This is implicitly passed to the fragment shader.
out vec4 fs_LightVec;       // The direction in which our virtual light lies, relative to each vertex. This is implicitly passed to the fragment shader.
out vec4 fs_Col;            // The color of each vertex. This is implicitly passed to the fragment shader.

const vec4 lightPos = vec4(5, 5, 3, 1); //The position of our virtual light, which is used to compute the shading of
                                        //the geometry in the fragment shader.

void main() {


    // get bind matrices based on joint IDs
    int id1 = vs_ids[0];
    mat4 fs_Bind1 = u_bindMatrices[id1];
    mat4 fs_Trans1 = u_jointTrans[id1];

    int id2 = vs_ids[1];
    mat4 fs_Bind2 = u_bindMatrices[id2];
    mat4 fs_Trans2 = u_jointTrans[id2];

    // pass joint influence on vertices
    float inf1 = vs_Inf[0];
    float inf2 = vs_Inf[1];

    //color influence
    fs_Col = vs_Col;

    mat3 invTranspose = mat3(u_ModelInvTr);
    fs_Nor = vec4(invTranspose * vec3(vs_Nor), 0);          // Pass the vertex normals to the fragment shader for interpolation.
                                                            // Transform the geometry's normals by the inverse transpose of the
                                                            // model matrix. This is necessary to ensure the normals remain
                                                            // perpendicular to the surface after the surface is transformed by
                                                            // the model matrix.


     vec4 newpos = inf1 * fs_Trans1 * fs_Bind1 * vs_Pos;
     newpos += inf2 * fs_Trans2 * fs_Bind2 * vs_Pos;

    fs_LightVec = lightPos - newpos;  // Compute the direction in which the light source lies

    gl_Position = u_ViewProj * newpos;// gl_Position is a built-in variable of OpenGL which is
                                             // used to render the final positions of the geometry's vertices
}
