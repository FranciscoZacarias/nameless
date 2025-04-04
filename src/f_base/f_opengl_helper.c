internal OGL_Shader ogl_make_shader(String source_path, GLenum kind) {
  Arena_Temp scratch = scratch_begin(0, 0);
  OGL_Shader result  = glCreateShader(kind);
  GLenum error       = glGetError();
  if (error != GL_NO_ERROR) {
    printf("Error %u in glCreateShader.\n", error);
  }
    
  File_Data shader_source = file_load(scratch.arena, source_path);
  glShaderSource(result, 1, &shader_source.data, &(GLint)shader_source.size);
  glCompileShader(result);
  
  GLint success;
  glGetShaderiv(result, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[1024];
    glGetShaderInfoLog(result, 1024, NULL, infoLog);
    printf("Error %d while compiling shader. Log: %s", success, infoLog);
    glDeleteShader(result);
    ERROR_MESSAGE_AND_EXIT("Error creating shader");
  }

  scratch_end(&scratch);
  return result;
}

internal OGL_Shader ogl_make_program(GLuint *shaders, u32 count) {
  OGL_Shader result = glCreateProgram();
  
  for (u32 i = 0; i < count; i += 1){
    glAttachShader(result, shaders[i]);
  }
  glLinkProgram(result);
    
  GLint success;
  glGetProgramiv(result, GL_LINK_STATUS, &success);
  if(!success) {
    char infoLog[1024];
    glGetProgramInfoLog(result, 1024, NULL, infoLog);
    printf("Error %d linking compiled shader program. Log: %s", success, infoLog);
    for (u32 i = 0; i < count; i += 1){
      glDeleteShader(shaders[i]);
    }
    ERROR_MESSAGE_AND_EXIT("Error creating shader program");
  }
  
  return result;
}

internal void renderer_set_uniform_mat4fv(u32 program, const char* uniform, Mat4f32 mat) {
  s32 uniform_location = glGetUniformLocation(program, uniform);
  if (uniform_location == -1) {
    printf("Mat4f32 :: Uniform %s not found\n", uniform);
  }
  glUniformMatrix4fv(uniform_location, 1, 1, &mat.data[0][0]);
}
