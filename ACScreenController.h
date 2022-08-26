#pragma region hello_world_example
/*
#define AC_SC_APPLICATION
#include "ACScreenController.h"
// Override base class with your custom functionality
class Example : public AC::ScreenController
{
public:
	Example()
	{
		// Name your application
		sAppName = "Example";
	}
public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Called once per frame, draws random coloured pixels
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				Draw(x, y, AC::Pixel(rand() % 256, rand() % 256, rand() % 256));
		return true;
	}
};
int main()
{
	Example demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();
	return 0;
}
*/
#pragma endregion

#ifndef AC_SC_DEF
#define AC_SC_DEF

#pragma region Standard includes
#include <cmath>
#include <cstdint>
#include <string>
#include <iostream>
#include <streambuf>
#include <sstream>
#include <chrono>
#include <vector>
#include <list>
#include <thread>
#include <atomic>
//#include <fstream>
#include <map>
#include <functional>
#include <algorithm>
#include <array>
#include <cstring>
#pragma endregion

#pragma region compiler_config
#define USE_EXPERIMENTAL_FS

#define ACT(s) L##s

#define UNUSED(x) (void)(x)

// Platform
#define AC_PLATFORM_WINAPI

// Renderer
#define AC_GFX_OPENGL10

#define NOMINMAX
#include <windows.h>

#pragma endregion

//The big boi being set up
#pragma region Controller_Declaration
namespace AC {
	class ScreenController;
	class Sprite;

	constexpr uint8_t  MouseButtons = 5;
	constexpr uint32_t DefaultPixel = (0xFF << 24);//plain black visible pixel

	//Pixel Class
	struct Pixel {
		union {
			uint32_t n = DefaultPixel;
			struct { uint8_t r; uint8_t g; uint8_t b; uint8_t a; };
		};

		enum Mode { NORMAL, MASK, ALPHA, CUSTOM };

		Pixel();
		Pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 0xFF);
		Pixel(uint32_t p);
		bool   operator ==(const Pixel& p) const;
		bool   operator !=(const Pixel& p) const;
		Pixel  operator + (const Pixel& p) const;
		Pixel  operator - (const Pixel& p) const;
		Pixel& operator +=(const Pixel& p);
		Pixel& operator -=(const Pixel& p);
		Pixel  invert() const;
	};

	Pixel PixelF(float red, float green, float blue, float alpha = 1.0f);

	#pragma region Colors+numbers
	static const Pixel
		GREY(192, 192, 192), DARK_GREY(128, 128, 128), VERY_DARK_GREY(64, 64, 64),
		RED(255, 0, 0), DARK_RED(128, 0, 0), VERY_DARK_RED(64, 0, 0),
		YELLOW(255, 255, 0), DARK_YELLOW(128, 128, 0), VERY_DARK_YELLOW(64, 64, 0),
		GREEN(0, 255, 0), DARK_GREEN(0, 128, 0), VERY_DARK_GREEN(0, 64, 0),
		CYAN(0, 255, 255), DARK_CYAN(0, 128, 128), VERY_DARK_CYAN(0, 64, 64),
		BLUE(0, 0, 255), DARK_BLUE(0, 0, 128), VERY_DARK_BLUE(0, 0, 64),
		MAGENTA(255, 0, 255), DARK_MAGENTA(128, 0, 128), VERY_DARK_MAGENTA(64, 0, 64),
		WHITE(255, 255, 255), BLACK(0, 0, 0), BLANK(0, 0, 0, 0);

	enum Key { //Oh the great keymap
		NONE,
		A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		K0, K1, K2, K3, K4, K5, K6, K7, K8, K9,
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
		UP, DOWN, LEFT, RIGHT,
		SPACE, TAB, SHIFT, CTRL, INS, DEL, HOME, END, PGUP, PGDN,
		BACK, ESCAPE, RETURN, ENTER, PAUSE, SCROLL,
		NP0, NP1, NP2, NP3, NP4, NP5, NP6, NP7, NP8, NP9,
		NP_MUL, NP_DIV, NP_ADD, NP_SUB, NP_DECIMAL, PERIOD,
		EQUALS, COMMA, MINUS,
		OEM_1, OEM_2, OEM_3, OEM_4, OEM_5, OEM_6, OEM_7, OEM_8,
		CAPS_LOCK, ENUM_END
	};

	struct HWButton {
		bool bPressed = false;	// Set once during the frame the event occurs
		bool bReleased = false;	// Set once during the frame the event occurs
		bool bHeld = false;		// Set true for all frames between pressed and released events
	};
	#pragma endregion

	//Custom vector 2d Class
#pragma region Vector_Templated
	template <class T>
	struct v2d_generic {
		T x = 0;
		T y = 0;
		v2d_generic() : x(0), y(0) {}
		v2d_generic(T _x, T _y) : x(_x), y(_y) {}
		v2d_generic(const v2d_generic& v) : x(v.x), y(v.y) {}
		v2d_generic& operator=(const v2d_generic& v) = default;
		T mag() const { return T(std::sqrt(x * x + y * y)); }
		T mag2() const { return x * x + y * y; }
		v2d_generic  norm() const { T r = 1 / mag(); return v2d_generic(x * r, y * r); }
		v2d_generic  perp() const { return v2d_generic(-y, x); }
		v2d_generic  floor() const { return v2d_generic(std::floor(x), std::floor(y)); }
		v2d_generic  ceil() const { return v2d_generic(std::ceil(x), std::ceil(y)); }
		v2d_generic  max(const v2d_generic& v) const { return v2d_generic(std::max(x, v.x), std::max(y, v.y)); }
		v2d_generic  min(const v2d_generic& v) const { return v2d_generic(std::min(x, v.x), std::min(y, v.y)); }
		T dot(const v2d_generic& rhs) const { return this->x * rhs.x + this->y * rhs.y; }
		T cross(const v2d_generic& rhs) const { return this->x * rhs.y - this->y * rhs.x; }
		v2d_generic  operator +  (const v2d_generic& rhs) const { return v2d_generic(this->x + rhs.x, this->y + rhs.y); }
		v2d_generic  operator -  (const v2d_generic& rhs) const { return v2d_generic(this->x - rhs.x, this->y - rhs.y); }
		v2d_generic  operator *  (const T& rhs)           const { return v2d_generic(this->x * rhs, this->y * rhs); }
		v2d_generic  operator *  (const v2d_generic& rhs) const { return v2d_generic(this->x * rhs.x, this->y * rhs.y); }
		v2d_generic  operator /  (const T& rhs)           const { return v2d_generic(this->x / rhs, this->y / rhs); }
		v2d_generic  operator /  (const v2d_generic& rhs) const { return v2d_generic(this->x / rhs.x, this->y / rhs.y); }
		v2d_generic& operator += (const v2d_generic& rhs) { this->x += rhs.x; this->y += rhs.y; return *this; }
		v2d_generic& operator -= (const v2d_generic& rhs) { this->x -= rhs.x; this->y -= rhs.y; return *this; }
		v2d_generic& operator *= (const T& rhs) { this->x *= rhs; this->y *= rhs; return *this; }
		v2d_generic& operator /= (const T& rhs) { this->x /= rhs; this->y /= rhs; return *this; }
		v2d_generic& operator *= (const v2d_generic& rhs) { this->x *= rhs.x; this->y *= rhs.y; return *this; }
		v2d_generic& operator /= (const v2d_generic& rhs) { this->x /= rhs.x; this->y /= rhs.y; return *this; }
		v2d_generic  operator +  () const { return { +x, +y }; }
		v2d_generic  operator -  () const { return { -x, -y }; }
		bool operator == (const v2d_generic& rhs) const { return (this->x == rhs.x && this->y == rhs.y); }
		bool operator != (const v2d_generic& rhs) const { return (this->x != rhs.x || this->y != rhs.y); }
		const std::string str() const { return std::string("(") + std::to_string(this->x) + "," + std::to_string(this->y) + ")"; }
		friend std::ostream& operator << (std::ostream& os, const v2d_generic& rhs) { os << rhs.str(); return os; }
		operator v2d_generic<int32_t>() const { return { static_cast<int32_t>(this->x), static_cast<int32_t>(this->y) }; }
		operator v2d_generic<float>() const { return { static_cast<float>(this->x), static_cast<float>(this->y) }; }
		operator v2d_generic<double>() const { return { static_cast<double>(this->x), static_cast<double>(this->y) }; }
	};

	// Note: joshinils has some good suggestions here, but they are complicated to implement at this moment, 
	// however they will appear in a future version of SC
	template<class T> inline v2d_generic<T> operator * (const float& lhs, const v2d_generic<T>& rhs)
	{
		return v2d_generic<T>((T)(lhs * (float)rhs.x), (T)(lhs * (float)rhs.y));
	}
	template<class T> inline v2d_generic<T> operator * (const double& lhs, const v2d_generic<T>& rhs)
	{
		return v2d_generic<T>((T)(lhs * (double)rhs.x), (T)(lhs * (double)rhs.y));
	}
	template<class T> inline v2d_generic<T> operator * (const int& lhs, const v2d_generic<T>& rhs)
	{
		return v2d_generic<T>((T)(lhs * (int)rhs.x), (T)(lhs * (int)rhs.y));
	}
	template<class T> inline v2d_generic<T> operator / (const float& lhs, const v2d_generic<T>& rhs)
	{
		return v2d_generic<T>((T)(lhs / (float)rhs.x), (T)(lhs / (float)rhs.y));
	}
	template<class T> inline v2d_generic<T> operator / (const double& lhs, const v2d_generic<T>& rhs)
	{
		return v2d_generic<T>((T)(lhs / (double)rhs.x), (T)(lhs / (double)rhs.y));
	}
	template<class T> inline v2d_generic<T> operator / (const int& lhs, const v2d_generic<T>& rhs)
	{
		return v2d_generic<T>((T)(lhs / (int)rhs.x), (T)(lhs / (int)rhs.y));
	}

	// To stop dandistine crying...
	template<class T, class U> inline bool operator < (const v2d_generic<T>& lhs, const v2d_generic<U>& rhs)
	{
		return lhs.y < rhs.y || (lhs.y == rhs.y && lhs.x < rhs.x);
	}
	template<class T, class U> inline bool operator > (const v2d_generic<T>& lhs, const v2d_generic<U>& rhs)
	{
		return lhs.y > rhs.y || (lhs.y == rhs.y && lhs.x > rhs.x);
	}

	typedef v2d_generic<int32_t> vec2dInt;
	typedef v2d_generic<float> vec2dFlt;
