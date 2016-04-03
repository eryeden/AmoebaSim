/*
The MIT License (MIT)

Copyright (c) 2015 Kazuki Kikuchi

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

//Widnowsの場合
//#ifdef _MSC_VER
//#pragma comment(lib, "glfw3.lib")
//#pragma comment(lib, "opengl32.lib")
//#pragma comment(lib, "GlU32.Lib")
//#pragma comment(lib, "assimp-vc120-mt.lib")
//#pragma comment(lib, "glew32.lib")
//#include <gl/glew.h>
//#define _CRT_SECURE_NO_WARNINGS
//#endif
//
//
//#include <stdio.h>
//#include <string>
//#include <vector>
//#include <iostream>
//#include <fstream>
//#include <algorithm>
//using namespace std;
//
//#include <cstdlib>
//#include <cstring>
//
//#define GL_GLEXT_PROTOTYPES
////GLEWの拡張を使うには此れ↑が必要（glGenBuffers等）
////Linuxではこれだけで良い　おそらくGLEWのライブラリ不必要
////http://stackoverflow.com/questions/3032386/glgenbuffers-not-defined
//#include <GLFW/glfw3.h>
//
//#include <assimp/Importer.hpp>  // C++ importer interface
//#include <assimp/scene.h>       // Output data structure
//#include <assimp/postprocess.h> // Post processing flags
//
//#include "ogldev_util.h"
//#include "ogldev_math_3d.h"
//#include "ogldev_texture.h"

#include "Importer.hpp"
#define INVALID_MATERIAL 0xFFFFFFFF
#define INVALID_OGL_VALUE 0xFFFFFFFF 

using namespace glm;

MdlImporter::MeshEntry::MeshEntry(){

	VertexBuffer = INVALID_OGL_VALUE;
	NormalBuffer = INVALID_OGL_VALUE;
	ElementBuffer = INVALID_OGL_VALUE;
	ColorBuffer = INVALID_OGL_VALUE;

	NumIndices = 0;
	MaterialIndex = INVALID_MATERIAL;
}

MdlImporter::MeshEntry::~MeshEntry()
{

	if (VertexBuffer != INVALID_OGL_VALUE){
		glDeleteBuffers(1, &VertexBuffer);
	}

	if (NormalBuffer != INVALID_OGL_VALUE){
		glDeleteBuffers(1, &NormalBuffer);
	}

	if (ElementBuffer != INVALID_OGL_VALUE){
		glDeleteBuffers(1, &ElementBuffer);
	}

	if (ColorBuffer != INVALID_OGL_VALUE){
		glDeleteBuffers(1, &ColorBuffer);
	}
}

void MdlImporter::MeshEntry::Init(
	const std::vector<glm::vec3> &_Vertices
	, const std::vector<glm::vec3> &_Normals
	, const std::vector<glm::vec3> &_Colors
	, const std::vector<unsigned int> &_Indices
	){
	
	glGenBuffers(1, &VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER
		, _Vertices.size() * sizeof(glm::vec3)
		, &_Vertices[0]
		, GL_STATIC_DRAW);

	glGenBuffers(1, &NormalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, NormalBuffer);
	glBufferData(GL_ARRAY_BUFFER
		, _Normals.size() * sizeof(glm::vec3)
		, &_Normals[0]
		, GL_STATIC_DRAW);
	
	glGenBuffers(1, &ColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBuffer);
	glBufferData(GL_ARRAY_BUFFER
		, _Colors.size() * sizeof(glm::vec3)
		, &_Colors[0]
		, GL_STATIC_DRAW);

	glGenBuffers(1, &ElementBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ElementBuffer);
	glBufferData(GL_ARRAY_BUFFER
		, _Indices.size() * sizeof(unsigned int)
		, &_Indices[0]
		, GL_STATIC_DRAW);

	NumIndices = _Indices.size();

}

void MdlImporter::MeshEntry::Init(
	const std::vector<glm::vec3> &_Vertices
	, const std::vector<glm::vec3> &_Normals
	, const std::vector<glm::vec2> &_UVs
	, const std::vector<unsigned int> &_Indices
	){

	glGenBuffers(1, &VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER
		, _Vertices.size() * sizeof(glm::vec3)
		, &_Vertices[0]
		, GL_STATIC_DRAW);

	glGenBuffers(1, &NormalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, NormalBuffer);
	glBufferData(GL_ARRAY_BUFFER
		, _Normals.size() * sizeof(glm::vec3)
		, &_Normals[0]
		, GL_STATIC_DRAW);

	glGenBuffers(1, &ColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBuffer);
	glBufferData(GL_ARRAY_BUFFER
		, _UVs.size() * sizeof(glm::vec2)
		, &_UVs[0]
		, GL_STATIC_DRAW);

	glGenBuffers(1, &ElementBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ElementBuffer);
	glBufferData(GL_ARRAY_BUFFER
		, _Indices.size() * sizeof(unsigned int)
		, &_Indices[0]
		, GL_STATIC_DRAW);

	NumIndices = _Indices.size();
}


MdlImporter::MdlImporter(const GLuint &_ShaderIDNonTexture, const GLuint &_ShaderIDTexture){
	UseTexture = false;
	ShaderIDNonTexture = _ShaderIDNonTexture;
	ShaderIDTexture = _ShaderIDTexture;

	//VAO
	//glGenVertexArrays(1, &VaoID);
	//glBindVertexArray(VaoID);
}

MdlImporter::MdlImporter() {
	UseTexture = false;
}


MdlImporter::~MdlImporter(){
	Clear();
}

void MdlImporter::Clear(){
	//テクスチャを破棄
	
}


//モデルを読む
bool MdlImporter::Load(const std::string& Filename)
{
	// Release the previously loaded mesh (if it exists)
	Clear();

	UseTexture = true;

	bool Ret = false;
	Assimp::Importer Importer;

	//ImporterよりaiSceneインスタンスの生成
	const aiScene* pScene = Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);


	if (pScene) {
		//aiSceneインスタンスよりMeshEntryのコンテナを生成する
		//頂点データ、マテリアルデータ(テクスチャデータ)を読みVBOを生成
		Ret = InitFromScene(pScene, Filename);
	}
	else {
		printf("Error parsing '%s': '%s'\n", Filename.c_str(), Importer.GetErrorString());
	}


	return Ret;
}

bool MdlImporter::Load(const std::string & Filename, const glm::vec3 & _Color){
	// Release the previously loaded mesh (if it exists)
	Clear();

	UseTexture = false;

	bool Ret = false;
	Assimp::Importer Importer;

	//ImporterよりaiSceneインスタンスの生成
	const aiScene* pScene = Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);


	if (pScene) {
		//aiSceneインスタンスよりMeshEntryのコンテナを生成する
		//頂点データ、マテリアルデータ(テクスチャデータ)を読みVBOを生成
		Ret = InitFromScene(pScene, Filename, _Color);
	}
	else {
		printf("Error parsing '%s': '%s'\n", Filename.c_str(), Importer.GetErrorString());
	}

	return Ret;
}

bool MdlImporter::InitFromScene(const aiScene* pScene, const std::string& Filename)
{

	//含まれるaiSceneエントリ数によりコンテナ数を更新
	m_Entries.resize(pScene->mNumMeshes);
	//テクスチャコンテナの数も更新
	//m_Textures.resize(pScene->mNumMaterials);

	// Initialize the meshes in the scene one by one
	for (unsigned int i = 0; i < m_Entries.size(); i++) {
		//メッシュへのポインタが得られるので、これよりメッシュエントリのi番目に頂点、頂点配列,
		//UV座標等を保存する、テクスチャコンテナの更新も行う
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(i, paiMesh);
	}

	//結局InitMaterialsにしかファイルパスを使ってない
	//ここでマテリアル（テクスチャデータ）の生成を行う
	return InitMaterials(pScene, Filename);
}


//テクスチャを使用しない場合
//テクスチャの代わりにVEC3の色情報が与えられるので、それで色づけする
//常に真が返される
bool MdlImporter::InitFromScene(const aiScene* pScene, const std::string& Filename, const glm::vec3 & _Color)
{

	//含まれるaiSceneエントリ数によりコンテナ数を更新
	m_Entries.resize(pScene->mNumMeshes);
	//テクスチャコンテナの数も更新
	//テクスチャは使わない
	//m_Textures.resize(pScene->mNumMaterials);

	// Initialize the meshes in the scene one by one
	for (unsigned int i = 0; i < m_Entries.size(); i++) {
		//メッシュへのポインタが得られるので、これよりメッシュエントリのi番目に頂点、頂点配列,
		//各頂点にカラー情報を付加する
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(i, paiMesh, _Color);

		printf("Model loaded: %d / %d\n", i+1, m_Entries.size());
	}
	
	return true;
}


//VBOを生成
void MdlImporter::InitMesh(unsigned int Index, const aiMesh* paiMesh)
{
	m_Entries[Index].MaterialIndex = paiMesh->mMaterialIndex;

	//std::vector<Vertex> Vertices;
	//std::vector<unsigned int> Indices;

	//頂点座標
	std::vector<glm::vec3> Vertices;
	//法線
	std::vector<glm::vec3> Normals;
	//面を構成するインデックス列が入る
	std::vector<unsigned int> Indices;
	//テクスチャUV座標
	std::vector<glm::vec2> UVs;

	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {
		const aiVector3D* pPos = &(paiMesh->mVertices[i]); //頂点
		const aiVector3D* pNormal = &(paiMesh->mNormals[i]); //法線
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

		//ここで頂点座標、法線、UV座標を追加する
		Vertices.push_back(vec3(pPos->x, pPos->y, pPos->z)); //頂点座標
		Normals.push_back(vec3(pNormal->x, pNormal->y, pNormal->z)); //法線
		UVs.push_back(vec2(pTexCoord->x, pTexCoord->y)); //UV座標

	}

	for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
		const aiFace& Face = paiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		Indices.push_back(Face.mIndices[0]);
		Indices.push_back(Face.mIndices[1]);
		Indices.push_back(Face.mIndices[2]);
		//面を構成する頂点インデックスを頂点コンテナに格納
	}

	//ここでVBOを生成
	//glBindVertexArray(VaoID); //VAOをバインド
	m_Entries[Index].Init(Vertices, Normals, UVs, Indices);
	//メッシュエントリクラスに頂点、
	//頂点インデックスコンテナをわたし、
	//おそらくVBOの初期化等を行わせる
}

//VBOを生成
void MdlImporter::InitMesh(unsigned int Index, const aiMesh* paiMesh, const glm::vec3 & _Color)
{
	m_Entries[Index].MaterialIndex = paiMesh->mMaterialIndex;

	//std::vector<Vertex> Vertices;
	//std::vector<unsigned int> Indices;

	//頂点座標
	std::vector<glm::vec3> Vertices;
	//法線
	std::vector<glm::vec3> Normals;
	//面を構成するインデックス列が入る
	std::vector<unsigned int> Indices;
	//カラー用
	std::vector<glm::vec3> Colors;

	Vertices.resize(paiMesh->mNumVertices);
	Normals.resize(paiMesh->mNumVertices);
	Colors.resize(paiMesh->mNumVertices);


	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {
		//テクスチャ情報は扱わない

		const aiVector3D* pPos = &(paiMesh->mVertices[i]); //頂点
		const aiVector3D* pNormal = &(paiMesh->mNormals[i]); //法線
		
		//ここで頂点座標、法線、色情報を追加する
		//Vertices.push_back(vec3(pPos->x, pPos->y, pPos->z)); //頂点座標
		//Normals.push_back(vec3(pNormal->x, pNormal->y, pNormal->z)); //法線
		//Colors.push_back(_Color); //色情報

		//座標軸変更なし
		//Vertices[i] = (vec3(pPos->x, pPos->y, pPos->z)); //頂点座標
		//Normals[i] = (vec3(pNormal->x, pNormal->y, pNormal->z)); //法線
		//Colors[i] = (_Color); //色情報

		/*
		軸変換
		X -> X
		Y -> Z
		Z -> -Y
		*/

		////座標軸変更あり
		//Vertices[i] = (vec3(pPos->x, pPos->z, -pPos->y)); //頂点座標
		//Normals[i] = (vec3(pNormal->x, pNormal->z, -pNormal->y)); //法線
		//Colors[i] = (_Color); //色情報

							  //座標軸変更あり
		Vertices[i] = (vec3(pPos->x, pPos->y, pPos->z)); //頂点座標
		Normals[i] = (vec3(pNormal->x, pNormal->y, pNormal->z)); //法線
		Colors[i] = (_Color); //色情報

		//printf("%3.2f%\n", ((double)(i + 1)/((double)paiMesh->mNumVertices) * 100.0));
	}

	//面を構成する頂点インデックスを頂点コンテナに格納
	for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
		const aiFace& Face = paiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		Indices.push_back(Face.mIndices[0]);
		Indices.push_back(Face.mIndices[1]);
		Indices.push_back(Face.mIndices[2]);	
	}

	//ここでVBOを生成
	m_Entries[Index].Init(Vertices, Normals, Colors, Indices);

	

}

