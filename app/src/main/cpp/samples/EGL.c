//
// Created by xucz on 2020/8/2.
//

#include "EGL.h"

const char vShaderStr[] =
        "#version 300 es                            \n"
        "layout(location = 0) in vec4 a_position;   \n"
        "layout(location = 1) in vec2 a_texCoord;   \n"
        "out vec2 v_texCoord;                       \n"
        "void main()                                \n"
        "{                                          \n"
        "   gl_Position = a_position;               \n"
        "   v_texCoord = a_texCoord;                \n"
        "}                                          \n";

// 普通渲染 Normal
const char fShaderStr0[] =
        "#version 300 es\n"
        "precision mediump float;\n"
        "in vec2 v_texCoord;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "uniform sampler2D s_TextureMap;\n"
        "void main()\n"
        "{\n"
        "    outColor = texture(s_TextureMap, v_texCoord);\n"
        "}";

// 马赛克 Mosaic
const char fShaderStr1[] =
        "#version 300 es\n"
        "precision highp float;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "in vec2 v_texCoord;\n"
        "uniform sampler2D s_TextureMap;\n"
        "uniform vec2 u_texSize;\n"
        "\n"
        "vec4 CrossStitching(vec2 uv) {\n"
        "    float stitchSize = u_texSize.x / 35.0;\n"
        "    int invert = 0;\n"
        "    vec4 color = vec4(0.0);\n"
        "    float size = stitchSize;\n"
        "    vec2 cPos = uv * u_texSize.xy;\n"
        "    vec2 tlPos = floor(cPos / vec2(size, size));\n"
        "    tlPos *= size;\n"
        "    int remX = int(mod(cPos.x, size));\n"
        "    int remY = int(mod(cPos.y, size));\n"
        "    if (remX == 0 && remY == 0)\n"
        "    tlPos = cPos;\n"
        "    vec2 blPos = tlPos;\n"
        "    blPos.y += (size - 1.0);\n"
        "    if ((remX == remY) || (((int(cPos.x) - int(blPos.x)) == (int(blPos.y) - int(cPos.y))))) {\n"
        "        if (invert == 1)\n"
        "        color = vec4(0.2, 0.15, 0.05, 1.0);\n"
        "        else\n"
        "        color = texture(s_TextureMap, tlPos * vec2(1.0 / u_texSize.x, 1.0 / u_texSize.y)) * 1.4;\n"
        "    } else {\n"
        "        if (invert == 1)\n"
        "        color = texture(s_TextureMap, tlPos * vec2(1.0 / u_texSize.x, 1.0 / u_texSize.y)) * 1.4;\n"
        "        else\n"
        "        color = vec4(0.0, 0.0, 0.0, 1.0);\n"
        "    }\n"
        "    return color;\n"
        "}\n"
        "void main() {\n"
        "    outColor = CrossStitching(v_texCoord);\n"
        "}";

// 网格 Grid
const char fShaderStr2[] =
        "#version 300 es\n"
        "precision highp float;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "in vec2 v_texCoord;\n"
        "uniform sampler2D s_TextureMap;\n"
        "uniform vec2 u_texSize;\n"
        "void main() {\n"
        "    float size = u_texSize.x / 75.0;\n"
        "    float radius = size * 0.5;\n"
        "    vec2 fragCoord = v_texCoord * u_texSize.xy;\n"
        "    vec2 quadPos = floor(fragCoord.xy / size) * size;\n"
        "    vec2 quad = quadPos/u_texSize.xy;\n"
        "    vec2 quadCenter = (quadPos + size/2.0);\n"
        "    float dist = length(quadCenter - fragCoord.xy);\n"
        "\n"
        "    if (dist > radius) {\n"
        "        outColor = vec4(0.25);\n"
        "    } else {\n"
        "        outColor = texture(s_TextureMap, v_texCoord);\n"
        "    }\n"
        "}";

// 旋转 Rotate
const char fShaderStr3[] =
        "#version 300 es\n"
        "precision highp float;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "in vec2 v_texCoord;\n"
        "uniform lowp sampler2D s_TextureMap;\n"
        "uniform vec2 u_texSize;\n"
        "void main() {\n"
        "    float radius = 200.0;\n"
        "    float angle = 0.8;\n"
        "    vec2 center = vec2(u_texSize.x / 2.0, u_texSize.y / 2.0);\n"
        "    vec2 tc = v_texCoord * u_texSize;\n"
        "    tc -= center;\n"
        "    float dist = length(tc);\n"
        "    if (dist < radius) {\n"
        "        float percent = (radius - dist) / radius;\n"
        "        float theta = percent * percent * angle * 8.0;\n"
        "        float s = sin(theta);\n"
        "        float c = cos(theta);\n"
        "        tc = vec2(dot(tc, vec2(c, -s)), dot(tc, vec2(s, c)));\n"
        "    }\n"
        "    tc += center;\n"
        "    outColor = texture(s_TextureMap, tc / u_texSize);\n"
        "}";