#pragma endregion


	//A great array of pixels
	class Sprite {
	public:
		Sprite();
		Sprite(int32_t w, int32_t h);
		~Sprite();


	public:
		int32_t width = 0;
		int32_t height = 0;
		enum Flip { NONE = 0, HORIZ = 1, VERT = 2 };

	public:
		Pixel GetPixel(int32_t x, int32_t y) const;
		bool  SetPixel(int32_t x, int32_t y, Pixel p);
		Pixel GetPixel(const AC::vec2dInt& a) const;
		bool  SetPixel(const AC::vec2dInt& a, Pixel p);
		Pixel* GetData();
		AC::Sprite* Duplicate();
		AC::Sprite* Duplicate(const AC::vec2dInt& vPos, const AC::vec2dInt& vSize);
		std::vector<AC::Pixel> pColData;

	};

	//weird gpu Sprite
	class Decal {
	public:
		Decal(AC::Sprite* spr, bool filter = false, bool clamp = true);
		Decal(const uint32_t nExistingTextureResource, AC::Sprite* spr);
		virtual ~Decal();
		void Update();
		void UpdateSprite();

	public: // But dont touch
		int32_t id = -1;
		AC::Sprite* sprite = nullptr;
		AC::vec2dFlt vUVScale = { 1.0f, 1.0f };
	};

	//Sprite and decal pushed into one
	class Renderable {
	public:
		Renderable() = default;
		//AC::rcode Load(const std::string& sFile, ResourcePack* pack = nullptr, bool filter = false, bool clamp = true);
		void Create(uint32_t width, uint32_t height, bool filter = false, bool clamp = true);
		AC::Decal* Decal() const;
		AC::Sprite* Sprite() const;

	private:
		std::unique_ptr<AC::Sprite> pSprite = nullptr;
		std::unique_ptr<AC::Decal> pDecal = nullptr;
	};

	//Werid little other things you need
	struct DecalInstance {
		AC::Decal* decal = nullptr;
		std::vector<AC::vec2dFlt> pos;
		std::vector<AC::vec2dFlt> uv;
		std::vector<float> w;
		std::vector<AC::Pixel> tint;
		uint32_t points = 0;
	};

	struct LayerDesc {
		AC::vec2dFlt vOffset = { 0, 0 };
		AC::vec2dFlt vScale = { 1, 1 };
		bool bShow = false;
		bool bUpdate = false;
		AC::Sprite* pDrawTarget = nullptr;
		uint32_t nResID = 0;
		std::vector<DecalInstance> vecDecalInstance;
		AC::Pixel tint = AC::WHITE;
		std::function<void()> funcHook = nullptr;
	};


	//things for rendering and base
	class SCX;
	static std::map<size_t, uint8_t> mapKeys;

	class ScreenController {
	public:
		ScreenController();
		virtual ~ScreenController();
	public:
		bool Construct(int32_t screen_w, int32_t screen_h, int32_t pixel_w, int32_t pixel_h,
			bool full_screen = false, bool vsync = false, bool cohesion = false);
		bool Start();

	public: // User Override Interfaces
		// Called once on application startup, use to load your resources
		virtual bool OnUserCreate();
		// Called every frame, and provides you with a time per frame value
		virtual bool OnUserUpdate(float fElapsedTime);
		// Called once on application termination, so you can be one clean coder
		virtual bool OnUserDestroy();

	public: // Hardware Interfaces
		// Returns true if window is currently in focus
		bool IsFocused() const;
		// Get the state of a specific keyboard button
		HWButton GetKey(Key k) const;
		// Get the state of a specific mouse button
		HWButton GetMouse(uint32_t b) const;
		// Get Mouse X coordinate in "pixel" space
		int32_t GetMouseX() const;
		// Get Mouse Y coordinate in "pixel" space
		int32_t GetMouseY() const;
		// Get Mouse Wheel Delta
		int32_t GetMouseWheel() const;
		// Get the mouse in window space
		const AC::vec2dInt& GetWindowMouse() const;
		// Gets the mouse as a vector to keep Tarriest happy
		const AC::vec2dInt& GetMousePos() const;

	public: // Utility
		// Returns the width of the screen in "pixels"
		int32_t ScreenWidth() const;
		// Returns the height of the screen in "pixels"
		int32_t ScreenHeight() const;
		// Returns the width of the currently selected drawing target in "pixels"
		int32_t GetDrawTargetWidth() const;
		// Returns the height of the currently selected drawing target in "pixels"
		int32_t GetDrawTargetHeight() const;
		// Returns the currently active draw target
		AC::Sprite* GetDrawTarget() const;
		// Resize the primary screen sprite
		void SetScreenSize(int w, int h);
		// Specify which Sprite should be the target of drawing functions, use nullptr
		// to specify the primary screen
		void SetDrawTarget(Sprite* target);
		// Gets the current Frames Per Second
		uint32_t GetFPS() const;
		// Gets last update of elapsed time
		float GetElapsedTime() const;
		// Gets Actual Window size
		const AC::vec2dInt& GetWindowSize() const;
		// Gets pixel scale
		const AC::vec2dInt& GetPixelSize() const;
		// Gets actual pixel scale
		const AC::vec2dInt& GetScreenPixelSize() const;

	public: // CONFIGURATION ROUTINES
		// Layer targeting functions
		void SetDrawTarget(uint8_t layer);
		void EnableLayer(uint8_t layer, bool b);
		void SetLayerOffset(uint8_t layer, const AC::vec2dFlt& offset);
		void SetLayerOffset(uint8_t layer, float x, float y);
		void SetLayerScale(uint8_t layer, const AC::vec2dFlt& scale);
		void SetLayerScale(uint8_t layer, float x, float y);
		void SetLayerTint(uint8_t layer, const AC::Pixel& tint);
		void SetLayerCustomRenderFunction(uint8_t layer, std::function<void()> f);

		std::vector<LayerDesc>& GetLayers();
		uint32_t CreateLayer();

		// Change the pixel mode for different optimisations
		// AC::Pixel::NORMAL = No transparency
		// AC::Pixel::MASK   = Transparent if alpha is < 255
		// AC::Pixel::ALPHA  = Full transparency
		void SetPixelMode(Pixel::Mode m);
		Pixel::Mode GetPixelMode();
		// Use a custom blend function
		void SetPixelMode(std::function<AC::Pixel(const int x, const int y, const AC::Pixel& pSource, const AC::Pixel& pDest)> pixelMode);
		// Change the blend factor from between 0.0f to 1.0f;
		void SetPixelBlend(float fBlend);



	public: // DRAWING ROUTINES
		// Draws a single Pixel
		virtual bool Draw(int32_t x, int32_t y, Pixel p = AC::WHITE);
		bool Draw(const AC::vec2dInt& pos, Pixel p = AC::WHITE);
		// Draws a line from (x1,y1) to (x2,y2)
		void DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Pixel p = AC::WHITE, uint32_t pattern = 0xFFFFFFFF);
		void DrawLine(const AC::vec2dInt& pos1, const AC::vec2dInt& pos2, Pixel p = AC::WHITE, uint32_t pattern = 0xFFFFFFFF);
		// Draws a rectangle at (x,y) to (x+w,y+h)
		void DrawRect(int32_t x, int32_t y, int32_t w, int32_t h, Pixel p = AC::WHITE);
		void DrawRect(const AC::vec2dInt& pos, const AC::vec2dInt& size, Pixel p = AC::WHITE);
		// Fills a rectangle at (x,y) to (x+w,y+h)
		void FillRect(int32_t x, int32_t y, int32_t w, int32_t h, Pixel p = AC::WHITE);
		void FillRect(const AC::vec2dInt& pos, const AC::vec2dInt& size, Pixel p = AC::WHITE);
		// Draws a triangle between points (x1,y1), (x2,y2) and (x3,y3)
		void DrawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p = AC::WHITE);
		void DrawTriangle(const AC::vec2dInt& pos1, const AC::vec2dInt& pos2, const AC::vec2dInt& pos3, Pixel p = AC::WHITE);
		// Flat fills a triangle between points (x1,y1), (x2,y2) and (x3,y3)
		void FillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p = AC::WHITE);
		void FillTriangle(const AC::vec2dInt& pos1, const AC::vec2dInt& pos2, const AC::vec2dInt& pos3, Pixel p = AC::WHITE);
		// Draws an entire sprite at location (x,y)
		void DrawSprite(int32_t x, int32_t y, Sprite* sprite, uint32_t scale = 1, uint8_t flip = AC::Sprite::NONE);
		void DrawSprite(const AC::vec2dInt& pos, Sprite* sprite, uint32_t scale = 1, uint8_t flip = AC::Sprite::NONE);
		// Draws an area of a sprite at location (x,y), where the
		// selected area is (ox,oy) to (ox+w,oy+h)
		void DrawPartialSprite(int32_t x, int32_t y, Sprite* sprite, int32_t ox, int32_t oy, int32_t w, int32_t h, uint32_t scale = 1, uint8_t flip = AC::Sprite::NONE);
		void DrawPartialSprite(const AC::vec2dInt& pos, Sprite* sprite, const AC::vec2dInt& sourcepos, const AC::vec2dInt& size, uint32_t scale = 1, uint8_t flip = AC::Sprite::NONE);
		// Draws a single line of text - traditional monospaced
		void DrawString(int32_t x, int32_t y, const std::string& sText, Pixel col = AC::WHITE, uint32_t scale = 1);
		void DrawString(const AC::vec2dInt& pos, const std::string& sText, Pixel col = AC::WHITE, uint32_t scale = 1);
		AC::vec2dInt GetTextSize(const std::string& s);
		// Draws a single line of text - non-monospaced
		void DrawStringProp(int32_t x, int32_t y, const std::string& sText, Pixel col = AC::WHITE, uint32_t scale = 1);
		void DrawStringProp(const AC::vec2dInt& pos, const std::string& sText, Pixel col = AC::WHITE, uint32_t scale = 1);
		AC::vec2dInt GetTextSizeProp(const std::string& s);

		// Decal Quad functions
		// Draws a whole decal, with optional scale and tinting
		void DrawDecal(const AC::vec2dFlt& pos, AC::Decal* decal, const AC::vec2dFlt& scale = { 1.0f,1.0f }, const AC::Pixel& tint = AC::WHITE);
		// Draws a region of a decal, with optional scale and tinting
		void DrawPartialDecal(const AC::vec2dFlt& pos, AC::Decal* decal, const AC::vec2dFlt& source_pos, const AC::vec2dFlt& source_size, const AC::vec2dFlt& scale = { 1.0f,1.0f }, const AC::Pixel& tint = AC::WHITE);
		void DrawPartialDecal(const AC::vec2dFlt& pos, const AC::vec2dFlt& size, AC::Decal* decal, const AC::vec2dFlt& source_pos, const AC::vec2dFlt& source_size, const AC::Pixel& tint = AC::WHITE);
		// Draws fully user controlled 4 vertices, pos(pixels), uv(pixels), colours
		void DrawExplicitDecal(AC::Decal* decal, const AC::vec2dFlt* pos, const AC::vec2dFlt* uv, const AC::Pixel* col, uint32_t elements = 4);
		// Draws a multiline string as a decal, with tiniting and scaling
		void DrawStringDecal(const AC::vec2dFlt& pos, const std::string& sText, const Pixel col = AC::WHITE, const AC::vec2dFlt& scale = { 1.0f, 1.0f });
		void DrawStringPropDecal(const AC::vec2dFlt& pos, const std::string& sText, const Pixel col = AC::WHITE, const AC::vec2dFlt& scale = { 1.0f, 1.0f });
		// Draws a single shaded filled rectangle as a decal
		void FillRectDecal(const AC::vec2dFlt& pos, const AC::vec2dFlt& size, const AC::Pixel col = AC::WHITE);
		// Draws a corner shaded rectangle as a decal
		void GradientFillRectDecal(const AC::vec2dFlt& pos, const AC::vec2dFlt& size, const AC::Pixel colTL, const AC::Pixel colBL, const AC::Pixel colBR, const AC::Pixel colTR);
		// Draws an arbitrary convex textured polygon using GPU
		void DrawPolygonDecal(AC::Decal* decal, const std::vector<AC::vec2dFlt>& pos, const std::vector<AC::vec2dFlt>& uv, const AC::Pixel tint = AC::WHITE);

		// Clears entire draw target to Pixel
		void Clear(Pixel p);
		// Clears the rendering back buffer
		void ClearBuffer(Pixel p, bool bDepth = true);
		// Returns the font image
		AC::Sprite* GetFontSprite();

	public: // Branding
		std::string sAppName;

	private: // Inner mysterious workings
		Sprite* pDrawTarget = nullptr;
		Pixel::Mode	nPixelMode = Pixel::NORMAL;
		float		fBlendFactor = 1.0f;
		AC::vec2dInt	vScreenSize = { 256, 240 };
		AC::vec2dFlt	vInvScreenSize = { 1.0f / 256.0f, 1.0f / 240.0f };
		AC::vec2dInt	vPixelSize = { 4, 4 };
		AC::vec2dInt   vScreenPixelSize = { 4, 4 };
		AC::vec2dInt	vMousePos = { 0, 0 };
		int32_t		nMouseWheelDelta = 0;
		AC::vec2dInt	vMousePosCache = { 0, 0 };
		AC::vec2dInt   vMouseWindowPos = { 0, 0 };
		int32_t		nMouseWheelDeltaCache = 0;
		AC::vec2dInt	vWindowSize = { 0, 0 };
		AC::vec2dInt	vViewPos = { 0, 0 };
		AC::vec2dInt	vViewSize = { 0,0 };
		bool		bFullScreen = false;
		AC::vec2dFlt	vPixel = { 1.0f, 1.0f };
		bool		bHasInputFocus = false;
		bool		bHasMouseFocus = false;
		bool		bEnableVSYNC = false;
		float		fFrameTimer = 1.0f;
		float		fLastElapsed = 0.0f;
		int			nFrameCount = 0;
		Sprite* fontSprite = nullptr;
		Decal* fontDecal = nullptr;
		Sprite* pDefaultDrawTarget = nullptr;
		std::vector<LayerDesc> vLayers;
		uint8_t		nTargetLayer = 0;
		uint32_t	nLastFPS = 0;
		bool        bPixelCohesion = false;
		std::function<AC::Pixel(const int x, const int y, const AC::Pixel&, const AC::Pixel&)> funcPixelMode;
		std::chrono::time_point<std::chrono::system_clock> m_tp1, m_tp2;
		std::vector<AC::vec2dInt> vFontSpacing;

		// State of keyboard		
		bool		pKeyNewState[256] = { 0 };
		bool		pKeyOldState[256] = { 0 };
		HWButton	pKeyboardState[256] = { 0 };

		// State of mouse
		bool		pMouseNewState[MouseButtons] = { 0 };
		bool		pMouseOldState[MouseButtons] = { 0 };
		HWButton	pMouseState[MouseButtons] = { 0 };

		// The main engine thread
		void		EngineThread();


		// If anything sets this flag to false, the engine
		// "should" shut down gracefully
		static std::atomic<bool> bAtomActive;

	public:
		// "Break In" Functions
		void AC_UpdateMouse(int32_t x, int32_t y);
		void AC_UpdateMouseWheel(int32_t delta);
		void AC_UpdateWindowSize(int32_t x, int32_t y);
		void AC_UpdateViewport();
		void AC_ConstructFontSheet();
		void AC_CoreUpdate();
		void AC_PrepareEngine();
		void AC_UpdateMouseState(int32_t button, bool state);
		void AC_UpdateKeyState(int32_t key, bool state);
		void AC_UpdateMouseFocus(bool state);
		void AC_UpdateKeyFocus(bool state);
		void AC_Terminate();
		void AC_Reanimate();
		bool AC_IsRunning();

		// At the very end of this file, chooses which
		// components to compile
		virtual void AC_ConfigureSystem();

		// NOTE: Items Here are to be deprecated, I have left them in for now
		// in case you are using them, but they will be removed.
		// AC::vec2dFlt	vSubPixelOffset = { 0.0f, 0.0f };

	public: // SCX Stuff
		friend class SCX;
		void pgex_Register(AC::SCX* pgex);

	private:
		std::vector<AC::SCX*> vExtensions;
	};


