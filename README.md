# Apollo

Windows 上的 OpenGL 2D 渲染练习。工程骨架参考 [Hazel](https://github.com/TheCherno/Hazel)，这一阶段补的是贴图、合批，和一个能走两步的小场景。

## 这一阶段有什么

- `Shader` 基类可以直接传 uniform。渲染代码不再把着色器转成 `OpenGLShader`。
- PNG 用 Windows 自带的 WIC 读进来，再上传到 GPU。`OpenGLTexture2D` 不再是空函数。
- `Renderer2D` 把四边形攒成一批再画。整张地板共用一张贴图时，合批后是 1 次 draw call。
- Sandbox 里用 WASD 移动，方块会挡住路，走到青色格子算到达终点。面板上可以关掉合批，draw call 会变成和四边形数量一样。

## 操作

- WASD：移动
- Q/E：旋转相机
- 面板里的「关闭合批」：用来对比合批前后的 draw call

## 编译

用 Visual Studio 2022 打开 `Apollo.sln`，启动项目选 Sandbox，编译运行。程序会把工作目录切到 exe 旁边，贴图从 `assets/textures` 读。

第一次编译前要先拉子模块（GLFW、glm、ImGui、spdlog）：

```
git submodule update --init
```

如果要重新生成解决方案，仓库根目录的 `GenerateProjects.bat` 会调用 `vendor/bin/premake5/premake5.exe`。这个可执行文件没有提交在仓库里，需要自己放一份 Premake 5。
