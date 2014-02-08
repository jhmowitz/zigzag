// ZigZagEngine.h

#pragma once

using namespace System;

namespace ZigZag
{
  public ref class ZigZagEngine
  {
  public:
    static IZigZagHost ^Host;

		ZigZagEngine(IZigZagHost ^host, array<String^> ^arguments);
		void Run();

		// Operations
		void ZoomTo(INT2 viewportX, INT2 viewportY, REAL zoomfactor);
		void InitCadre();
  };
}
