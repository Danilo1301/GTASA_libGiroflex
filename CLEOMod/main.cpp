#include "pch.h"
#include "Mod.h"

MYMODCFG(net.danilo1301.giroflex, Giroflex, Mod::Version, Danilo1301)
BEGIN_DEPLIST()
//ADD_DEPENDENCY_VER(net.rusjj.aml, 1.0.0.6)
END_DEPLIST()

extern "C" void OnModPreLoad()
{
    Mod::OnModPreLoad();
}

extern "C" void OnModLoad()
{
    Mod::OnModLoad();
}

/*
https://gtaforums.com/topic/923185-cleo-mod-menu-like-in-gta-v/

void DrawRect(CRect posn, CRGBA color)
{
    void (*func)(CRect, CRGBA) = (void (*)(CRect, CRGBA)) pDrawRect;
    func(posn, color);

    float x = 20.0f;
    float y = 200.0f;
    float w = 300.0f;
    float h = 50.0f;

    CRect rect = { x, y, x + w, y + h };
    CRGBA color = { 255, 0, 0, 255 };

    DrawRect(rect, color);
}

pDrawRect = aml->GetSym(hGTASA, "CSprite2d4DrawERK5CRectRK5CRGBA");

*/


/*
const char* valRes;

__decl_op(GET_STRING, 0x0EF7); // 0EF7=1,%1d% = get_string
void GET_STRING(__handler_params)
{
    valRes = "";
    valRes = "GIRO_ID";

    char szTemp[256];
    sprintf(szTemp, "GET_STRING");
    Log::opcodes << szTemp << std::endl;

    cleo->GetPointerToScriptVar(handle)->i = (int)valRes;

    //cleo->GetPointerToScriptVar();

 

    char str[64];
    cleo->ReadStringLong(handle, str, sizeof(str)); str[sizeof(str) - 1] = 0;

   

    char szTemp[256];
    sprintf(szTemp, "GET_STRING i=%d u=%uz", param->i, param->u);
    Log::opcodes << szTemp << std::endl;


    char* variable = (char*)u;
    strcpy(variable, "GIRO_ID");
   
}
*/