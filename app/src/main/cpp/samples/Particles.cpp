//
// Created by xucz on 2020/8/16.
//

#include "Particles.h"

extern "C" {

// 最大粒子数
#define MAX_PARTICLES 500

typedef struct __Particle {
    GLfloat dx, dy, dz; // offset
    GLfloat dxSpeed, dySpeed, dzSpeed;
    GLubyte r, g, b, a;
    GLfloat life;
    GLfloat cameraDistance;
} Particle;

typedef struct __UserData {

    GLuint program;
    GLuint u_MVPMatrixLoc;
    GLuint s_TextureMapLoc;

    GLuint vboIds[3];
    GLuint vaoId;
    
    GLuint imageTexture;

    glm::mat4 m_MVPMatrix;

    int angleX;
    int angleY;
    float scaleX;
    float scaleY;

    Particle m_ParticlesContainer[MAX_PARTICLES];
    GLfloat* m_pParticlesPosData;
    GLubyte* m_pParticlesColorData;
    int m_LastUsedParticle;

} UserData;

void ParticlesGenerateNewParticle(Particle *particle){
    if(!particle){
       return;
    }

    particle->life = 5.0f;
    particle->cameraDistance = -1.0f;
    particle->dx = (rand() % 2000 - 1000.0f) / 3000.0f;
    particle->dy = (rand() % 2000 - 1000.0f) / 3000.0f;
    particle->dz = (rand() % 2000 - 1000.0f) / 3000.0f;

    float spread = 1.5f;

    glm::vec3 maindir = glm::vec3(0.0f, 2.0f, 0.0f);
    glm::vec3 randomdir = glm::vec3(
            (rand() % 2000 - 1000.0f) / 1000.0f,
            (rand() % 2000 - 1000.0f) / 1000.0f,
            (rand() % 2000 - 1000.0f) / 1000.0f
            );

    glm::vec3 speed = maindir + randomdir * spread;
    particle->dxSpeed = speed.x;
    particle->dySpeed = speed.y;
    particle->dzSpeed = speed.z;

    particle->r = static_cast<unsigned char>(rand()% 256);
    particle->g = static_cast<unsigned char>(rand()% 256);
    particle->b = static_cast<unsigned char>(rand()% 256);
    particle->a = static_cast<unsigned char>((rand()% 256) / 3);
}

int ParticleFindUnusedParticle(ESContext *esContext){
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        return -1;
    } 
    for(int i = userData->m_LastUsedParticle; i < MAX_PARTICLES; i++){
        if(userData->m_ParticlesContainer[i].life <= 0){
            userData->m_LastUsedParticle = i;
            return i;
        }
    }
    for(int i = 0; i < userData->m_LastUsedParticle; i++){
        if(userData->m_ParticlesContainer[i].life <= 0){
            userData->m_LastUsedParticle = i;
            return i;
        }
    }
    
    return -1;
}

int ParticleUpdateParticles(ESContext *esContext){
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        return 0;
    }
    
    int newParticles = 300;
    for(int i = 0; i < newParticles; i++){
        int particleIndex = ParticleFindUnusedParticle(esContext);
        if(particleIndex >= 0){
            ParticlesGenerateNewParticle(&userData->m_ParticlesContainer[particleIndex]);
        }
    }
    
    
    // 模拟所有粒子
    int particlesCount = 0;
    for(int i = 0; i < MAX_PARTICLES; i++){
        // ！！注意，不能写成Particle p = &userData->m_ParticlesContainer[i];，这里的p已是一个新实例，非容器里的实例了
        Particle *p = &userData->m_ParticlesContainer[i];
        if(p->life > 0.0f){
            float delta = 0.1f;
            // Decrease life
            glm::vec3 speed = glm::vec3(p->dxSpeed, p->dySpeed, p->dzSpeed);
            glm::vec3 pos = glm::vec3(p->dx, p->dy, p->dz);
            
            p->life -= delta;
            if(p->life > 0.0f){
                speed += glm::vec3(0.0f, 0.081f, 0.0f) * delta * 0.5f;
                pos += speed * delta;
                
                p->dxSpeed = speed.x;
                p->dySpeed = speed.y;
                p->dzSpeed = speed.z;

                p->dx = pos.x;
                p->dy = pos.y;
                p->dz = pos.z;
                
                userData->m_pParticlesPosData[3 * particlesCount + 0] = p->dx;
                userData->m_pParticlesPosData[3 * particlesCount + 1] = p->dy;
                userData->m_pParticlesPosData[3 * particlesCount + 2] = p->dz;
                
                userData->m_pParticlesColorData[ 4 * particlesCount + 0] = p->r;
                userData->m_pParticlesColorData[ 4 * particlesCount + 1] = p->g;
                userData->m_pParticlesColorData[ 4 * particlesCount + 2] = p->b;
                userData->m_pParticlesColorData[ 4 * particlesCount + 3] = p->a;
            }
            particlesCount++;
        }
    }
    
    // 更新VBO
    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 3 * sizeof(GLfloat), 
            NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particlesCount * 3 * sizeof(GLfloat),
            userData->m_pParticlesPosData);
    
    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[2]);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte),
            NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particlesCount * 4 * sizeof(GLubyte),
            userData->m_pParticlesColorData);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    
    return particlesCount;
}