//メッシュエントリの初期化（VBOの生成）の後呼ばれる 要ファイルパス
//テクスチャデータの生成　ASSIMPSではマテリアルと呼ばれているらしい
bool MdlImporter::InitMaterials(const aiScene* pScene, const std::string& Filename)
{
	// Extract the directory part from the file name
	std::string::size_type SlashIndex = Filename.find_last_of("/");
	std::string Dir;

	if (SlashIndex == std::string::npos) {
		Dir = ".";
	}
	else if (SlashIndex == 0) {
		Dir = "/";
	}
	else {
		Dir = Filename.substr(0, SlashIndex);
	}

	bool Ret = true;

	// Initialize the materials
	//マテリアルデータの数だけループをまわす
	//メッシュデータの数とマテリアルデータの数は一致していないらしい
	for (unsigned int i = 0; i < pScene->mNumMaterials; i++) {
		//aiSceneよりマテリアル抽出　テクスチャのデータが含まれているらしい
		const aiMaterial* pMaterial = pScene->mMaterials[i];

		//m_Textures[i] = NULL;

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString Path;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string FullPath = Dir + "/" + Path.data; //テクスチャデータへのフルパス
				//m_Textures[i] = new Texture(GL_TEXTURE_2D, FullPath.c_str());
				//テクスチャコンテナのi番目にテクスチャインスタンスを追加

				//if (!m_Textures[i]->Load()) {
				//	//テクスチャファイルが読めなければ消す
				//	printf("Error loading texture '%s'\n", FullPath.c_str());
				//	delete m_Textures[i];
				//	m_Textures[i] = NULL;
				//	Ret = false;
				//}
				//else {
				//	printf("Loaded texture '%s'\n", FullPath.c_str());
				//}
			}
		}

		// Load a white texture in case the model does not include its own texture
		//テクスチャデータが読めなかったら白濁データを読む
		/*if (!m_Textures[i]) {
			m_Textures[i] = new Texture(GL_TEXTURE_2D, "../Content/white.png");

			Ret = m_Textures[i]->Load();
		}*/
	}

	return Ret;
}


