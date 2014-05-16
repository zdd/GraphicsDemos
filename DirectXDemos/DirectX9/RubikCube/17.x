xof 0302txt 0032
Header {
 1;
 0;
 1;
}
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

template FrameTransformMatrix {
 <F6F23F41-7686-11cf-8F52-0040333594A3>
 Matrix4x4 frameMatrix;
}

template Frame {
 <3D82AB46-62DA-11cf-AB39-0020AF71E433>
 [...]
}
template FloatKeys {
 <10DD46A9-775B-11cf-8F52-0040333594A3>
 DWORD nValues;
 array FLOAT values[nValues];
}

template TimedFloatKeys {
 <F406B180-7B3B-11cf-8F52-0040333594A3>
 DWORD time;
 FloatKeys tfkeys;
}

template AnimationKey {
 <10DD46A8-775B-11cf-8F52-0040333594A3>
 DWORD keyType;
 DWORD nKeys;
 array TimedFloatKeys keys[nKeys];
}

template AnimationOptions {
 <E2BF56C0-840F-11cf-8F52-0040333594A3>
 DWORD openclosed;
 DWORD positionquality;
}

template Animation {
 <3D82AB4F-62DA-11cf-AB39-0020AF71E433>
 [...]
}

template AnimationSet {
 <3D82AB50-62DA-11cf-AB39-0020AF71E433>
 [Animation]
}

template XSkinMeshHeader {
 <3cf169ce-ff7c-44ab-93c0-f78f62d172e2>
 WORD nMaxSkinWeightsPerVertex;
 WORD nMaxSkinWeightsPerFace;
 WORD nBones;
}

template VertexDuplicationIndices {
 <b8d65549-d7c9-4995-89cf-53a9a8b031e3>
 DWORD nIndices;
 DWORD nOriginalVertices;
 array DWORD indices[nIndices];
}

