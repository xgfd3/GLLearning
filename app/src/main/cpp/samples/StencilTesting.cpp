//
// Created by xucz on 2020/8/10.
//

#include "StencilTesting.h"

extern "C" {

typedef struct __UserData {
    GLuint imageTexture;

    GLuint program;
    GLuint mMVPMatrixLoc;
    GLuint mModelMatrixLoc;
    GLuint textureLoc;
    GLuint viewPosLoc;
    GLuint lightPositionLoc;
    GLuint lightDirectionLoc;
    GLuint lightColorLoc;
    GLuint lightCutOffLoc;
    GLuint lightOuterCutOffLoc;
    GLuint lightConstantLoc;
    GLuint lightLinearLoc;
    GLuint lightQuadraticLoc;

    GLuint frameProgram;
    GLuint frameMVPMatrixLoc;


    GLuint vboId;
    GLuint vaoId;


    glm::mat4 mMVPMatrix;
    glm::mat4 mModelMatrix;

    int angleX;
    int angleY;
    float scaleX;
    float scaleY;

} UserData;

void StencilTestingInit(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if (!userData) {
        userData = new UserData();
        esContext->userData = userData;
    }

    // 编译着色器
    char vShaderStr[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "layout(location = 0) in vec4 a_position;\n"
            "layout(location = 1) in vec2 a_texCoord;\n"
            "layout(location = 2) in vec3 a_normal;\n"
            "\n"
            "out vec3 normal;\n"
            "out vec3 fragPos;\n"
            "out vec2 v_texCoord;\n"
            "\n"
            "uniform mat4 u_MVPMatrix;\n"
            "uniform mat4 u_ModelMatrix;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    gl_Position = u_MVPMatrix * a_position;\n"
            "    fragPos = vec3(u_ModelMatrix * a_position);\n"
            "    normal = mat3(transpose(inverse(u_ModelMatrix))) * a_normal;\n"
            "    v_texCoord = a_texCoord;\n"
            "}";

    char fShaderStr[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "struct Light {\n"
            "    vec3 position;\n"
            "    vec3 direction;\n"
            "    vec3 color;\n"
            "    float cutOff;\n"
            "    float outerCutOff;\n"
            "\n"
            "    float constant;\n"
            "    float linear;\n"
            "    float quadratic;\n"
            "};\n"
            "\n"
            "in vec3 normal;\n"
            "in vec3 fragPos;\n"
            "in vec2 v_texCoord;\n"
            "\n"
            "layout(location = 0) out vec4 outColor;\n"
            "uniform sampler2D s_TextureMap;\n"
            "\n"
            "uniform vec3 viewPos;\n"
            "uniform Light light;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    vec4 objectColor = texture(s_TextureMap, v_texCoord);\n"
            "\n"
            "    vec3 lightDir = normalize(light.position - fragPos);\n"
            "\n"
            "    // Check if lighting is inside the spotlight cone\n"
            "    float theta = dot(lightDir, normalize(-light.direction));\n"
            "\n"
            "    float epsilon = light.cutOff - light.outerCutOff;\n"
            "    float intensity = clamp((theta - light.outerCutOff) / epsilon,0.0, 1.0);\n"
            "\n"
            "    // Ambient\n"
            "    float ambientStrength = 0.25;\n"
            "    vec3 ambient = ambientStrength * light.color;\n"
            "\n"
            "    // Diffuse\n"
            "    vec3 norm = normalize(normal);\n"
            "    float diff = max(dot(norm, lightDir), 0.0);\n"
            "    vec3 diffuse = diff * light.color;\n"
            "\n"
            "    // Specular\n"
            "    vec3 viewDir = normalize(viewPos - fragPos);\n"
            "    vec3 reflectDir = reflect(-lightDir, norm);\n"
            "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);\n"
            "    vec3 specular = spec * light.color;\n"
            "\n"
            "    // Attenuation\n"
            "    float distance    = length(light.position - fragPos);\n"
            "    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));\n"
            "\n"
            "    // ambient  *= attenuation;  // Also remove attenuation from ambient, because if we move too far, the light in spotlight would then be darker than outside (since outside spotlight we have ambient lighting).\n"
            "    diffuse  *= attenuation;\n"
            "    specular *= attenuation;\n"
            "\n"
            "    diffuse *= intensity;\n"
            "    specular*= intensity;\n"
            "\n"
            "    vec3 finalColor = (ambient + diffuse + specular) * vec3(objectColor);\n"
            "    outColor = vec4(finalColor, 1.0f);\n"
            "}";

    char fFrameShaderStr[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "\n"
            "layout(location = 0) out vec4 outColor;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
            "}";


    GLuint program = esLoadProgram(vShaderStr, fShaderStr);
    if (!program) {
        return;
    }
    userData->program = program;
    userData->mMVPMatrixLoc = glGetUniformLocation(program, "u_MVPMatrix");
    userData->mModelMatrixLoc = glGetUniformLocation(program, "u_ModelMatrix");
    userData->textureLoc = glGetUniformLocation(program, "s_TextureMap");
    userData->viewPosLoc = glGetUniformLocation(program, "viewPos");
    userData->lightPositionLoc = glGetUniformLocation(program, "light.position");
    userData->lightDirectionLoc = glGetUniformLocation(program, "light.direction");
    userData->lightColorLoc = glGetUniformLocation(program, "light.color");
    userData->lightCutOffLoc = glGetUniformLocation(program, "light.cutOff");
    userData->lightOuterCutOffLoc = glGetUniformLocation(program, "light.outerCutOff");
    userData->lightConstantLoc = glGetUniformLocation(program, "light.constant");
    userData->lightLinearLoc = glGetUniformLocation(program, "light.linear");
    userData->lightQuadraticLoc = glGetUniformLocation(program, "light.quadratic");


    GLuint frameProgram = esLoadProgram(vShaderStr, fFrameShaderStr);
    if (!frameProgram) {
        return;
    }
    userData->frameProgram = frameProgram;
    userData->frameMVPMatrixLoc = glGetUniformLocation(frameProgram, "u_MVPMatrix");

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

    // 使用VBO保存数据
    glGenBuffers(1, &userData->vboId);
    glBindBuffer(GL_ARRAY_BUFFER, userData->vboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);


    // 使用VAO记录设置过程
    glGenVertexArrays(1, &userData->vaoId);
    glBindVertexArray(userData->vaoId);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboId);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (void *)( 5 * sizeof(GLfloat) ));
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindVertexArray(GL_NONE);

    // 创建一个纹理并加载图片
    glGenTextures(1, &userData->imageTexture);
    glBindTexture(GL_TEXTURE_2D, userData->imageTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    auto *image = static_cast<NativeImage *>(esContext->imageData[0]);
    if(image && image->format == IMAGE_FORMAT_RGBA){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height,
                0, GL_RGBA, GL_UNSIGNED_BYTE, image->ppPlane[0]);
    }
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

}

void StencilTestingUnInit(ESContext *esContext) {
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
    if(userData->frameProgram){
        glDeleteProgram(userData->frameProgram);
        userData->frameProgram = 0;
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

void StencilTestingUpdateMatrix(ESContext *esContext, float scale){
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        return;
    }
    float ratio = 1.0f * esContext->width / esContext->height;

    int angleX = userData->angleX % 360;
    int angleY = userData->angleY % 360;

    // 计算弧度
    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);

    // 裁剪坐标
    glm::mat4 Projection = glm::perspective(45.f, ratio, 0.1f, 100.f);

    // 观察坐标
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 3),
            glm::vec3(0, 0, 0),
            glm::vec3(0, 1, 0)
            );

    // 世界坐标
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(scale, scale, scale));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.f, 0.f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.f, 0.f));
    Model = glm::translate(Model, glm::vec3(0.0f, 0.f, 0.f));

    userData->mMVPMatrix = Projection * View * Model;
    userData->mModelMatrix = Model;
}