#pragma region Renderer_Definition

#if defined(AC_PLATFORM_WINAPI)
#include <dwmapi.h>
#include <GL/gl.h>
#if !defined(__MINGW32__)
#pragma comment(lib, "Dwmapi.lib")
#endif
	typedef BOOL(WINAPI wglSwapInterval_t) (int interval);
	static wglSwapInterval_t* wglSwapInterval = nullptr;
	typedef HDC glDeviceContext_t;
	typedef HGLRC glRenderContext_t;
#endif
	class Renderer {
	public:
		bool CreateDevice(std::vector<void*> params, bool bFullScreen, bool bVSYNC);
		bool DestroyDevice();
		void       DisplayFrame();
		void       PrepareDrawing();
		void       DrawLayerQuad(const AC::vec2dFlt& offset, const AC::vec2dFlt& scale, const AC::Pixel tint);
		void       DrawDecal(const AC::DecalInstance& decal);
		uint32_t   CreateTexture(const uint32_t width, const uint32_t height, const bool filtered = false, const bool clamp = true);
		void       UpdateTexture(uint32_t id, AC::Sprite* spr);
		void       ReadTexture(uint32_t id, AC::Sprite* spr);
		uint32_t   DeleteTexture(const uint32_t id);
		void       ApplyTexture(uint32_t id);
		void       UpdateViewport(const AC::vec2dInt& pos, const AC::vec2dInt& size);
		void       ClearBuffer(AC::Pixel p, bool bDepth);
		static AC::ScreenController* ptrSC;
	private:
		glDeviceContext_t glDeviceContext = 0;
		glRenderContext_t glRenderContext = 0;

		bool bSync = false;

	};

	bool Renderer::CreateDevice(std::vector<void*> params, bool bFullScreen, bool bVSYNC)
	{
#if defined(AC_PLATFORM_WINAPI)
		// Create Device Context
		glDeviceContext = GetDC((HWND)(params[0]));
		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR), 1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			PFD_MAIN_PLANE, 0, 0, 0, 0
		};

		int pf = 0;
		if (!(pf = ChoosePixelFormat(glDeviceContext, &pfd))) return false;
		SetPixelFormat(glDeviceContext, pf, &pfd);

		if (!(glRenderContext = wglCreateContext(glDeviceContext))) return false;
		wglMakeCurrent(glDeviceContext, glRenderContext);

		// Remove Frame cap
		wglSwapInterval = (wglSwapInterval_t*)wglGetProcAddress("wglSwapIntervalEXT");
		if (wglSwapInterval && !bVSYNC) wglSwapInterval(0);
		bSync = bVSYNC;
#endif

#if defined(AC_PLATFORM_X11)
#else
		glEnable(GL_TEXTURE_2D); // Turn on texturing
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
#endif
		return true;
	}

	bool Renderer::DestroyDevice()
	{
		wglDeleteContext(glRenderContext);

		return true;
	}

	void Renderer::DisplayFrame()
	{
#if defined(AC_PLATFORM_WINAPI)
		SwapBuffers(glDeviceContext);
		if (bSync) DwmFlush(); // Woooohooooooo!!!! SMOOOOOOOTH!
#endif	
	}

	void Renderer::PrepareDrawing()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}


	void Renderer::DrawLayerQuad(const AC::vec2dFlt& offset, const AC::vec2dFlt& scale, const AC::Pixel tint)
	{
		glBegin(GL_QUADS);
		glColor4ub(tint.r, tint.g, tint.b, tint.a);
		glTexCoord2f(0.0f * scale.x + offset.x, 1.0f * scale.y + offset.y);
		glVertex3f(-1.0f /*+ vSubPixelOffset.x*/, -1.0f /*+ vSubPixelOffset.y*/, 0.0f);
		glTexCoord2f(0.0f * scale.x + offset.x, 0.0f * scale.y + offset.y);
		glVertex3f(-1.0f /*+ vSubPixelOffset.x*/, 1.0f /*+ vSubPixelOffset.y*/, 0.0f);
		glTexCoord2f(1.0f * scale.x + offset.x, 0.0f * scale.y + offset.y);
		glVertex3f(1.0f /*+ vSubPixelOffset.x*/, 1.0f /*+ vSubPixelOffset.y*/, 0.0f);
		glTexCoord2f(1.0f * scale.x + offset.x, 1.0f * scale.y + offset.y);
		glVertex3f(1.0f /*+ vSubPixelOffset.x*/, -1.0f /*+ vSubPixelOffset.y*/, 0.0f);
		glEnd();
	}

	void Renderer::DrawDecal(const AC::DecalInstance& decal)
	{

		if (decal.decal == nullptr)
			glBindTexture(GL_TEXTURE_2D, 0);
		else
			glBindTexture(GL_TEXTURE_2D, decal.decal->id);

			glBegin(GL_TRIANGLE_FAN);

		for (uint32_t n = 0; n < decal.points; n++) {
			glColor4ub(decal.tint[n].r, decal.tint[n].g, decal.tint[n].b, decal.tint[n].a);
			glTexCoord4f(decal.uv[n].x, decal.uv[n].y, 0.0f, decal.w[n]);
			glVertex2f(decal.pos[n].x, decal.pos[n].y);
		}
		glEnd();
	}

	uint32_t Renderer::CreateTexture(const uint32_t width, const uint32_t height, const bool filtered, const bool clamp)
	{
		UNUSED(width);
		UNUSED(height);
		uint32_t id = 0;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		if (filtered) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}

		if (clamp) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		}
		else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		return id;
	}

	uint32_t Renderer::DeleteTexture(const uint32_t id)
	{
		glDeleteTextures(1, &id);
		return id;
	}

	void Renderer::UpdateTexture(uint32_t id, AC::Sprite* spr)
	{
		UNUSED(id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, spr->width, spr->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, spr->GetData());
	}

	void Renderer::ReadTexture(uint32_t id, AC::Sprite* spr)
	{
		glReadPixels(0, 0, spr->width, spr->height, GL_RGBA, GL_UNSIGNED_BYTE, spr->GetData());
	}

	void Renderer::ApplyTexture(uint32_t id)
	{
		glBindTexture(GL_TEXTURE_2D, id);
	}

	void Renderer::ClearBuffer(AC::Pixel p, bool bDepth)
	{
		glClearColor(float(p.r) / 255.0f, float(p.g) / 255.0f, float(p.b) / 255.0f, float(p.a) / 255.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		if (bDepth) glClear(GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::UpdateViewport(const AC::vec2dInt& pos, const AC::vec2dInt& size)
	{
#if defined(AC_PLATFORM_GLUT)
		if (!mFullScreen) glutReshapeWindow(size.x, size.y);
#else
		glViewport(pos.x, pos.y, size.x, size.y);
#endif
	}
#pragma endregion

	static std::unique_ptr<Renderer> renderer;

#pragma region Platform_Definition
#if defined(_WIN32) && !defined(__MINGW32__)
#pragma comment(lib, "user32.lib")		// Visual Studio Only
#pragma comment(lib, "gdi32.lib")		// For other Windows Compilers please add
#pragma comment(lib, "opengl32.lib")	// these libs to your linker input
#endif
	class Platform {
	public:
		bool ThreadCleanUp();
		bool CreateGraphics(bool bFullScreen, bool bEnableVSYNC, const AC::vec2dInt& vViewPos, const AC::vec2dInt& vViewSize);
		bool CreateWindowPane(const AC::vec2dInt& vWindowPos, AC::vec2dInt& vWindowSize, bool bFullScreen);
		bool SetWindowTitle(const std::string& s);
		bool StartSystemEventLoop();
		bool HandleSystemEvent();
		static AC::ScreenController* ptrSC;
	private:
		HWND AC_hWnd = nullptr;
		std::wstring wsAppName;

		std::wstring ConvertS2W(std::string s)
		{
			int count = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
			wchar_t* buffer = new wchar_t[count];
			MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, buffer, count);
			std::wstring w(buffer);
			delete[] buffer;
			return w;
		}
	public:
		// Windows Event Handler - this is statically connected to the windows event system
		static LRESULT CALLBACK AC_WindowEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
		{
			switch (uMsg) {
			case WM_MOUSEMOVE:
			{
				// Thanks @ForAbby (Discord)
				uint16_t x = lParam & 0xFFFF; uint16_t y = (lParam >> 16) & 0xFFFF;
				int16_t ix = *(int16_t*)&x;   int16_t iy = *(int16_t*)&y;
				ptrSC->AC_UpdateMouse(ix, iy);
				return 0;
			}
			case WM_SIZE:       ptrSC->AC_UpdateWindowSize(lParam & 0xFFFF, (lParam >> 16) & 0xFFFF);	return 0;
			case WM_MOUSEWHEEL:	ptrSC->AC_UpdateMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));           return 0;
			case WM_MOUSELEAVE: ptrSC->AC_UpdateMouseFocus(false);                                    return 0;
			case WM_SETFOCUS:	ptrSC->AC_UpdateKeyFocus(true);                                       return 0;
			case WM_KILLFOCUS:	ptrSC->AC_UpdateKeyFocus(false);                                      return 0;
			case WM_KEYDOWN:	ptrSC->AC_UpdateKeyState(mapKeys[wParam], true);                      return 0;
			case WM_KEYUP:		ptrSC->AC_UpdateKeyState(mapKeys[wParam], false);                     return 0;
			case WM_SYSKEYDOWN: ptrSC->AC_UpdateKeyState(mapKeys[wParam], true);						return 0;
			case WM_SYSKEYUP:	ptrSC->AC_UpdateKeyState(mapKeys[wParam], false);						return 0;
			case WM_LBUTTONDOWN:ptrSC->AC_UpdateMouseState(0, true);                                  return 0;
			case WM_LBUTTONUP:	ptrSC->AC_UpdateMouseState(0, false);                                 return 0;
			case WM_RBUTTONDOWN:ptrSC->AC_UpdateMouseState(1, true);                                  return 0;
			case WM_RBUTTONUP:	ptrSC->AC_UpdateMouseState(1, false);                                 return 0;
			case WM_MBUTTONDOWN:ptrSC->AC_UpdateMouseState(2, true);                                  return 0;
			case WM_MBUTTONUP:	ptrSC->AC_UpdateMouseState(2, false);                                 return 0;
			case WM_CLOSE:		ptrSC->AC_Terminate();                                                return 0;
			case WM_DESTROY:	PostQuitMessage(0); DestroyWindow(hWnd);								return 0;
			}
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	};

	bool Platform::ThreadCleanUp()
	{
		renderer->DestroyDevice();
		PostMessage(AC_hWnd, WM_DESTROY, 0, 0);
		return true;
	}

	bool Platform::CreateGraphics(bool bFullScreen, bool bEnableVSYNC, const AC::vec2dInt& vViewPos, const AC::vec2dInt& vViewSize)
	{
		if (renderer->CreateDevice({ AC_hWnd }, bFullScreen, bEnableVSYNC) == true) {
			renderer->UpdateViewport(vViewPos, vViewSize);
			return true;
		}
		else
			return false;
	}

	bool Platform::CreateWindowPane(const AC::vec2dInt& vWindowPos, AC::vec2dInt& vWindowSize, bool bFullScreen)
	{
		WNDCLASS wc;
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.lpfnWndProc = AC_WindowEvent;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.lpszMenuName = nullptr;
		wc.hbrBackground = nullptr;
		wc.lpszClassName = ACT("AC_PIXEL_GAME_ENGINE");
		RegisterClass(&wc);

		// Define window furniture
		DWORD dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		DWORD dwStyle = WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_THICKFRAME;

		AC::vec2dInt vTopLeft = vWindowPos;

		// Handle Fullscreen
		if (bFullScreen) {
			dwExStyle = 0;
			dwStyle = WS_VISIBLE | WS_POPUP;
			HMONITOR hmon = MonitorFromWindow(AC_hWnd, MONITOR_DEFAULTTONEAREST);
			MONITORINFO mi = { sizeof(mi) };
			if (!GetMonitorInfo(hmon, &mi)) return false;
			vWindowSize = { mi.rcMonitor.right, mi.rcMonitor.bottom };
			vTopLeft.x = 0;
			vTopLeft.y = 0;
		}

		// Keep client size as requested
		RECT rWndRect = { 0, 0, vWindowSize.x, vWindowSize.y };
		AdjustWindowRectEx(&rWndRect, dwStyle, FALSE, dwExStyle);
		int width = rWndRect.right - rWndRect.left;
		int height = rWndRect.bottom - rWndRect.top;

		AC_hWnd = CreateWindowEx(dwExStyle, ACT("AC_PIXEL_GAME_ENGINE"), ACT(""), dwStyle,
			vTopLeft.x, vTopLeft.y, width, height, NULL, NULL, GetModuleHandle(nullptr), this);

		// Create Keyboard Mapping
		mapKeys[0x00] = Key::NONE;
		mapKeys[0x41] = Key::A; mapKeys[0x42] = Key::B; mapKeys[0x43] = Key::C; mapKeys[0x44] = Key::D; mapKeys[0x45] = Key::E;
		mapKeys[0x46] = Key::F; mapKeys[0x47] = Key::G; mapKeys[0x48] = Key::H; mapKeys[0x49] = Key::I; mapKeys[0x4A] = Key::J;
		mapKeys[0x4B] = Key::K; mapKeys[0x4C] = Key::L; mapKeys[0x4D] = Key::M; mapKeys[0x4E] = Key::N; mapKeys[0x4F] = Key::O;
		mapKeys[0x50] = Key::P; mapKeys[0x51] = Key::Q; mapKeys[0x52] = Key::R; mapKeys[0x53] = Key::S; mapKeys[0x54] = Key::T;
		mapKeys[0x55] = Key::U; mapKeys[0x56] = Key::V; mapKeys[0x57] = Key::W; mapKeys[0x58] = Key::X; mapKeys[0x59] = Key::Y;
		mapKeys[0x5A] = Key::Z;

		mapKeys[VK_F1] = Key::F1; mapKeys[VK_F2] = Key::F2; mapKeys[VK_F3] = Key::F3; mapKeys[VK_F4] = Key::F4;
		mapKeys[VK_F5] = Key::F5; mapKeys[VK_F6] = Key::F6; mapKeys[VK_F7] = Key::F7; mapKeys[VK_F8] = Key::F8;
		mapKeys[VK_F9] = Key::F9; mapKeys[VK_F10] = Key::F10; mapKeys[VK_F11] = Key::F11; mapKeys[VK_F12] = Key::F12;

		mapKeys[VK_DOWN] = Key::DOWN; mapKeys[VK_LEFT] = Key::LEFT; mapKeys[VK_RIGHT] = Key::RIGHT; mapKeys[VK_UP] = Key::UP;
		mapKeys[VK_RETURN] = Key::ENTER; //mapKeys[VK_RETURN] = Key::RETURN;

		mapKeys[VK_BACK] = Key::BACK; mapKeys[VK_ESCAPE] = Key::ESCAPE; mapKeys[VK_RETURN] = Key::ENTER; mapKeys[VK_PAUSE] = Key::PAUSE;
		mapKeys[VK_SCROLL] = Key::SCROLL; mapKeys[VK_TAB] = Key::TAB; mapKeys[VK_DELETE] = Key::DEL; mapKeys[VK_HOME] = Key::HOME;
		mapKeys[VK_END] = Key::END; mapKeys[VK_PRIOR] = Key::PGUP; mapKeys[VK_NEXT] = Key::PGDN; mapKeys[VK_INSERT] = Key::INS;
		mapKeys[VK_SHIFT] = Key::SHIFT; mapKeys[VK_CONTROL] = Key::CTRL;
		mapKeys[VK_SPACE] = Key::SPACE;

		mapKeys[0x30] = Key::K0; mapKeys[0x31] = Key::K1; mapKeys[0x32] = Key::K2; mapKeys[0x33] = Key::K3; mapKeys[0x34] = Key::K4;
		mapKeys[0x35] = Key::K5; mapKeys[0x36] = Key::K6; mapKeys[0x37] = Key::K7; mapKeys[0x38] = Key::K8; mapKeys[0x39] = Key::K9;

		mapKeys[VK_NUMPAD0] = Key::NP0; mapKeys[VK_NUMPAD1] = Key::NP1; mapKeys[VK_NUMPAD2] = Key::NP2; mapKeys[VK_NUMPAD3] = Key::NP3; mapKeys[VK_NUMPAD4] = Key::NP4;
		mapKeys[VK_NUMPAD5] = Key::NP5; mapKeys[VK_NUMPAD6] = Key::NP6; mapKeys[VK_NUMPAD7] = Key::NP7; mapKeys[VK_NUMPAD8] = Key::NP8; mapKeys[VK_NUMPAD9] = Key::NP9;
		mapKeys[VK_MULTIPLY] = Key::NP_MUL; mapKeys[VK_ADD] = Key::NP_ADD; mapKeys[VK_DIVIDE] = Key::NP_DIV; mapKeys[VK_SUBTRACT] = Key::NP_SUB; mapKeys[VK_DECIMAL] = Key::NP_DECIMAL;

		// Thanks scripticuk
		mapKeys[VK_OEM_1] = Key::OEM_1;			// On US and UK keyboards this is the ';:' key
		mapKeys[VK_OEM_2] = Key::OEM_2;			// On US and UK keyboards this is the '/?' key
		mapKeys[VK_OEM_3] = Key::OEM_3;			// On US keyboard this is the '~' key
		mapKeys[VK_OEM_4] = Key::OEM_4;			// On US and UK keyboards this is the '[{' key
		mapKeys[VK_OEM_5] = Key::OEM_5;			// On US keyboard this is '\|' key.
		mapKeys[VK_OEM_6] = Key::OEM_6;			// On US and UK keyboards this is the ']}' key
		mapKeys[VK_OEM_7] = Key::OEM_7;			// On US keyboard this is the single/double quote key. On UK, this is the single quote/@ symbol key
		mapKeys[VK_OEM_8] = Key::OEM_8;			// miscellaneous characters. Varies by keyboard
		mapKeys[VK_OEM_PLUS] = Key::EQUALS;		// the '+' key on any keyboard
		mapKeys[VK_OEM_COMMA] = Key::COMMA;		// the comma key on any keyboard
		mapKeys[VK_OEM_MINUS] = Key::MINUS;		// the minus key on any keyboard
		mapKeys[VK_OEM_PERIOD] = Key::PERIOD;	// the period key on any keyboard
		mapKeys[VK_CAPITAL] = Key::CAPS_LOCK;
		return true;
	}

	bool Platform::SetWindowTitle(const std::string& s)
	{
#ifdef UNICODE
		SetWindowText(AC_hWnd, ConvertS2W(s).c_str());
#else
		SetWindowText(AC_hWnd, s.c_str());
#endif
		return true;
	}

	bool Platform::StartSystemEventLoop()
	{
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0) > 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return true;
	}

	bool Platform::HandleSystemEvent() { return false; }