void ParticlesInit(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        userData = new UserData();
        userData->scaleX = userData->scaleY = 1.0f;
        esContext->userData = userData;
    }
    

    // 编译着色器
    char vShaderStr[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "layout(location = 0) in vec3 a_vertex;\n"
            "layout(location = 1) in vec2 a_texCoord;\n"
            "layout(location = 2) in vec3 a_offset;\n"
            "layout(location = 3) in vec4 a_particlesColor;\n"
            "uniform mat4 u_MVPMatrix;\n"
            "out vec2 v_texCoord;\n"
            "out vec4 v_color;\n"
            "void main()\n"
            "{\n"
            "    gl_Position = u_MVPMatrix * vec4(a_vertex - vec3(0.0, 0.95, 0.0) + a_offset, 1.0);\n"
            "    v_texCoord = a_texCoord;\n"
            "    v_color = a_particlesColor;\n"
            "}"
            ;

    char fShaderStr[] =
            "#version 300 es\n"
            "precision mediump float;\n"
            "in vec2 v_texCoord;\n"
            "in vec4 v_color;\n"
            "layout(location = 0) out vec4 outColor;\n"
            "uniform sampler2D s_TextureMap;\n"
            "void main()\n"
            "{\n"
            "    outColor = texture(s_TextureMap, v_texCoord) * v_color;\n"
            "}"
            ;

    GLuint program = esLoadProgram(vShaderStr, fShaderStr);
    if(!program){
       return;
    }
    userData->program = program;
    userData->u_MVPMatrixLoc = glGetUniformLocation(program, "u_MVPMatrix");
    userData->s_TextureMapLoc = glGetUniformLocation(program, "s_TextureMap");

    //生成粒子数据
    userData->m_pParticlesPosData = new GLfloat[MAX_PARTICLES * 3];
    userData->m_pParticlesColorData = new GLubyte[MAX_PARTICLES * 4];

    for(int i = 0; i < MAX_PARTICLES; i++){
        ParticlesGenerateNewParticle(&userData->m_ParticlesContainer[i]);
    }

    // 确定顶点坐标和纹理坐标
    static const GLfloat g_vertex_buffer_data[] = {
            //position                     //texture coord
            -0.05f, -0.05f, -0.05f,        0.0f, 0.0f,
             0.05f, -0.05f, -0.05f,        1.0f, 0.0f,
             0.05f,  0.05f, -0.05f,        1.0f, 1.0f,
             0.05f,  0.05f, -0.05f,        1.0f, 1.0f,
            -0.05f,  0.05f, -0.05f,        0.0f, 1.0f,
            -0.05f, -0.05f, -0.05f,        0.0f, 0.0f,

            -0.05f, -0.05f,  0.05f,        0.0f, 0.0f,
             0.05f, -0.05f,  0.05f,        1.0f, 0.0f,
             0.05f,  0.05f,  0.05f,        1.0f, 1.0f,
             0.05f,  0.05f,  0.05f,        1.0f, 1.0f,
            -0.05f,  0.05f,  0.05f,        0.0f, 1.0f,
            -0.05f, -0.05f,  0.05f,        0.0f, 0.0f,

            -0.05f,  0.05f,  0.05f,        1.0f, 0.0f,
            -0.05f,  0.05f, -0.05f,        1.0f, 1.0f,
            -0.05f, -0.05f, -0.05f,        0.0f, 1.0f,
            -0.05f, -0.05f, -0.05f,        0.0f, 1.0f,
            -0.05f, -0.05f,  0.05f,        0.0f, 0.0f,
            -0.05f,  0.05f,  0.05f,        1.0f, 0.0f,

             0.05f,  0.05f,  0.05f,        1.0f, 0.0f,
             0.05f,  0.05f, -0.05f,        1.0f, 1.0f,
             0.05f, -0.05f, -0.05f,        0.0f, 1.0f,
             0.05f, -0.05f, -0.05f,        0.0f, 1.0f,
             0.05f, -0.05f,  0.05f,        0.0f, 0.0f,
             0.05f,  0.05f,  0.05f,        1.0f, 0.0f,

            -0.05f, -0.05f, -0.05f,        0.0f, 1.0f,
             0.05f, -0.05f, -0.05f,        1.0f, 1.0f,
             0.05f, -0.05f,  0.05f,        1.0f, 0.0f,
             0.05f, -0.05f,  0.05f,        1.0f, 0.0f,
            -0.05f, -0.05f,  0.05f,        0.0f, 0.0f,
            -0.05f, -0.05f, -0.05f,        0.0f, 1.0f,

            -0.05f,  0.05f, -0.05f,        0.0f, 1.0f,
             0.05f,  0.05f, -0.05f,        1.0f, 1.0f,
             0.05f,  0.05f,  0.05f,        1.0f, 0.0f,
             0.05f,  0.05f,  0.05f,        1.0f, 0.0f,
            -0.05f,  0.05f,  0.05f,        0.0f, 0.0f,
            -0.05f,  0.05f, -0.05f,        0.0f, 1.0f,
    };

    // 生成VBO缓存 坐标数据，偏移量和色值
    glGenBuffers(3, userData->vboIds);

    // 缓存数据或初始化VBO容量
    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 3 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[2]);
    glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    // 生成VAO记录赋值过程
    glGenVertexArrays(1, &userData->vaoId);
    // !! 注意要先绑定
    glBindVertexArray(userData->vaoId);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[1]);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[2]);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, 0);
    GO_CHECK_GL_ERROR();

    // 设置实例变量
    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 0);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);

    glBindVertexArray(GL_NONE);
    GO_CHECK_GL_ERROR();
    
    // 加载纹理
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

