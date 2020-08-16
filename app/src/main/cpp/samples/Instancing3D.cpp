//
// Created by xucz on 2020/8/14.
//

#include "Instancing3D.h"

extern "C" {

typedef struct __UserData {

    GLuint program;
    GLuint u_MVPMatrixLoc;
    GLuint u_ModelMatrixLoc;
    GLuint s_TextureMapLoc;
    GLuint viewPosLoc;
    GLuint lightPositionLoc;
    GLuint lightDirectionLoc;
    GLuint lightColorLoc;
    GLuint lightCutOffLoc;
    GLuint lightOuterCutOffLoc;
    GLuint lightConstantLoc;
    GLuint lightLinearLoc;
    GLuint lightQuadraticLoc;

    GLuint vboIds[2];
    GLuint vaoId;

    GLuint imageTexture;

    glm::mat4 m_MVPMatrix;
    glm::mat4 m_ModelMatrix;
    int angleX;
    int angleY;
    float scaleX;
    float scaleY;

} UserData;

void Instancing3DInit(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if (!userData) {
        userData = new UserData();
        esContext->userData = userData;
    }
    userData->scaleY = userData->scaleX = 2.0f;

    // 编写着色器
    char vShaderStr[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "layout(location = 0) in vec4 a_position;\n"
            "layout(location = 1) in vec2 a_texCoord;\n"
            "layout(location = 2) in vec3 a_normal;\n"
            "layout(location = 3) in vec3 offset;\n"
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
            "    gl_Position = u_MVPMatrix * (a_position + vec4(offset, 1.0));\n"
            "    fragPos = vec3(u_ModelMatrix * (a_position + vec4(offset, 1.0)));\n"
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
            "    vec3 lightDir = normalize(light.position - fragPos);\n"
            "    // Check if lighting is inside the spotlight cone\n"
            "    float theta = dot(lightDir, normalize(-light.direction));\n"
            "    float epsilon = light.cutOff - light.outerCutOff;\n"
            "    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);\n"
            "    // Ambient\n"
            "    float ambientStrength = 0.4;\n"
            "    vec3 ambient = ambientStrength * light.color;\n"
            "    // Diffuse\n"
            "    vec3 norm = normalize(normal);\n"
            "    float diff = max(dot(norm, lightDir), 0.0);\n"
            "    vec3 diffuse = diff * light.color;\n"
            "    // Specular\n"
            "    vec3 viewDir = normalize(viewPos - fragPos);\n"
            "    vec3 reflectDir = reflect(-lightDir, norm);\n"
            "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);\n"
            "    vec3 specular = spec * light.color;\n"
            "    // Attenuation\n"
            "    float distance = length(light.position - fragPos);\n"
            "    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));\n"
            "    diffuse *= attenuation;\n"
            "    specular *= attenuation;\n"
            "    diffuse *= intensity;\n"
            "    specular *= intensity;\n"
            "    vec3 finalColor = (ambient + diffuse + specular) * vec3(objectColor);\n"
            "    outColor = vec4(finalColor, 1.0f);\n"
            "}";


    GLuint program = esLoadProgram(vShaderStr, fShaderStr);
    if (!program) {
        return;
    }
    userData->program = program;
    userData->u_MVPMatrixLoc = glGetUniformLocation(program, "u_MVPMatrix");
    userData->u_ModelMatrixLoc = glGetUniformLocation(program, "u_ModelMatrix");
    userData->s_TextureMapLoc = glGetUniformLocation(program, "s_TextureMap");
    userData->viewPosLoc = glGetUniformLocation(program, "viewPos");
    userData->lightPositionLoc = glGetUniformLocation(program, "light.position");
    userData->lightDirectionLoc = glGetUniformLocation(program, "light.direction");
    userData->lightColorLoc = glGetUniformLocation(program, "light.color");
    userData->lightCutOffLoc = glGetUniformLocation(program, "light.cutOff");
    userData->lightOuterCutOffLoc = glGetUniformLocation(program, "light.outerCutOff");
    userData->lightConstantLoc = glGetUniformLocation(program, "light.constant");
    userData->lightLinearLoc = glGetUniformLocation(program, "light.linear");
    userData->lightQuadraticLoc = glGetUniformLocation(program, "light.quadratic");

    // 确定顶点坐标，纹理坐标，法向量坐标，偏移坐标，
    GLfloat vertices[] = {
            //position                //texture coord  //normal
            -0.08f, -0.08f, -0.08f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
            0.08f, -0.08f, -0.08f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
            0.08f, 0.08f, -0.08f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
            0.08f, 0.08f, -0.08f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
            -0.08f, 0.08f, -0.08f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
            -0.08f, -0.08f, -0.08f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,

            -0.08f, -0.08f, 0.08f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.08f, -0.08f, 0.08f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.08f, 0.08f, 0.08f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            0.08f, 0.08f, 0.08f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -0.08f, 0.08f, 0.08f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -0.08f, -0.08f, 0.08f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

            -0.08f, 0.08f, 0.08f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            -0.08f, 0.08f, -0.08f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
            -0.08f, -0.08f, -0.08f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
            -0.08f, -0.08f, -0.08f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
            -0.08f, -0.08f, 0.08f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            -0.08f, 0.08f, 0.08f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

            0.08f, 0.08f, 0.08f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.08f, 0.08f, -0.08f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            0.08f, -0.08f, -0.08f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            0.08f, -0.08f, -0.08f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            0.08f, -0.08f, 0.08f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.08f, 0.08f, 0.08f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

            -0.08f, -0.08f, -0.08f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
            0.08f, -0.08f, -0.08f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
            0.08f, -0.08f, 0.08f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
            0.08f, -0.08f, 0.08f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
            -0.08f, -0.08f, 0.08f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
            -0.08f, -0.08f, -0.08f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,

            -0.08f, 0.08f, -0.08f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            0.08f, 0.08f, -0.08f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            0.08f, 0.08f, 0.08f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.08f, 0.08f, 0.08f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.08f, 0.08f, 0.08f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.08f, 0.08f, -0.08f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    };

    glm::vec3 translations[125];
    int index = 0;
    GLfloat offset = 0.2f;
    for (GLint y = -10; y < 10; y += 4) {
        for (GLint x = -10; x < 10; x += 4) {
            for (GLint z = -10; z < 10; z += 4) {
                glm::vec3 translation;
                translation.x = (GLfloat) x / 10.0f + offset;
                translation.y = (GLfloat) y / 10.0f + offset;
                translation.z = (GLfloat) z / 10.0f + offset;
                translations[index++] = translation;
            }
        }
    }

    // 使用VBO缓存数据
    glGenBuffers(2, userData->vboIds);
    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, 125 * sizeof(glm::vec3), translations, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    // 使用VAO记录
    glGenVertexArrays(1, &userData->vaoId);
    glBindVertexArray(userData->vaoId);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (void *) (3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (void *) (5 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[1]);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    // 实例化变量配置
    glVertexAttribDivisor(3, 1);

    glBindVertexArray(GL_NONE);

    // 生成一个图片纹理，并加载图片
    glGenTextures(1, &userData->imageTexture);
    glBindTexture(GL_TEXTURE_2D, userData->imageTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    auto *image = static_cast<NativeImage *>(esContext->imageData[0]);
    if (image && image->format == IMAGE_FORMAT_RGBA) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width, image->height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, image->ppPlane[0]);
    }
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

}

void Instancing3DUnInit(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if (!userData) {
        return;
    }
    if (userData->program) {
        glDeleteProgram(userData->program);
        userData->program = GL_NONE;
    }
    if (userData->vboIds[0]) {
        glDeleteBuffers(2, userData->vboIds);
        userData->vboIds[0] = GL_NONE;
    }
    if (userData->vaoId) {
        glDeleteVertexArrays(1, &userData->vaoId);
        userData->vaoId = GL_NONE;
    }
    if (userData->imageTexture) {
        glDeleteTextures(1, &userData->imageTexture);
        userData->imageTexture = GL_NONE;
    }
    delete userData;
    esContext->userData = nullptr;
}

void Instancing3DUpdateMatrix(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if (!userData) {
        return;
    }
    int angleX = (userData->angleX + 10) % 360;
    int angleY = (userData->angleY + 10) % 360;
    float scaleX = userData->scaleX;
    float scaleY = userData->scaleY;
    float scale = scaleX > scaleY ? scaleX : scaleY;
    float ratio = 1.0f * esContext->width / esContext->height;

    // 角度转成弧度
    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);

    // Projection Matix
    glm::mat4 Projection = glm::perspective(45.f, ratio, 0.1f, 100.f);

    // View Matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 3),
            glm::vec3(0, 0, 0),
            glm::vec3(0, 1, 0)
    );

    // Model Matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(scale, scale, scale));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0, 0));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0));

    userData->m_ModelMatrix = Model;
    userData->m_MVPMatrix = Projection * View * Model;
}