// 边缘 Edge
const char fShaderStr4[] =
        "#version 300 es\n"
        "precision highp float;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "in vec2 v_texCoord;\n"
        "uniform lowp sampler2D s_TextureMap;\n"
        "uniform vec2 u_texSize;\n"
        "void main() {\n"
        "    vec2 pos = v_texCoord.xy;\n"
        "    vec2 onePixel = vec2(1, 1) / u_texSize;\n"
        "    vec4 color = vec4(0);\n"
        "    mat3 edgeDetectionKernel = mat3(\n"
        "    -1, -1, -1,\n"
        "    -1, 8, -1,\n"
        "    -1, -1, -1\n"
        "    );\n"
        "    for(int i = 0; i < 3; i++) {\n"
        "        for(int j = 0; j < 3; j++) {\n"
        "            vec2 samplePos = pos + vec2(i - 1 , j - 1) * onePixel;\n"
        "            vec4 sampleColor = texture(s_TextureMap, samplePos);\n"
        "            sampleColor *= edgeDetectionKernel[i][j];\n"
        "            color += sampleColor;\n"
        "        }\n"
        "    }\n"
        "    outColor = vec4(color.rgb, 1.0);\n"
        "}";
// 放大 EnLarge
const char fShaderStr5[] =
        "#version 300 es\n"
        "precision highp float;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "in vec2 v_texCoord;\n"
        "uniform sampler2D s_TextureMap;\n"
        "uniform vec2 u_texSize;\n"
        "\n"
        "vec2 warpPositionToUse(vec2 centerPostion, vec2 currentPosition, float radius, float scaleRatio, float aspectRatio)\n"
        "{\n"
        "    vec2 positionToUse = currentPosition;\n"
        "    vec2 currentPositionToUse = vec2(currentPosition.x, currentPosition.y * aspectRatio + 0.5 - 0.5 * aspectRatio);\n"
        "    vec2 centerPostionToUse = vec2(centerPostion.x, centerPostion.y * aspectRatio + 0.5 - 0.5 * aspectRatio);\n"
        "    //float r = distance(currentPositionToUse, centerPostionToUse);\n"
        "    float r = distance(currentPosition, centerPostion);\n"
        "    if(r < radius)\n"
        "    {\n"
        "        float alpha = 1.0 - scaleRatio * pow(r / radius - 1.0, 2.0);\n"
        "        positionToUse = centerPostion + alpha * (currentPosition - centerPostion);\n"
        "    }\n"
        "    return positionToUse;\n"
        "}\n"
        "\n"
        "void main() {\n"
        "    vec2 centerPostion = vec2(0.5, 0.5);\n"
        "    float radius = 0.34;\n"
        "    float scaleRatio = 1.0;\n"
        "    float aspectRatio = u_texSize.x / u_texSize.y;\n"
        "    outColor = texture(s_TextureMap, warpPositionToUse(centerPostion, v_texCoord, radius, scaleRatio, aspectRatio));\n"
        "}";
// UnKnow
const char fShaderStr6[] =
        "#version 300 es\n"
        "precision highp float;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "in vec2 v_texCoord;\n"
        "uniform sampler2D s_TextureMap;\n"
        "uniform vec2 u_texSize;\n"
        "\n"
        "vec2 reshape(vec2 src, vec2 dst, vec2 curPos, float radius)\n"
        "{\n"
        "    vec2 pos = curPos;\n"
        "\n"
        "    float r = distance(curPos, src);\n"
        "\n"
        "    if (r < radius)\n"
        "    {\n"
        "        float alpha = 1.0 -  r / radius;\n"
        "        vec2 displacementVec = (dst - src) * pow(alpha, 2.0);\n"
        "        pos = curPos - displacementVec;\n"
        "\n"
        "    }\n"
        "    return pos;\n"
        "}\n"
        "\n"
        "void main() {\n"
        "    vec2 srcPos = vec2(0.5, 0.5);\n"
        "    vec2 dstPos = vec2(0.6, 0.5);\n"
        "    float radius = 0.18;\n"
        "    float scaleRatio = 1.0;\n"
        "    float aspectRatio = u_texSize.x / u_texSize.y;\n"
        "    \n"
        "    if(radius <= distance(v_texCoord, srcPos) && distance(v_texCoord, srcPos) <= radius + 0.008)\n"
        "    {\n"
        "        outColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
        "    } \n"
        "    else\n"
        "    {\n"
        "        outColor = texture(s_TextureMap, reshape(srcPos, dstPos, v_texCoord, radius));\n"
        "    }\n"
        "}";

