#include <iostream>
#include <filesystem>
#include <windows.h>
#include <gdiplus.h>

using namespace Gdiplus;
namespace fs = std::filesystem;

// Function to get the CLSID of the encoder
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0;
    UINT size = 0;
    GetImageEncodersSize(&num, &size);
    if (size == 0) return -1;

    ImageCodecInfo* pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == nullptr) return -1;
    GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT i = 0; i < num; i++) {
        if (wcscmp(pImageCodecInfo[i].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[i].Clsid;
            free(pImageCodecInfo);
            return i;
        }
    }

    free(pImageCodecInfo);
    return -1;
}

// New helper method that checks if the image is a PNG and converts it to JPG if so.
std::wstring ConvertPngToJpg(const std::wstring& filePath) {
    fs::path p(filePath);
    // Check if file extension is .png (case insensitive)
    if (p.extension() == L".png" || p.extension() == L".PNG") {
        // Load the image using GDI+ Bitmap.
        Bitmap* bmp = Bitmap::FromFile(filePath.c_str());
        if (bmp == nullptr || bmp->GetLastStatus() != Ok) {
            std::wcerr << L"  -> Failed to load PNG image: " << filePath << std::endl;
            delete bmp;
            return filePath; // Return original if failed.
        }
        // Create a clone of the image to release the file lock.
        Bitmap* bmpClone = bmp->Clone(0, 0, bmp->GetWidth(), bmp->GetHeight(), bmp->GetPixelFormat());
        delete bmp; // Now the file is no longer locked.
        if (bmpClone == nullptr || bmpClone->GetLastStatus() != Ok) {
            std::wcerr << L"  -> Failed to clone PNG image: " << filePath << std::endl;
            delete bmpClone;
            return filePath;
        }
        
        // Create new file path with .jpg extension.
        fs::path newPath = p;
        newPath.replace_extension(L".jpg");

        // Get the JPEG encoder.
        CLSID jpgClsid;
        if (GetEncoderClsid(L"image/jpeg", &jpgClsid) < 0) {
            std::wcerr << L"  -> JPEG encoder not found." << std::endl;
            delete bmpClone;
            return filePath;
        }
        
        // Save the cloned image as JPEG.
        Status status = bmpClone->Save(newPath.c_str(), &jpgClsid, NULL);
        if (status != Ok) {
            std::wcerr << L"  -> Failed to save JPEG image: " << newPath.wstring() << std::endl;
            delete bmpClone;
            return filePath;
        }
        
        delete bmpClone;
        
        // Now that the file is no longer locked, delete the original PNG file.
        if (fs::remove(filePath)) {
            std::wcout << L"  -> Converted " << filePath << L" to " << newPath.wstring() << std::endl;
        } else {
            std::wcerr << L"  -> Failed to delete original PNG: " << filePath << std::endl;
        }
        // Return the new file path (with a .jpg extension).
        return newPath.wstring();
    }
    // If not PNG, just return the original path.
    return filePath;
}

void ProcessImages() {
    // Initialize GDI+
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    if (GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) != Ok) {
        std::cerr << "Failed to initialize GDI+." << std::endl;
        return;
    }

    // Retrieve current primary monitor dimensions.
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    // Calculate target minimum dimensions (80% of screen).
    int targetWidth = static_cast<int>(2 * screenWidth);
    int targetHeight = static_cast<int>(2 * screenHeight);

    // Path to the folder with images.
    std::wstring folderPath = L"assets/Textures/Backgrounds/Valoria/";

    // Iterate through all files in the folder.
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            std::wstring filePath = entry.path().wstring();
            // Run our conversion method before resizing.
            filePath = ConvertPngToJpg(filePath);

            // Load image using GDI+ Bitmap.
            Bitmap* bitmap = Bitmap::FromFile(filePath.c_str());
            if (bitmap == nullptr || bitmap->GetLastStatus() != Ok) {
                std::wcerr << L"  -> Failed to load image: " << filePath << std::endl;
                delete bitmap;
                continue;
            }

            UINT imgWidth = bitmap->GetWidth();
            UINT imgHeight = bitmap->GetHeight();

            bool needsResize = (imgWidth < targetWidth) || (imgHeight < targetHeight);
            if (needsResize) {
                // Calculate the scale factor.
                double scaleX = static_cast<double>(targetWidth) / imgWidth;
                double scaleY = static_cast<double>(targetHeight) / imgHeight;
                double scaleFactor = std::max(scaleX, scaleY);
                int newWidth = static_cast<int>(imgWidth * scaleFactor);
                int newHeight = static_cast<int>(imgHeight * scaleFactor);

                // Create a new Bitmap for the scaled image.
                Bitmap* newBitmap = new Bitmap(newWidth, newHeight, bitmap->GetPixelFormat());
                if (newBitmap == nullptr) {
                    std::wcerr << L"  -> Failed to create new bitmap." << std::endl;
                    delete bitmap;
                    continue;
                }
                Graphics graphics(newBitmap);
                graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);
                graphics.DrawImage(bitmap, 0, 0, newWidth, newHeight);

                // Delete the original bitmap to free up memory.
                delete bitmap;

                // Delete the original file.
                if (fs::remove(filePath)) {
                    // std::wcout << L"  -> Deleted original image: " << filePath << std::endl;
                } else {
                    std::wcerr << L"  -> Failed to delete original image: " << filePath << std::endl;
                }

                // Save the resized image using the JPEG encoder (since we prefer JPEG now).
                CLSID jpgClsid;
                if (GetEncoderClsid(L"image/jpeg", &jpgClsid) < 0) {
                    std::wcerr << L"  -> JPEG encoder not found." << std::endl;
                } else {
                    Status saveStatus = newBitmap->Save(filePath.c_str(), &jpgClsid, NULL);
                    if (saveStatus != Ok) {
                        std::wcerr << L"  -> Failed to save image: " << filePath << std::endl;
                    } else {
                        // std::wcout << L"  -> Image saved successfully." << std::endl;
                    }
                }
                delete newBitmap;
            } else {
                // No scaling needed; just free the memory.
                delete bitmap;
            }
        }
    }
    GdiplusShutdown(gdiplusToken);
}

int main() {
    ProcessImages();
    return 0;
}
