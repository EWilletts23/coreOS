#include "kernelUtil.h"
#include "BasicRenderer.h"


BasicRenderer* GlobalRenderer;

BasicRenderer::BasicRenderer(Framebuffer* targetFramebuffer, PSF1_FONT* psf1_Font)
{
    TargetFramebuffer = targetFramebuffer;
    PSF1_Font = psf1_Font;
    Colour = 0xffffffff;
    CursorPosition = {0, 0};
}

void BasicRenderer::PutPix(uint32_t x, uint32_t y, uint32_t colour){
    *(uint32_t*)((uint64_t)TargetFramebuffer->BaseAddress + (x*4) + (y * TargetFramebuffer->PixelsPerScanLine * 4)) = colour;
}

uint32_t BasicRenderer::GetPix(uint32_t x, uint32_t y){
    return *(uint32_t*)((uint64_t)TargetFramebuffer->BaseAddress + (x*4) + (y * TargetFramebuffer->PixelsPerScanLine * 4));
}

void BasicRenderer::ClearMouseCursor(uint8_t* mouseCursor, Point position){
    if (!MouseDrawn) return;

    int xMax = 16;
    int yMax = 16;
    int differenceX = TargetFramebuffer->Width - position.X;
    int differenceY = TargetFramebuffer->Height - position.Y;

    if (differenceX < 16) xMax = differenceX;
    if (differenceY < 16) yMax = differenceY;

    for (int y = 0; y < yMax; y++){
        for (int x = 0; x < xMax; x++){
            int bit = y * 16 + x;
            int byte = bit / 8;
            if ((mouseCursor[byte] & (0b10000000 >> (x % 8))))
            {
                if (GetPix(position.X + x, position.Y + y) == MouseCursorBufferAfter[x + y *16]){
                    PutPix(position.X + x, position.Y + y, MouseCursorBuffer[x + y * 16]);
                }
            }
        }
    }
}

void BasicRenderer::DrawOverlayMouseCursor(uint8_t* mouseCursor, Point position, uint32_t colour){


    int xMax = 16;
    int yMax = 16;
    int differenceX = TargetFramebuffer->Width - position.X;
    int differenceY = TargetFramebuffer->Height - position.Y;

    if (differenceX < 16) xMax = differenceX;
    if (differenceY < 16) yMax = differenceY;

    for (int y = 0; y < yMax; y++){
        for (int x = 0; x < xMax; x++){
            int bit = y * 16 + x;
            int byte = bit / 8;
            if ((mouseCursor[byte] & (0b10000000 >> (x % 8))))
            {
                MouseCursorBuffer[x + y * 16] = GetPix(position.X + x, position.Y + y);
                PutPix(position.X + x, position.Y + y, colour);
                MouseCursorBufferAfter[x + y * 16] = GetPix(position.X + x, position.Y + y);

            }
        }
    }

    MouseDrawn = true;
}

void BasicRenderer::Clear(){
    uint64_t fbBase = (uint64_t)TargetFramebuffer->BaseAddress;
    uint64_t bytesPerScanline = TargetFramebuffer->PixelsPerScanLine * 4;
    uint64_t fbHeight = TargetFramebuffer->Height;
    uint64_t fbSize = TargetFramebuffer->BufferSize;

    for (int verticalScanline = 0; verticalScanline < fbHeight; verticalScanline ++){
        uint64_t pixPtrBase = fbBase + (bytesPerScanline * verticalScanline);
        for (uint32_t* pixPtr = (uint32_t*)pixPtrBase; pixPtr < (uint32_t*)(pixPtrBase + bytesPerScanline); pixPtr ++){
            *pixPtr = ClearColour;
        }
    }
}

void BasicRenderer::ClearChar(){

    if (CursorPosition.X == 0){
        CursorPosition.X = TargetFramebuffer->Width;
        CursorPosition.Y -= 16;
        if (CursorPosition.Y < 0) CursorPosition.Y = 0;
    }

    unsigned int xOff = CursorPosition.X;
    unsigned int yOff = CursorPosition.Y;

    unsigned int* pixPtr = (unsigned int*)TargetFramebuffer->BaseAddress;
    for (unsigned long y = yOff; y < yOff + 16; y++){
        for (unsigned long x = xOff - 8; x < xOff; x++){
                    *(unsigned int*)(pixPtr + x + (y * TargetFramebuffer->PixelsPerScanLine)) = ClearColour;
        }
    }

    CursorPosition.X -= 8;

    if (CursorPosition.X < 0){
        CursorPosition.X = TargetFramebuffer->Width;
        CursorPosition.Y -= 16;
        if (CursorPosition.Y < 0) CursorPosition.Y = 0;
    }

}

