//
// Created by xucz on 2020/8/9.
//

#include "BasicLighting.h"
extern "C" {

typedef struct __UserData {
    GLuint imageTexture;


    GLuint program;
    GLuint MVPMatrixLoc;
    GLuint modelMatrixLoc;
    GLuint lightPosLoc;
    GLuint lightColorLoc;
    GLuint viewPosLoc;
    GLuint textureLoc;

    GLuint vboId;
    GLuint vaoId;

    glm::mat4 MVPMatrix;
    glm::mat4 ModelMatrix;

    int m_AngleX;
    int m_AngleY;

    float m_ScaleX;
    float m_ScaleY;
} UserData;

void BasicLightingInit(ESContext *esContext){
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        userData = new UserData();
        esContext->userData = userData;
    }

    userData->ModelMatrix = glm::mat4(0.0f);
    userData->m_ScaleY = 1.0f;
    userData->m_ScaleX = 1.0f;

    // 创建一个纹理来加载图片
    glGenTextures(1, &userData->imageTexture);
    glBindTexture(GL_TEXTURE_2D, userData->imageTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    auto *image = static_cast<NativeImage *>(esContext->imageData[0]);
    if(image && image->format == IMAGE_FORMAT_RGBA){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, image->ppPlane[0]);
    }
    glBindTexture(GL_TEXTURE_2D, GL_NONE);


    // 编译着色器
    char vShaderStr[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "layout(location = 0) in vec4 a_position;\n" // 顶点坐标
            "layout(location = 1) in vec2 a_texCoord;\n" // 纹理坐标
            "layout(location = 2) in vec3 a_normal;\n" // 法向量
            "uniform mat4 u_MVPMatrix;\n"
            "uniform mat4 u_ModelMatrix;\n"
            "uniform vec3 lightPos;\n" // 光照坐标点
            "uniform vec3 lightColor;\n"// 光照颜色
            "uniform vec3 viewPos;\n"// 光照对准坐标
            "out vec2 v_texCoord;\n"
            "out vec3 ambient;\n"
            "out vec3 diffuse;\n"
            "out vec3 specular;\n"
            "void main()\n"
            "{\n"
            "    gl_Position = u_MVPMatrix * a_position;\n"
            "    vec3 fragPos = vec3(u_ModelMatrix * a_position);\n"
            "\n"
            "    // Ambient\n"  // 环境光
            "    float ambientStrength = 0.1;\n"
            "    ambient = ambientStrength * lightColor;\n"
            "\n"
            "    // Diffuse\n" // 散射光
            "    float diffuseStrength = 0.5;\n"
            "    vec3 unitNormal = normalize(vec3(u_ModelMatrix * vec4(a_normal, 1.0)));\n"
            "    vec3 lightDir = normalize(lightPos - fragPos);\n"
            "    float diff = max(dot(unitNormal, lightDir), 0.0);\n"
            "    diffuse = diffuseStrength * diff * lightColor;\n"
            "\n"
            "    // Specular\n" // 镜面光
            "    float specularStrength = 0.9;\n"
            "    vec3 viewDir = normalize(viewPos - fragPos);\n"
            "    vec3 reflectDir = reflect(-lightDir, unitNormal);\n"
            "    float spec = pow(max(dot(unitNormal, reflectDir), 0.0), 16.0);\n"
            "    specular = specularStrength * spec * lightColor;\n"
            "    v_texCoord = a_texCoord;\n"
            "}";

    char fShaderStr[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "in vec2 v_texCoord;\n"
            "in vec3 ambient;\n"
            "in vec3 diffuse;\n"
            "in vec3 specular;\n"
            "layout(location = 0) out vec4 outColor;\n"
            "uniform sampler2D s_TextureMap;\n"
            "void main()\n"
            "{\n"
            "    vec4 objectColor = texture(s_TextureMap, v_texCoord);\n"
            "    vec3 finalColor = (ambient + diffuse + specular) * vec3(objectColor);\n"
            "    outColor = vec4(finalColor, 1.0);\n"
            "}";

    GLuint program = esLoadProgram(vShaderStr, fShaderStr);
    if(!program){
        return;
    }
    userData->program = program;
    userData->MVPMatrixLoc = glGetUniformLocation(program, "u_MVPMatrix");
    userData->modelMatrixLoc = glGetUniformLocation(program, "u_ModelMatrix");
    userData->lightPosLoc = glGetUniformLocation(program, "lightPos");
    userData->lightColorLoc = glGetUniformLocation(program, "lightColor");
    userData->viewPosLoc = glGetUniformLocation(program, "viewPos");
    userData->textureLoc = glGetUniformLocation(program, "s_TextureMap");

    // 确定顶点坐标，纹理坐标，法向量
    GLfloat vertices[] = {
            //position顶点         //texture coord纹理  //normal法向量
            -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,      0.0f,  0.0f, -1.0f,
             0.5f, -0.5f, -0.5f,   1.0f, 0.0f,      0.0f,  0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,   1.0f, 1.0f,      0.0f,  0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,   1.0f, 1.0f,      0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,      0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,      0.0f,  0.0f, -1.0f,

            -0.5f, -0.5f, 0.5f,    0.0f, 0.0f,      0.0f,  0.0f,  1.0f,
             0.5f, -0.5f, 0.5f,    1.0f, 0.0f,      0.0f,  0.0f,  1.0f,
             0.5f,  0.5f, 0.5f,    1.0f, 1.0f,      0.0f,  0.0f,  1.0f,
             0.5f,  0.5f, 0.5f,    1.0f, 1.0f,      0.0f,  0.0f,  1.0f,
            -0.5f,  0.5f, 0.5f,    0.0f, 1.0f,      0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, 0.5f,    0.0f, 0.0f,      0.0f,  0.0f,  1.0f,

            -0.5f,  0.5f,  0.5f,   1.0f, 0.0f,     -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,   1.0f, 1.0f,     -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,     -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,     -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,     -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,   1.0f, 0.0f,     -1.0f,  0.0f,  0.0f,

             0.5f,  0.5f,  0.5f,   1.0f, 0.0f,      1.0f,  0.0f,  0.0f,
             0.5f,  0.5f, -0.5f,   1.0f, 1.0f,      1.0f,  0.0f,  0.0f,
             0.5f, -0.5f, -0.5f,   0.0f, 1.0f,      1.0f,  0.0f,  0.0f,
             0.5f, -0.5f, -0.5f,   0.0f, 1.0f,      1.0f,  0.0f,  0.0f,
             0.5f, -0.5f,  0.5f,   0.0f, 0.0f,      1.0f,  0.0f,  0.0f,
             0.5f,  0.5f,  0.5f,   1.0f, 0.0f,      1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,      0.0f, -1.0f,  0.0f,
             0.5f, -0.5f, -0.5f,   1.0f, 1.0f,      0.0f, -1.0f,  0.0f,
             0.5f, -0.5f,  0.5f,   1.0f, 0.0f,      0.0f, -1.0f,  0.0f,
             0.5f, -0.5f,  0.5f,   1.0f, 0.0f,      0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,      0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,      0.0f, -1.0f,  0.0f,

            -0.5f, 0.5f, -0.5f,    0.0f, 1.0f,      0.0f,  1.0f,  0.0f,
             0.5f, 0.5f, -0.5f,    1.0f, 1.0f,      0.0f,  1.0f,  0.0f,
             0.5f, 0.5f,  0.5f,    1.0f, 0.0f,      0.0f,  1.0f,  0.0f,
             0.5f, 0.5f,  0.5f,    1.0f, 0.0f,      0.0f,  1.0f,  0.0f,
            -0.5f, 0.5f,  0.5f,    0.0f, 0.0f,      0.0f,  1.0f,  0.0f,
            -0.5f, 0.5f, -0.5f,    0.0f, 1.0f,      0.0f,  1.0f,  0.0f,
    };

    // 使用VBO保存数组数据
    glGenBuffers(1, &userData->vboId);
    glBindBuffer(GL_ARRAY_BUFFER, userData->vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    // 使用VAO记录坐标赋值过程
    glGenVertexArrays(1, &userData->vaoId);
    glBindVertexArray(userData->vaoId);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboId);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (3 + 2 + 3) * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (3 + 2 + 3) * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, (3 + 2 + 3) * sizeof(GLfloat),(void *)(5 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindVertexArray(GL_NONE);




}

void BasicLightingUnInit(ESContext *esContext){
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        return;
    }
    if(userData->imageTexture){
        glDeleteTextures(1, &userData->imageTexture);
        userData->imageTexture = 0;
    }
    if(userData->program){
        glDeleteProgram(userData->program);
        userData->program = 0;
    }
    if(userData->vboId){
        glDeleteBuffers(1, &userData->vboId);
        userData->vboId = 0;
    }
    if(userData->vaoId){
        glDeleteVertexArrays(1, &userData->vaoId);
        userData->vaoId = 0;
    }
    delete userData;
    esContext->userData = nullptr;
}

void BasicLightingUpdateMatrix(ESContext *esContext){
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        return;
    }
    int angleX = userData->m_AngleX;
    int angleY = userData->m_AngleY;
    float scaleX = userData->m_ScaleX;
    //float scaleY = userData->m_ScaleY;
    float ratio = 1.0f * esContext->width / esContext->height;

    LOGCATE("BasicLightingSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX,
            angleY, ratio);
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);

    // Projection matrix 裁剪坐标
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    //      透视投影
    glm::mat4 Projection = glm::perspective(45.0f, ratio, 0.1f, 100.f);

    // View matrix 观察坐标
    glm::mat4 View = glm::lookAt(
            glm::vec3(-3, 0, 3), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix 世界坐标
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(scaleX, scaleX, scaleX));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

    userData->MVPMatrix = Projection * View * Model;
    userData->ModelMatrix = Model;
}