// 形变 deformation
const char fShaderStr7[] =
        "#version 300 es\n"
        "precision highp float;\n"
        "layout(location = 0) out vec4 outColor;\n"
        "in vec2 v_texCoord;\n"
        "uniform sampler2D s_TextureMap;\n"
        "uniform vec2 u_texSize;\n"
        "\n"
        "float distanceTex(vec2 pos0, vec2 pos1, float aspectRatio)\n"
        "{\n"
        "    vec2 newPos0 = vec2(pos0.x, pos0.y * aspectRatio + 0.5 - 0.5 * aspectRatio);\n"
        "    vec2 newPos1 = vec2(pos1.x, pos1.y * aspectRatio + 0.5 - 0.5 * aspectRatio);\n"
        "    return distance(newPos0, newPos1);\n"
        "}\n"
        "\n"
        "vec2 reshape(vec2 src, vec2 dst, vec2 curPos, float radius, float aspectRatio)\n"
        "{\n"
        "    vec2 pos = curPos;\n"
        "\n"
        "    vec2 newSrc = vec2(src.x, src.y * aspectRatio + 0.5 - 0.5 * aspectRatio);\n"
        "    vec2 newDst = vec2(dst.x, dst.y * aspectRatio + 0.5 - 0.5 * aspectRatio);\n"
        "    vec2 newCur = vec2(curPos.x, curPos.y * aspectRatio + 0.5 - 0.5 * aspectRatio);\n"
        "\n"
        "\n"
        "    float r = distance(newSrc, newCur);\n"
        "\n"
        "    if (r < radius)\n"
        "    {\n"
        "        float alpha = 1.0 -  r / radius;\n"
        "        vec2 displacementVec = (dst - src) * pow(alpha, 1.7);\n"
        "        pos = curPos - displacementVec;\n"
        "\n"
        "    }\n"
        "    return pos;\n"
        "}\n"
        "\n"
        "void main() {\n"
        "    vec2 srcPos = vec2(0.5, 0.5);\n"
        "    vec2 dstPos = vec2(0.55, 0.55);\n"
        "    float radius = 0.30;\n"
        "    float scaleRatio = 1.0;\n"
        "    float aspectRatio = u_texSize.y/u_texSize.x;\n"
        "\n"
        "    if(radius <= distanceTex(v_texCoord, srcPos, aspectRatio) && distanceTex(v_texCoord, srcPos, aspectRatio) <= radius + 0.008)\n"
        "    {\n"
        "        outColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
        "    }\n"
        "    else\n"
        "    {\n"
        "        outColor = texture(s_TextureMap, reshape(srcPos, dstPos, v_texCoord, radius, aspectRatio));\n"
        "    }\n"
        "}";

//顶点坐标
const GLfloat vVertices[] = {
        -1.0f, -1.0f, 0.0f, // bottom left
        1.0f, -1.0f, 0.0f, // bottom right
        -1.0f, 1.0f, 0.0f, // top left
        1.0f, 1.0f, 0.0f, // top right
};

//正常纹理坐标
const GLfloat vTexCoors[] = {
        0.0f, 1.0f, // bottom left
        1.0f, 1.0f, // bottom right
        0.0f, 0.0f, // top left
        1.0f, 0.0f, // top right
};

//fbo 纹理坐标与正常纹理方向不同(上下镜像)
const GLfloat vFboTexCoors[] = {
        0.0f, 0.0f,  // bottom left
        1.0f, 0.0f,  // bottom right
        0.0f, 1.0f,  // top left
        1.0f, 1.0f,  // top right
};

const GLushort eglIndices[] = {0, 1, 2, 1, 3, 2};


typedef struct _UserData{
    GLuint imageTextureId;
    GLuint fboId;
    GLuint fboTextureId;

    GLuint *vboIds;
    GLuint vaoId;

    GLuint drawProgram;
} UserData;