void BasicRenderer::DrawWindow(const char* Title, const char* Line1, const char* Line2, const char* Line3, int Type, int X, int Y){
    if(Type == 1){
        // MSG BOX with OK button
        CursorPosition = {(TargetFramebuffer->Width / 2) - 122, TargetFramebuffer->Height / 2 - 51};
        for (int i = 0; i < 20; i++){
            for (int t = 0; t < 242; t++){
                PutPix(CursorPosition.X, CursorPosition.Y, 0x00303030);
                CursorPosition.X++;
            }
            CursorPosition.X = (TargetFramebuffer->Width / 2) - 122;
            CursorPosition.Y++;
        }
        for (int r = 0; r < 99; r++){
            PutPix(CursorPosition.X, CursorPosition.Y, 0x00303030);
            CursorPosition.X = (TargetFramebuffer->Width / 2) + 119;           
            PutPix(CursorPosition.X, CursorPosition.Y, 0x00303030);
            CursorPosition.X = (TargetFramebuffer->Width / 2) - 122;
            CursorPosition.Y++;
        }
        for (int e = 0; e < 242; e++){
            PutPix(CursorPosition.X, CursorPosition.Y, 0x00303030);
            CursorPosition.X++;
        }
        CursorPosition = {(TargetFramebuffer->Width / 2) - 121, TargetFramebuffer->Height / 2 - 31};

        for (int i = 0; i < 99; i++){
            for (int t = 0; t < 240; t++){
                PutPix(CursorPosition.X, CursorPosition.Y, 0x00454545);
                CursorPosition.X++;
            }
            CursorPosition.X = (TargetFramebuffer->Width / 2) - 121;
            CursorPosition.Y++;
        }

        CursorPosition = {(TargetFramebuffer->Width / 2) - 120, (TargetFramebuffer->Height / 2) - 49};
        Print(Title);
        CursorPosition = {(TargetFramebuffer->Width / 2) - 120, (TargetFramebuffer->Height / 2) - 29};
        Print(Line1);
        CursorPosition.X = (TargetFramebuffer->Width / 2) - 120;
        CursorPosition.Y += 16;
        Print(Line2);
        CursorPosition.X = (TargetFramebuffer->Width / 2) - 120;
        CursorPosition.Y += 16;
        Print(Line3);
        Button(200, 200);

    }
    
}

void BasicRenderer::Button(int X, int Y){
    bool ButtonClicked = false;
    CursorPosition = {X, Y};

    for (int i = 0; i < 16; i++){

        for (int t = 0; t < 32; t++){
            PutPix(CursorPosition.X, CursorPosition.Y, 0x00000000);
            CursorPosition.X++;
        }

        CursorPosition.X = X;
        CursorPosition.Y++;
    }

    CursorPosition = {X + 8, Y};
    Print("OK");
    while (ButtonClicked == false){
        if (MousePosition.X >= X && MousePosition.X <= X + 32 && MousePosition.Y >= Y && MousePosition.Y <= Y + 16 && LeftClick == true){
            Print("Clicked!");
            ButtonClicked == true;
        }
    }


}

void BasicRenderer::Next(){
    CursorPosition.X = 0;
    CursorPosition.Y += 16;
}

void BasicRenderer::Center(int OffsetX, int OffsetY){
    CursorPosition.X = (TargetFramebuffer->Width / 2) + OffsetX;
    CursorPosition.Y = (TargetFramebuffer->Height / 2) + OffsetY;

}

void BasicRenderer::Print(const char* str){
    
    char* chr = (char*)str;
    while(*chr != 0){
        PutChar(*chr, CursorPosition.X, CursorPosition.Y);
        CursorPosition.X+=8;
        if(CursorPosition.X + 8 > TargetFramebuffer->Width)
        {
            CursorPosition.X = 0;
            CursorPosition.Y += 16;
        }
        chr++;
    }
}

void BasicRenderer::PutChar(char chr, unsigned int xOff, unsigned int yOff)
{
    unsigned int* pixPtr = (unsigned int*)TargetFramebuffer->BaseAddress;
    char* fontPtr = (char*)PSF1_Font->glyphBuffer + (chr * PSF1_Font->psf1_Header->charsize);
    for (unsigned long y = yOff; y < yOff + 16; y++){
        for (unsigned long x = xOff; x < xOff+8; x++){
            if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0){
                    *(unsigned int*)(pixPtr + x + (y * TargetFramebuffer->PixelsPerScanLine)) = Colour;
                }

        }
        fontPtr++;
    }
}

void BasicRenderer::PutChar(char chr)
{
    PutChar(chr, CursorPosition.X, CursorPosition.Y);
    CursorPosition.X += 8;
    if (CursorPosition.X + 8 > TargetFramebuffer->Width){
        CursorPosition.X = 0; 
        CursorPosition.Y += 16;
    }
}