void StencilTestingDraw(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData || !userData->program || !userData->frameProgram){
        return;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(1.f, 1.f, 1.f, 0.f);

    // 设置模块记录方式
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilMask(0xFF);

    // 绘制立方体
    glUseProgram(userData->program);
    glBindVertexArray(userData->vaoId);

    // 加载纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->imageTexture);
    glUniform1i(userData->textureLoc, 0);

    // 设置变换矩阵
    StencilTestingUpdateMatrix(esContext, 1.0f);
    glUniformMatrix4fv(userData->mMVPMatrixLoc, 1, GL_FALSE, &userData->mMVPMatrix[0][0]);
    glUniformMatrix4fv(userData->mModelMatrixLoc, 1, GL_FALSE, &userData->mModelMatrix[0][0]);

    // 设置观察坐标
    glUniform3f(userData->viewPosLoc, 0.f, 0.f, 3.f);

    // 设置光源位置、颜色、方向
    glUniform3f(userData->lightPositionLoc, 0.f, 0.f, 3.f);
    glUniform3f(userData->lightColorLoc, 1.f, 1.f, 1.f);
    glUniform3f(userData->lightDirectionLoc, 0.f, 0.f, -1.f);

    // 用于计算边缘的过度，cutOff 表示内切光角，outerCutOff 表示外切光角
    glUniform1f(userData->lightCutOffLoc, glm::cos(glm::radians(10.5f)));
    glUniform1f(userData->lightOuterCutOffLoc, glm::cos(glm::radians(11.5f)));

    // 衰减系数,常数项 constant，一次项 linear 和二次项 quadratic。
    glUniform1f(userData->lightConstantLoc,  1.0f);
    glUniform1f(userData->lightLinearLoc,    0.09);
    glUniform1f(userData->lightQuadraticLoc, 0.032);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    GO_CHECK_GL_ERROR();
    glBindVertexArray(GL_NONE);

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);

    // 绘制边框
    glBindVertexArray(userData->vaoId);
    glUseProgram(userData->frameProgram);
    StencilTestingUpdateMatrix(esContext, 1.05f);
    glUniformMatrix4fv(userData->frameMVPMatrixLoc, 1, GL_FALSE, &userData->mMVPMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(GL_NONE);

    //glStencilMask(0xFF);
    glEnable(GL_DEPTH_TEST);
}

void StencilTestingUpdateTransformMatrix(ESContext *esContext, GLfloat rotateX, GLfloat rotateY,
                                         GLfloat scaleX, GLfloat scaleY) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        return;
    }
    userData->angleX = rotateX;
    userData->angleY = rotateY;
}


}