#pragma endregion

	static std::unique_ptr<Platform> platform;

	//The great hook to your own personal program
	class SCX {
		friend class AC::ScreenController;
	public:
		SCX(bool bHook = false);
	protected:
		virtual void OnBeforeUserCreate();
		virtual void OnAfterUserCreate();
		virtual void OnBeforeUserUpdate(float& fElapsedTime);
		virtual void OnAfterUserUpdate(float fElapsedTime);

	protected:
		static ScreenController* controller;
	};
}

#pragma endregion

#endif // end Declaration of application (frontend)



#ifdef AC_SC_APPLICATION
#undef AC_SC_APPLICATION

//The big boi being built
#pragma region Controller_Definition
namespace AC {
#pragma region Pixel_Definition
	Pixel::Pixel()
	{
		r = 0; g = 0; b = 0; a = 0xFF;
	}

	Pixel::Pixel(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
	{
		n = red | (green << 8) | (blue << 16) | (alpha << 24);
	} // Thanks jarekpelczar 

	Pixel::Pixel(uint32_t p)
	{
		n = p;
	}

	bool Pixel::operator==(const Pixel& p) const
	{
		return n == p.n;
	}

	bool Pixel::operator!=(const Pixel& p) const
	{
		return n != p.n;
	}

	Pixel  Pixel::operator + (const Pixel& p) const
	{
		uint8_t nR = uint8_t(std::min(255, std::max(0, int(r) + int(p.r))));
		uint8_t nG = uint8_t(std::min(255, std::max(0, int(g) + int(p.g))));
		uint8_t nB = uint8_t(std::min(255, std::max(0, int(b) + int(p.b))));
		return Pixel(nR, nG, nB, a);
	}

	Pixel  Pixel::operator - (const Pixel& p) const
	{
		uint8_t nR = uint8_t(std::min(255, std::max(0, int(r) - int(p.r))));
		uint8_t nG = uint8_t(std::min(255, std::max(0, int(g) - int(p.g))));
		uint8_t nB = uint8_t(std::min(255, std::max(0, int(b) - int(p.b))));
		return Pixel(nR, nG, nB, a);
	}

	Pixel& Pixel::operator += (const Pixel& p)
	{
		this->r = uint8_t(std::min(255, std::max(0, int(r) + int(p.r))));
		this->g = uint8_t(std::min(255, std::max(0, int(g) + int(p.g))));
		this->b = uint8_t(std::min(255, std::max(0, int(b) + int(p.b))));
		return *this;
	}

	Pixel& Pixel::operator -= (const Pixel& p) // Thanks Au Lit
	{
		this->r = uint8_t(std::min(255, std::max(0, int(r) - int(p.r))));
		this->g = uint8_t(std::min(255, std::max(0, int(g) - int(p.g))));
		this->b = uint8_t(std::min(255, std::max(0, int(b) - int(p.b))));
		return *this;
	}

	Pixel Pixel::invert() const
	{
		uint8_t nR = uint8_t(std::min(255, std::max(0, 255 - int(r))));
		uint8_t nG = uint8_t(std::min(255, std::max(0, 255 - int(g))));
		uint8_t nB = uint8_t(std::min(255, std::max(0, 255 - int(b))));
		return Pixel(nR, nG, nB, a);
	}

	Pixel PixelF(float red, float green, float blue, float alpha)
	{
		return Pixel(uint8_t(red * 255.0f), uint8_t(green * 255.0f), uint8_t(blue * 255.0f), uint8_t(alpha * 255.0f));
	}
#pragma endregion
#pragma region Sprite_Definition

	Sprite::Sprite()
	{
		width = 0; height = 0;
	}

	//Sprite::Sprite(const std::string& sImageFile, AC::ResourcePack* pack)
	//{
	//	LoadFromFile(sImageFile, pack);
	//}

	Sprite::Sprite(int32_t w, int32_t h)
	{
		width = w;		height = h;
		pColData.resize(width * height);
		pColData.resize(width * height, DefaultPixel);
	}

	Sprite::~Sprite()
	{
		pColData.clear();
	}

	Pixel Sprite::GetPixel(const AC::vec2dInt& a) const
	{
		return GetPixel(a.x, a.y);
	}

	bool Sprite::SetPixel(const AC::vec2dInt& a, Pixel p)
	{
		return SetPixel(a.x, a.y, p);
	}

	Pixel Sprite::GetPixel(int32_t x, int32_t y) const
	{
		if (x >= 0 && x < width && y >= 0 && y < height)
			return pColData[y * width + x];
		else
			return Pixel(0, 0, 0, 0);
	}

	bool Sprite::SetPixel(int32_t x, int32_t y, Pixel p)
	{
		if (x >= 0 && x < width && y >= 0 && y < height) {
			pColData[y * width + x] = p;
			return true;
		}
		else
			return false;
	}

	Pixel* Sprite::GetData()
	{
		return pColData.data();
	}

	AC::Sprite* Sprite::Duplicate()
	{
		AC::Sprite* spr = new AC::Sprite(width, height);
		std::memcpy(spr->GetData(), GetData(), width * height * sizeof(AC::Pixel));
		return spr;
	}

	AC::Sprite* Sprite::Duplicate(const AC::vec2dInt& vPos, const AC::vec2dInt& vSize)
	{
		AC::Sprite* spr = new AC::Sprite(vSize.x, vSize.y);
		for (int y = 0; y < vSize.y; y++)
			for (int x = 0; x < vSize.x; x++)
				spr->SetPixel(x, y, GetPixel(vPos.x + x, vPos.y + y));
		return spr;
	}
#pragma endregion
#pragma region Decal_Definition

	Decal::Decal(AC::Sprite* spr, bool filter, bool clamp)
	{
		id = -1;
		if (spr == nullptr) return;
		sprite = spr;
		id = renderer->CreateTexture(sprite->width, sprite->height, filter, clamp);
		Update();
	}

	Decal::Decal(const uint32_t nExistingTextureResource, AC::Sprite* spr)
	{
		if (spr == nullptr) return;
		id = nExistingTextureResource;
	}

	void Decal::Update()
	{
		if (sprite == nullptr) return;
		vUVScale = { 1.0f / float(sprite->width), 1.0f / float(sprite->height) };
		renderer->ApplyTexture(id);
		renderer->UpdateTexture(id, sprite);
	}

	void Decal::UpdateSprite()
	{
		if (sprite == nullptr) return;
		renderer->ApplyTexture(id);
		renderer->ReadTexture(id, sprite);
	}

	Decal::~Decal()
	{
		if (id != -1) {
			renderer->DeleteTexture(id);
			id = -1;
		}
	}
#pragma endregion
#pragma region Renderable_Definition
	void Renderable::Create(uint32_t width, uint32_t height, bool filter, bool clamp)
	{
		pSprite = std::make_unique<AC::Sprite>(width, height);
		pDecal = std::make_unique<AC::Decal>(pSprite.get(), filter, clamp);
	}

	AC::Decal* Renderable::Decal() const
	{
		return pDecal.get();
	}

	AC::Sprite* Renderable::Sprite() const
	{
		return pSprite.get();
	}
#pragma endregion
#pragma region ScreenController_Definition
	ScreenController::ScreenController()
	{
		sAppName = "Undefined";
		AC::SCX::controller = this;

		// Bring in relevant Platform & Rendering systems depending
		// on compiler parameters
		AC_ConfigureSystem();
	}

	ScreenController::~ScreenController()
	{}


	bool ScreenController::Construct(int32_t screen_w, int32_t screen_h, int32_t pixel_w, int32_t pixel_h, bool full_screen, bool vsync, bool cohesion)
	{
		bPixelCohesion = cohesion;
		vScreenSize = { screen_w, screen_h };
		vInvScreenSize = { 1.0f / float(screen_w), 1.0f / float(screen_h) };
		vPixelSize = { pixel_w, pixel_h };
		vWindowSize = vScreenSize * vPixelSize;
		bFullScreen = full_screen;
		bEnableVSYNC = vsync;
		vPixel = 2.0f / vScreenSize;

		if (vPixelSize.x <= 0 || vPixelSize.y <= 0 || vScreenSize.x <= 0 || vScreenSize.y <= 0)
			return false;
		return true;
	}


	void ScreenController::SetScreenSize(int w, int h)
	{
		vScreenSize = { w, h };
		vInvScreenSize = { 1.0f / float(w), 1.0f / float(h) };
		for (auto& layer : vLayers) {
			delete layer.pDrawTarget; // Erase existing layer sprites
			layer.pDrawTarget = new Sprite(vScreenSize.x, vScreenSize.y);
			layer.bUpdate = true;
		}
		SetDrawTarget(nullptr);
		renderer->ClearBuffer(AC::BLACK, true);
		renderer->DisplayFrame();
		renderer->ClearBuffer(AC::BLACK, true);
		renderer->UpdateViewport(vViewPos, vViewSize);
	}

#if !defined(SC_USE_CUSTOM_START)
	bool ScreenController::Start()
	{

		// Construct the window
		if (!platform->CreateWindowPane({ 30,30 }, vWindowSize, bFullScreen)) return false;
		AC_UpdateWindowSize(vWindowSize.x, vWindowSize.y);

		// Start the thread
		bAtomActive = true;
		std::thread t = std::thread(&ScreenController::EngineThread, this);

		// Some implementations may form an event loop here
		platform->StartSystemEventLoop();

		// Wait for thread to be exited
		t.join();


		return true;
	}
#endif

	void ScreenController::SetDrawTarget(Sprite* target)
	{
		if (target) {
			pDrawTarget = target;
		}
		else {
			nTargetLayer = 0;
			pDrawTarget = vLayers[0].pDrawTarget;
		}
	}

	void ScreenController::SetDrawTarget(uint8_t layer)
	{
		if (layer < vLayers.size()) {
			pDrawTarget = vLayers[layer].pDrawTarget;
			vLayers[layer].bUpdate = true;
			nTargetLayer = layer;
		}
	}

	void ScreenController::EnableLayer(uint8_t layer, bool b)
	{
		if (layer < vLayers.size()) vLayers[layer].bShow = b;
	}

	void ScreenController::SetLayerOffset(uint8_t layer, const AC::vec2dFlt& offset)
	{
		SetLayerOffset(layer, offset.x, offset.y);
	}

	void ScreenController::SetLayerOffset(uint8_t layer, float x, float y)
	{
		if (layer < vLayers.size()) vLayers[layer].vOffset = { x, y };
	}

	void ScreenController::SetLayerScale(uint8_t layer, const AC::vec2dFlt& scale)
	{
		SetLayerScale(layer, scale.x, scale.y);
	}

	void ScreenController::SetLayerScale(uint8_t layer, float x, float y)
	{
		if (layer < vLayers.size()) vLayers[layer].vScale = { x, y };
	}

	void ScreenController::SetLayerTint(uint8_t layer, const AC::Pixel& tint)
	{
		if (layer < vLayers.size()) vLayers[layer].tint = tint;
	}

	void ScreenController::SetLayerCustomRenderFunction(uint8_t layer, std::function<void()> f)
	{
		if (layer < vLayers.size()) vLayers[layer].funcHook = f;
	}

	std::vector<LayerDesc>& ScreenController::GetLayers()
	{
		return vLayers;
	}

	uint32_t ScreenController::CreateLayer()
	{
		LayerDesc ld;
		ld.pDrawTarget = new AC::Sprite(vScreenSize.x, vScreenSize.y);
		ld.nResID = renderer->CreateTexture(vScreenSize.x, vScreenSize.y);
		renderer->UpdateTexture(ld.nResID, ld.pDrawTarget);
		vLayers.push_back(ld);
		return uint32_t(vLayers.size()) - 1;
	}

	Sprite* ScreenController::GetDrawTarget() const
	{
		return pDrawTarget;
	}

	int32_t ScreenController::GetDrawTargetWidth() const
	{
		if (pDrawTarget)
			return pDrawTarget->width;
		else
			return 0;
	}

	int32_t ScreenController::GetDrawTargetHeight() const
	{
		if (pDrawTarget)
			return pDrawTarget->height;
		else
			return 0;
	}

	uint32_t ScreenController::GetFPS() const
	{
		return nLastFPS;
	}

	bool ScreenController::IsFocused() const
	{
		return bHasInputFocus;
	}

	HWButton ScreenController::GetKey(Key k) const
	{
		return pKeyboardState[k];
	}

	HWButton ScreenController::GetMouse(uint32_t b) const
	{
		return pMouseState[b];
	}

	int32_t ScreenController::GetMouseX() const
	{
		return vMousePos.x;
	}

	int32_t ScreenController::GetMouseY() const
	{
		return vMousePos.y;
	}

	const AC::vec2dInt& ScreenController::GetMousePos() const
	{
		return vMousePos;
	}

	int32_t ScreenController::GetMouseWheel() const
	{
		return nMouseWheelDelta;
	}

	int32_t ScreenController::ScreenWidth() const
	{
		return vScreenSize.x;
	}

	int32_t ScreenController::ScreenHeight() const
	{
		return vScreenSize.y;
	}

	float ScreenController::GetElapsedTime() const
	{
		return fLastElapsed;
	}

	const AC::vec2dInt& ScreenController::GetWindowSize() const
	{
		return vWindowSize;
	}

	const AC::vec2dInt& ScreenController::GetPixelSize() const
	{
		return vPixelSize;
	}

	const AC::vec2dInt& ScreenController::GetScreenPixelSize() const
	{
		return vScreenPixelSize;
	}

	const AC::vec2dInt& ScreenController::GetWindowMouse() const
	{
		return vMouseWindowPos;
	}

	bool ScreenController::Draw(const AC::vec2dInt& pos, Pixel p)
	{
		return Draw(pos.x, pos.y, p);
	}

	// This is it, the critical function that plots a pixel
	bool ScreenController::Draw(int32_t x, int32_t y, Pixel p)
	{
		if (!pDrawTarget) return false;

		if (nPixelMode == Pixel::NORMAL) {
			return pDrawTarget->SetPixel(x, y, p);
		}

		if (nPixelMode == Pixel::MASK) {
			if (p.a == 255)
				return pDrawTarget->SetPixel(x, y, p);
		}

		if (nPixelMode == Pixel::ALPHA) {
			Pixel d = pDrawTarget->GetPixel(x, y);
			float a = (float)(p.a / 255.0f) * fBlendFactor;
			float c = 1.0f - a;
			float r = a * (float)p.r + c * (float)d.r;
			float g = a * (float)p.g + c * (float)d.g;
			float b = a * (float)p.b + c * (float)d.b;
			return pDrawTarget->SetPixel(x, y, Pixel((uint8_t)r, (uint8_t)g, (uint8_t)b/*, (uint8_t)(p.a * fBlendFactor)*/));
		}

		if (nPixelMode == Pixel::CUSTOM) {
			return pDrawTarget->SetPixel(x, y, funcPixelMode(x, y, p, pDrawTarget->GetPixel(x, y)));
		}

		return false;
	}


	void ScreenController::DrawLine(const AC::vec2dInt& pos1, const AC::vec2dInt& pos2, Pixel p, uint32_t pattern)
	{
		DrawLine(pos1.x, pos1.y, pos2.x, pos2.y, p, pattern);
	}

	void ScreenController::DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, Pixel p, uint32_t pattern)
	{
		int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
		dx = x2 - x1; dy = y2 - y1;

		auto rol = [&](void) { pattern = (pattern << 1) | (pattern >> 31); return pattern & 1; };

		// straight lines idea by gurkanctn
		if (dx == 0) // Line is vertical
		{
			if (y2 < y1) std::swap(y1, y2);
			for (y = y1; y <= y2; y++) if (rol()) Draw(x1, y, p);
			return;
		}

		if (dy == 0) // Line is horizontal
		{
			if (x2 < x1) std::swap(x1, x2);
			for (x = x1; x <= x2; x++) if (rol()) Draw(x, y1, p);
			return;
		}

		// Line is Funk-aye
		dx1 = abs(dx); dy1 = abs(dy);
		px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
		if (dy1 <= dx1) {
			if (dx >= 0) {
				x = x1; y = y1; xe = x2;
			}
			else {
				x = x2; y = y2; xe = x1;
			}

			if (rol()) Draw(x, y, p);

			for (i = 0; x < xe; i++) {
				x = x + 1;
				if (px < 0)
					px = px + 2 * dy1;
				else {
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
					px = px + 2 * (dy1 - dx1);
				}
				if (rol()) Draw(x, y, p);
			}
		}
		else {
			if (dy >= 0) {
				x = x1; y = y1; ye = y2;
			}
			else {
				x = x2; y = y2; ye = y1;
			}

			if (rol()) Draw(x, y, p);

			for (i = 0; y < ye; i++) {
				y = y + 1;
				if (py <= 0)
					py = py + 2 * dx1;
				else {
					if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
					py = py + 2 * (dx1 - dy1);
				}
				if (rol()) Draw(x, y, p);
			}
		}
	}

