#include "myChromaKeyShader.h"

myChromaKeyShader::myChromaKeyShader()
{
    //ctor
}
//--------------------------------------------------------------
void myChromaKeyShader::setup(int fboW, int fboH)
{
    MY_TEXTURE_ID1 = GL_TEXTURE3;
    MY_TEXTURE_ID2 = GL_TEXTURE4;
    ofBackground(255,255,255);
    ofSetVerticalSync(true);

    _w = fboW;
    _h = fboH;

    shaderCK.loadShader("shaders/bg+chroma_keying");

    // PREPARE TEXTURES

    // DisableArbTex !!!
    //////////////////
    //ofDisableArbTex();

    ////////////For use as background after chromakey
    myImg.loadImage("images/Squirrel.jpg");
    myImg.setImageType(OF_IMAGE_COLOR);
    myImg.resize(fboW, fboH);

    ofTexture& myTextureRef = myImg.getTextureReference();

    ////////////For use as empty blue/green background for subtraction
    myImg2.loadImage("images/Crate.jpg");
    myImg2.setImageType(OF_IMAGE_COLOR);
    myImg2.resize(fboW, fboH);

    ofTexture& myTextureRef2 = myImg2.getTextureReference();

    ///// multitextureLoad stuff
    glActiveTexture(MY_TEXTURE_ID1);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB , myTextureRef.getTextureData().textureID);

    glActiveTexture(MY_TEXTURE_ID2);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB , myTextureRef2.getTextureData().textureID);

    // go back to default texture unit !!
    glActiveTexture(GL_TEXTURE0);

    // PREPARE CHROMAKEY PARAMETER
    //
    tol = 2.0;
    _r = 0;
    _g = 255;
    _b = 0;
}
