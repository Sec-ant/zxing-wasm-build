/*
 * Copyright 2016 Nu-book Inc.
 * Copyright 2023 Axel Waggershauser
 * Copyright 2023 Ze-Zheng Wu
 */
// SPDX-License-Identifier: Apache-2.0

#include "ReadBarcode.h"

#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <memory>
#include <stdexcept>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace ZXing;

struct ReadResult
{
	std::string format{};
	std::string text{};
	emscripten::val bytes;
	std::string error{};
	Position position{};
	std::string symbologyIdentifier{};
	std::string eccLevel{};
	std::string version{};
	int orientation;
	bool isMirrored;
	bool isInverted;
};

std::vector<ReadResult> readBarcodes(ImageView iv, bool tryHarder, const std::string &formats, int maxSymbols)
{
	try
	{
		DecodeHints hints;
		hints.setTryHarder(tryHarder);
		hints.setTryRotate(tryHarder);
		hints.setTryInvert(tryHarder);
		hints.setTryDownscale(tryHarder);
		hints.setFormats(BarcodeFormatsFromString(formats));
		hints.setMaxNumberOfSymbols(maxSymbols);
		//		hints.setReturnErrors(maxSymbols > 1);

		auto results = ReadBarcodes(iv, hints);

		std::vector<ReadResult> readResults{};
		readResults.reserve(results.size());

		thread_local const emscripten::val Uint8Array = emscripten::val::global("Uint8Array");

		for (auto &result : results)
		{
			ByteArray bytes = result.bytes();
			readResults.push_back({
					ToString(result.format()),
					result.text(),
					std::move(Uint8Array.new_(emscripten::typed_memory_view(bytes.size(), bytes.data()))),
					ToString(result.error()),
					result.position(),
					result.symbologyIdentifier(),
					result.ecLevel(),
					result.version(),
					result.orientation(),
					result.isMirrored(),
					result.isInverted(),
			});
		}

		return readResults;
	}
	catch (const std::exception &e)
	{
		return {{"", "", {}, e.what()}};
	}
	catch (...)
	{
		return {{"", "", {}, "Unknown error"}};
	}
	return {};
}

std::vector<ReadResult> readBarcodesFromImage(int bufferPtr, int bufferLength, bool tryHarder, std::string formats, int maxSymbols)
{
	int width, height, channels;
	std::unique_ptr<stbi_uc, void (*)(void *)> buffer(
			stbi_load_from_memory(reinterpret_cast<const unsigned char *>(bufferPtr), bufferLength, &width, &height, &channels, 1),
			stbi_image_free);
	if (buffer == nullptr)
		return {{"", "", {}, "Error loading image"}};

	return readBarcodes({buffer.get(), width, height, ImageFormat::Lum}, tryHarder, formats, maxSymbols);
}

ReadResult readBarcodeFromImage(int bufferPtr, int bufferLength, bool tryHarder, std::string formats)
{
	return FirstOrDefault(readBarcodesFromImage(bufferPtr, bufferLength, tryHarder, formats, 1));
}

std::vector<ReadResult> readBarcodesFromPixmap(int bufferPtr, int imgWidth, int imgHeight, bool tryHarder, std::string formats, int maxSymbols)
{
	return readBarcodes({reinterpret_cast<uint8_t *>(bufferPtr), imgWidth, imgHeight, ImageFormat::RGBX}, tryHarder, formats, maxSymbols);
}

ReadResult readBarcodeFromPixmap(int bufferPtr, int imgWidth, int imgHeight, bool tryHarder, std::string formats)
{
	return FirstOrDefault(readBarcodesFromPixmap(bufferPtr, imgWidth, imgHeight, tryHarder, formats, 1));
}

EMSCRIPTEN_BINDINGS(BarcodeReader)
{
	using namespace emscripten;

	value_object<ReadResult>("ReadResult")
			.field("format", &ReadResult::format)
			.field("text", &ReadResult::text)
			.field("bytes", &ReadResult::bytes)
			.field("error", &ReadResult::error)
			.field("position", &ReadResult::position)
			.field("symbologyIdentifier", &ReadResult::symbologyIdentifier)
			.field("eccLevel", &ReadResult::eccLevel)
			.field("version", &ReadResult::version)
			.field("orientation", &ReadResult::orientation)
			.field("isMirrored", &ReadResult::isMirrored)
			.field("isInverted", &ReadResult::isInverted);

	value_object<ZXing::PointI>("Point")
			.field("x", &ZXing::PointI::x)
			.field("y", &ZXing::PointI::y);

	value_object<ZXing::Position>("Position")
			.field("topLeft", emscripten::index<0>())
			.field("topRight", emscripten::index<1>())
			.field("bottomRight", emscripten::index<2>())
			.field("bottomLeft", emscripten::index<3>());

	register_vector<ReadResult>("vector<ReadResult>");

	function("readBarcodeFromImage", &readBarcodeFromImage);
	function("readBarcodeFromPixmap", &readBarcodeFromPixmap);

	function("readBarcodesFromImage", &readBarcodesFromImage);
	function("readBarcodesFromPixmap", &readBarcodesFromPixmap);
};
