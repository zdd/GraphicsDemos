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
Frame Object08 {
   FrameTransformMatrix {
1.000000,0.000000,0.000000,0.000000,
0.000000,1.000000,0.000000,0.000000,
0.000000,0.000000,1.000000,0.000000,
-10.200000,10.200000,10.200001,1.000000;;
 }
Frame Object08_pivot {
   FrameTransformMatrix {
1.000000,0.000000,0.000000,0.000000,
0.000000,1.000000,0.000000,0.000000,
0.000000,0.000000,1.000000,0.000000,
20.400000,-5.000000,-20.400002,1.000000;;
 }
Mesh Object08_pivot2 {
 47;
-25.400000;0.000000;15.400002;,
-25.400000;0.000000;25.400002;,
-15.400000;0.000000;25.400002;,
-15.400000;0.000000;15.400002;,
-24.900000;10.000000;15.900002;,
-15.900000;10.000000;24.900002;,
-24.900000;10.000000;24.900002;,
-15.900000;10.000000;15.900002;,
-25.400000;0.000000;15.400002;,
-15.400000;0.000000;15.400002;,
-15.400000;10.000000;15.400002;,
-25.400000;10.000000;15.400002;,
-15.400000;0.000000;15.400002;,
-15.400000;0.000000;25.400002;,
-15.400000;10.000000;25.400002;,
-15.400000;10.000000;15.400002;,
-15.900000;0.500000;25.400002;,
-24.900000;0.500000;25.400002;,
-24.900000;9.500000;25.400002;,
-15.900000;9.500000;25.400002;,
-25.400000;0.500000;24.900002;,
-25.400000;0.500000;15.900002;,
-25.400000;9.500000;15.900002;,
-25.400000;9.500000;24.900002;,
-25.400000;10.000000;15.400002;,
-15.400000;10.000000;15.400002;,
-15.400000;10.000000;15.400002;,
-15.400000;10.000000;25.400002;,
-15.400000;10.000000;25.400002;,
-25.400000;10.000000;25.400002;,
-25.400000;10.000000;25.400002;,
-25.400000;10.000000;15.400002;,
-25.400000;0.000000;25.400002;,
-25.400000;0.000000;15.400002;,
-25.400000;0.000000;15.400002;,
-25.400000;10.000000;15.400002;,
-25.400000;10.000000;15.400002;,
-25.400000;10.000000;25.400002;,
-25.400000;10.000000;25.400002;,
-25.400000;0.000000;25.400002;,
-15.400000;0.000000;25.400002;,
-25.400000;0.000000;25.400002;,
-25.400000;0.000000;25.400002;,
-25.400000;10.000000;25.400002;,
-15.400000;10.000000;25.400002;,
-15.400000;10.000000;25.400002;,
-15.400000;0.000000;25.400002;;

 36;
3;2,1,0;,
3;3,2,0;,
3;6,5,4;,
3;5,7,4;,
3;10,9,8;,
3;11,10,8;,
3;14,13,12;,
3;15,14,12;,
3;18,17,16;,
3;19,18,16;,
3;22,21,20;,
3;23,22,20;,
3;7,25,24;,
3;4,7,24;,
3;5,27,26;,
3;7,5,26;,
3;6,29,28;,
3;5,6,28;,
3;4,31,30;,
3;6,4,30;,
3;21,33,32;,
3;20,21,32;,
3;22,35,34;,
3;21,22,34;,
3;22,23,36;,
3;23,37,36;,
3;20,39,38;,
3;23,20,38;,
3;16,17,40;,
3;17,41,40;,
3;18,43,42;,
3;17,18,42;,
3;18,19,43;,
3;19,44,43;,
3;16,46,45;,
3;19,16,45;;
MeshMaterialList {
 10;
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
  7,
  7,
  7,
  7,
  7,
  7,
  7,
  7,
  8,
  8,
  8,
  8,
  8,
  8,
  8,
  8;;
Material Material__26 {
 0.588000;0.588000;0.588000;1.000000;;
4.000000;
 0.000000;0.000000;0.000000;;
 0.117600;0.117600;0.117600;;
 }
Material _1_-_Default {
 1.000000;0.000000;0.000000;1.000000;;
4.000000;
 0.000000;0.000000;0.000000;;
 0.200000;0.000000;0.000000;;
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
Material _2_-_Default {
 0.000000;0.478431;0.000000;1.000000;;
4.000000;
 0.000000;0.000000;0.000000;;
 0.000000;0.095686;0.000000;;
 }
Material _3_-_Default {
 0.066667;0.109804;1.000000;1.000000;;
4.000000;
 0.000000;0.000000;0.000000;;
 0.013333;0.021961;0.200000;;
 }
Material Material__25 {
 0.588000;0.588000;0.588000;1.000000;;
4.000000;
 0.000000;0.000000;0.000000;;
 0.117600;0.117600;0.117600;;
 }
Material Material__27 {
 1.000000;1.000000;1.000000;1.000000;;
4.000000;
 0.000000;0.000000;0.000000;;
 0.200000;0.200000;0.200000;;
 }
Material Material__30 {
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
 47;
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
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
0.000000;1.000000;0.000000;,
-1.000000;0.000000;0.000000;,
-1.000000;0.000000;0.000000;,
-1.000000;0.000000;0.000000;,
-1.000000;0.000000;0.000000;,
-1.000000;0.000000;0.000000;,
-1.000000;0.000000;0.000000;,
-1.000000;0.000000;0.000000;,
-1.000000;0.000000;0.000000;,
0.000000;0.000000;1.000000;,
0.000000;0.000000;1.000000;,
0.000000;0.000000;1.000000;,
0.000000;0.000000;1.000000;,
0.000000;0.000000;1.000000;,
0.000000;0.000000;1.000000;,
0.000000;0.000000;1.000000;;

 36;
3;2,1,0;,
3;3,2,0;,
3;6,5,4;,
3;5,7,4;,
3;10,9,8;,
3;11,10,8;,
3;14,13,12;,
3;15,14,12;,
3;18,17,16;,
3;19,18,16;,
3;22,21,20;,
3;23,22,20;,
3;7,25,24;,
3;4,7,24;,
3;5,27,26;,
3;7,5,26;,
3;6,29,28;,
3;5,6,28;,
3;4,31,30;,
3;6,4,30;,
3;21,33,32;,
3;20,21,32;,
3;22,35,34;,
3;21,22,34;,
3;22,23,36;,
3;23,37,36;,
3;20,39,38;,
3;23,20,38;,
3;16,17,40;,
3;17,41,40;,
3;18,43,42;,
3;17,18,42;,
3;18,19,43;,
3;19,44,43;,
3;16,46,45;,
3;19,16,45;;
 }
MeshTextureCoords {
 47;
1.000000;1.000000;,
1.000000;0.000000;,
0.000000;0.000000;,
0.000000;1.000000;,
0.000000;1.000000;,
1.000000;0.000000;,
0.000000;0.000000;,
1.000000;1.000000;,
0.000000;1.000000;,
1.000000;1.000000;,
1.000000;0.000000;,
0.000000;0.000000;,
0.000000;1.000000;,
1.000000;1.000000;,
1.000000;0.000000;,
0.000000;0.000000;,
0.000000;1.000000;,
1.000000;1.000000;,
1.000000;0.000000;,
0.000000;0.000000;,
0.000000;1.000000;,
1.000000;1.000000;,
1.000000;0.000000;,
0.000000;0.000000;,
0.000000;0.000000;,
1.000000;0.000000;,
0.000000;0.000000;,
1.000000;0.000000;,
0.000000;0.000000;,
1.000000;0.000000;,
0.000000;0.000000;,
1.000000;0.000000;,
1.000000;0.000000;,
1.000000;1.000000;,
0.000000;1.000000;,
0.000000;0.000000;,
1.000000;0.000000;,
0.000000;0.000000;,
1.000000;0.000000;,
1.000000;1.000000;,
0.000000;0.000000;,
1.000000;0.000000;,
1.000000;1.000000;,
1.000000;0.000000;,
0.000000;0.000000;,
1.000000;0.000000;,
1.000000;1.000000;;
}
}
 }
 }