void MdlImporter::RenderTexture(){

	//テクスチャ描画するシェーダーを使用
	//glUseProgram(ShaderIDTexture);

	//glBindVertexArray(VaoID);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	for (unsigned int i = 0; i < m_Entries.size(); i++) {
		//glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].VB);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*) 12);
		//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*) 20);

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Entries[i].IB);

		glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].VertexBuffer);
		glVertexAttribPointer(
			0
			, 3
			, GL_FLOAT
			, GL_FALSE
			, 0
			, (void *)0
			);
		glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].ColorBuffer);
		glVertexAttribPointer(
			1
			, 2
			, GL_FLOAT
			, GL_FALSE
			, 0
			, (void *)0
			);
		glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].NormalBuffer);
		glVertexAttribPointer(
			2
			, 3
			, GL_FLOAT
			, GL_FALSE
			, 0
			, (void *)0
			);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Entries[i].ElementBuffer);

		const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;

		/*if (MaterialIndex < m_Textures.size() && m_Textures[MaterialIndex]) {
			m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
		}*/

		glDrawElements(GL_TRIANGLES, m_Entries[i].NumIndices, GL_UNSIGNED_INT, 0);
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}


//Use NON-TEXTURED shader
void MdlImporter::RenderColor(){

	//単に色つけするだけのシェーダーを使用
	//glUseProgram(ShaderIDNonTexture);

	//glBindVertexArray(VaoID);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	for (unsigned int i = 0; i < m_Entries.size(); i++) {

		glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].VertexBuffer);
		glVertexAttribPointer(
				0
				, 3
				, GL_FLOAT
				, GL_FALSE
				, 0
				, (void *)0
			);

		glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].ColorBuffer);
		glVertexAttribPointer(
			1
			, 3
			, GL_FLOAT
			, GL_FALSE
			, 0
			, (void *)0
			);

		glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].NormalBuffer);
		glVertexAttribPointer(
			2
			, 3
			, GL_FLOAT
			, GL_FALSE
			, 0
			, (void *)0
			);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Entries[i].ElementBuffer);

		glDrawElements(GL_TRIANGLES
			, m_Entries[i].NumIndices
			, GL_UNSIGNED_INT
			, (void *)0
			);


	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}


//描画
void MdlImporter::Render()
{
	if (UseTexture) {
		RenderTexture();
	}
	else {
		RenderColor();
	}
}


































