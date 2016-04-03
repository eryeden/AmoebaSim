#version 330 core

//ポストプロセシング用フラグメントシェーダー
//ここで多くのポストプロセスを行う
//現在はレンダリングされた結果を格納しているテクスチャをそのまま描画している
//uniform sampler2D fbo_texture;
//in vec2 f_texcoord;

//void main(void) {
//  gl_FragColor = texture2D(fbo_texture, f_texcoord);
//}


in vec2 UV;
out vec3 color;

uniform sampler2D renderedTexture;

void main(){
	color = texture( renderedTexture, UV).xyz ;
}


