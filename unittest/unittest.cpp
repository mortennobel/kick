//
//  unittest.cpp
//  KickCPP
//
//  Created by Morten Nobel-Jørgensen on 10/14/13.
//  Copyright (c) 2013 Morten Nobel-Joergensen. All rights reserved.
//


#include "tinytest/tinytest.h"
#include "EngineTest.h"
#include "math_test.h"

#include "glm_ext.h"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/string_cast.hpp"
#include <string>

using namespace std;

TINYTEST_START_SUITE(ObjSuite);
TINYTEST_ADD_TEST(TestShader);
TINYTEST_ADD_TEST(TestMaterial);
TINYTEST_ADD_TEST(TestLoadTextFile);
TINYTEST_ADD_TEST(TestLoadBinaryFile);
TINYTEST_ADD_TEST(TestLoadTexture);
TINYTEST_ADD_TEST(TestLoadTextureTypes);
TINYTEST_ADD_TEST(TestShaderPreCompiler);
TINYTEST_ADD_TEST(TestDefaultShaders);
TINYTEST_ADD_TEST(TestMesh);
TINYTEST_ADD_TEST(TestMeshData);
TINYTEST_ADD_TEST(RayClosestPointsTest);
TINYTEST_ADD_TEST(TestMeshFactory);
TINYTEST_ADD_TEST(TestShaderLoading);
TINYTEST_ADD_TEST(TestKeyInputButton);
TINYTEST_ADD_TEST(TestKeyMapping);
TINYTEST_ADD_TEST(TestKeyMappingMulti);
TINYTEST_ADD_TEST(TestMouseInputButton);
TINYTEST_ADD_TEST(TestTransform);
TINYTEST_ADD_TEST(TestGetComponent);
TINYTEST_ADD_TEST(TestGetComponents);
TINYTEST_ADD_TEST(TestDeleteComponent);
TINYTEST_ADD_TEST(TestTransformComponent);
TINYTEST_ADD_TEST(TestTransformRotationsComponent);
TINYTEST_ADD_TEST(TestTransformLookAt);
TINYTEST_END_SUITE();

int main(int argc, char* argv[])
{
    // Manually launch TinyTest
    TinyTestRegistry registry;
    registry.m_headSuite = NULL;
    initEngine(argc, argv);

TINYTEST_RUN_SUITE(ObjSuite);
TINYTEST_END_MAIN();