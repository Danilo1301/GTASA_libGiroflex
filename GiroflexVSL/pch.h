#pragma once

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <filesystem>
#include <map>
#include <list>
//#include <windows.h>
#include <sys/stat.h>

#define _USE_MATH_DEFINES
#include <cmath>

#include "menu/MenuStructs.h"
#include "menu/SimpleGTA.h"
#include "SimpleGTA.h"

#include "json/json.h"

#include "ibass.h"

extern RwMatrix* (*RwMatrixTranslate)(RwMatrix* matrix, const RwV3d* translation, RwOpCombineType combineOp);
extern RwReal (*RwV3dNormalize)(RwV3d* out, const RwV3d* in);
extern void (*CMatrix_CopyToRwMatrix)(CMatrix*, RwMatrix *matrix);
extern RwMatrix* (*RwMatrixCreate)(void);
extern RwBool (*RwMatrixDestroy)(RwMatrix* mpMat);

/*
static unsigned char ucharIntensity(unsigned char uc, float intensity) {
    return (unsigned char)std::clamp((int)round(((float)uc) * intensity), 0, 255);
}
*/

static double DistanceBetween(CVector vec1, CVector vec2)
{
    double a = ((double)vec1.x - (double)vec2.x);
    double b = ((double)vec1.y - (double)vec2.y);
    double c = ((double)vec1.z - (double)vec2.z);

    return sqrt(a * a + b * b + c * c);
}

static double arch_fn_parabola(float x, float curve, float len)
{
	return -(pow(x, 2) * (double)curve) + (((double)len * (double)curve) * (double)x);
}

static std::string to_upper(std::string data) {
    std::for_each(data.begin(), data.end(), [](char& c) {
        c = ::toupper(c);
        });
    return data;
}

static std::string to_lower(std::string data) {
    std::for_each(data.begin(), data.end(), [](char& c) {
        c = ::tolower(c);
        });
    return data;
}

static Json::Value ColorToJSON(CRGBA color) {
	Json::Value json;
	json["r"] = color.r;
	json["g"] = color.g;
	json["b"] = color.b;
	json["a"] = color.a;
	return json;
}

static CRGBA ColorFromJSON(Json::Value json) {
	CRGBA color(0, 0, 0, 0);
	color.r = json["r"].asInt();
	color.g = json["g"].asInt();
	color.b = json["b"].asInt();
	color.a = json["a"].empty() ? 255 : json["a"].asInt();
	return color;
}

static Json::Value CVectorToJSON(CVector vec) {
	Json::Value json;
	json["x"] = vec.x;
	json["y"] = vec.y;
	json["z"] = vec.z;
	return json;
}

static CVector CVectorFromJSON(Json::Value json) {
	CVector vec(0, 0, 0);
	vec.x = json["x"].asFloat();
	vec.y = json["y"].asFloat();
	vec.z = json["z"].asFloat();
	return vec;
}

template<class T>
static Json::Value ValidateValue(Json::Value value, T defaultValue)
{
	if (value.empty()) return defaultValue;
	return value;
}

static CVector ValidateCVector(Json::Value value, CVector defaultValue)
{
	if (value.empty()) return defaultValue;
	return CVectorFromJSON(value);
}

static CRGBA ValidateColor(Json::Value value, CRGBA defaultValue)
{
	if (value.empty()) return defaultValue;
	return ColorFromJSON(value);
}

static int GetRandomNumber(int min, int max)
{
    int n = max - min + 1;
    int remainder = RAND_MAX % n;
    int x;
    do{
        x = rand();
    }while (x >= RAND_MAX - remainder);
    return min + x % n;
}

static std::string CVectorToString(CVector vec)
{
    return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z) + ")";
}

static std::string CVector2DToString(CVector2D vec)
{
    return "(" + std::to_string(vec.x) + ", " + std::to_string(vec.y) + ")";
}

static double GetAngleBetweenVectors(CVector v1, CVector v2, CVector v3)
{
	double v12 = sqrt(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2));
	double v13 = sqrt(pow(v1.x - v3.x, 2) + pow(v1.y - v3.y, 2));
	double v23 = sqrt(pow(v2.x - v3.x, 2) + pow(v2.y - v3.y, 2));
	return acos((pow(v12, 2) + pow(v13, 2) - pow(v23, 2)) / (2 * (v12 * v13)));
}

static CVector RotateVectorAroundZ(CVector vector, float angleRadians) {

    //float angleRadians = angleDegrees * (3.14159265f / 180.0f);

    // Calcula o cosseno e o seno do ângulo
    float cosTheta = cosf(angleRadians);
    float sinTheta = sinf(angleRadians);

    // Aplica a rotação no plano XY
    CVector rotatedVector = CVector(0, 0, 0);
    rotatedVector.x = vector.x * cosTheta - vector.y * sinTheta;
    rotatedVector.y = vector.x * sinTheta + vector.y * cosTheta;
    rotatedVector.z = vector.z; // A componente Z não muda

    return rotatedVector;
}