template SkinWeights {
 <6f0d123b-bad2-4167-a0d0-80224f25fabb>
 STRING transformNodeName;
 DWORD nWeights;
 array DWORD vertexIndices[nWeights];
 array FLOAT weights[nWeights];
 Matrix4x4 matrixOffset;
}
Frame Object11 {
   FrameTransformMatrix {
1.000000,0.000000,0.000000,0.000000,
0.000000,1.000000,0.000000,0.000000,
0.000000,0.000000,1.000000,0.000000,
10.200000,0.000000,-10.200001,1.000000;;
 }
Frame Object11_pivot {
   FrameTransformMatrix {
1.000000,0.000000,0.000000,0.000000,
0.000000,1.000000,0.000000,0.000000,
0.000000,0.000000,1.000000,0.000000,
0.000000,5.200000,0.000000,1.000000;;
 }
Mesh Object11_pivot2 {
 41;
-5.000000;-10.200000;-5.000000;,
5.000000;-10.200000;5.000000;,
5.000000;-10.200000;-5.000000;,
-5.000000;-10.200000;5.000000;,
-5.000000;-0.200000;-5.000000;,
5.000000;-0.200000;-5.000000;,
5.000000;-0.200000;5.000000;,
-5.000000;-0.200000;5.000000;,
-4.000000;-9.200000;-5.000000;,
4.000000;-1.200000;-5.000000;,
-4.000000;-1.200000;-5.000000;,
4.000000;-9.200000;-5.000000;,
5.000000;-9.700000;-4.500000;,
5.000000;-9.700000;4.500000;,
5.000000;-0.700000;4.500000;,
5.000000;-0.700000;-4.500000;,
5.000000;-10.200000;5.000000;,
-5.000000;-0.200000;5.000000;,
5.000000;-0.200000;5.000000;,
-5.000000;-10.200000;5.000000;,
-5.000000;-10.200000;5.000000;,
-5.000000;-0.200000;-5.000000;,
-5.000000;-0.200000;5.000000;,
-5.000000;-10.200000;-5.000000;,
-5.000000;-10.200000;-5.000000;,
4.500000;-9.700000;-5.000000;,
-4.500000;-9.700000;-5.000000;,
5.000000;-10.200000;-5.000000;,
4.500000;-0.700000;-5.000000;,
5.000000;-0.200000;-5.000000;,
5.000000;-0.200000;-5.000000;,
-5.000000;-0.200000;-5.000000;,
-4.500000;-0.700000;-5.000000;,
-5.000000;-0.200000;-5.000000;,
5.000000;-10.200000;-5.000000;,
5.000000;-10.200000;5.000000;,
5.000000;-10.200000;5.000000;,
5.000000;-0.200000;5.000000;,
5.000000;-0.200000;5.000000;,
5.000000;-0.200000;-5.000000;,
5.000000;-0.200000;-5.000000;;

 36;
3;2,1,0;,
3;1,3,0;,
3;6,5,4;,
3;7,6,4;,
3;10,9,8;,
3;9,11,8;,
3;14,13,12;,
3;15,14,12;,
3;18,17,16;,
3;17,19,16;,
3;22,21,20;,
3;21,23,20;,
3;26,25,24;,
3;25,27,24;,
3;25,28,27;,
3;28,29,27;,
3;32,31,30;,
3;28,32,30;,
3;26,24,33;,
3;32,26,33;,
3;8,11,26;,
3;11,25,26;,
3;9,28,25;,
3;11,9,25;,
3;9,10,28;,
3;10,32,28;,
3;8,26,32;,
3;10,8,32;,
3;13,35,34;,
3;12,13,34;,
3;14,37,36;,
3;13,14,36;,
3;15,39,38;,
3;14,15,38;,
3;12,34,40;,
3;15,12,40;;
MeshMaterialList {
 9;
 36;
  0,
  0,
  1,
  1,
  2,
  2,
  3,
  3,
  4,
  4,
  5,
  5,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  6,
  7,
  7,
  7,
  7,
  7,
  7,
  7,
  7;;
Material Material__26 {
 0.588000;0.588000;0.588000;1.000000;;
4.000000;
 0.000000;0.000000;0.000000;;
 0.117600;0.117600;0.117600;;
 }
Material Material__25 {
 0.588000;0.588000;0.588000;1.000000;;
4.000000;
 0.000000;0.000000;0.000000;;
 0.117600;0.117600;0.117600;;
 }
Material _9_-_Default {
 0.000000;0.772549;0.886275;1.000000;;
4.000000;
 0.000000;0.000000;0.000000;;
 0.000000;0.154510;0.177255;;
 }
Material _7_-_Default {
 0.992157;0.909804;0.000000;1.000000;;
4.000000;
 0.000000;0.000000;0.000000;;
 0.198431;0.181961;0.000000;;
 }
Material Material__30 {
 1.000000;1.000000;1.000000;1.000000;;
4.000000;
 0.000000;0.000000;0.000000;;
 0.200000;0.200000;0.200000;;
 }
Material Material__27 {
 1.000000;1.000000;1.000000;1.000000;;
4.000000;
 0.000000;0.000000;0.000000;;
 0.200000;0.200000;0.200000;;
 }
Material Material__29 {
 1.000000;1.000000;1.000000;1.000000;;
4.000000;
 0.000000;0.000000;0.000000;;
 0.200000;0.200000;0.200000;;
 }
Material Material__28 {
 1.000000;1.000000;1.000000;1.000000;;
4.000000;
 0.000000;0.000000;0.000000;;
 0.200000;0.200000;0.200000;;
 }
Material Material__35 {
 0.588000;0.588000;0.588000;1.000000;;
4.000000;
 0.000000;0.000000;0.000000;;
 0.117600;0.117600;0.117600;;
 }
}

 MeshNormals {
 41;
0.000000;-1.000000;0.000000;,
0.000000;-1.000000;0.000000;,
0.000000;-1.000000;0.000000;,
0.000000;-1.000000;0.000000;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
0.000000;0.000000;-1.000000;,
0.000000;0.000000;-1.000000;,
0.000000;0.000000;-1.000000;,
0.000000;0.000000;-1.000000;,
1.000000;0.000000;0.000000;,
1.000000;0.000000;0.000000;,
1.000000;0.000000;0.000000;,
1.000000;0.000000;0.000000;,
0.000000;0.000000;1.000000;,
0.000000;0.000000;1.000000;,
0.000000;0.000000;1.000000;,
0.000000;0.000000;1.000000;,
-1.000000;0.000000;0.000000;,
-1.000000;0.000000;0.000000;,
-1.000000;0.000000;0.000000;,
-1.000000;0.000000;0.000000;,
0.000000;0.000000;-1.000000;,
0.000000;0.000000;-1.000000;,
0.000000;0.000000;-1.000000;,
0.000000;0.000000;-1.000000;,
0.000000;0.000000;-1.000000;,
0.000000;0.000000;-1.000000;,
0.000000;0.000000;-1.000000;,
0.000000;0.000000;-1.000000;,
0.000000;0.000000;-1.000000;,
0.000000;0.000000;-1.000000;,
1.000000;0.000000;0.000000;,
1.000000;0.000000;0.000000;,
1.000000;0.000000;0.000000;,
1.000000;0.000000;0.000000;,
1.000000;0.000000;0.000000;,
1.000000;0.000000;0.000000;,
1.000000;0.000000;0.000000;;

 36;
3;2,1,0;,
3;1,3,0;,
3;6,5,4;,
3;7,6,4;,
3;10,9,8;,
3;9,11,8;,
3;14,13,12;,
3;15,14,12;,
3;18,17,16;,
3;17,19,16;,
3;22,21,20;,
3;21,23,20;,
3;26,25,24;,
3;25,27,24;,
3;25,28,27;,
3;28,29,27;,
3;32,31,30;,
3;28,32,30;,
3;26,24,33;,
3;32,26,33;,
3;8,11,26;,
3;11,25,26;,
3;9,28,25;,
3;11,9,25;,
3;9,10,28;,
3;10,32,28;,
3;8,26,32;,
3;10,8,32;,
3;13,35,34;,
3;12,13,34;,
3;14,37,36;,
3;13,14,36;,
3;15,39,38;,
3;14,15,38;,
3;12,34,40;,
3;15,12,40;;
 }
MeshTextureCoords {
 41;
1.000000;1.000000;,
0.000000;0.000000;,
0.000000;1.000000;,
1.000000;0.000000;,
0.000000;1.000000;,
1.000000;1.000000;,
1.000000;0.000000;,
0.000000;0.000000;,
0.000000;1.000000;,
1.000000;0.000000;,
0.000000;0.000000;,
1.000000;1.000000;,
0.000000;1.000000;,
1.000000;1.000000;,
1.000000;0.000000;,
0.000000;0.000000;,
0.000000;1.000000;,
1.000000;0.000000;,
0.000000;0.000000;,
1.000000;1.000000;,
0.000000;1.000000;,
1.000000;0.000000;,
0.000000;0.000000;,
1.000000;1.000000;,
1.000000;1.000000;,
1.000000;1.000000;,
0.000000;1.000000;,
0.000000;1.000000;,
1.000000;0.000000;,
0.000000;0.000000;,
1.000000;1.000000;,
0.000000;1.000000;,
0.000000;0.000000;,
1.000000;0.000000;,
0.000000;1.000000;,
0.000000;0.000000;,
0.000000;1.000000;,
0.000000;0.000000;,
1.000000;0.000000;,
1.000000;1.000000;,
0.000000;0.000000;;
}
}
 }
 }
