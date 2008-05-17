xof 0303txt 0032

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

template EffectInstance {
 <e331f7e4-0559-4cc2-8e99-1cec1657928f>
 STRING EffectFilename;
 [...]
}

Frame Frame_SCENE_ROOT {

 FrameTransformMatrix {
  1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
 }

 Frame Frame1_room1 {

  FrameTransformMatrix {
   1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000,0.000000,0.000000,0.000000,0.000000,1.000000;;
  }

  Mesh Mesh_room1 {
   202;
   6.000000;0.000000;-18.000000;,
   6.000000;0.000000;-18.000000;,
   6.000000;0.000000;-18.000000;,
   8.000000;0.000000;-18.000000;,
   8.000000;0.000000;-18.000000;,
   8.000000;0.000000;-18.000000;,
   6.000000;2.500000;-18.000000;,
   6.000000;2.500000;-18.000000;,
   6.000000;2.500000;-18.000000;,
   6.000000;2.500000;-18.000000;,
   6.000000;2.500000;-18.000000;,
   8.000000;2.500000;-18.000000;,
   8.000000;2.500000;-18.000000;,
   8.000000;2.500000;-18.000000;,
   8.000000;2.500000;-18.000000;,
   8.000000;2.500000;-18.000000;,
   -4.000000;0.000000;-3.150000;,
   -4.000000;0.000000;-3.150000;,
   -4.000000;0.000000;-3.150000;,
   -4.000000;3.000000;-3.150000;,
   -4.000000;3.000000;-3.150000;,
   -4.000000;3.000000;-3.150000;,
   3.000000;3.000000;-3.150000;,
   3.000000;3.000000;-3.150000;,
   3.000000;3.000000;-3.150000;,
   3.000000;0.000000;-3.150000;,
   3.000000;0.000000;-3.150000;,
   3.000000;0.000000;-3.150000;,
   -4.000000;0.000000;3.000000;,
   -4.000000;0.000000;3.000000;,
   -4.000000;0.000000;3.000000;,
   -4.000000;3.000000;3.000000;,
   -4.000000;3.000000;3.000000;,
   -4.000000;3.000000;3.000000;,
   3.000000;3.000000;3.000000;,
   3.000000;3.000000;3.000000;,
   3.000000;3.000000;3.000000;,
   3.000000;0.000000;3.000000;,
   3.000000;0.000000;3.000000;,
   3.000000;0.000000;3.000000;,
   -1.100000;0.000000;-14.000000;,
   -1.100000;0.000000;-14.000000;,
   -1.100000;0.000000;-14.000000;,
   9.100000;0.000000;-14.000000;,
   9.100000;0.000000;-14.000000;,
   9.100000;0.000000;-14.000000;,
   9.100000;0.000000;-8.000000;,
   9.100000;0.000000;-8.000000;,
   9.100000;0.000000;-8.000000;,
   -1.100000;0.000000;-8.000000;,
   -1.100000;0.000000;-8.000000;,
   -1.100000;0.000000;-8.000000;,
   -1.100000;3.000000;-14.000000;,
   -1.100000;3.000000;-14.000000;,
   -1.100000;3.000000;-14.000000;,
   9.100000;3.000000;-14.000000;,
   9.100000;3.000000;-14.000000;,
   9.100000;3.000000;-14.000000;,
   9.100000;3.000000;-8.000000;,
   9.100000;3.000000;-8.000000;,
   9.100000;3.000000;-8.000000;,
   -1.100000;3.000000;-8.000000;,
   -1.100000;3.000000;-8.000000;,
   -1.100000;3.000000;-8.000000;,
   0.000000;0.000000;-8.000000;,
   0.000000;0.000000;-8.000000;,
   0.000000;0.000000;-8.000000;,
   0.000000;2.400000;-8.000000;,
   0.000000;2.400000;-8.000000;,
   0.000000;2.400000;-8.000000;,
   2.000000;0.000000;-8.000000;,
   2.000000;0.000000;-8.000000;,
   2.000000;0.000000;-8.000000;,
   2.000000;2.400000;-8.000000;,
   2.000000;2.400000;-8.000000;,
   2.000000;2.400000;-8.000000;,
   0.000000;0.000000;-3.150000;,
   0.000000;0.000000;-3.150000;,
   0.000000;0.000000;-3.150000;,
   0.000000;2.400000;-3.150000;,
   0.000000;2.400000;-3.150000;,
   0.000000;2.400000;-3.150000;,
   2.000000;0.000000;-3.150000;,
   2.000000;0.000000;-3.150000;,
   2.000000;0.000000;-3.150000;,
   2.000000;2.400000;-3.150000;,
   2.000000;2.400000;-3.150000;,
   2.000000;2.400000;-3.150000;,
   7.900000;3.286147;-17.875191;,
   7.900000;3.286147;-17.875191;,
   7.900000;3.286147;-17.875191;,
   7.900000;3.286147;-17.875191;,
   7.900000;2.813694;-14.306327;,
   7.900000;2.813694;-14.306327;,
   7.900000;2.813694;-14.306327;,
   7.900000;2.813694;-14.306327;,
   6.100000;2.813694;-14.306327;,
   6.100000;2.813694;-14.306327;,
   6.100000;2.813694;-14.306327;,
   6.100000;2.813694;-14.306327;,
   6.100000;3.286147;-17.875191;,
   6.100000;3.286147;-17.875191;,
   6.100000;3.286147;-17.875191;,
   6.100000;3.286147;-17.875191;,
   7.810000;3.854587;-17.627853;,
   7.810000;3.854587;-17.627853;,
   7.810000;3.854587;-17.627853;,
   7.810000;3.854587;-17.627853;,
   7.810000;3.168410;-14.461348;,
   7.810000;3.168410;-14.461348;,
   7.810000;3.168410;-14.461348;,
   7.810000;3.168410;-14.461348;,
   6.190000;3.168410;-14.461348;,
   6.190000;3.168410;-14.461348;,
   6.190000;3.168410;-14.461348;,
   6.190000;3.168410;-14.461348;,
   6.190000;3.854587;-17.627853;,
   6.190000;3.854587;-17.627853;,
   6.190000;3.854587;-17.627853;,
   6.190000;3.854587;-17.627853;,
   7.729000;4.302808;-17.483141;,
   7.729000;4.302808;-17.483141;,
   7.729000;4.302808;-17.483141;,
   7.729000;4.302808;-17.483141;,
   7.729000;3.827978;-14.606060;,
   7.729000;3.827978;-14.606060;,
   7.729000;3.827978;-14.606060;,
   7.729000;3.827978;-14.606060;,
   6.271000;3.827978;-14.606060;,
   6.271000;3.827978;-14.606060;,
   6.271000;3.827978;-14.606060;,
   6.271000;3.827978;-14.606060;,
   6.271000;4.302808;-17.483141;,
   6.271000;4.302808;-17.483141;,
   6.271000;4.302808;-17.483141;,
   6.271000;4.302808;-17.483141;,
   7.656100;4.715828;-17.353245;,
   7.656100;4.715828;-17.353245;,
   7.656100;4.715828;-17.353245;,
   7.656100;4.715828;-17.353245;,
   7.656100;4.522747;-14.735956;,
   7.656100;4.522747;-14.735956;,
   7.656100;4.522747;-14.735956;,
   7.656100;4.522747;-14.735956;,
   6.343900;4.522747;-14.735956;,
   6.343900;4.522747;-14.735956;,
   6.343900;4.522747;-14.735956;,
   6.343900;4.522747;-14.735956;,
   6.343900;4.715828;-17.353245;,
   6.343900;4.715828;-17.353245;,
   6.343900;4.715828;-17.353245;,
   6.343900;4.715828;-17.353245;,
   7.590490;5.093523;-17.317421;,
   7.590490;5.093523;-17.317421;,
   7.590490;5.093523;-17.317421;,
   7.590490;5.160525;-14.956411;,
   7.590490;5.160525;-14.956411;,
   7.590490;5.160525;-14.956411;,
   6.409510;5.160525;-14.956411;,
   6.409510;5.160525;-14.956411;,
   6.409510;5.160525;-14.956411;,
   6.409510;5.093523;-17.317421;,
   6.409510;5.093523;-17.317421;,
   6.409510;5.093523;-17.317421;,
   -21.500000;11.400000;-8.099998;,
   -21.500000;5.900000;-18.200001;,
   -21.500000;5.900000;-18.200001;,
   19.000000;11.400000;-8.099998;,
   19.000000;5.900000;-18.200001;,
   19.000000;5.900000;-18.200001;,
   19.000000;0.000000;-40.599998;,
   -21.500000;0.000000;-40.599998;,
   -21.500000;0.000000;-18.200001;,
   -21.500000;0.000000;-18.200001;,
   19.000000;0.000000;-18.200001;,
   19.000000;0.000000;-18.200001;,
   6.000000;0.000000;-14.000000;,
   6.000000;0.000000;-14.000000;,
   6.000000;0.000000;-14.000000;,
   8.000000;0.000000;-14.000000;,
   8.000000;0.000000;-14.000000;,
   8.000000;0.000000;-14.000000;,
   8.000000;2.500000;-14.000000;,
   8.000000;2.500000;-14.000000;,
   8.000000;2.500000;-14.000000;,
   8.000000;2.500000;-14.000000;,
   6.000000;2.500000;-14.000000;,
   6.000000;2.500000;-14.000000;,
   6.000000;2.500000;-14.000000;,
   6.000000;2.500000;-14.000000;,
   5.999246;0.000000;-18.200001;,
   5.999246;0.000000;-18.200001;,
   5.999246;0.000000;-18.200001;,
   5.999246;2.585236;-18.200001;,
   5.999246;2.585236;-18.200001;,
   5.999246;2.585236;-18.200001;,
   8.000754;0.000000;-18.200001;,
   8.000754;0.000000;-18.200001;,
   8.000754;0.000000;-18.200001;,
   8.000754;2.585236;-18.200001;,
   8.000754;2.585236;-18.200001;,
   8.000754;2.585236;-18.200001;;
   126;
   3;85,82,22;,
   3;3,182,11;,
   3;6,176,0;,
   3;152,158,161;,
   3;16,31,28;,
   3;19,34,32;,
   3;25,35,23;,
   3;29,36,37;,
   3;26,22,82;,
   3;38,17,30;,
   3;76,67,79;,
   3;83,64,77;,
   3;80,73,86;,
   3;87,70,84;,
   3;79,65,67;,
   3;83,64,77;,
   3;86,68,73;,
   3;87,70,84;,
   3;43,58,55;,
   3;40,61,49;,
   3;52,59,62;,
   3;50,63,66;,
   3;44,56,179;,
   3;46,64,71;,
   3;12,92,88;,
   3;183,96,93;,
   3;186,100,97;,
   3;7,89,101;,
   3;90,108,104;,
   3;94,112,109;,
   3;98,116,113;,
   3;102,105,117;,
   3;106,124,120;,
   3;110,128,125;,
   3;114,132,129;,
   3;118,121,133;,
   3;122,140,136;,
   3;126,144,141;,
   3;130,148,145;,
   3;134,137,149;,
   3;138,155,153;,
   3;142,159,156;,
   3;146,162,160;,
   3;150,154,163;,
   3;1,193,8;,
   3;4,199,196;,
   3;194,13,9;,
   3;165,167,168;,
   3;170,197,174;,
   3;172,166,190;,
   3;5,191,2;,
   3;2,177,180;,
   3;3,181,182;,
   3;6,187,176;,
   3;152,157,158;,
   3;16,20,31;,
   3;19,24,34;,
   3;25,39,35;,
   3;29,33,36;,
   3;21,18,81;,
   3;81,85,22;,
   3;22,21,81;,
   3;78,81,18;,
   3;2,180,5;,
   3;38,27,83;,
   3;38,83,77;,
   3;38,77,17;,
   3;76,65,67;,
   3;83,71,64;,
   3;80,68,73;,
   3;87,74,70;,
   3;79,76,65;,
   3;83,71,64;,
   3;86,80,68;,
   3;87,74,70;,
   3;43,47,58;,
   3;40,53,61;,
   3;52,57,59;,
   3;60,48,75;,
   3;75,69,63;,
   3;63,60,75;,
   3;72,75,48;,
   3;69,66,63;,
   3;54,41,188;,
   3;188,184,56;,
   3;56,54,188;,
   3;178,188,41;,
   3;184,179,56;,
   3;46,45,180;,
   3;46,180,177;,
   3;46,177,42;,
   3;46,42,51;,
   3;46,51,64;,
   3;12,185,92;,
   3;183,189,96;,
   3;186,10,100;,
   3;7,14,89;,
   3;90,95,108;,
   3;94,99,112;,
   3;98,103,116;,
   3;102,91,105;,
   3;106,111,124;,
   3;110,115,128;,
   3;114,119,132;,
   3;118,107,121;,
   3;122,127,140;,
   3;126,131,144;,
   3;130,135,148;,
   3;134,123,137;,
   3;138,143,155;,
   3;142,147,159;,
   3;146,151,162;,
   3;150,139,154;,
   3;1,192,193;,
   3;4,15,199;,
   3;194,200,13;,
   3;165,164,167;,
   3;170,171,173;,
   3;170,173,191;,
   3;170,191,197;,
   3;169,175,201;,
   3;201,195,166;,
   3;166,169,201;,
   3;198,201,175;,
   3;195,190,166;,
   3;5,197,191;;

   MeshNormals {
    202;
    1.000000;0.000000;0.000000;,
    0.999993;0.000000;-0.003772;,
    0.000000;1.000000;0.000000;,
    -1.000000;0.000000;0.000000;,
    -0.999993;0.000000;-0.003772;,
    0.000000;1.000000;0.000000;,
    1.000000;0.000000;0.000000;,
    0.000000;-0.156797;0.987631;,
    0.999993;0.000000;-0.003772;,
    0.000000;-0.919940;-0.392060;,
    0.952760;-0.303723;0.000000;,
    -1.000000;0.000000;0.000000;,
    -0.992007;-0.126186;0.000000;,
    0.000000;-0.919940;-0.392060;,
    0.000000;-0.156797;0.987631;,
    -0.999993;0.000000;-0.003772;,
    1.000000;0.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;0.000000;1.000000;,
    0.000000;-1.000000;0.000000;,
    1.000000;0.000000;0.000000;,
    0.000000;0.000000;1.000000;,
    0.000000;0.000000;1.000000;,
    -1.000000;0.000000;0.000000;,
    0.000000;-1.000000;0.000000;,
    -1.000000;0.000000;0.000000;,
    0.000000;0.000000;1.000000;,
    0.000000;1.000000;0.000000;,
    1.000000;0.000000;0.000000;,
    0.000000;0.000000;-1.000000;,
    0.000000;1.000000;0.000000;,
    1.000000;0.000000;0.000000;,
    0.000000;-1.000000;0.000000;,
    0.000000;0.000000;-1.000000;,
    0.000000;-1.000000;0.000000;,
    -1.000000;0.000000;0.000000;,
    0.000000;0.000000;-1.000000;,
    0.000000;0.000000;-1.000000;,
    0.000000;1.000000;0.000000;,
    -1.000000;0.000000;0.000000;,
    1.000000;0.000000;0.000000;,
    0.000000;0.000000;1.000000;,
    0.000000;1.000000;0.000000;,
    -1.000000;0.000000;0.000000;,
    0.000000;0.000000;1.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    -1.000000;0.000000;0.000000;,
    0.000000;0.000000;-1.000000;,
    1.000000;0.000000;0.000000;,
    0.000000;0.000000;-1.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;-1.000000;0.000000;,
    1.000000;0.000000;0.000000;,
    0.000000;0.000000;1.000000;,
    -1.000000;0.000000;0.000000;,
    0.000000;0.000000;1.000000;,
    0.000000;-1.000000;0.000000;,
    -1.000000;0.000000;0.000000;,
    0.000000;-1.000000;0.000000;,
    0.000000;0.000000;-1.000000;,
    1.000000;0.000000;0.000000;,
    0.000000;-1.000000;0.000000;,
    0.000000;0.000000;-1.000000;,
    0.000000;1.000000;0.000000;,
    1.000000;0.000000;0.000000;,
    0.000000;0.000000;-1.000000;,
    1.000000;0.000000;0.000000;,
    0.000000;-1.000000;0.000000;,
    0.000000;0.000000;-1.000000;,
    -1.000000;0.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;0.000000;-1.000000;,
    0.000000;-1.000000;0.000000;,
    -1.000000;0.000000;0.000000;,
    0.000000;0.000000;-1.000000;,
    1.000000;0.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;0.000000;1.000000;,
    1.000000;0.000000;0.000000;,
    0.000000;-1.000000;0.000000;,
    0.000000;0.000000;1.000000;,
    0.000000;0.000000;1.000000;,
    0.000000;1.000000;0.000000;,
    -1.000000;0.000000;0.000000;,
    0.000000;0.000000;1.000000;,
    0.000000;-1.000000;0.000000;,
    -1.000000;0.000000;0.000000;,
    -0.992007;-0.126186;0.000000;,
    0.000000;-0.156797;0.987631;,
    -0.988789;-0.148027;-0.019596;,
    0.000000;-0.398983;0.916958;,
    -0.992007;-0.126186;0.000000;,
    0.000000;-0.698656;-0.715458;,
    0.000000;-0.400455;-0.916317;,
    -0.988789;-0.148027;-0.019596;,
    0.000000;-0.698656;-0.715458;,
    0.952760;-0.303723;0.000000;,
    0.965026;-0.259886;-0.034404;,
    0.000000;-0.400455;-0.916317;,
    0.952760;-0.303723;0.000000;,
    0.000000;-0.156797;0.987631;,
    0.000000;-0.398983;0.916958;,
    0.965026;-0.259886;-0.034404;,
    -0.988789;-0.148027;-0.019596;,
    0.000000;-0.398983;0.916958;,
    -0.985393;-0.166430;-0.036065;,
    0.000000;-0.307243;0.951631;,
    -0.988789;-0.148027;-0.019596;,
    0.000000;-0.400455;-0.916317;,
    0.000000;-0.214308;-0.976766;,
    -0.985393;-0.166430;-0.036065;,
    0.000000;-0.400455;-0.916317;,
    0.965026;-0.259886;-0.034404;,
    0.991409;-0.127830;-0.027701;,
    0.000000;-0.214308;-0.976766;,
    0.965026;-0.259886;-0.034404;,
    0.000000;-0.398983;0.916958;,
    0.000000;-0.307243;0.951631;,
    0.991409;-0.127830;-0.027701;,
    -0.985393;-0.166430;-0.036065;,
    0.000000;-0.307243;0.951631;,
    -0.985845;-0.165421;-0.027301;,
    0.000000;-0.300016;0.953934;,
    -0.985393;-0.166430;-0.036065;,
    0.000000;-0.214308;-0.976766;,
    0.000000;-0.183779;-0.982968;,
    -0.985845;-0.165421;-0.027301;,
    0.000000;-0.214308;-0.976766;,
    0.991409;-0.127830;-0.027701;,
    0.994033;-0.107622;-0.017762;,
    0.000000;-0.183779;-0.982968;,
    0.991409;-0.127830;-0.027701;,
    0.000000;-0.307243;0.951631;,
    0.000000;-0.300016;0.953934;,
    0.994033;-0.107622;-0.017762;,
    -0.985845;-0.165421;-0.027301;,
    0.000000;-0.300016;0.953934;,
    -0.985368;-0.169980;-0.012540;,
    0.000000;-0.094425;0.995532;,
    -0.985845;-0.165421;-0.027301;,
    0.000000;-0.183779;-0.982968;,
    0.000000;-0.326695;-0.945130;,
    -0.985368;-0.169980;-0.012540;,
    0.000000;-0.183779;-0.982968;,
    0.994033;-0.107622;-0.017762;,
    0.994444;-0.104978;-0.007744;,
    0.000000;-0.326695;-0.945130;,
    0.994033;-0.107622;-0.017762;,
    0.000000;-0.300016;0.953934;,
    0.000000;-0.094425;0.995532;,
    0.994444;-0.104978;-0.007744;,
    0.000000;-0.999598;0.028367;,
    -0.985368;-0.169980;-0.012540;,
    0.000000;-0.094425;0.995532;,
    -0.985368;-0.169980;-0.012540;,
    0.000000;-0.326695;-0.945130;,
    0.000000;-0.999598;0.028367;,
    0.000000;-0.999598;0.028367;,
    0.000000;-0.326695;-0.945130;,
    0.994444;-0.104978;-0.007744;,
    0.000000;-0.999598;0.028367;,
    0.994444;-0.104978;-0.007744;,
    0.000000;-0.094425;0.995532;,
    0.000000;0.878228;-0.478243;,
    0.000000;0.878228;-0.478243;,
    0.000000;0.000000;-1.000000;,
    0.000000;0.878228;-0.478243;,
    0.000000;0.878228;-0.478243;,
    0.000000;0.000000;-1.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;0.000000;-1.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;0.000000;-1.000000;,
    1.000000;0.000000;0.000000;,
    0.000000;1.000000;0.000000;,
    0.000000;0.000000;1.000000;,
    0.000000;0.000000;1.000000;,
    0.000000;1.000000;0.000000;,
    -1.000000;0.000000;0.000000;,
    -1.000000;0.000000;0.000000;,
    0.000000;-0.698656;-0.715458;,
    0.000000;0.000000;1.000000;,
    -0.992007;-0.126186;0.000000;,
    0.952760;-0.303723;0.000000;,
    1.000000;0.000000;0.000000;,
    0.000000;0.000000;1.000000;,
    0.000000;-0.698656;-0.715458;,
    0.000000;0.000000;-1.000000;,
    0.000000;1.000000;0.000000;,
    0.999993;0.000000;-0.003772;,
    0.999993;0.000000;-0.003772;,
    0.000000;-0.919940;-0.392060;,
    0.000000;0.000000;-1.000000;,
    -0.999993;0.000000;-0.003772;,
    0.000000;1.000000;0.000000;,
    0.000000;0.000000;-1.000000;,
    -0.999993;0.000000;-0.003772;,
    0.000000;-0.919940;-0.392060;,
    0.000000;0.000000;-1.000000;;
    126;
    3;85,82,22;,
    3;3,182,11;,
    3;6,176,0;,
    3;152,158,161;,
    3;16,31,28;,
    3;19,34,32;,
    3;25,35,23;,
    3;29,36,37;,
    3;26,22,82;,
    3;38,17,30;,
    3;76,67,79;,
    3;83,64,77;,
    3;80,73,86;,
    3;87,70,84;,
    3;79,65,67;,
    3;83,64,77;,
    3;86,68,73;,
    3;87,70,84;,
    3;43,58,55;,
    3;40,61,49;,
    3;52,59,62;,
    3;50,63,66;,
    3;44,56,179;,
    3;46,64,71;,
    3;12,92,88;,
    3;183,96,93;,
    3;186,100,97;,
    3;7,89,101;,
    3;90,108,104;,
    3;94,112,109;,
    3;98,116,113;,
    3;102,105,117;,
    3;106,124,120;,
    3;110,128,125;,
    3;114,132,129;,
    3;118,121,133;,
    3;122,140,136;,
    3;126,144,141;,
    3;130,148,145;,
    3;134,137,149;,
    3;138,155,153;,
    3;142,159,156;,
    3;146,162,160;,
    3;150,154,163;,
    3;1,193,8;,
    3;4,199,196;,
    3;194,13,9;,
    3;165,167,168;,
    3;170,197,174;,
    3;172,166,190;,
    3;5,191,2;,
    3;2,177,180;,
    3;3,181,182;,
    3;6,187,176;,
    3;152,157,158;,
    3;16,20,31;,
    3;19,24,34;,
    3;25,39,35;,
    3;29,33,36;,
    3;21,18,81;,
    3;81,85,22;,
    3;22,21,81;,
    3;78,81,18;,
    3;2,180,5;,
    3;38,27,83;,
    3;38,83,77;,
    3;38,77,17;,
    3;76,65,67;,
    3;83,71,64;,
    3;80,68,73;,
    3;87,74,70;,
    3;79,76,65;,
    3;83,71,64;,
    3;86,80,68;,
    3;87,74,70;,
    3;43,47,58;,
    3;40,53,61;,
    3;52,57,59;,
    3;60,48,75;,
    3;75,69,63;,
    3;63,60,75;,
    3;72,75,48;,
    3;69,66,63;,
    3;54,41,188;,
    3;188,184,56;,
    3;56,54,188;,
    3;178,188,41;,
    3;184,179,56;,
    3;46,45,180;,
    3;46,180,177;,
    3;46,177,42;,
    3;46,42,51;,
    3;46,51,64;,
    3;12,185,92;,
    3;183,189,96;,
    3;186,10,100;,
    3;7,14,89;,
    3;90,95,108;,
    3;94,99,112;,
    3;98,103,116;,
    3;102,91,105;,
    3;106,111,124;,
    3;110,115,128;,
    3;114,119,132;,
    3;118,107,121;,
    3;122,127,140;,
    3;126,131,144;,
    3;130,135,148;,
    3;134,123,137;,
    3;138,143,155;,
    3;142,147,159;,
    3;146,151,162;,
    3;150,139,154;,
    3;1,192,193;,
    3;4,15,199;,
    3;194,200,13;,
    3;165,164,167;,
    3;170,171,173;,
    3;170,173,191;,
    3;170,191,197;,
    3;169,175,201;,
    3;201,195,166;,
    3;166,169,201;,
    3;198,201,175;,
    3;195,190,166;,
    3;5,197,191;;
   }

   VertexDuplicationIndices {
    202;
    200;
    0,
    0,
    0,
    3,
    3,
    3,
    6,
    6,
    6,
    6,
    6,
    11,
    11,
    11,
    11,
    11,
    16,
    16,
    16,
    19,
    19,
    19,
    22,
    22,
    22,
    25,
    25,
    25,
    28,
    28,
    28,
    31,
    31,
    31,
    34,
    34,
    34,
    37,
    37,
    37,
    40,
    40,
    40,
    43,
    43,
    43,
    46,
    46,
    46,
    49,
    49,
    49,
    52,
    52,
    52,
    55,
    55,
    55,
    58,
    58,
    58,
    61,
    61,
    61,
    64,
    64,
    64,
    67,
    67,
    67,
    70,
    70,
    70,
    73,
    73,
    73,
    76,
    76,
    76,
    79,
    79,
    79,
    82,
    82,
    82,
    85,
    85,
    85,
    88,
    88,
    88,
    88,
    92,
    92,
    92,
    92,
    96,
    96,
    96,
    96,
    100,
    100,
    100,
    100,
    104,
    104,
    104,
    104,
    108,
    108,
    108,
    108,
    112,
    112,
    112,
    112,
    116,
    116,
    116,
    116,
    120,
    120,
    120,
    120,
    124,
    124,
    124,
    124,
    128,
    128,
    128,
    128,
    132,
    132,
    132,
    132,
    136,
    136,
    136,
    136,
    140,
    140,
    140,
    140,
    144,
    144,
    144,
    144,
    148,
    148,
    148,
    148,
    152,
    152,
    152,
    155,
    155,
    155,
    158,
    158,
    158,
    161,
    161,
    161,
    164,
    165,
    165,
    167,
    168,
    168,
    170,
    171,
    172,
    172,
    174,
    174,
    176,
    176,
    176,
    179,
    179,
    179,
    182,
    182,
    182,
    182,
    186,
    186,
    186,
    186,
    190,
    190,
    190,
    193,
    193,
    193,
    196,
    196,
    196,
    199,
    199,
    199;
   }

   MeshMaterialList {
    1;
    126;
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0;

    Material {
     0.784314;0.784314;0.784314;1.000000;;
     51.200001;
     0.000000;0.000000;0.000000;;
     0.000000;0.000000;0.000000;;
     
     EffectInstance {
      "specular.fx";
      EffectParamFloats {
       "Diffuse";
       4;
       1.000000,1.000000,1.000000,1.000000;
      }
      EffectParamFloats {
       "Specular";
       4;
       1.000000,1.000000,1.000000,1.000000;
      }
      EffectParamFloats {
       "Reflectivity";
       1;
       0.800000;
      }
      EffectParamFloats {
       "Power";
       1;
       20.000000;
      }
     }
    }
   }
  }
 }
}