static RwMatrix* CloneRwMatrix(const RwMatrix* originalMatrix)
{
    // Cria uma nova matriz
    RwMatrix* clonedMatrix = RwMatrixCreate();
    if (!clonedMatrix)
        return NULL; // Retorna NULL em caso de falha

    // Copia os vetores da matriz original para a nova matriz
    clonedMatrix->right = originalMatrix->right;
    clonedMatrix->up = originalMatrix->up;
    clonedMatrix->at = originalMatrix->at;
    clonedMatrix->pos = originalMatrix->pos;

    // Define o status como idêntico ao da original
    clonedMatrix->flags = originalMatrix->flags;

    return clonedMatrix;
}

static CVector CMatrixGetForward(CMatrix* cmatrix)
{
	RwMatrix* matrix = RwMatrixCreate();
	
	//entity->m_matrix->CopyToRwMatrix(matrix);
	CMatrix_CopyToRwMatrix(cmatrix, matrix);

	RwV3d forward = { matrix->up.x, matrix->up.y, matrix->up.z };

	RwMatrixDestroy(matrix);

	return CVector(forward.x, forward.y, forward.z);
}

static CVector MatrixGetForward(RwMatrix* matrix)
{
	RwV3d forward = { matrix->up.x, matrix->up.y, matrix->up.z };

	return CVector(forward.x, forward.y, forward.z);
}

static CVector TransformFromMatrixSpace(RwMatrix* originalMatrix, CVector pos)
{
	RwMatrix* matrix = CloneRwMatrix(originalMatrix);

	RwV3d forward = { matrix->up.x, matrix->up.y, matrix->up.z };
	RwV3d right = { matrix->right.x, matrix->right.y, matrix->right.z };
	RwV3d up = { matrix->at.x, matrix->at.y, matrix->at.z };

	RwV3dNormalize(&forward, &forward);
	RwV3dNormalize(&right, &right);
	RwV3dNormalize(&up, &up);

	RwV3d translate;
	translate.x = forward.x * pos.y;
	translate.y = forward.y * pos.y;
	translate.z = forward.z * pos.y;

	translate.x += right.x * pos.x;
	translate.y += right.y * pos.x;
	translate.z += right.z * pos.x;

	translate.x += up.x * pos.z;
	translate.y += up.y * pos.z;
	translate.z += up.z * pos.z;
	
	//RwIm3DTransform
	
	// Translate the matrix
	RwMatrixTranslate(matrix, &translate, rwCOMBINEPOSTCONCAT);

	auto finalPos = CVector(matrix->pos.x, matrix->pos.y, matrix->pos.z);

	RwMatrixDestroy(matrix);

	return finalPos;
}

static CVector TransformFromObjectSpace(CEntity* entity, CVector pos)
{
	RwMatrix* matrix = RwMatrixCreate();
	
	//entity->m_matrix->CopyToRwMatrix(matrix);
	CMatrix_CopyToRwMatrix(entity->m_matrix, matrix);

	RwV3d forward = { matrix->up.x, matrix->up.y, matrix->up.z };
	RwV3d right = { matrix->right.x, matrix->right.y, matrix->right.z };
	RwV3d up = { matrix->at.x, matrix->at.y, matrix->at.z };

	RwV3dNormalize(&forward, &forward);
	RwV3dNormalize(&right, &right);
	RwV3dNormalize(&up, &up);

	RwV3d translate;
	translate.x = forward.x * pos.y;
	translate.y = forward.y * pos.y;
	translate.z = forward.z * pos.y;

	translate.x += right.x * pos.x;
	translate.y += right.y * pos.x;
	translate.z += right.z * pos.x;

	translate.x += up.x * pos.z;
	translate.y += up.y * pos.z;
	translate.z += up.z * pos.z;
	
	//RwIm3DTransform
	
	// Translate the matrix
	RwMatrixTranslate(matrix, &translate, rwCOMBINEPOSTCONCAT);

	auto finalPos = CVector(matrix->pos.x, matrix->pos.y, matrix->pos.z);

	RwMatrixDestroy(matrix);

	return finalPos;
}

static float calculateAngleVec2D(const CVector2D& A, const CVector2D& B, const CVector2D& C) {
    // Vetores AB e BC
    CVector2D AB(B.x - A.x, B.y - A.y);
    CVector2D BC(C.x - B.x, C.y - B.y);

    // Ângulos em relação ao eixo x usando atan2
    float angleAB = std::atan2(AB.y, AB.x);
    float angleBC = std::atan2(BC.y, BC.x);

    // Diferença entre os ângulos
    float angle = angleBC - angleAB;

    // Converte de radianos para graus, se necessário
    angle = angle * 180.0f / M_PI;

    // Garantir que o ângulo esteja no intervalo [0, 360)
    if (angle < 0) {
        angle += 360.0f;
    }

    return angle;
}