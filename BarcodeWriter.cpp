/*
 * Copyright 2016 Nu-book Inc.
 * Copyright 2023 Ze-Zheng Wu
 */
// SPDX-License-Identifier: Apache-2.0

#include "BarcodeFormat.h"
#include "BitMatrix.h"
#include "CharacterSet.h"
#include "MultiFormatWriter.h"

#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <exception>
#include <memory>
#include <string>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

using namespace ZXing;

struct WriteResult
{
	emscripten::val image;
	std::string error;
};

WriteResult
writeBarcodeToImage(std::wstring text, std::string format, std::string encoding, int margin, int width, int height, int eccLevel)
{
	try
	{
		auto barcodeFormat = BarcodeFormatFromString(format);
		if (barcodeFormat == BarcodeFormat::None)
			return {{}, "Unsupported format: " + format};

		MultiFormatWriter writer(barcodeFormat);
		if (margin >= 0)
			writer.setMargin(margin);

		CharacterSet charset = CharacterSetFromString(encoding);
		if (charset != CharacterSet::Unknown)
			writer.setEncoding(charset);

		if (eccLevel >= 0 && eccLevel <= 8)
			writer.setEccLevel(eccLevel);

		auto buffer = ToMatrix<uint8_t>(writer.encode(text, width, height));

		int len;
		uint8_t *bytes = stbi_write_png_to_mem(buffer.data(), 0, buffer.width(), buffer.height(), 1, &len);

		if (bytes == nullptr)
			return {{}, "Unknown error"};

		thread_local const emscripten::val Uint8Array = emscripten::val::global("Uint8Array");

		emscripten::val js_bytes = Uint8Array.new_(emscripten::typed_memory_view(len, bytes));
		STBIW_FREE(bytes);

		return { js_bytes };
	}
	catch (const std::exception &e)
	{
		return {{}, e.what()};
	}
	catch (...)
	{
		return {{}, "Unknown error"};
	}
}

EMSCRIPTEN_BINDINGS(BarcodeWriter)
{
	using namespace emscripten;

	value_object<WriteResult>("WriteResult")
			.field("image", &WriteResult::image)
			.field("error", &WriteResult::error);

	function("writeBarcodeToImage", &writeBarcodeToImage);
}