	void ScreenController::DrawRect(const AC::vec2dInt& pos, const AC::vec2dInt& size, Pixel p)
	{
		DrawRect(pos.x, pos.y, size.x, size.y, p);
	}

	void ScreenController::DrawRect(int32_t x, int32_t y, int32_t w, int32_t h, Pixel p)
	{
		DrawLine(x, y, x + w, y, p);
		DrawLine(x + w, y, x + w, y + h, p);
		DrawLine(x + w, y + h, x, y + h, p);
		DrawLine(x, y + h, x, y, p);
	}

	void ScreenController::Clear(Pixel p)
	{
		int pixels = GetDrawTargetWidth() * GetDrawTargetHeight();
		Pixel* m = GetDrawTarget()->GetData();
		for (int i = 0; i < pixels; i++) m[i] = p;
	}

	void ScreenController::ClearBuffer(Pixel p, bool bDepth)
	{
		renderer->ClearBuffer(p, bDepth);
	}

	AC::Sprite* ScreenController::GetFontSprite()
	{
		return fontSprite;
	}

	void ScreenController::FillRect(const AC::vec2dInt& pos, const AC::vec2dInt& size, Pixel p)
	{
		FillRect(pos.x, pos.y, size.x, size.y, p);
	}

	void ScreenController::FillRect(int32_t x, int32_t y, int32_t w, int32_t h, Pixel p)
	{
		int32_t x2 = x + w;
		int32_t y2 = y + h;

		if (x < 0) x = 0;
		if (x >= (int32_t)GetDrawTargetWidth()) x = (int32_t)GetDrawTargetWidth();
		if (y < 0) y = 0;
		if (y >= (int32_t)GetDrawTargetHeight()) y = (int32_t)GetDrawTargetHeight();

		if (x2 < 0) x2 = 0;
		if (x2 >= (int32_t)GetDrawTargetWidth()) x2 = (int32_t)GetDrawTargetWidth();
		if (y2 < 0) y2 = 0;
		if (y2 >= (int32_t)GetDrawTargetHeight()) y2 = (int32_t)GetDrawTargetHeight();

		for (int i = x; i < x2; i++)
			for (int j = y; j < y2; j++)
				Draw(i, j, p);
	}