void Instancing3DDraw(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if (!userData || !userData->program) {
        return;
    }

    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(1.f, 1.f, 1.f, 0.f);

    glEnable(GL_DEPTH_TEST);

    glUseProgram(userData->program);

    glBindVertexArray(userData->vaoId);

    // 变换矩阵
    Instancing3DUpdateMatrix(esContext);
    glUniformMatrix4fv(userData->u_MVPMatrixLoc, 1, GL_FALSE, &userData->m_MVPMatrix[0][0]);
    glUniformMatrix4fv(userData->u_ModelMatrixLoc, 1, GL_FALSE, &userData->m_ModelMatrix[0][0]);

    // 加载纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->imageTexture);
    glUniform1i(userData->s_TextureMapLoc, 0);

    // 设置观察坐标
    glUniform3f(userData->viewPosLoc, 0.f, 0.f, 3.f);

    // 设置光源，颜色和方向
    glUniform3f(userData->lightPositionLoc, 0.f, 0.f, 3.f);
    glUniform3f(userData->lightColorLoc, 1.f, 1.f, 1.f);
    glUniform3f(userData->lightDirectionLoc, 0.f, 0.f, -1.f);

    // 用于计算边缘的过度，cutOff 表示内切光角，outerCutOff 表示外切光角
    glUniform1f(userData->lightCutOffLoc, glm::cos(glm::radians(10.5f)));
    glUniform1f(userData->lightOuterCutOffLoc, glm::cos(glm::radians(11.5f)));


    // 衰减系数,常数项 constant，一次项 linear 和二次项 quadratic
    glUniform1f(userData->lightConstantLoc, 1.f);
    glUniform1f(userData->lightLinearLoc, 0.09f);
    glUniform1f(userData->lightQuadraticLoc, 0.032);

    // 绘制
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 125);
    GO_CHECK_GL_ERROR();

    glBindVertexArray(GL_NONE);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void Instancing3DUpdateTransformMatrix(ESContext *esContext, GLfloat angleX, GLfloat angleY,
                                       GLfloat scaleX, GLfloat scaleY) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if (!userData) {
        return;
    }
    userData->angleY = angleY;
    userData->angleX = angleX;
    userData->scaleX = 2 * scaleX;
    userData->scaleY = 2 * scaleY;
}


}