void ParticlesUnInit(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        return;
    }
    if(userData->program){
        glDeleteProgram(userData->program);
        userData->program = GL_NONE;
    }
    if(userData->vboIds[0]){
        glDeleteBuffers(3, userData->vboIds);
        userData->vboIds[0] = GL_NONE;
    }
    if(userData->vaoId){
        glDeleteVertexArrays(1, &userData->vaoId);
        userData->vaoId = GL_NONE;
    }
    if(userData->imageTexture){
        glDeleteTextures(1, &userData->imageTexture);
        userData->imageTexture = GL_NONE;
    }
    if(userData->m_pParticlesColorData){
        delete(userData->m_pParticlesColorData);
        userData->m_pParticlesColorData = nullptr;
    }
    if(userData->m_pParticlesPosData){
        delete userData->m_pParticlesPosData;
        userData->m_pParticlesPosData = nullptr;
    }
    delete userData;
    esContext->userData = nullptr;
}

void ParticleUpdateMatrix(ESContext *esContext){
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        return;
    }
    float ratio = 1.0f * esContext->width / esContext->height;
    
    int angleX = userData->angleX % 360;
    int angleY = userData->angleY % 360;
    float scale = userData->scaleX;
    
    //转化为弧度角
    float radiansX = MATH_PI / 80.f * angleX;
    float radiansY = MATH_PI / 80.f * angleY;
    
    glm::mat4 Projection = glm::perspective(45.f, ratio, 0.1f, 100.f);
    
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 6, 0),
            glm::vec3(0, 0, 0),
            glm::vec3(0, 0, 1)
            );
    
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(scale, scale, scale));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.f, 0.f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.f, 1.f, 0.f));
    Model = glm::translate(Model, glm::vec3(0.f, 0.f, 0.f));
    
    userData->m_MVPMatrix = Projection * View * Model;
}

void ParticlesDraw(ESContext *esContext) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData || !userData->program){
        return;
    }
    
    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(1.f, 1.f, 1.f, 0.f);
    
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    // 更新粒子
    int particleCount = ParticleUpdateParticles(esContext);
    
    glUseProgram(userData->program);
    glBindVertexArray(userData->vaoId);
    
    // 加载纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->imageTexture);
    glUniform1i(userData->s_TextureMapLoc, 0);
    
    // 更新变换矩阵
    ParticleUpdateMatrix(esContext);
    glUniformMatrix4fv(userData->u_MVPMatrixLoc, 1, GL_FALSE, &userData->m_MVPMatrix[0][0]);
    
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, particleCount);
    GO_CHECK_GL_ERROR();
}

void ParticlesUpdateTransformMatrix(ESContext *esContext, GLfloat rotateX, GLfloat rotateY,
                                    GLfloat scaleX, GLfloat scaleY) {
    auto *userData = static_cast<UserData *>(esContext->userData);
    if(!userData){
        return;
    }
    userData->angleY = rotateY;
    userData->angleX = rotateX;
    userData->scaleX = scaleX;
    userData->scaleY = scaleY;
}

}