void BasicLightingDraw(ESContext *esContext){
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData || !userData->program){
        return;
    }

    // 开启深度测试
    // ！！使用时必须在创建Windows前选配置时添加EGL_DEPTH_SIZE，否则不会生效，模块测试同理
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);


    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);


    glUseProgram(userData->program);
    glBindVertexArray(userData->vaoId);

    // 给MVP和Model变量赋值
    BasicLightingUpdateMatrix(esContext);
    glUniformMatrix4fv(userData->MVPMatrixLoc, 1, GL_FALSE, &userData->MVPMatrix[0][0]);
    glUniformMatrix4fv(userData->modelMatrixLoc, 1, GL_FALSE, &userData->ModelMatrix[0][0]);
    // 设置光照坐标
    glUniform3f(userData->lightPosLoc, -2.0f, 0.0f, 2.0f);
    glUniform3f(userData->lightColorLoc, 1.0f, 1.0f, 1.0f);
    glUniform3f(userData->viewPosLoc, -3.0f, 1.0f, 3.0f);

    // 设置纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->imageTexture);
    glUniform1i(userData->textureLoc, 0);

    // 绘制
    glDrawArrays(GL_TRIANGLES, 0, 36);
    GO_CHECK_GL_ERROR();

    glBindVertexArray(GL_NONE);

}

void BasicLightingUpdateTransformMatrix(ESContext *esContext, GLfloat rotateX, GLfloat rotateY,
                                        GLfloat scaleX, GLfloat scaleY){
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        return;
    }
    userData->m_AngleX = rotateX;
    userData->m_AngleY = rotateY;
    userData->m_ScaleX = scaleX;
    userData->m_ScaleY = scaleY;
}


}