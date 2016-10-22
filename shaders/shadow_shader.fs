uniform sampler2D sampler;

varying vec2 uvCoord;                                                              
                                                                                    
void main()                                                                         
{                                                                                   
    float depth = texture2D(sampler, uvCoord).x;                               
    depth = 1.0 - (1.0 - depth) * 25.0;                                             
    gl_FragColor = vec4(depth);                                                        
}