	void ScreenController::DrawTriangle(const AC::vec2dInt& pos1, const AC::vec2dInt& pos2, const AC::vec2dInt& pos3, Pixel p)
	{
		DrawTriangle(pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y, p);
	}

	void ScreenController::DrawTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p)
	{
		DrawLine(x1, y1, x2, y2, p);
		DrawLine(x2, y2, x3, y3, p);
		DrawLine(x3, y3, x1, y1, p);
	}

	void ScreenController::FillTriangle(const AC::vec2dInt& pos1, const AC::vec2dInt& pos2, const AC::vec2dInt& pos3, Pixel p)
	{
		FillTriangle(pos1.x, pos1.y, pos2.x, pos2.y, pos3.x, pos3.y, p);
	}

	// https://www.avrfreaks.net/sites/default/files/triangles.c
	void ScreenController::FillTriangle(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x3, int32_t y3, Pixel p)
	{
		auto drawline = [&](int sx, int ex, int ny) { for (int i = sx; i <= ex; i++) Draw(i, ny, p); };

		int t1x, t2x, y, minx, maxx, t1xp, t2xp;
		bool changed1 = false;
		bool changed2 = false;
		int signx1, signx2, dx1, dy1, dx2, dy2;
		int e1, e2;
		// Sort vertices
		if (y1 > y2) { std::swap(y1, y2); std::swap(x1, x2); }
		if (y1 > y3) { std::swap(y1, y3); std::swap(x1, x3); }
		if (y2 > y3) { std::swap(y2, y3); std::swap(x2, x3); }

		t1x = t2x = x1; y = y1;   // Starting points
		dx1 = (int)(x2 - x1);
		if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
		else signx1 = 1;
		dy1 = (int)(y2 - y1);

		dx2 = (int)(x3 - x1);
		if (dx2 < 0) { dx2 = -dx2; signx2 = -1; }
		else signx2 = 1;
		dy2 = (int)(y3 - y1);

		if (dy1 > dx1) { std::swap(dx1, dy1); changed1 = true; }
		if (dy2 > dx2) { std::swap(dy2, dx2); changed2 = true; }

		e2 = (int)(dx2 >> 1);
		// Flat top, just process the second half
		if (y1 == y2) goto next;
		e1 = (int)(dx1 >> 1);

		for (int i = 0; i < dx1;) {
			t1xp = 0; t2xp = 0;
			if (t1x < t2x) { minx = t1x; maxx = t2x; }
			else { minx = t2x; maxx = t1x; }
			// process first line until y value is about to change
			while (i < dx1) {
				i++;
				e1 += dy1;
				while (e1 >= dx1) {
					e1 -= dx1;
					if (changed1) t1xp = signx1;//t1x += signx1;
					else          goto next1;
				}
				if (changed1) break;
				else t1x += signx1;
			}
			// Move line
		next1:
			// process second line until y value is about to change
			while (1) {
				e2 += dy2;
				while (e2 >= dx2) {
					e2 -= dx2;
					if (changed2) t2xp = signx2;//t2x += signx2;
					else          goto next2;
				}
				if (changed2)     break;
				else              t2x += signx2;
			}
		next2:
			if (minx > t1x) minx = t1x;
			if (minx > t2x) minx = t2x;
			if (maxx < t1x) maxx = t1x;
			if (maxx < t2x) maxx = t2x;
			drawline(minx, maxx, y);    // Draw line from min to max points found on the y
										// Now increase y
			if (!changed1) t1x += signx1;
			t1x += t1xp;
			if (!changed2) t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y == y2) break;
		}
	next:
		// Second half
		dx1 = (int)(x3 - x2); if (dx1 < 0) { dx1 = -dx1; signx1 = -1; }
		else signx1 = 1;
		dy1 = (int)(y3 - y2);
		t1x = x2;

		if (dy1 > dx1) {   // swap values
			std::swap(dy1, dx1);
			changed1 = true;
		}
		else changed1 = false;

		e1 = (int)(dx1 >> 1);

		for (int i = 0; i <= dx1; i++) {
			t1xp = 0; t2xp = 0;
			if (t1x < t2x) { minx = t1x; maxx = t2x; }
			else { minx = t2x; maxx = t1x; }
			// process first line until y value is about to change
			while (i < dx1) {
				e1 += dy1;
				while (e1 >= dx1) {
					e1 -= dx1;
					if (changed1) { t1xp = signx1; break; }//t1x += signx1;
					else          goto next3;
				}
				if (changed1) break;
				else   	   	  t1x += signx1;
				if (i < dx1) i++;
			}
		next3:
			// process second line until y value is about to change
			while (t2x != x3) {
				e2 += dy2;
				while (e2 >= dx2) {
					e2 -= dx2;
					if (changed2) t2xp = signx2;
					else          goto next4;
				}
				if (changed2)     break;
				else              t2x += signx2;
			}
		next4:

			if (minx > t1x) minx = t1x;
			if (minx > t2x) minx = t2x;
			if (maxx < t1x) maxx = t1x;
			if (maxx < t2x) maxx = t2x;
			drawline(minx, maxx, y);
			if (!changed1) t1x += signx1;
			t1x += t1xp;
			if (!changed2) t2x += signx2;
			t2x += t2xp;
			y += 1;
			if (y > y3) return;
		}
	}

	void ScreenController::DrawSprite(const AC::vec2dInt& pos, Sprite* sprite, uint32_t scale, uint8_t flip)
	{
		DrawSprite(pos.x, pos.y, sprite, scale, flip);
	}

	void ScreenController::DrawSprite(int32_t x, int32_t y, Sprite* sprite, uint32_t scale, uint8_t flip)
	{
		if (sprite == nullptr)
			return;

		int32_t fxs = 0, fxm = 1, fx = 0;
		int32_t fys = 0, fym = 1, fy = 0;
		if (flip & AC::Sprite::Flip::HORIZ) { fxs = sprite->width - 1; fxm = -1; }
		if (flip & AC::Sprite::Flip::VERT) { fys = sprite->height - 1; fym = -1; }

		if (scale > 1) {
			fx = fxs;
			for (int32_t i = 0; i < sprite->width; i++, fx += fxm) {
				fy = fys;
				for (int32_t j = 0; j < sprite->height; j++, fy += fym)
					for (uint32_t is = 0; is < scale; is++)
						for (uint32_t js = 0; js < scale; js++)
							Draw(x + (i * scale) + is, y + (j * scale) + js, sprite->GetPixel(fx, fy));
			}
		}
		else {
			fx = fxs;
			for (int32_t i = 0; i < sprite->width; i++, fx += fxm) {
				fy = fys;
				for (int32_t j = 0; j < sprite->height; j++, fy += fym)
					Draw(x + i, y + j, sprite->GetPixel(fx, fy));
			}
		}
	}

	void ScreenController::DrawPartialSprite(const AC::vec2dInt& pos, Sprite* sprite, const AC::vec2dInt& sourcepos, const AC::vec2dInt& size, uint32_t scale, uint8_t flip)
	{
		DrawPartialSprite(pos.x, pos.y, sprite, sourcepos.x, sourcepos.y, size.x, size.y, scale, flip);
	}

	void ScreenController::DrawPartialSprite(int32_t x, int32_t y, Sprite* sprite, int32_t ox, int32_t oy, int32_t w, int32_t h, uint32_t scale, uint8_t flip)
	{
		if (sprite == nullptr)
			return;

		int32_t fxs = 0, fxm = 1, fx = 0;
		int32_t fys = 0, fym = 1, fy = 0;
		if (flip & AC::Sprite::Flip::HORIZ) { fxs = w - 1; fxm = -1; }
		if (flip & AC::Sprite::Flip::VERT) { fys = h - 1; fym = -1; }

		if (scale > 1) {
			fx = fxs;
			for (int32_t i = 0; i < w; i++, fx += fxm) {
				fy = fys;
				for (int32_t j = 0; j < h; j++, fy += fym)
					for (uint32_t is = 0; is < scale; is++)
						for (uint32_t js = 0; js < scale; js++)
							Draw(x + (i * scale) + is, y + (j * scale) + js, sprite->GetPixel(fx + ox, fy + oy));
			}
		}
		else {
			fx = fxs;
			for (int32_t i = 0; i < w; i++, fx += fxm) {
				fy = fys;
				for (int32_t j = 0; j < h; j++, fy += fym)
					Draw(x + i, y + j, sprite->GetPixel(fx + ox, fy + oy));
			}
		}
	}

	void ScreenController::DrawPartialDecal(const AC::vec2dFlt& pos, AC::Decal* decal, const AC::vec2dFlt& source_pos, const AC::vec2dFlt& source_size, const AC::vec2dFlt& scale, const AC::Pixel& tint)
	{
		AC::vec2dFlt vScreenSpacePos =
		{
			(std::floor(pos.x) * vInvScreenSize.x) * 2.0f - 1.0f,
			((std::floor(pos.y) * vInvScreenSize.y) * 2.0f - 1.0f) * -1.0f
		};

		AC::vec2dFlt vScreenSpaceDim =
		{
			vScreenSpacePos.x + (2.0f * source_size.x * vInvScreenSize.x) * scale.x,
			vScreenSpacePos.y - (2.0f * source_size.y * vInvScreenSize.y) * scale.y
		};

		DecalInstance di;
		di.points = 4;
		di.decal = decal;
		di.tint = { tint, tint, tint, tint };
		di.pos = { { vScreenSpacePos.x, vScreenSpacePos.y }, { vScreenSpacePos.x, vScreenSpaceDim.y }, { vScreenSpaceDim.x, vScreenSpaceDim.y }, { vScreenSpaceDim.x, vScreenSpacePos.y } };
		AC::vec2dFlt uvtl = source_pos * decal->vUVScale;
		AC::vec2dFlt uvbr = uvtl + (source_size * decal->vUVScale);
		di.uv = { { uvtl.x, uvtl.y }, { uvtl.x, uvbr.y }, { uvbr.x, uvbr.y }, { uvbr.x, uvtl.y } };
		di.w = { 1,1,1,1 };
		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}

	void ScreenController::DrawPartialDecal(const AC::vec2dFlt& pos, const AC::vec2dFlt& size, AC::Decal* decal, const AC::vec2dFlt& source_pos, const AC::vec2dFlt& source_size, const AC::Pixel& tint)
	{
		AC::vec2dFlt vScreenSpacePos =
		{
			(std::floor(pos.x) * vInvScreenSize.x) * 2.0f - 1.0f,
			((std::floor(pos.y) * vInvScreenSize.y) * 2.0f - 1.0f) * -1.0f
		};

		AC::vec2dFlt vScreenSpaceDim =
		{
			vScreenSpacePos.x + (2.0f * size.x * vInvScreenSize.x),
			vScreenSpacePos.y - (2.0f * size.y * vInvScreenSize.y)
		};

		DecalInstance di;
		di.points = 4;
		di.decal = decal;
		di.tint = { tint, tint, tint, tint };
		di.pos = { { vScreenSpacePos.x, vScreenSpacePos.y }, { vScreenSpacePos.x, vScreenSpaceDim.y }, { vScreenSpaceDim.x, vScreenSpaceDim.y }, { vScreenSpaceDim.x, vScreenSpacePos.y } };
		AC::vec2dFlt uvtl = (source_pos)*decal->vUVScale;
		AC::vec2dFlt uvbr = uvtl + ((source_size)*decal->vUVScale);
		di.uv = { { uvtl.x, uvtl.y }, { uvtl.x, uvbr.y }, { uvbr.x, uvbr.y }, { uvbr.x, uvtl.y } };
		di.w = { 1,1,1,1 };
		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}


	void ScreenController::DrawDecal(const AC::vec2dFlt& pos, AC::Decal* decal, const AC::vec2dFlt& scale, const AC::Pixel& tint)
	{
		AC::vec2dFlt vScreenSpacePos =
		{
			(std::floor(pos.x) * vInvScreenSize.x) * 2.0f - 1.0f,
			((std::floor(pos.y) * vInvScreenSize.y) * 2.0f - 1.0f) * -1.0f
		};

		AC::vec2dFlt vScreenSpaceDim =
		{
			vScreenSpacePos.x + (2.0f * (float(decal->sprite->width) * vInvScreenSize.x)) * scale.x,
			vScreenSpacePos.y - (2.0f * (float(decal->sprite->height) * vInvScreenSize.y)) * scale.y
		};

		DecalInstance di;
		di.decal = decal;
		di.points = 4;
		di.tint = { tint, tint, tint, tint };
		di.pos = { { vScreenSpacePos.x, vScreenSpacePos.y }, { vScreenSpacePos.x, vScreenSpaceDim.y }, { vScreenSpaceDim.x, vScreenSpaceDim.y }, { vScreenSpaceDim.x, vScreenSpacePos.y } };
		di.uv = { { 0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f} };
		di.w = { 1, 1, 1, 1 };
		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}

	void ScreenController::DrawExplicitDecal(AC::Decal* decal, const AC::vec2dFlt* pos, const AC::vec2dFlt* uv, const AC::Pixel* col, uint32_t elements)
	{
		DecalInstance di;
		di.decal = decal;
		di.pos.resize(elements);
		di.uv.resize(elements);
		di.w.resize(elements);
		di.tint.resize(elements);
		di.points = elements;
		for (uint32_t i = 0; i < elements; i++) {
			di.pos[i] = { (pos[i].x * vInvScreenSize.x) * 2.0f - 1.0f, ((pos[i].y * vInvScreenSize.y) * 2.0f - 1.0f) * -1.0f };
			di.uv[i] = uv[i];
			di.tint[i] = col[i];
			di.w[i] = 1.0f;
		}
		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}

	void ScreenController::DrawPolygonDecal(AC::Decal* decal, const std::vector<AC::vec2dFlt>& pos, const std::vector<AC::vec2dFlt>& uv, const AC::Pixel tint)
	{
		DecalInstance di;
		di.decal = decal;
		di.points = uint32_t(pos.size());
		di.pos.resize(di.points);
		di.uv.resize(di.points);
		di.w.resize(di.points);
		di.tint.resize(di.points);
		for (uint32_t i = 0; i < di.points; i++) {
			di.pos[i] = { (pos[i].x * vInvScreenSize.x) * 2.0f - 1.0f, ((pos[i].y * vInvScreenSize.y) * 2.0f - 1.0f) * -1.0f };
			di.uv[i] = uv[i];
			di.tint[i] = tint;
			di.w[i] = 1.0f;
		}
		vLayers[nTargetLayer].vecDecalInstance.push_back(di);
	}

	void ScreenController::FillRectDecal(const AC::vec2dFlt& pos, const AC::vec2dFlt& size, const AC::Pixel col)
	{
		std::array<AC::vec2dFlt, 4> points = { { {pos}, {pos.x, pos.y + size.y}, {pos + size}, {pos.x + size.x, pos.y} } };
		std::array<AC::vec2dFlt, 4> uvs = { {{0,0},{0,0},{0,0},{0,0}} };
		std::array<AC::Pixel, 4> cols = { {col, col, col, col} };
		DrawExplicitDecal(nullptr, points.data(), uvs.data(), cols.data(), 4);
	}

	void ScreenController::GradientFillRectDecal(const AC::vec2dFlt& pos, const AC::vec2dFlt& size, const AC::Pixel colTL, const AC::Pixel colBL, const AC::Pixel colBR, const AC::Pixel colTR)
	{
		std::array<AC::vec2dFlt, 4> points = { { {pos}, {pos.x, pos.y + size.y}, {pos + size}, {pos.x + size.x, pos.y} } };
		std::array<AC::vec2dFlt, 4> uvs = { {{0,0},{0,0},{0,0},{0,0}} };
		std::array<AC::Pixel, 4> cols = { {colTL, colBL, colBR, colTR} };
		DrawExplicitDecal(nullptr, points.data(), uvs.data(), cols.data(), 4);
	}

	void ScreenController::DrawStringDecal(const AC::vec2dFlt& pos, const std::string& sText, const Pixel col, const AC::vec2dFlt& scale)
	{
		AC::vec2dFlt spos = { 0.0f, 0.0f };
		for (auto c : sText) {
			if (c == '\n') {
				spos.x = 0; spos.y += 8.0f * scale.y;
			}
			else {
				int32_t ox = (c - 32) % 16;
				int32_t oy = (c - 32) / 16;
				DrawPartialDecal(pos + spos, fontDecal, { float(ox) * 8.0f, float(oy) * 8.0f }, { 8.0f, 8.0f }, scale, col);
				spos.x += 8.0f * scale.x;
			}
		}
	}

	void ScreenController::DrawStringPropDecal(const AC::vec2dFlt& pos, const std::string& sText, const Pixel col, const AC::vec2dFlt& scale)
	{
		AC::vec2dFlt spos = { 0.0f, 0.0f };
		for (auto c : sText) {
			if (c == '\n') {
				spos.x = 0; spos.y += 8.0f * scale.y;
			}
			else {
				int32_t ox = (c - 32) % 16;
				int32_t oy = (c - 32) / 16;
				DrawPartialDecal(pos + spos, fontDecal, { float(ox) * 8.0f + float(vFontSpacing[c - 32].x), float(oy) * 8.0f }, { float(vFontSpacing[c - 32].y), 8.0f }, scale, col);
				spos.x += float(vFontSpacing[c - 32].y) * scale.x;
			}
		}
	}

	AC::vec2dInt ScreenController::GetTextSize(const std::string& s)
	{
		AC::vec2dInt size = { 0,1 };
		AC::vec2dInt pos = { 0,1 };
		for (auto c : s) {
			if (c == '\n') { pos.y++;  pos.x = 0; }
			else pos.x++;
			size.x = std::max(size.x, pos.x);
			size.y = std::max(size.y, pos.y);
		}
		return size * 8;
	}

	void ScreenController::DrawString(const AC::vec2dInt& pos, const std::string& sText, Pixel col, uint32_t scale)
	{
		DrawString(pos.x, pos.y, sText, col, scale);
	}

	void ScreenController::DrawString(int32_t x, int32_t y, const std::string& sText, Pixel col, uint32_t scale)
	{
		int32_t sx = 0;
		int32_t sy = 0;
		Pixel::Mode m = nPixelMode;
		// Thanks @tucna, spotted bug with col.ALPHA :P
		if (m != Pixel::CUSTOM) // Thanks @Megarev, required for "shaders"
		{
			if (col.a != 255)		SetPixelMode(Pixel::ALPHA);
			else					SetPixelMode(Pixel::MASK);
		}
		for (auto c : sText) {
			if (c == '\n') {
				sx = 0; sy += 8 * scale;
			}
			else {
				int32_t ox = (c - 32) % 16;
				int32_t oy = (c - 32) / 16;

				if (scale > 1) {
					for (uint32_t i = 0; i < 8; i++)
						for (uint32_t j = 0; j < 8; j++)
							if (fontSprite->GetPixel(i + ox * 8, j + oy * 8).r > 0)
								for (uint32_t is = 0; is < scale; is++)
									for (uint32_t js = 0; js < scale; js++)
										Draw(x + sx + (i * scale) + is, y + sy + (j * scale) + js, col);
				}
				else {
					for (uint32_t i = 0; i < 8; i++)
						for (uint32_t j = 0; j < 8; j++)
							if (fontSprite->GetPixel(i + ox * 8, j + oy * 8).r > 0)
								Draw(x + sx + i, y + sy + j, col);
				}
				sx += 8 * scale;
			}
		}
		SetPixelMode(m);
	}

	AC::vec2dInt ScreenController::GetTextSizeProp(const std::string& s)
	{
		AC::vec2dInt size = { 0,1 };
		AC::vec2dInt pos = { 0,1 };
		for (auto c : s) {
			if (c == '\n') { pos.y += 1;  pos.x = 0; }
			else pos.x += vFontSpacing[c - 32].y;
			size.x = std::max(size.x, pos.x);
			size.y = std::max(size.y, pos.y);
		}

		size.y *= 8;
		return size;
	}

	void ScreenController::DrawStringProp(const AC::vec2dInt& pos, const std::string& sText, Pixel col, uint32_t scale)
	{
		DrawStringProp(pos.x, pos.y, sText, col, scale);
	}

	void ScreenController::DrawStringProp(int32_t x, int32_t y, const std::string& sText, Pixel col, uint32_t scale)
	{
		int32_t sx = 0;
		int32_t sy = 0;
		Pixel::Mode m = nPixelMode;

		if (m != Pixel::CUSTOM) {
			if (col.a != 255)		SetPixelMode(Pixel::ALPHA);
			else					SetPixelMode(Pixel::MASK);
		}
		for (auto c : sText) {
			if (c == '\n') {
				sx = 0; sy += 8 * scale;
			}
			else {
				int32_t ox = (c - 32) % 16;
				int32_t oy = (c - 32) / 16;

				if (scale > 1) {
					for (int32_t i = 0; i < vFontSpacing[c - 32].y; i++)
						for (int32_t j = 0; j < 8; j++)
							if (fontSprite->GetPixel(i + ox * 8 + vFontSpacing[c - 32].x, j + oy * 8).r > 0)
								for (int32_t is = 0; is < int(scale); is++)
									for (int32_t js = 0; js < int(scale); js++)
										Draw(x + sx + (i * scale) + is, y + sy + (j * scale) + js, col);
				}
				else {
					for (int32_t i = 0; i < vFontSpacing[c - 32].y; i++)
						for (int32_t j = 0; j < 8; j++)
							if (fontSprite->GetPixel(i + ox * 8 + vFontSpacing[c - 32].x, j + oy * 8).r > 0)
								Draw(x + sx + i, y + sy + j, col);
				}
				sx += vFontSpacing[c - 32].y * scale;
			}
		}
		SetPixelMode(m);
	}

	void ScreenController::SetPixelMode(Pixel::Mode m)
	{
		nPixelMode = m;
	}

	Pixel::Mode ScreenController::GetPixelMode()
	{
		return nPixelMode;
	}

	void ScreenController::SetPixelMode(std::function<AC::Pixel(const int x, const int y, const AC::Pixel&, const AC::Pixel&)> pixelMode)
	{
		funcPixelMode = pixelMode;
		nPixelMode = Pixel::Mode::CUSTOM;
	}

	void ScreenController::SetPixelBlend(float fBlend)
	{
		fBlendFactor = fBlend;
		if (fBlendFactor < 0.0f) fBlendFactor = 0.0f;
		if (fBlendFactor > 1.0f) fBlendFactor = 1.0f;
	}

	// User must override these functions as required. I have not made
	// them abstract because I do need a default behaviour to occur if
	// they are not overwritten

	bool ScreenController::OnUserCreate()
	{
		return false;
	}

	bool ScreenController::OnUserUpdate(float fElapsedTime)
	{
		UNUSED(fElapsedTime);  return false;
	}

	bool ScreenController::OnUserDestroy()
	{
		return true;
	}

	void ScreenController::AC_UpdateViewport()
	{
		int32_t ww = vScreenSize.x * vPixelSize.x;
		int32_t wh = vScreenSize.y * vPixelSize.y;
		float wasp = (float)ww / (float)wh;

		if (bPixelCohesion) {
			vScreenPixelSize = (vWindowSize / vScreenSize);
			vViewSize = (vWindowSize / vScreenSize) * vScreenSize;
		}
		else {
			vViewSize.x = (int32_t)vWindowSize.x;
			vViewSize.y = (int32_t)((float)vViewSize.x / wasp);

			if (vViewSize.y > vWindowSize.y) {
				vViewSize.y = vWindowSize.y;
				vViewSize.x = (int32_t)((float)vViewSize.y * wasp);
			}
		}

		vViewPos = (vWindowSize - vViewSize) / 2;
	}

	void ScreenController::AC_UpdateWindowSize(int32_t x, int32_t y)
	{
		vWindowSize = { x, y };
		AC_UpdateViewport();
	}

	void ScreenController::AC_UpdateMouseWheel(int32_t delta)
	{
		nMouseWheelDeltaCache += delta;
	}

	void ScreenController::AC_UpdateMouse(int32_t x, int32_t y)
	{
		// Mouse coords come in screen space
		// But leave in pixel space
		bHasMouseFocus = true;
		vMouseWindowPos = { x, y };
		// Full Screen mode may have a weird viewport we must clamp to
		x -= vViewPos.x;
		y -= vViewPos.y;
		vMousePosCache.x = (int32_t)(((float)x / (float)(vWindowSize.x - (vViewPos.x * 2)) * (float)vScreenSize.x));
		vMousePosCache.y = (int32_t)(((float)y / (float)(vWindowSize.y - (vViewPos.y * 2)) * (float)vScreenSize.y));
		if (vMousePosCache.x >= (int32_t)vScreenSize.x)	vMousePosCache.x = vScreenSize.x - 1;
		if (vMousePosCache.y >= (int32_t)vScreenSize.y)	vMousePosCache.y = vScreenSize.y - 1;
		if (vMousePosCache.x < 0) vMousePosCache.x = 0;
		if (vMousePosCache.y < 0) vMousePosCache.y = 0;
	}

	void ScreenController::AC_UpdateMouseState(int32_t button, bool state)
	{
		pMouseNewState[button] = state;
	}

	void ScreenController::AC_UpdateKeyState(int32_t key, bool state)
	{
		pKeyNewState[key] = state;
	}

	void ScreenController::AC_UpdateMouseFocus(bool state)
	{
		bHasMouseFocus = state;
	}

	void ScreenController::AC_UpdateKeyFocus(bool state)
	{
		bHasInputFocus = state;
	}

	void ScreenController::AC_Reanimate()
	{
		bAtomActive = true;
	}

	bool ScreenController::AC_IsRunning()
	{
		return bAtomActive;
	}

	void ScreenController::AC_Terminate()
	{
		bAtomActive = false;
	}

	void ScreenController::EngineThread()
	{

		// Do engine context specific initialisation
		AC_PrepareEngine();

		// Create user resources as part of this thread
		for (auto& ext : vExtensions) ext->OnBeforeUserCreate();
		if (!OnUserCreate()) bAtomActive = false;
		for (auto& ext : vExtensions) ext->OnAfterUserCreate();

		while (bAtomActive) {
			// Run as fast as possible
			while (bAtomActive) { AC_CoreUpdate(); }

			// Allow the user to free resources if they have overrided the destroy function
			if (!OnUserDestroy()) {
				// User denied destroy for some reason, so continue running
				bAtomActive = true;
			}
		}

		platform->ThreadCleanUp();
	}

	void ScreenController::AC_PrepareEngine()
	{
		// Start OpenGL, the context is owned by the game thread
		if (platform->CreateGraphics(bFullScreen, bEnableVSYNC, vViewPos, vViewSize) == false) return;

		// Construct default font sheet
		AC_ConstructFontSheet();

		// Create Primary Layer "0"
		CreateLayer();
		vLayers[0].bUpdate = true;
		vLayers[0].bShow = true;
		SetDrawTarget(nullptr);

		m_tp1 = std::chrono::system_clock::now();
		m_tp2 = std::chrono::system_clock::now();
	}


	void ScreenController::AC_CoreUpdate()
	{
		// Handle Timing
		m_tp2 = std::chrono::system_clock::now();
		std::chrono::duration<float> elapsedTime = m_tp2 - m_tp1;
		m_tp1 = m_tp2;

		// Our time per frame coefficient
		float fElapsedTime = elapsedTime.count();
		fLastElapsed = fElapsedTime;

		// Some platforms will need to check for events
		platform->HandleSystemEvent();

		// Compare hardware input states from previous frame
		auto ScanHardware = [&](HWButton* pKeys, bool* pStateOld, bool* pStateNew, uint32_t nKeyCount) {
			for (uint32_t i = 0; i < nKeyCount; i++) {
				pKeys[i].bPressed = false;
				pKeys[i].bReleased = false;
				if (pStateNew[i] != pStateOld[i]) {
					if (pStateNew[i]) {
						pKeys[i].bPressed = !pKeys[i].bHeld;
						pKeys[i].bHeld = true;
					}
					else {
						pKeys[i].bReleased = true;
						pKeys[i].bHeld = false;
					}
				}
				pStateOld[i] = pStateNew[i];
			}
		};

		ScanHardware(pKeyboardState, pKeyOldState, pKeyNewState, 256);
		ScanHardware(pMouseState, pMouseOldState, pMouseNewState, MouseButtons);

		// Cache mouse coordinates so they remain consistent during frame
		vMousePos = vMousePosCache;
		nMouseWheelDelta = nMouseWheelDeltaCache;
		nMouseWheelDeltaCache = 0;

		//	renderer->ClearBuffer(AC::BLACK, true);

		// Handle Frame Update
		for (auto& ext : vExtensions) ext->OnBeforeUserUpdate(fElapsedTime);
		if (!OnUserUpdate(fElapsedTime)) bAtomActive = false;
		for (auto& ext : vExtensions) ext->OnAfterUserUpdate(fElapsedTime);

		// Display Frame
		renderer->UpdateViewport(vViewPos, vViewSize);
		renderer->ClearBuffer(AC::BLACK, true);

		// Layer 0 must always exist
		vLayers[0].bUpdate = true;
		vLayers[0].bShow = true;
		renderer->PrepareDrawing();

		for (auto layer = vLayers.rbegin(); layer != vLayers.rend(); ++layer) {
			if (layer->bShow) {
				if (layer->funcHook == nullptr) {
					renderer->ApplyTexture(layer->nResID);
					if (layer->bUpdate) {
						renderer->UpdateTexture(layer->nResID, layer->pDrawTarget);
						layer->bUpdate = false;
					}

					renderer->DrawLayerQuad(layer->vOffset, layer->vScale, layer->tint);

					// Display Decals in order for this layer
					for (auto& decal : layer->vecDecalInstance)
						renderer->DrawDecal(decal);
					layer->vecDecalInstance.clear();
				}
				else {
					// Mwa ha ha.... Have Fun!!!
					layer->funcHook();
				}
			}
		}

		// Present Graphics to screen
		renderer->DisplayFrame();

		// Update Title Bar
		fFrameTimer += fElapsedTime;
		nFrameCount++;
		if (fFrameTimer >= 1.0f) {
			nLastFPS = nFrameCount;
			fFrameTimer -= 1.0f;
			std::string sTitle = sAppName + " - FPS: " + std::to_string(nFrameCount);
			platform->SetWindowTitle(sTitle);
			nFrameCount = 0;
		}
	}

	void ScreenController::AC_ConstructFontSheet()
	{
		std::string data;
		data += "?Q`0001oOch0o01o@F40o0<AGD4090LAGD<090@A7ch0?00O7Q`0600>00000000";
		data += "O000000nOT0063Qo4d8>?7a14Gno94AA4gno94AaOT0>o3`oO400o7QN00000400";
		data += "Of80001oOg<7O7moBGT7O7lABET024@aBEd714AiOdl717a_=TH013Q>00000000";
		data += "720D000V?V5oB3Q_HdUoE7a9@DdDE4A9@DmoE4A;Hg]oM4Aj8S4D84@`00000000";
		data += "OaPT1000Oa`^13P1@AI[?g`1@A=[OdAoHgljA4Ao?WlBA7l1710007l100000000";
		data += "ObM6000oOfMV?3QoBDD`O7a0BDDH@5A0BDD<@5A0BGeVO5ao@CQR?5Po00000000";
		data += "Oc``000?Ogij70PO2D]??0Ph2DUM@7i`2DTg@7lh2GUj?0TO0C1870T?00000000";
		data += "70<4001o?P<7?1QoHg43O;`h@GT0@:@LB@d0>:@hN@L0@?aoN@<0O7ao0000?000";
		data += "OcH0001SOglLA7mg24TnK7ln24US>0PL24U140PnOgl0>7QgOcH0K71S0000A000";
		data += "00H00000@Dm1S007@DUSg00?OdTnH7YhOfTL<7Yh@Cl0700?@Ah0300700000000";
		data += "<008001QL00ZA41a@6HnI<1i@FHLM81M@@0LG81?O`0nC?Y7?`0ZA7Y300080000";
		data += "O`082000Oh0827mo6>Hn?Wmo?6HnMb11MP08@C11H`08@FP0@@0004@000000000";
		data += "00P00001Oab00003OcKP0006@6=PMgl<@440MglH@000000`@000001P00000000";
		data += "Ob@8@@00Ob@8@Ga13R@8Mga172@8?PAo3R@827QoOb@820@0O`0007`0000007P0";
		data += "O`000P08Od400g`<3V=P0G`673IP0`@3>1`00P@6O`P00g`<O`000GP800000000";
		data += "?P9PL020O`<`N3R0@E4HC7b0@ET<ATB0@@l6C4B0O`H3N7b0?P01L3R000000020";

		fontSprite = new AC::Sprite(128, 48);
		int px = 0, py = 0;
		for (size_t b = 0; b < 1024; b += 4) {
			uint32_t sym1 = (uint32_t)data[b + 0] - 48;
			uint32_t sym2 = (uint32_t)data[b + 1] - 48;
			uint32_t sym3 = (uint32_t)data[b + 2] - 48;
			uint32_t sym4 = (uint32_t)data[b + 3] - 48;
			uint32_t r = sym1 << 18 | sym2 << 12 | sym3 << 6 | sym4;

			for (int i = 0; i < 24; i++) {
				int k = r & (1 << i) ? 255 : 0;
				fontSprite->SetPixel(px, py, AC::Pixel(k, k, k, k));
				if (++py == 48) { px++; py = 0; }
			}
		}

		fontDecal = new AC::Decal(fontSprite);

		constexpr std::array<uint8_t, 96> vSpacing = { {
			0x03,0x25,0x16,0x08,0x07,0x08,0x08,0x04,0x15,0x15,0x08,0x07,0x15,0x07,0x24,0x08,
			0x08,0x17,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x24,0x15,0x06,0x07,0x16,0x17,
			0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x17,0x08,0x08,0x17,0x08,0x08,0x08,
			0x08,0x08,0x08,0x08,0x17,0x08,0x08,0x08,0x08,0x17,0x08,0x15,0x08,0x15,0x08,0x08,
			0x24,0x18,0x17,0x17,0x17,0x17,0x17,0x17,0x17,0x33,0x17,0x17,0x33,0x18,0x17,0x17,
			0x17,0x17,0x17,0x17,0x07,0x17,0x17,0x18,0x18,0x17,0x17,0x07,0x33,0x07,0x08,0x00, } };

		for (auto c : vSpacing) vFontSpacing.push_back({ c >> 4, c & 15 });

	}

	void ScreenController::pgex_Register(AC::SCX* pgex)
	{
		if (std::find(vExtensions.begin(), vExtensions.end(), pgex) == vExtensions.end())
			vExtensions.push_back(pgex);
	}


	SCX::SCX(bool bHook) { if (bHook) controller->pgex_Register(this); }
	void SCX::OnBeforeUserCreate() {}
	void SCX::OnAfterUserCreate() {}
	void SCX::OnBeforeUserUpdate(float& fElapsedTime) {}
	void SCX::OnAfterUserUpdate(float fElapsedTime) {}

	// Need a couple of statics as these are singleton instances
	// read from multiple locations
	std::atomic<bool> ScreenController::bAtomActive{ false };
	AC::ScreenController* AC::SCX::controller = nullptr;
	AC::ScreenController* AC::Platform::ptrSC = nullptr;
	AC::ScreenController* AC::Renderer::ptrSC = nullptr;
	//std::unique_ptr<ImageLoader> AC::Sprite::loader = nullptr;
};
#pragma endregion

#pragma endregion 


#pragma region Platform_Config
	namespace AC {
		void ScreenController::AC_ConfigureSystem()
		{
			platform = std::make_unique<AC::Platform>();
			renderer = std::make_unique<AC::Renderer>();

			// Associate components with SC instance
			platform->ptrSC = this;
			renderer->ptrSC = this;
		}
	}
#pragma endregion


#endif // end Definition of application (backend)