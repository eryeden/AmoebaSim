#version 400 core

/*
どうやらポストプロセスとか言ってレンダリングが終了しフレームバッファに結果が書き込まれた後に呼ばれる処理らしい
なので、ここでいうテクスチャとは、フレームバッファのことなのではないか
俺が今まで考えていたシェーダーとは別のところで動いているらしい

ポストプロセシングの流れ

バーテックスバッファ、フラグメントバッファは与えられた頂点における頂点情報、色情報しか知ることはできないが、
テクスチャにレンダリングすることでシェーダーがレンダリング結果の全域にアクセスすることができるようになる。

そのため、まずフレームバッファオブジェクトを用いてレンダリング提唱をテクスチャとして、そのテクスチャに対して
ポストフィルタプロセスを適用する。

流れ
＃１
	フレームバッファオブジェクトを生成し、これを通して深度バッファをレンダバッファに保存、
	カラーバッファをテクスチャに保存する
	３Dシーンをレンダリングするならば深度バッファは必要

	GL_CLAMP_TO_EDGEを指定、GL_REPEATによるボーダーラッピング効果を防ぐため



*/

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

uniform float rt_w;//フレーム幅
uniform float rt_h;//フレーム高さ

// Output data ; will be interpolated for each fragment.
out vec2 win_size;

void main(){
	gl_Position =  vec4(vertexPosition_modelspace,1);
	win_size = vec2(rt_w, rt_h);
}