void EGLInit(ESContext *esContext) {
    UserData *userData = esContext->userData;
    if(!userData){
        userData = malloc(sizeof(UserData));
        memset(userData, 0, sizeof(UserData));
        esContext->userData = userData;
    }

    // 生成一个纹理，用于保存图像
    glGenTextures(1, &userData->imageTextureId);
    glBindTexture(GL_TEXTURE_2D, userData->imageTextureId);
    //  设置s轴拉伸为截取
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //  设置t轴拉伸为截取
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //  设置纹理采样方式
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    
    // 生成一个FBO
    glGenFramebuffers(1, &userData->fboId);
    //    创建一个纹理，用于连接FBO
    glGenTextures(1, &userData->fboTextureId);
    glBindTexture(GL_TEXTURE_2D, userData->fboTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    //    连接纹理
    glBindFramebuffer(GL_FRAMEBUFFER, userData->fboId);
    glBindTexture(GL_TEXTURE_2D, userData->fboTextureId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, userData->fboTextureId, 0);
    //    分配内存
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, esContext->width, esContext->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    //    检查FBO
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE ){
        // error
        return;
    }
    //    解绑
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);


    // 使用VBO保存顶点纹理坐标数组，及图元数据
    GLuint buffers[3];
    glGenBuffers(3, buffers);
    userData->vboIds = buffers;

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vTexCoors), vTexCoors, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(eglIndices), eglIndices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);

    // 使用VAO记录设置顶点着色的过程
    GLuint arrays[1];
    glGenVertexArrays(1, arrays);
    userData->vaoId = arrays[0];

    glBindVertexArray(arrays[0]);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
    glBindVertexArray(GL_NONE);

}


void EGLUninit(ESContext *esContext) {
    UserData *userData = esContext->userData;
    if(!userData){
        return;
    }
    if(userData->imageTextureId){
        glDeleteTextures(1, &userData->imageTextureId);
        userData->imageTextureId = GL_NONE;
    }
    if(userData->drawProgram){
        glDeleteProgram(userData->drawProgram);
        userData->drawProgram = GL_NONE;
    }
    if(userData->fboTextureId){
        glDeleteTextures(1, &userData->fboTextureId);
        userData->fboTextureId = GL_NONE;
    }
    if(userData->fboId){
        glDeleteFramebuffers(1, &userData->fboId);
        userData->fboId = GL_NONE;
    }
    if(userData->vaoId){
        glDeleteVertexArrays(1, &userData->vaoId);
        userData->vaoId = GL_NONE;
    }
    if(userData->vboIds){
        glDeleteBuffers(3, userData->vboIds);
        userData->vboIds = NULL;
    }
    free(userData);
    esContext->userData = NULL;
}

void EGLDraw(ESContext *esContext, const char *fragShaderSrc){
    UserData *userData = esContext->userData;
    if(!userData){
        return;
    }
    NativeImage *image = esContext->imageData[0];
    if(!image){
        return;
    }
    if(image->format != IMAGE_FORMAT_RGBA){
        return;
    }

    // 编译着色器
    GLuint program = esLoadProgram(vShaderStr, fragShaderSrc);
    GLint texLoc = glGetUniformLocation(program, "s_TextureMap");
    GLint texSizeLoc = glGetUniformLocation(program, "u_texSize");
    userData->drawProgram = program;

    // 使用FBO
    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    //  !! 需要指定大小，否则不会显示
    glViewport(0, 0, esContext->width, esContext->height);
    glBindFramebuffer(GL_FRAMEBUFFER, userData->fboId);

    glUseProgram(program);

    // 把图像绘制到纹理上
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->imageTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 加载纹理到着色器
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->imageTextureId);
    glUniform1i(texLoc, 0);

    // 加载变换坐标到片段着色器
    // !! texSizeLoc有可能为0
    if(texSizeLoc > -1){
        GLfloat texSize[2];
        texSize[0] = esContext->width;
        texSize[1] = esContext->height;
        glUniform2f(texSizeLoc, texSize[0], texSize[1]);
    }


    // 绘制
    glBindVertexArray(userData->vaoId);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(GL_NONE);
}


void EGLDrawNormal(ESContext *esContext) {
    EGLDraw(esContext, fShaderStr0);
}

void EGLDrawMosaic(ESContext *esContext) {
    EGLDraw(esContext, fShaderStr1);
}

void EGLDrawGrid(ESContext *esContext) {
    EGLDraw(esContext, fShaderStr2);
}

void EGLDrawRotate(ESContext *esContext) {
    EGLDraw(esContext, fShaderStr3);
}

void EGLDrawEdge(ESContext *esContext) {
    EGLDraw(esContext, fShaderStr4);
}


void EGLDrawEnLarge(ESContext *esContext) {
    EGLDraw(esContext, fShaderStr5);
}


void EGLDrawUnKnow(ESContext *esContext) {
    EGLDraw(esContext, fShaderStr6);
}


void EGLDrawDeformation(ESContext *esContext) {
    EGLDraw(esContext, fShaderStr7);
}


