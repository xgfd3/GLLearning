# GLLearning
  OpenGL在Android上的使用范例，包含GLSample、Camera采集、Camera渲染滤镜。native层主要使用c语言编写，基于cmake编译，可以在AndroidStudio里直接编写c代码。

# 代码结构
## GLSample
![GLSample](https://github.com/xgfd3/GLLearning/blob/master/doc/GLSample.png)
## Camera采集和滤镜
![GLCamera](https://github.com/xgfd3/GLLearning/blob/master/doc/GLCamear.png)

# 范例
## GLSample
- [三角形](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/samples/Triangle.c)
- [TextureMap加载png图片到纹理](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/samples/TextureMap.c)
- [YUVTextureMap加载yuv数据到纹理](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/samples/YUVTextureMap.c)
- [VBO使用](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/samples/VBO.c)
- [VAO使用](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/samples/VAO.c)
- [FBO使用](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/samples/FBO.c)
- [FBO使用](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/samples/FBO.c)
- [EGL滤镜-马赛克/网格/旋转/边缘/放大/形变](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/samples/EGL.c)
- [TransformFeedback变换反馈](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/samples/TransformFeedback.c)
- [CoordinateSystem坐标系统](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/samples/CoordSystem.cpp)
- [基础光照](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/samples/BasicLighting.cpp)
- [深度测试](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/samples/DepthTesting.cpp)
- [模板测试](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/samples/StencilTesting.cpp)
- [混合](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/samples/Blending.cpp)
- [实例化](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/samples/Instancing.cpp)
- [实例化3D](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/samples/Instancing3D.cpp)
- [粒子效果](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/samples/Particles.cpp)
- [天空盒](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/samples/SkyBox.cpp)

## Camera采集和滤镜
- [Camera采集-前置/后置](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/java/com/xucz/opengldemo/camera/CameraBase.java)
- [Camera采集-输出OES纹理/2D纹理](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/java/com/xucz/opengldemo/camera/CameraCapture.java)
- [Camera滤镜-网格](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/camera/Grid.cpp)
- [Camera滤镜-分屏](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/camera/SplitScreen.cpp)
- [Camera滤镜-缩放的圆](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/camera/ScaleCircle.cpp)
- [Camera滤镜-LUT](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/camera/LUTFilter.cpp)
- [Camera滤镜-分色偏移](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/camera/SeparationShift.cpp)
- [Camera滤镜-灵魂出窍](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/camera/SoulOut.cpp)
- [Camera滤镜-旋转的圆](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/camera/RotateCircle.cpp)
- [Camera滤镜-画中画](https://github.com/xgfd3/GLLearning/blob/master/app/src/main/cpp/camera/PictInPict.cpp)


# 参考
- [OpenGLES极简教程](https://mp.weixin.qq.com/mp/appmsgalbum?__biz=MzIwNTIwMzAzNg==&action=getalbum&album_id=1340922292116996097&subscene=159&subscene=158&scenenote=https%3A%2F%2Fmp.weixin.qq.com%2Fs%3F__biz%3DMzIwNTIwMzAzNg%3D%3D%26mid%3D2654161543%26idx%3D1%26sn%3Dbb69fdacc5d9e85e4dea8e87e29abd66%26chksm%3D8cf399b4bb8410a249167518e518798a7bc29b0e9c5d1343876928ff0cf5cd794f0878c73789%26scene%3D158%23rd#wechat_redirect)







