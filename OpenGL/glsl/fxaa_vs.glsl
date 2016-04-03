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

// Output data ; will be interpolated for each fragment.
out vec2 UV;
//out vec4 posPos;


uniform float rt_w; //スクリーン幅
uniform float rt_h;//スクリーン高さ
uniform float FXAA_SUBPIX_SHIFT = 0.0;//1.0/4.0;


void main(){
	gl_Position =  vec4(vertexPosition_modelspace,1);
	//UV = (vertexPosition_modelspace.xy+vec2(1,1))/2.0;

	vec2 rcpFrame = vec2(1.0/rt_w, 1.0/rt_h);

//	posPos.xy = gl_MultiTexCoord0; //UV座標であろう
//    posPos.zw = gl_MultiTexCoord0 - (rcpFrame * (0.5 + FXAA_SUBPIX_SHIFT));
}


