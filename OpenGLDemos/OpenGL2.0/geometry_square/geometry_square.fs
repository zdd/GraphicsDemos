void main(void) {
    gl_FragColor[0] = gl_FragCoord.x / 640;
    gl_FragColor[1] = gl_FragCoord.y / 640;
    gl_FragColor[2] = 0.5;
}

